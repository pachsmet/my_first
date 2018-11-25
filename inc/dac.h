
#ifndef _DAC_H_
#define _DAC_H_

#define pi  3.14159f
#define SINUS_BUFFER_SIZE 3000
#define DEFAULT_DAC_AMPLITUDE 1800//1900//1200
#define DAC_ZERO 2047
#define DAC_AMPLITUDE DEFAULT_DAC_AMPLITUDE
#define ADC_STM32_Ref 3.0f
#define dac_period_100k (24*32)
#define dac_period_200k (24*16)

extern float g_sinusBufferFloat[SINUS_BUFFER_SIZE];

void DacInit(void);

/*
	Stop DAC and set frequency
*/
void DacSetFrequency(uint32_t frequency);
void DacSetPeriod(uint32_t sinusPeriod, uint16_t amplitude);
void DacStart(void);
void DacSetDelitel(uint8_t delitel, uint8_t save);
uint8_t DacGetDelitel(void);
uint32_t DacPeriod(void);
float DacFrequency(void);
uint32_t DacSamplesPerPeriod(void);
uint32_t DacSampleTicks(void);
void DacSinusCalculate(void);
#endif//_DAC_H_

