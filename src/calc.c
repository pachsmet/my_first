
#include "hw_config.h"
#include <math.h>
#include "adc.h"
#include "dac.h"
#include "systick.h"

void AdcClearChData(AdcSummaryChannel* ch)
{
	ch->adc_min = 0xFFFF;
	ch->adc_max = 0;
	ch->k_sin = 0.1f;
	ch->k_cos = 0.2f;
	ch->adc_mid = 0;
	ch->square_error = 0.123f;
}

void AdcClearData(AdcSummaryData* data)
{
	data->count = 0;
	AdcClearChData(&data->ch_v);
	AdcClearChData(&data->ch_i);
	data->error = false;
	data->nop_number = 33;
}

static void CalcSquareError(AdcSummaryChannel* ch, uint32_t* in, uint8_t v, uint16_t count)
{
	uint16_t nsamples = DacSamplesPerPeriod();
	uint16_t nsamples4 = nsamples>>2;

	float sum_error = 0;
	for(uint16_t i=0; i<count; i++)
	{
		float sin_table = g_sinusBufferFloat[i%nsamples];
		float cos_table = g_sinusBufferFloat[(i+nsamples4)%nsamples];

		float d = ch->adc_mid + sin_table*ch->k_sin + cos_table*ch->k_cos;
		if (v==1)		d -= (in[i]>>16)&0xFFFF;
		else d -= (in[i]&0xFFFF);

		sum_error += d*d;
	}
	ch->square_error = sqrt(sum_error/(count-1));
}

uint8_t grafic_u[100];
uint8_t grafic_i[100];
uint8_t i_grafic;

void AdcCalcData(AdcSummaryData* data, uint32_t* inV_I, uint16_t count)
{
	uint16_t nsamples = DacSamplesPerPeriod();
	uint16_t nsamples4 = nsamples>>2;

	float sin_v = 0;
	float cos_v = 0;
	float sin_i = 0;
	float cos_i = 0;

	data->ch_v.k_sin = 1;
	data->ch_v.k_cos = 2;
	data->ch_i.k_sin = 3;
	data->ch_i.k_cos = 4;

	uint32_t mid_sum_v = 0;
	uint32_t mid_sum_i = 0;

	uint8_t count_grafic=0;
	i_grafic=0;
	
	for(uint16_t i=0; i<count; i++)
	{
		{
			uint16_t cV = inV_I[i]>>16;
			if(cV < data->ch_v.adc_min)
				data->ch_v.adc_min = cV;
			if(cV > data->ch_v.adc_max)
				data->ch_v.adc_max = cV;

			mid_sum_v += cV;
		}
		{
			uint16_t cI = inV_I[i];
			if(cI < data->ch_i.adc_min)
				data->ch_i.adc_min = cI;
			if(cI > data->ch_i.adc_max)
				data->ch_i.adc_max = cI;

			mid_sum_i += cI;
		}		
		
		if (DacPeriod()>7200){
		if (count_grafic<(count/100))count_grafic++;
		else {
			count_grafic=0;
			if (i_grafic<100){
			  grafic_u[i_grafic]=(inV_I[i]>>20);
			  grafic_i[i_grafic]=(inV_I[i]>>4);
			  i_grafic++;
			}
		}
		}
		else if (DacPeriod()==7200){
		if (count_grafic<2)count_grafic++;
		else {
			count_grafic=0;
			if (i_grafic<100){
			  grafic_u[i_grafic]=(inV_I[i]>>20);
			  grafic_i[i_grafic]=(inV_I[i]>>4);
			  i_grafic++;
			}
		}
		}				
		else {
			if (i_grafic<100){
			  grafic_u[i_grafic]=(inV_I[i]>>20);
			  grafic_i[i_grafic]=(inV_I[i]>>4);
			  i_grafic++;		
			}				
		}
		
		
	}
	float mid_v = mid_sum_v/(float)count;
	float mid_i = mid_sum_i/(float)count;
	data->ch_v.adc_mid = mid_v;
	data->ch_i.adc_mid = mid_i;

	for(uint16_t i=0; i<count; i++)
	{
		float sin_table = g_sinusBufferFloat[i%nsamples];
		float cos_table = g_sinusBufferFloat[(i+nsamples4)%nsamples];
		{
			float cV = (inV_I[i]>>16)-mid_v;
			sin_v += cV * sin_table;
			cos_v += cV * cos_table;
		}
		{
			float cI = (inV_I[i]&0xFFFF)-mid_i;
			sin_i += cI * sin_table;
			cos_i += cI * cos_table;
		}
	}
	data->ch_v.k_sin = sin_v*2.0f/(float)count;
	data->ch_v.k_cos = cos_v*2.0f/(float)count;
	data->ch_i.k_sin = sin_i*2.0f/(float)count;
	data->ch_i.k_cos = cos_i*2.0f/(float)count;

	//CalcSquareError(&data->ch_v, inV_I, 1, count);
	//CalcSquareError(&data->ch_i, inV_I, 0, count);
}
