
#include "hw_config.h"
#include <math.h>
#include "adc.h"
#include "dac.h"
#include "systick.h"
#include "calc_rc.h"

#include "lcd_interface.h"


/*
	g_resultBuffer организован не очень удобно.
	В нем хранятся uint16_t данные.
	Вначале идет g_ResultBufferSize сэмплов от V канала.
	Потом идет g_ResultBufferSize сэмплов от I канала, начиная
	со смещения g_resultBuffer[g_ResultBufferSize/2].

	g_resultBuffer - данные непосредственно от ADC, постоянно пишутся в циклическом режиме.
	g_resultBufferCopy - копия данных, пишется только по требованию и не обновляется после этого.
*/
uint32_t g_ResultBufferSize = RESULT_BUFFER_SIZE;
uint32_t g_resultBuffer[RESULT_BUFFER_SIZE] __attribute__ ((section(".dont_ccm")));	
//uint32_t g_resultBufferCopy[RESULT_BUFFER_SIZE];

extern uint32_t SinusBufferSize;

static volatile uint8_t g_adc_cycles;
static volatile uint8_t g_cur_cycle;

static volatile bool g_usb_request_data = false; //Данные затребованы для получения
static volatile bool g_usb_sampled_data = false; //Данные отсэмплированны и помещенны в буфер g_resultBufferCopy

uint16_t g_adcStatus = 0;
uint16_t g_adc_cur_read_pos;
bool g_adc_read_buffer = false;
uint32_t g_adc_elapsed_time = 0;

AdcSummaryData g_data;

void AdcRoundSize(uint32_t dac_samples_per_period)
{
	//требуется g_ResultBufferSize%dac_samples_per_period==0
	g_ResultBufferSize = (RESULT_BUFFER_SIZE/dac_samples_per_period)*dac_samples_per_period;
}

void AdcStop()
{
	//StopTimer();
	//g_adc_elapsed_time = GetTime();
	g_adcStatus = 2;

	ADC_StopConversion(ADC1);
	ADC_StopConversion(ADC2);
}

//////////////

static void NVIC_Configuration12(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	
}

static void AdcInit12()
{
	RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);

	NVIC_Configuration12();

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_VoltageRegulatorCmd(ADC1, ENABLE);
	ADC_VoltageRegulatorCmd(ADC2, ENABLE);
	delay_ms(1);//delay_us(20);

	ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1) != RESET );

	ADC_SelectCalibrationMode(ADC2, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC2);
	while(ADC_GetCalibrationStatus(ADC2) != RESET );

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_RegSimul;// ADC_Mode_Independent
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_SynClkModeDiv1;
	//ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;// ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_Circular; //ADC_DMAMode_OneShot;//
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
	ADC_CommonInit(ADC1, &ADC_CommonInitStructure);

	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_3; //ADC_ExternalTrigConvEvent_1 for ADC34
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_RisingEdge;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Enable;
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	//ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None; //я выключил
	ADC_Init(ADC2, &ADC_InitStructure);

	ADC_DMAConfig(ADC1, ADC_DMAMode_Circular);//ADC_DMAMode_OneShot
	//ADC_DMAConfig(ADC2, ADC_DMAMode_Circular);
}

extern uint32_t dac_adc_freq;
static void AdcStartPre12()
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
	//DMA_DeInit(DMA2_Channel1);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1_2->CDR);//(uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_resultBuffer[0];//(uint32_t)&g_resultBuffer[g_ResultBufferSize/2];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = RESULT_BUFFER_SIZE;//g_ResultBufferSize;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1, ENABLE);
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
  DMA_SetCurrDataCounter(DMA1_Channel1, 0);

  uint8_t sample_ticks;// = DacSampleTicks()<72?ADC_SampleTime_7Cycles5:ADC_SampleTime_19Cycles5; //24 для 187 и 10 кГц ,48 для 1кГц 360 1 кГЦ
	
	if (dac_adc_freq==3000000)sample_ticks=ADC_SampleTime_7Cycles5;
	else if (dac_adc_freq==1500000)sample_ticks=ADC_SampleTime_19Cycles5;
	else sample_ticks=ADC_SampleTime_181Cycles5;
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1/*PA0 - I*/, 1, sample_ticks);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_3/*PA6 - U*/, 1, sample_ticks);

	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);

	g_adcStatus = 1;
	g_adc_cycles = 0;
}

void AdcInit()
{
	AdcInit12();
}

void AdcStartPre()
{
	AdcStartPre12();
}



void AdcDacStartSynchro(uint32_t period, uint16_t amplitude)
{
	if(g_adcStatus==1)
		AdcStop();//Потенциально здесь может все зависнуть, если цикл внутри AdcQuant не завершился
	
	g_cur_cycle = 0;

	LcdRepaint();

	DacSetPeriod(period, amplitude);
	AdcRoundSize(DacSamplesPerPeriod());
	AdcStartPre();

	ADC_StartConversion(ADC1);
	ADC_StartConversion(ADC2);

	g_adc_elapsed_time = 0;

	TIM_Cmd(TIM2, ENABLE); //Start DAC
}




//Когда данные скопированны в g_resultBufferCopy
static void AdcOnComplete()
{
//	g_usb_request_data = false;

	//uint16_t* inV = (uint16_t*)g_resultBufferCopy;
	//uint16_t* inI = (uint16_t*)&g_resultBufferCopy[g_ResultBufferSize/2];

	g_data.count = g_ResultBufferSize;

	AdcCalcData(&g_data, g_resultBuffer, g_ResultBufferSize);

	ProcessData();

//	if(ProcessGetState()==STATE_NOP)
//	{
//		g_usb_sampled_data = true;
//	} else
//	{
//		g_usb_request_data = true;
//	}

}


volatile uint8_t adc_ready=0;

void AdcQuant()
{	
	
	if(g_adcStatus==0){
		return;
	}
	g_adcStatus=0;
	

	AdcClearData(&g_data);
	AdcOnComplete();
	
	adc_ready=1;
  while (DMA1_Channel2->CNDTR>1);
	DMA1_Channel1->CCR |= (uint16_t)(DMA_CCR_EN);

}

//for ADC1
void DMA1_Channel1_IRQHandler(void)
{

	if (DMA_GetITStatus(DMA1_FLAG_TC1) == SET)		
	{ 
		DMA_ClearITPendingBit(DMA1_FLAG_TC1);	
    DMA1_Channel1->CCR &= ~(uint16_t)(DMA_CCR_EN);
		DMA1_Channel1->CNDTR=RESULT_BUFFER_SIZE;//g_ResultBufferSize;
    g_adc_cycles++;

		g_adcStatus=1;
	}		
	if (DMA_GetITStatus(DMA1_FLAG_HT1) == SET)
	{
		DMA_ClearITPendingBit(DMA1_FLAG_HT1);	
	}			
}


////for DAC
//void DMA1_Channel2_IRQHandler(void)
//{

//	
//	if (DMA_GetITStatus(DMA1_FLAG_TC2) == SET)
//	{ 
//		DMA_ClearITPendingBit(DMA1_FLAG_TC2);	
//	  DMA1_Channel1->CCR |= (uint16_t)(DMA_CCR_EN);
//	  DMA1_Channel2->CCR &= ~DMA_IT_TC;		
//	}	
//	
//	if (DMA_GetITStatus(DMA1_FLAG_HT2) == SET)
//	{
//		DMA_ClearITPendingBit(DMA1_FLAG_HT2);	

//	}		
//	
//}


