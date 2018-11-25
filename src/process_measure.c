

#include "hw_config.h"
#include "calc_rc.h"
#include "mcp6s21.h"
#include "lcd_interface.h"
#include "dac.h"
#include "corrector.h"
#include "menu.h"
#include "quadrature_encoder.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define goodMax_187 (2048+(1100/1.28))
#define goodMax_93 (2048+(1100/1.08))
#define goodMax_10 (2048+1100)

uint16_t goodMax  =(2048+1000);//1400 1652
uint16_t goodMin  =(2048-1000);

static STATES state = STATE_NOP;
bool startFast;

#define gainValuesCount 8
uint8_t gainValues[gainValuesCount] = {1,2,5,10,20,50,100,200}; //{1,2,4,5,8,10,16,32}; коэффициенты усиления ПГА

const uint8_t gainIdxPtrCentral[] = {0,1,2}; // доступные коэффицуиенты усиления в режимах всех кроме КЗ и ОПЕН
const uint8_t gainIdxCountCentral = 3; // их количество

const uint8_t gainIdxPtrOpenShort[] = {0,1,2,3,4,5}; //,6,7 доступные коэффицуиенты усиления в режимах КЗ и ОПЕН
const uint8_t gainIdxCountOpenShort = 6; // их количество

const uint8_t* gainIdxPtr = NULL; // ссылка на однин или второй массив выше в зависимости от ситуации
uint8_t gainIdxCount = 0; // тоже смое текущий порядковый номер гейна


volatile uint8_t resistorIdx; // кучка порядковых номеров гейнов для напряжения и тока и порядковый номер резистора
volatile uint8_t gainVoltageIdx;
volatile uint8_t gainCurrentIdx;
volatile uint8_t gainVoltageIdx_temp;
volatile uint8_t gainCurrentIdx_temp;

uint8_t gainIndexIterator; // вспомогательные переменные
bool gainIndexStopV;// вспомогательные переменные
bool gainIndexStopI;// вспомогательные переменные

uint16_t computeXCount; // вспомогательные переменные усреднения 
uint16_t computeXIterator;// вспомогательные переменные усреднения
volatile uint8_t predefinedResistorIdx=0; // предустановленное значение резистора, только тут их вписывать смысла нет
uint8_t predefinedGainVoltageIdx=0; //напряжения
uint8_t predefinedGainCurrentIdx=0; //тока

bool useCorrector; // используется или нет корректор

static uint8_t initWaitCount = 0; // просто счетчик сколько циклов АЦП пропускать

bool bContinuousMode = false; // тип замеров непрерывный или разовый 
bool bCalibration = false;    // калибровка ли или нет

static bool debugRepaint = false;// хрень какая-то с отладкой связана она не работает все равно

extern int printD;// хрень какая-то с отладкой связана 

static AdcSummaryData sum_data;// данные с АЦП, ссылка на структуру, заполняется в другом месте

void OnStartGainAuto(void); // референции функций 
void OnResistorIndex(void);
void OnStartGainIndex(void);
void OnGainIndex(void);
void OnMeasure(void);
void OnMeasureStart(void);

double error_Re;
double error_Rim;
	
void SetGainCentralIdx(void) // задаем ссылки на массив с гейнами и их количество для всего кроме КЗ и ОПЕН, т.е. 8 штук
{
	gainIdxPtr = gainIdxPtrCentral;
	gainIdxCount = gainIdxCountCentral;
}

void SetGainOpenShortIdx(void)// задаем ссылки на массив с гейнами и их количество для КЗ и ОПЕН, т.е. 3 штуки
{
	gainIdxPtr = gainIdxPtrOpenShort;
	
	if ((int)DacFrequency()>187000)gainIdxCount = gainIdxCountOpenShort-1;
	else if ((int)DacFrequency()>93000)gainIdxCount = gainIdxCountOpenShort-1;
	else 
		gainIdxCount = gainIdxCountOpenShort;
}


float getGainValue(uint8_t idx) // читаем коэффициенты усиления
{
	float mulPre = 1.5f;//2.00f; //3.74f; КУ инструментального усилителя
    float mulX = 1.0f;
    if(idx<gainValuesCount)
    	mulX = gainValues[idx];
    return mulPre*mulX;
}

float getGainValueV(void)// читаем коэффициенты усиления напряжения
{
	return getGainValue(gainVoltageIdx);
}

