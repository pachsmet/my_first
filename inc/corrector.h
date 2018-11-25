
#ifndef _CORRECTOR_H_
#define _CORRECTOR_H_

#include "calc_rc.h"

#define CORRECTOR2X_RESISTOR_COUNT 3
#define CORRECTOR2X_GAIN_COUNT 3
#define CORRECTOR_OPEN_SHORT_GAIN_COUNT 8//6
#define PREDEFINED_PERIODS_COUNT 5

//COEFF_CORRECTOR_SIZE == one flash page!!!!
#define COEFF_CORRECTOR_SIZE 2048

typedef struct ZmOpen
{
	complexf Zstdm;//measured load
	complexf Zom;//measured open fixtures
} ZmOpen;

typedef struct ZmShort
{
	complexf Zstdm;//measured load
	complexf Zsm;//measured short
} ZmShort;

typedef struct CoeffCorrector2x
{
	ZmOpen Zm[CORRECTOR2X_GAIN_COUNT];

	/*
		Для разных коэффициэнтов усиления используем разные резисторы.
		Для gain=0, gain=1 используем маленький номинал
		Для gain=2 используем большой.
	*/
	float R[CORRECTOR2X_GAIN_COUNT];//real load value
	float C[CORRECTOR2X_GAIN_COUNT];
} CoeffCorrector2x;

typedef struct CoeffCorrectorOpen
{
	ZmOpen Zm[CORRECTOR_OPEN_SHORT_GAIN_COUNT];
	float R;//precize real value
	float C;//capacitance load
	uint32_t maxGainIndex;
} CoeffCorrectorOpen;

typedef struct CoeffCorrectorShort
{
	ZmShort Zm[CORRECTOR_OPEN_SHORT_GAIN_COUNT];
	float R100;//real load value 100 Om
	float R1;//real load value 1 Om (for gain=7)
} CoeffCorrectorShort;

//sizeof(CoeffCorrector)<2048
typedef struct CoeffCorrector
{
	uint32_t period;//period==0 - not filled
	CoeffCorrector2x x2x[CORRECTOR2X_RESISTOR_COUNT];
	CoeffCorrectorOpen open;
	CoeffCorrectorShort cshort;
} CoeffCorrector;

void SetCorrector2x(uint8_t resistor, uint8_t gain, float* data);
void SetCorrector2xR(uint8_t resistor, float* data);
void SetCorrectorOpen(uint8_t gain, float* data);
void SetCorrectorOpenR(uint8_t maxGainIndex, float* data);

void SetCorrectorShort(uint8_t gain, float* data);
void SetCorrectorShortR(float* data);
void SetCorrectorPeriod(uint32_t period);

/*
Коэффициэнт, на который нужно умножить R для того, чтобы избавится от 
неточности усиления при разных коэффициэнтах усиления.
*/
complexf GainCorrector(uint8_t gain_index_V, uint8_t gain_index_I);

complexf Corrector(complexf Zxm);

//Очистить весь flash необходимый для записи калибровоячных констант
bool CorrectorFlashClear(void);
bool CorrectorFlashClearCurrent(void);

//Записать текущие константы в нужный кусок flash.
//Обязательно очищать данные перед записью!!!!!!!
bool CorrectorFlashCurrentData(void);

//Прочитать корректирующие коэффициэнты, соответствующие DacPeriod()
void CorrectorLoadData(void);

CoeffCorrector* GetCorrector(void);

void ClearCorrector(void);

//return index in predefinedPeriods array
//return 255 if not found
uint8_t PredefinedPeriodIndex(void);

//index=0..7
int8_t GetGainValidIdx(uint8_t index);


#endif//_CORRECTOR_H_
