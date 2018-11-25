

#include "main.h"
#include "dac.h"
#include "adc.h"
#include "systick.h"
#include "mcp6s21.h"
#include "lcd.h"
#include "corrector.h"
#include "quadrature_encoder.h"
#include "lcd_interface.h"
#include "vbat.h"
#include <ssd1306.h>
#include "eeprom.h"	

#include "fonts.h"	
#include "menu.h"	

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "parser.h"

extern __IO uint8_t Receive_Buffer[64];
extern __IO  uint32_t Receive_length ;
extern __IO  uint32_t length ;
uint8_t Send_Buffer[64];
uint32_t packet_sent=1;
uint32_t packet_receive=1;
uint8_t get_buff;
extern  uint32_t g_resultBuffer[RESULT_BUFFER_SIZE];
static uint8_t  *g_resultBuffer_uint8_t=(uint8_t*) &g_resultBuffer[0];

uint16_t VirtAddVarTab[NB_OF_VAR] = 
{
	0x555, 0x556, 0x557, 0x558, 0x559, 0x560, 0x561, 0x562, 0x563, 0x564,
	0x565, 0x566, 0x567, 0x568,	0x569, 0x570, 0x571, 0x572, 0x573, 0x574,

};
//#define NDEBUG
extern FontDef_t Font_7x10;
extern FontDef_t Font_11x18;
extern FontDef_t Font_16x26;

void LcdHello()
{
  LcdClear();

  LcdUpdate();
	
}

//------------------------------------------------------------------------------
void overClock(uint32_t PLLMULL){
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE;  // set HSE as system clock
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != 4) {/* Ждем*/}
	RCC->CR &= ~(uint32_t)RCC_CR_PLLON; // выключаем PLL
	while((RCC->CR & RCC_CR_PLLRDY) != 0) {/* Ждем*/} // Ожидаем, пока PLL остановится
	RCC->CFGR &= ~(RCC_CFGR_PLLMULL);
	RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC | PLLMULL);
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == 0) {/* Ждем*/} // Ожидаем, пока PLL выставит бит готовности
	/* Выбираем PLL как источник системной частоты */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
	/* Ожидаем, пока PLL выберется как источник системной частоты */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08) {/* Ждем*/}
}
//------------------------------------------------------------------------------
void SetSysClockTo56(void)
{ 
	overClock(RCC_CFGR_PLLMULL7);	
}
//------------------------------------------------------------------------------
void SetSysClockTo72(void)
{
	overClock(RCC_CFGR_PLLMULL9);
}
//------------------------------------------------------------------------------
void SetSysClockTo120(void)
{
	overClock(RCC_CFGR_PLLMULL15);
}
//------------------------------------------------------------------------------
extern uint16_t MENU_F_187_5KHz_on_off;
extern uint8_t filtr_on;
#define KOL_KLETOK_T 17
#define SIZE_BUF_SETTINGS 64
extern uint32_t dac_adc_freq;
	
unsigned char buff[SIZE_BUF_SETTINGS]={"osc v3"};
uint16_t  *buff_16bit=(uint16_t*) &buff[0];		

int main(void)
{ 
	#ifdef NDEBUG
	#warning "FLASH Read OUT Protection ON. DEBUG is OFF."
		if (FLASH_GetReadOutProtectionStatus() == RESET)
		{
			FLASH_Unlock();                  
			FLASH_ReadOutProtection(ENABLE); 
			FLASH_Lock();                    
		}
	#endif	
	
  Set_System();
	delay_init();
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_CRC, ENABLE);
		
	Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();		
		
  delay_ms(100);			
	SSD1306_Init(128, 64); 	

	/* Unlock the Flash Program Erase controller */
  FLASH_Unlock();	
  /* EEPROM Init */
  EE_Init();
  FLASH_Lock();	
		
	uint16_t temp, temp2;
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_lcd_rotate], &temp);	
	SSD1306_Rotate(temp,0);	
		
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_lcd_invers], &temp);
	SSD1306_Set_invers(temp,0);
	
	
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_lcd_type], &temp);				
	if (temp>0) SSD1306_SetType(0,0);	
  else 	SSD1306_SetType(1,0);
	
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_lcd_brightness], &temp);				
	if (temp>0) SSD1306_SetBrightness(temp,0);	
  else 	SSD1306_SetBrightness(255,0);
	LcdHello();
				
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_timer_off], &temp);	
	if ((temp!=0)&&(temp!=2*60)&&(temp!=5*60)&&(temp!=10*60))temp=0;
	Set_Timer_off(temp,0);
	
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_dac_period_h], &temp);	
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_dac_period_l], &temp2);		
	uint32_t dac_period;
  dac_period = (temp<<16)+temp2;
	if ((dac_period!=720000)&&(dac_period!=72000)&&(dac_period!=7200)&&(dac_period!=dac_period_100k)&&(dac_period!=dac_period_200k)) dac_period=72000;
		
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_dac_div], &temp);		
	if ((temp!=1)&&(temp!=3)&&(temp!=9))temp=1;
	DacSetDelitel(temp, 0);	
	
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_ser_par], &temp);	
	MenuSetSerial(temp, 0);		
	
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_187_5KHz_on_off], &temp);		
	if (temp==1) MENU_F_187_5KHz_on_off=1;
	
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_filtr_on], &temp);		
	filtr_on=temp;		
	if (filtr_on>4)filtr_on=0;
	
	extern uint16_t filter_window;
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_filtr_window_on], &temp);		
	filter_window=temp;		
	if (filter_window>2)filter_window=0;
	
	extern uint16_t filter_lcd;
	EE_ReadVariable(VirtAddVarTab[adres_eeprom_filtr_lcd_on], &temp);		
	filter_lcd=temp;		
	if (filter_lcd>1)filter_lcd=1;	
	
	delay_ms(2000);			
	
  VBatInit();
  DacInit();
  AdcInit();
  MCPInit();
  QuadEncInit();
		
  ClearCorrector();	
	
	bContinuousMode = true;
	AdcDacStartSynchro(dac_period, DEFAULT_DAC_AMPLITUDE);
	ProcessStartComputeX(0/*count*/, 
			255/*predefinedResistorIdx*/,
			255/*predefinedGainVoltageIdx*/,
			255/*uint8_t predefinedGainCurrentIdx*/,
			true/*useCorrector*/
		);
			
	LcdRepaint();
	
	//SSD1306_Sleep();
			
  while (1)
  {
    AdcQuant();
		
		if (get_buff){	
			if (get_buff==1)get_buff=0;	
			else {
				send_settings_to_pc();
				while (packet_sent == 0);
					 CDC_Send_DATA ((unsigned char*)&buff[0], 16);
			}
			
			for( uint16_t i=0; i<RESULT_BUFFER_SIZE*4; i+=VIRTUAL_COM_PORT_DATA_SIZE-1){
				while (packet_sent == 0);
					if (i< RESULT_BUFFER_SIZE*4-(VIRTUAL_COM_PORT_DATA_SIZE-1)) CDC_Send_DATA ((unsigned char*)&g_resultBuffer_uint8_t[i], VIRTUAL_COM_PORT_DATA_SIZE-1);
				  else CDC_Send_DATA ((unsigned char*)&g_resultBuffer_uint8_t[i], ((RESULT_BUFFER_SIZE*4)-i));
			}
			
		}
		
		if (bDeviceState == CONFIGURED)
    {
      CDC_Receive_DATA();
      /*Check to see if we have data yet */
      if (Receive_length  != 0)
      {
        for (uint16_t i=0; i<Receive_length; i++){
		      PARS_Parser(Receive_Buffer[i]);	
					//if (packet_sent == 1)CDC_Send_DATA ((unsigned char*)&Receive_Buffer[i],1);
	      } 
          
        Receive_length = 0;
      }
    }
  }
}