float getGainValueI(void)// читаем коэффициенты усиления тока
{
	return getGainValue(gainCurrentIdx);
}

float getResistorOm(void)// читаем сопротивление резистора
{
	float R = 100.0;
	switch(resistorIdx) {
	case 0: R = 1e2f; break;
	case 1: R = 1e3f; break;
	case 2: R = 1e4f; break;
	case 3: R = 1e5f; break;
	default:;
	}
	return R;
}

uint16_t CalcOptimalCount;
uint16_t ProcessCalcOptimalCount(void) //расчет оптимального количество значений усреднения
{
	uint16_t count = 100;
	
	if((!startFast) && (predefinedGainVoltageIdx==255 && predefinedGainCurrentIdx==255)) count=1;
		
	CalcOptimalCount=count;// для отладки добавил
	return count;
}

void ProcessStartComputeX(uint16_t count, uint8_t predefinedResistorIdx_, // начало подбора данных по резисотрам и гейнам приводятся в начальное положение
			uint8_t predefinedGainVoltageIdx_,
			uint8_t predefinedGainCurrentIdx_,
			bool useCorrector_
			)
{
	SetGainCentralIdx();                            // устанавливаем по гейны на 8 шт
	calculatedValues = false;
	computeXCount = count;                          
	//if(count==0)
	//{
		computeXCount = ProcessCalcOptimalCount(); //если только начали то берем оптимальное количество замеров в зависимости от частоты АЦП
	//}

	predefinedResistorIdx = predefinedResistorIdx_; // загружаем предустановленные значения, для калиьбровки используются
	predefinedGainVoltageIdx = predefinedGainVoltageIdx_;
	predefinedGainCurrentIdx = predefinedGainCurrentIdx_;
	useCorrector = useCorrector_; // всегда используется кроме режимов по ЮСБ которые не пашут, для калиьбровки не используется
	OnStartGainAuto(); // старт гейн авто

}

STATES ProcessGetState(void) // смотрим текущее состояние
{
	return state;
}

void ProcessData(void) // вот тут смотрим че делать в зависимости от состояния
{
	switch(state)
	{
	case STATE_NOP: // ниче 
		return;
	case STATE_INIT_WAIT: // тоже ниче через ниче
		OnInitWait();
		return;
	case STATE_RESISTOR_INDEX: // запускаем выбор резистора
		OnResistorIndex();
		break;
	case STATE_RESISTOR_INDEX_WAIT:// планируем запуск выбора резистора но ниче не делаем больше
		state = STATE_RESISTOR_INDEX;
		break;
	case STATE_GAIN_INDEX: // запускаем выбор гейнов ПГА
		OnGainIndex();
		break;
	case STATE_GAIN_INDEX_WAIT:// планируем запуск выбора гейнов ПГА
		state = STATE_GAIN_INDEX;
		break;
	case STATE_MEASURE: // делаем замер и смотрим нормально все вышло или нет, если нет то перебор всего сначала если норм то по короткой схеме новые замеры
		OnMeasure();
		break;
	case STATE_MEASURE_WAIT:// планируем замер 
		state = STATE_MEASURE;
		break;
	}
}
extern uint8_t filtr_on;

float erroru;
float errori;
void OnStartGainAuto(void) // старт автоподбора гейнов
{
	if(debugRepaint)
	{
		LcdRepaint(); // отладочная информация 
	}

	CorrectorLoadData(); // коректов видимо с калибровками что-то подготавливает

	resistorIdx = 0; // обнуляются положения гейнов и резистора
	gainVoltageIdx = 0;
	gainCurrentIdx = 0;

	if(predefinedGainVoltageIdx!=255 && predefinedGainCurrentIdx!=255)
	{
		gainVoltageIdx = predefinedGainVoltageIdx; // если есть предустановки гейнов они считываются
		gainCurrentIdx = predefinedGainCurrentIdx;
	}

	MCPSetGain(true, gainVoltageIdx); // тут физически задаются данные в ПГА напряжения
	MCPSetGain(false, gainCurrentIdx);// тут физически задаются данные в ПГА тока
	
	SetLowPassFilter(DacPeriod(), filtr_on); // задается фильтр в зависимости от частоты

	if(predefinedResistorIdx!=255)
	{
		resistorIdx = predefinedResistorIdx;// если есть предустановки резистора они считываются
		SetResistor(resistorIdx); // физический выбор резистора
	} else
	{
		SetResistor(resistorIdx); // физический выбор резистора
	}

	initWaitCount = bCalibration?10:2; // выбор сколько ждать, если калибровка то 10 если нет то 2 чего 2 или 10 не известно, циклов замеров АЦП скорее всего
	state = STATE_INIT_WAIT; // состояние ожидание
}

