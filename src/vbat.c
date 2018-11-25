

#include "hw_config.h"
#include "systick.h"
#include "lcd.h"
#include "dac.h"

void VBatInit()
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	/* ADC Channel configuration */
	RCC_ADCCLKConfig(RCC_ADC34PLLCLK_Div2);  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC34, ENABLE);

	/* GPIOC Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	/* Configure ADC Channel11 as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	ADC_StructInit(&ADC_InitStructure);

	/* Calibration procedure */  
	ADC_VoltageRegulatorCmd(ADC4, ENABLE);

	/* Insert delay equal to 10 µs */
	delay_ms(1);//delay_us(10);

	ADC_SelectCalibrationMode(ADC4, ADC_CalibrationMode_Single);
	ADC_StartCalibration(ADC4);

	while(ADC_GetCalibrationStatus(ADC4) != RESET );
	//calibration_value = ADC_GetCalibrationValue(ADC4);

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                                                                    
	ADC_CommonInitStructure.ADC_Clock = ADC_Clock_AsynClkMode;                    
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;             
	ADC_CommonInitStructure.ADC_DMAMode = ADC_DMAMode_OneShot;                  
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
	ADC_CommonInit(ADC4, &ADC_CommonInitStructure);

	ADC_InitStructure.ADC_ContinuousConvMode = ADC_ContinuousConvMode_Disable;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; 
	ADC_InitStructure.ADC_ExternalTrigConvEvent = ADC_ExternalTrigConvEvent_0;         
	ADC_InitStructure.ADC_ExternalTrigEventEdge = ADC_ExternalTrigEventEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_OverrunMode = ADC_OverrunMode_Disable;   
	ADC_InitStructure.ADC_AutoInjMode = ADC_AutoInjec_Disable;  
	ADC_InitStructure.ADC_NbrOfRegChannel = 1;
	ADC_Init(ADC4, &ADC_InitStructure);

	/* ADC4 regular channel11 configuration */ 
	ADC_RegularChannelConfig(ADC4, ADC_Channel_4, 1, ADC_SampleTime_601Cycles5);//ADC_SampleTime_19Cycles5

	/* Enable ADC4 */
	ADC_Cmd(ADC4, ENABLE);

	/* wait for ADRDY */
	while(!ADC_GetFlagStatus(ADC4, ADC_FLAG_RDY));

	ADC_StartConversion(ADC4);
}

void VBatQuant()
{
	/* Start ADC4 Software Conversion */ 
   static uint8_t countAKB_low;
	
	/* Test EOC flag */
	//while(ADC_GetFlagStatus(ADC4, ADC_FLAG_EOC) == RESET);

	/* Get ADC4 converted data */
	int adcValue = ADC_GetConversionValue(ADC4);

	//Не задерживаем вывод на экран. Батарейка все равно медленно разряжается, поэтому показываем старое значение
	ADC_StartConversion(ADC4);

	const int Vup = (4096*4.2/ADC_STM32_Ref)/2;   //4.2 V
	const int Vdown = (4096*3.3/ADC_STM32_Ref)/2;  //3.3 V
	int value = 0;

	if (adcValue<Vdown) countAKB_low++;
	else countAKB_low=0;
	
	if (countAKB_low>100) Power_off(1);
		
	//Vup->6.5 Vdown->0
	value = adcValue-Vdown;
	if(value<0)
		value = 0;

	value = (value*65)/(10*(Vup-Vdown));

	//Draw battery
	byte x0 = 128-21, y0 = 0;
	LcdLine(x0, x0, y0+1, y0+7, PIXEL_ON );
	LcdLine(x0+1, x0+3, y0+1, y0+1, PIXEL_ON );
	LcdLine(x0+1, x0+3, y0+7, y0+7, PIXEL_ON );

	LcdLine(x0+3, x0+3, y0+1, y0+0, PIXEL_ON );
	LcdLine(x0+3, x0+3, y0+7, y0+8, PIXEL_ON );

	LcdLine(x0+3, x0+19, y0+0, y0+0, PIXEL_ON );
	LcdLine(x0+3, x0+19, y0+8, y0+8, PIXEL_ON );
	LcdLine(x0+20, x0+20, y0+0, y0+8, PIXEL_ON );

	if(value>5)
		LcdSingleBar( x0+2, y0+3+3, 3, 2, PIXEL_ON );

	for(byte i=0; i<5; i++)
	{
		if(value>4-i)
			LcdSingleBar( x0+5+i*3, y0+3+4, 5, 2, PIXEL_ON );
	}
	
 }