extern bool useCorrector; // используется или нет корректор
extern uint16_t computeXCount; // вспомогательные переменные усреднения 
extern volatile uint8_t predefinedResistorIdx; // предустановленное значение резистора, только тут их вписывать смысла нет
extern uint8_t predefinedGainVoltageIdx; //напряжения
extern uint8_t predefinedGainCurrentIdx; //тока
/*----------------------------------------------------------------------------
 *      обработчик PARS_Handler
 *---------------------------------------------------------------------------*/
void PARS_Handler(uint8_t argc, char *argv[])
{
	uint16_t value = 0;

	//=========================================		
	if (PARS_EqualStr(argv[0], "get_buff")){
		if (argc > 1){
			get_buff = PARS_StrToUint(argv[1]);
		}			
	}	
	//=========================================		
	if (PARS_EqualStr(argv[0], "setR")){
		if (argc > 1){			
			predefinedResistorIdx = PARS_StrToUint(argv[1]);
		}			
	}	
	//=========================================		
	if (PARS_EqualStr(argv[0], "setU")){
		if (argc > 1){			
			predefinedGainVoltageIdx = PARS_StrToUint(argv[1]);
		}			
	}	
	//=========================================			
	if (PARS_EqualStr(argv[0], "setI")){
		if (argc > 1){			
			predefinedGainCurrentIdx = PARS_StrToUint(argv[1]);
		}			
	}	
	//=========================================			
	if (PARS_EqualStr(argv[0], "setCount")){
		if (argc > 1){			
			computeXCount = PARS_StrToUint(argv[1]);
		}			
	}	
	//=========================================				
	if (PARS_EqualStr(argv[0], "setRUI")){
		if (argc > 3){			
			predefinedResistorIdx = PARS_StrToUint(argv[1]);
			predefinedGainVoltageIdx = PARS_StrToUint(argv[2]);
			predefinedGainCurrentIdx = PARS_StrToUint(argv[3]);
		}			
	}	
	//=========================================			
	if (PARS_EqualStr(argv[0], "setDACperiod")){
		if (argc > 1){			
			uint32_t dac_period = PARS_StrToUint32(argv[1]);
      AdcDacStartSynchro(dac_period, DEFAULT_DAC_AMPLITUDE);
		}			
	}	
	//=========================================				
	//=========================================			
	if (PARS_EqualStr(argv[0], "setLCD")){
		if (argc > 1){			
			uint32_t on = PARS_StrToUint32(argv[1]);
      if (on==1)SSD1306_Wake();
				else SSD1306_Sleep();	
		}			
	}	
	//=========================================				
	
}	

extern double error_Re;
extern double error_Rim;	
extern volatile uint8_t resistorIdx; 
extern volatile uint8_t gainVoltageIdx;
extern volatile uint8_t gainCurrentIdx;

void send_settings_to_pc(void){	 	 		    
			buff_16bit[pc_adres_ADC_freqH]= ((uint16_t)(dac_adc_freq>>16));
			buff_16bit[pc_adres_ADC_freqL]= ((uint16_t)(dac_adc_freq&0xFFFF));
	
			buff_16bit[pc_adres_error_Re]= ((uint16_t)(round(error_Re*100)));
			buff_16bit[pc_adres_RUI]= (uint16_t)(((resistorIdx&0x0F)<<12)|((gainVoltageIdx&0x0F)<<8)|((gainCurrentIdx&0x0F)<<4));
	
	//=============================================================================
 }
	

#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
