#include "stm32f30x.h"
#include <stdio.h>
#include "hw_config.h"
#include "platform_config.h"
#include "math.h"
#include "stm32f30x_it.h"


void USB_Config (void);
void PARS_Handler(uint8_t argc, char *argv[]);
void send_settings_to_pc(void);
void refresh_settings_to_pc(void);
			 
#define pc_adres_ADC_freqH 6
#define pc_adres_ADC_freqL 7
#define pc_adres_error_Re 4
#define pc_adres_error_Rim 5
#define pc_adres_RUI 3


//116 Kb to program
//Должно быть кратно 2 Kb = 0x800
//#define FLASH_PROGRAM_MAX_SIZE 0x1D000
#define FLASH_PROGRAM_MAX_SIZE (0x3F000-(2048*6)) //240 Kb
#define FLASH_START_ARRAY  (FLASH_BASE+FLASH_PROGRAM_MAX_SIZE)
//0x0803F000

#define adres_eeprom_lcd_rotate 1
#define adres_eeprom_lcd_invers 2
#define adres_eeprom_lcd_brightness 3
#define adres_eeprom_dac_period_h 4
#define adres_eeprom_dac_period_l 5
#define adres_eeprom_timer_off 6
#define adres_eeprom_dac_div 7
#define adres_eeprom_ser_par 8
#define adres_eeprom_lcd_type 9
#define adres_eeprom_187_5KHz_on_off 10
#define adres_eeprom_filtr_on 11
#define adres_eeprom_filtr_window_on 12
#define adres_eeprom_filtr_lcd_on 13