void OnInitWait(void) // думаем че делать дальше
{
	if(initWaitCount>0) // вот тут как раз и ждем то что выше выставили, пропускаем циклы
	{
		initWaitCount--;
		return; // валим
	}

	if(predefinedGainVoltageIdx!=255 && predefinedGainCurrentIdx!=255)
	{
		OnMeasureStart(); // если есть предустанвоки гейнов, то делаем замер
	} else
	if(predefinedResistorIdx!=255)
	{
		OnStartGainIndex(); // если хотя бы есть резистор, то с подбираем гейны
	} else
	{
		state = STATE_RESISTOR_INDEX_WAIT; // ничего нет, ждем, цикл пропускаем, потом с резистора начнем
	}
}

int dI;
void OnResistorIndex(void) // подбер резистора
{
	AdcSummaryChannel* chI = &g_data.ch_i; // берем данные по току с АЦП
	dI = chI->adc_max - chI->adc_min; // смотрим минимальное и максимальное значение

	uint8_t resistorIdx_max=3;
	if ((int)DacFrequency()>93000) resistorIdx_max=2;
	else resistorIdx_max=3;
	
	if ((int)DacFrequency()>187000)goodMax = goodMax_187;
	else if ((int)DacFrequency()>93000)goodMax = goodMax_93;
	else goodMax=goodMax_10;
  goodMin  =(4095-goodMax);
	uint16_t goodDelta =(goodMax-goodMin);
	
	if(dI*10>goodDelta || resistorIdx>=resistorIdx_max) // если дельта тока умножннная на 10 больше ГУДДЕЛЬТЫ(они в начале заданы, тут добавил выше чтоб видно было че это) или резистор уже равен 3
	{
		OnStartGainIndex(); // мы видим ше это не КЗ и выбираем центральные гейны т.е. их 3 штуки всего
	} else
	{
		state = STATE_RESISTOR_INDEX_WAIT; // если все плохо и дельта тока низкая то выбираем цикл ожидания резистора, пропускаем такт замеров.
		resistorIdx++; // добавляем резистор
		SetResistor(resistorIdx); // выставляем физические его
	}
}

void OnStartGainIndex(void) // стар подбор гейнов, самая жопа будет после этого
{
	state = STATE_GAIN_INDEX; // режим выбор гейнов

    gainIndexStopV = false; // все обнуляем
    gainIndexStopI = false;
    gainIndexIterator = 0;
    gainVoltageIdx = 0;
    gainCurrentIdx = 0;
	
	uint8_t resistorIdx_max=3;
	if ((int)DacFrequency()>93000) resistorIdx_max=2;
	else resistorIdx_max=3;
	
	if(resistorIdx==0)  // тут смотрим че за режим замеров, по резистору и амплитуде тока и напряжения
    { 
		int vmin = g_data.ch_v.adc_min; // если резистор на нуле, значит шуп замкнут смотрим дельты тока и напряжения
		int vmax = g_data.ch_v.adc_max;
		int imin = g_data.ch_i.adc_min;
		int imax = g_data.ch_i.adc_max;
		if((imax-imin) < (vmax-vmin)) // если амплитуда тока меньше амплитуды напряжения значит
        {
            //gainIndexStopV = true; //гейн напряжения  не будем трогать
            SetGainCentralIdx(); // всего гейнов берем 3 шт, центральная часть - режим СЕРЕДИНА
        } else
        {
					//gainIndexStopI = true; // если амплитуда тока выше, чем амплитуда напряжения, значит ток не трогаем
					SetGainOpenShortIdx(); // режим у нас опен-шорт (почему он одновременно и опен и шорт хз), но коэффициентов гейна 8, в даном случае похоже режим ОПЕН
        }
    } else
    if(resistorIdx==resistorIdx_max) // если резистор равен 3 
    {
        //gainIndexStopV = true; // напряжение не трогаем
        SetGainOpenShortIdx(); // режим у нас опен-шорт (почему он одновременно и опен и шорт хз), но коэффициентов гейна 8, в даном случае похоже режим ШОРТ
    } else 
    {
        //gainIndexStopV = true; // резистор где-то на середине, напряжение не трогаем
        SetGainCentralIdx(); // режим СЕРЕДИНА
    }

	OnGainIndex(); // сам подбор т.е. самая жопа
}

