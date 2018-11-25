

#ifndef _ADC_H_
#define _ADC_H_

#define RESULT_BUFFER_SIZE 3000


typedef struct {
	uint16_t adc_min;
	uint16_t adc_max;
	float k_sin;
	float k_cos;
	float adc_mid;
	float square_error;
} AdcSummaryChannel;


typedef struct {
	uint16_t count;
	AdcSummaryChannel ch_v;
	AdcSummaryChannel ch_i;
	bool error;
	uint32_t nop_number;
} AdcSummaryData;

void AdcInit(void);
void AdcDacStartSynchro(uint32_t period, uint16_t amplitude);
void AdcQuant(void);
void AdcSendLastCompute(void);

extern uint16_t g_adcStatus;
extern bool g_adc_read_buffer;
extern uint32_t g_adc_elapsed_time;

extern uint32_t g_resultBufferCopy[RESULT_BUFFER_SIZE];
extern uint32_t g_ResultBufferSize;

extern AdcSummaryData g_data;


//Calc functions
void AdcClearData(AdcSummaryData* data);
void AdcCalcData(AdcSummaryData* data, uint32_t* inV_I, uint16_t count);

#endif//_ADC_H_
