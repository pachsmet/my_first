
#ifndef _PROCESS_MEASURE_H_
#define _PROCESS_MEASURE_H_

#include "adc.h"

typedef enum STATES
{
	STATE_NOP=0,
	STATE_INIT_WAIT,
	STATE_RESISTOR_INDEX,
	STATE_RESISTOR_INDEX_WAIT,
	STATE_GAIN_INDEX,
	STATE_GAIN_INDEX_WAIT,
	STATE_MEASURE,
	STATE_MEASURE_WAIT,
} STATES;

void ProcessData(void);
uint16_t ProcessCalcOptimalCount(void);
void ProcessStartComputeX(
			uint16_t count, uint8_t predefinedResistorIdx_,
			uint8_t predefinedGainVoltageIdx_,
			uint8_t predefinedGainCurrentIdx_,
			bool useCorrector_
	);
STATES ProcessGetState(void);
void SendRVI(void);
void OnInitWait(void);
extern volatile uint8_t predefinedResistorIdx;
extern volatile uint8_t resistorIdx;
extern volatile uint8_t gainVoltageIdx;
extern volatile uint8_t gainCurrentIdx;
extern volatile uint8_t gainVoltageIdx_temp;
extern volatile uint8_t gainCurrentIdx_temp;			
extern bool isSerial;
extern bool bContinuousMode; //Заполнять перед ProcessStartComputeX
extern bool bCalibration;

float getGainValue(uint8_t idx);
float getGainValueV(void);
float getGainValueI(void);
float getResistorOm(void);

#endif //_PROCESS_MEASURE_H_
			
			
			