void OnGainIndex(void) // вот и она
{
	int vmin = g_data.ch_v.adc_min; // берем минимальные и максимальные значения тока и напряжения
	int vmax = g_data.ch_v.adc_max;
	int imin = g_data.ch_i.adc_min;
	int imax = g_data.ch_i.adc_max;

	uint8_t gainIdx = gainIdxPtr[gainIndexIterator]; // берем значение гейна по его номеру который берем их gainIndexIterator

	CoeffCorrector* corr = GetCorrector(); // что-то с корректора берем

	
	uint8_t resistorIdx_max=3;
	if ((int)DacFrequency()>93000) resistorIdx_max=2;
	
	if(resistorIdx==resistorIdx_max && gainIdx>gainIdxCount )// gainIdx>corr->open.maxGainIndex
	{
		gainIndexStopI = true;   // останавливаем подбор тока если резистор равен 3 и достигнут максимальный гейн, который берется в корректоре, он вроде равен 8, и че бы просто с цыфрой 8 не сравнить.
		if(debugRepaint) //отладка
			printD = 135;
	} else
	{
		if(debugRepaint)//отладка
			printD = 246;
	}
	
	if ((int)DacFrequency()>187000)goodMax = goodMax_187;
	else if ((int)DacFrequency()>93000)goodMax = goodMax_93;
	else goodMax=goodMax_10;
  goodMin  =(4095-goodMax);
	
	if(!gainIndexStopV && vmax<goodMax && vmin>goodMin) // если поббор напряжения не остановлен и напряжение в допуске, т.е. нет зашкала ни по верху ни по низу 
	{
		gainVoltageIdx = gainIdx; // значение гена что в начале брали приписываем на гейн напряжения
	} else
	{
		MCPSetGain(true, gainVoltageIdx); // если есть зашкал или побдор напряжения остановлен, записываем физически значение гейна в ПГА
		gainIndexStopV = true; // и останавливаем подбор. Т.е. если все плохо- ушли в зашкал, то останавливаемся на предыдущей итерации подбора
	}

	if(!gainIndexStopI && imax<goodMax && imin>goodMin) // теперь ток тоже самое, нет зашкала
	{
		gainCurrentIdx = gainIdx; // текущее усиление берем в запоминаем как усиление тока
	} else
	{
		MCPSetGain(false, gainCurrentIdx); // если зашкал, то физически в ПГА записываем прошлую итерацию
		gainIndexStopI = true; // стоп подбор
	}

	
	gainIndexIterator++; // прибавляем к номеру гейна еще одно значение
	if(gainIndexIterator>=gainIdxCount) // gainIdxCount если уже гейны кончились (всего их или 3 иили 8 в зависимости от режмима)
	{
		MCPSetGain(true, gainVoltageIdx); //физически в ПГА записываем прошлую итерацию
		MCPSetGain(false, gainCurrentIdx);//физически в ПГА записываем прошлую итерацию, хер его знает зачем если уже записывали
		OnMeasureStart();//state = STATE_NOP; делаем замер в следующий раз с подборами все
	} else 
	{
		gainIdx = gainIdxPtr[gainIndexIterator]; // если гейны не кончились
		if(!gainIndexStopV) 
			MCPSetGain(true, gainIdx); // если остановки подбора напряжения не было вписываем в ПГА текущее усиление
		if(!gainIndexStopI)
			MCPSetGain(false, gainIdx); // если остановки подбора тока не было вписываем в ПГА текущее усиление
		state = STATE_GAIN_INDEX_WAIT; // подбор продолжится на следующем цикле(после нового замера АЦП)
	}

	if(debugRepaint){ // отладка
		LcdRepaint();
	}	
	//LcdRepaint_RVI(); // вывод на экран чтоб мельтишили в процессе подбора значения, я добавлял раньше только конечный результат отображался
}

void OnMeasureStart(void) // старт замера
{
	//LcdRepaint();
	state = STATE_MEASURE_WAIT; // просто запоминаем что мерить будем в следующий раз
	computeXIterator = 0; // количестко замеров для усреднения обнуляем
}

uint8_t Measure_ok; // добавил чтобы не выводить неверные значения
volatile uint8_t timer_of_Measure_ok;
float d;

uint8_t	resistorIdx_old;
uint8_t	gainVoltageIdx_old;
uint8_t	gainCurrentIdx_old;
uint32_t time, time_temp;

extern uint16_t countSumma;
extern uint8_t usrednenie;

float fbuf_Re[100];
float fbuf_Im[100];
float sum_Re;
float sum_Im;
#define goodMax_startFast  (2048+1652)
#define goodMin_startFast  (4096-goodMax_startFast)

void OnMeasure(void) // замер
{
	int vmin = g_data.ch_v.adc_min;
	int vmax = g_data.ch_v.adc_max;
	int imin = g_data.ch_i.adc_min;
	int imax = g_data.ch_i.adc_max;	
		
	if (vmax>goodMax_startFast || vmin<goodMin_startFast || imax>goodMax_startFast || imin<goodMin_startFast){
		ProcessStartComputeX(computeXCount, predefinedResistorIdx, 
				predefinedGainVoltageIdx, predefinedGainCurrentIdx,
				useCorrector);		
		computeXIterator=0;
		startFast=0;
		return;
	}	
		
	if(computeXIterator==0) // если первый замер, количество замеров нулевае 
	{
		sum_data = g_data; //в сумму берем тукущие данные
		sum_Re=0;
		sum_Im=0;	
	}
	else // если нет начинаем складывать все
	{
		sum_data.ch_i.k_sin += g_data.ch_i.k_sin;
		sum_data.ch_i.k_cos += g_data.ch_i.k_cos;
		//sum_data.ch_i.square_error += g_data.ch_i.square_error;

		sum_data.ch_v.k_sin += g_data.ch_v.k_sin;
		sum_data.ch_v.k_cos += g_data.ch_v.k_cos;
		//sum_data.ch_v.square_error += g_data.ch_v.square_error;

		if(g_data.error)
			sum_data.error = true;			
	}
	
	complexf zV = g_data.ch_v.k_sin + g_data.ch_v.k_cos*I;
	complexf zI = g_data.ch_i.k_sin + g_data.ch_i.k_cos*I;
	fbuf_Re[computeXIterator] = creal (zV/zI);
	sum_Re+=fbuf_Re[computeXIterator];
	fbuf_Im[computeXIterator] = cimag (zV/zI);
	sum_Im+=fbuf_Im[computeXIterator];
	
	computeXIterator++; // считаем сколько замеров сделали
	

	if(computeXIterator<computeXCount) // пока нужное количество не намерием дальше не идем
	{
		return;
	}

  // все намерили штук 100 замеров и считаем че вышло
	//calculate result
	sum_data.ch_i.k_sin /= computeXCount; // усредняем
	sum_data.ch_i.k_cos /= computeXCount;
	//sum_data.ch_i.square_error /= computeXCount;

	sum_data.ch_v.k_sin /= computeXCount;
	sum_data.ch_v.k_cos /= computeXCount;
	//sum_data.ch_v.square_error /= computeXCount;
	sum_Re/=computeXCount;	
	sum_Im/=computeXCount;
	g_data = sum_data; // обрано теперь из среднего в ту переменную, где все и брали
	
	error_Re=0;
	error_Rim=0;
	for (uint8_t i=0; i<computeXCount; i++){
		error_Re+=(fbuf_Re[i]-sum_Re)*(fbuf_Re[i]-sum_Re);
		error_Rim+=(fbuf_Im[i]-sum_Im)*(fbuf_Im[i]-sum_Im);		
	}	
	if (sum_Re<0)sum_Re=-sum_Re;
	if (sum_Im<0)sum_Im=-sum_Im;	
	
	error_Re=  100*sqrt(error_Re/(computeXCount-1))/sum_Re;
	error_Rim=	100*sqrt(error_Rim/(computeXCount-1))/sum_Im;
		
	bool oldLastZxFilled = lastZxFilled; // что-то мутное
	complexf oldLastZx = lastZx;// что-то мутное
//=====================================================			
	OnCalculate(useCorrector);// сам расчет из данных, не интересно
//=====================================================		
	
	if(bCalibration) // если у нас калибровка
	{
		computeXCount = 200;
		Measure_ok=1; // выводим на экран значения
		state = STATE_NOP; // стейт нчие не делаем
		OnCalibrationComplete(); // завершаем ее, че там не смотрел
		timer_of_Measure_ok=0;
    time_temp++;
		LcdRepaint(); // выводим на экран 	
	  Measure_ok=0;
	}
//=====================================================			
	else
	if(bContinuousMode) // если нет, но включен какой то континиус моде, который походу всегда включен(не разовый замер)
	{
		startFast = false; // быстрый старт выключаем
		
		if(predefinedGainVoltageIdx!=255 && predefinedGainCurrentIdx!=255){
      time_temp++;	
		  LcdRepaint(); // выводим на экран 
	    Measure_ok=0;
		}
		else {

		if(oldLastZxFilled) // хрен его знает, но вцелом тут ясно решаем хорошо замерили или плохо
		{
			d = cabs(oldLastZx-lastZx)/cabs(lastZx); // толи это расчет ошибки толи отклонеие от прошлого замера, т.е. сменили измеряемый элемент
			if(d>1)
				d = 1;
			if(d<10e-2f) // если эта Д меньше чего-то 
			{
				int vmin = g_data.ch_v.adc_min;
				int vmax = g_data.ch_v.adc_max;
				int imin = g_data.ch_i.adc_min;
				int imax = g_data.ch_i.adc_max;
								
				if(vmax<goodMax_startFast && vmin>goodMin_startFast && imax<goodMax_startFast && imin>goodMin_startFast) // и амплитуды тока и напряжения у нас в допуске(нет зашкала)
				{
					//ТО Все хорошо, можно не пересчитывать коэффициэнты и не переставлять резистор
					startFast = true; // включаем быстрый старт, который выше отключили. Т.е. если какая-то "Д" херовая или зашкал то будет подбор опять резистораи гейнов, если все норм, то ничего не подбирается
				}
			}
			else {
		   usrednenie=0; 
			}
						
			float Rre_temp;
			if (Rre<0)Rre_temp=-Rre;
			else Rre_temp=Rre;
			
			uint8_t resistorIdx_max=3;			
			uint8_t gainCurrentIdx_max=gainIdxCountOpenShort-1;			
			if ((int)DacFrequency()>187000){
				gainCurrentIdx_max = 4;
				resistorIdx_max=2;
			}
	    else if ((int)DacFrequency()>93000){
				gainCurrentIdx_max = 4;
				resistorIdx_max=2;
			}
	  			
			if ((((Rre_temp>10e+6f)||(Rre<0))&&(resistorIdx==resistorIdx_max)&&(gainCurrentIdx==gainCurrentIdx_max)) && (vmax<goodMax_startFast && vmin>goodMin_startFast && imax<goodMax_startFast && imin>goodMin_startFast)) startFast = true;
			else if (((Rre_temp<100e-3f) &&(resistorIdx==0) && (gainVoltageIdx==gainIdxCountOpenShort) )&& (vmax<goodMax_startFast && vmin>goodMin_startFast && imax<goodMax_startFast && imin>goodMin_startFast)) startFast = true;	
      else if 	(((resistorIdx==resistorIdx_max)&&(gainCurrentIdx==gainCurrentIdx_max)&&(gainVoltageIdx==gainCurrentIdx_max))&& (vmax<goodMax_startFast && vmin>goodMin_startFast && imax<goodMax_startFast && imin>goodMin_startFast)) startFast = true;
		}		
	}
		
		computeXCount = ProcessCalcOptimalCount(); 
		
		if(startFast) {// если быстрый старт
		Measure_ok=1;
//=====================================================				
	  if (Measure_ok==1) {
			timer_of_Measure_ok=0;
		}
    time_temp++;	
		LcdRepaint(); // выводим на экран 
	  Measure_ok=0;
//=====================================================		
		if	((resistorIdx_old!=resistorIdx)||(gainVoltageIdx_old!=gainVoltageIdx)||(gainCurrentIdx_old!=gainCurrentIdx)){
			resistorIdx_old=resistorIdx;
			gainVoltageIdx_old=gainVoltageIdx;
			gainCurrentIdx_old=gainCurrentIdx;
			Reset_Timer_off();			//если есть смена значит прибором пользуются
		}		
//=====================================================		
		OnMeasureStart(); // то сразу замер		
		}
		else{ 
			//extern uint8_t dac_on;
			//if (dac_on==0)LcdRepaint(); // выводим на экран 
			// если нет то начинай сначала, но с учетом предустановок
			ProcessStartComputeX(computeXCount, predefinedResistorIdx, 
				predefinedGainVoltageIdx, predefinedGainCurrentIdx,
				useCorrector);
		}
	}
	else // если разовый замер, то пофиг, ниче не делаем больше
	{
		state = STATE_NOP;
	}

}


