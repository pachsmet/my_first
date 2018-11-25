

#include "hw_config.h"
#include "quadrature_encoder.h"
#include "lcd_interface.h"
#include "menu.h"
#include "dac.h"
#include "adc.h"
#include "process_measure.h"
#include "number_edit.h"
#include "corrector.h"
#include "systick.h"
#include <ssd1306.h>

#include <main.h>
#include "eeprom.h"	
extern uint16_t VirtAddVarTab[NB_OF_VAR];
uint8_t filtr_on=1;
#define SIZEOF(x) (sizeof(x)/sizeof(x[0]))

uint16_t MENU_F_187_5KHz_on_off;
uint16_t filter_window;
uint16_t filter_lcd;
uint8_t dac_on=1;
uint8_t pos_dop_menu;

typedef enum MenuEnum {
	MENU_NONE = 0, //Несуществующий пункт меню
	MENU_MAIN_RETURN, //Выход из main menu
	MENU_MAIN_FREQUENCY,
	MENU_MAIN_DAC,	
	MENU_DAC_1,		
	MENU_DAC_2,		
	//MENU_DAC_3,			
	MENU_MAIN_SER_PAR,
	MENU_MAIN_VIEW_PARAM, //Какой из параметров отображается.
	MENU_LCD_TOGGLE_INVERS, //Подсветка дисплея
	MENU_LCD_SET_LIGHT, //Подсветка дисплея	
	MENU_LCD_SET_LIGHT_5,	
	MENU_LCD_SET_LIGHT_25,
	MENU_LCD_SET_LIGHT_50,	
	MENU_LCD_SET_LIGHT_100,	
	MENU_LCD_TOGGLE_ROTATE, //Поворот дисплея	
	MENU_MAIN_SETTINGS,
	MENU_MAIN_LCD,
	MENU_LCD_TYPE,
	MENU_LCD_SSD1306,
	MENU_LCD_SH1106,	
	MENU_LCD_AUTO_OFF,
	MENU_LCD_AUTO_OFF_2,
	MENU_LCD_AUTO_OFF_5,
	MENU_LCD_AUTO_OFF_10,
	MENU_LCD_AUTO_OFF_0,	
	MENU_MAIN_CORRECTION,
	MENU_RETURN, //Возврат в main menu
	MENU_F_100Hz,
	MENU_F_1KHz,
	MENU_F_10KHz,
	MENU_F_93_75KHz,
	MENU_F_187_5KHz,
	MENU_F_187_5KHz_on,	
	MENU_F_187_5KHz_on_1,	
	MENU_F_187_5KHz_on_0,		
	MENU_SP_SERIAL,
	MENU_SP_PARALLEL,
	MENU_V_RIM,
	MENU_V_LC,
	MENU_CORRECTION_SHORT,
	MENU_CORRECTION_1_Om,
	MENU_CORRECTION_100_Om,
	MENU_CORRECTION_1_KOm,
	MENU_CORRECTION_10_KOm,
	MENU_CORRECTION_100_KOm,
	MENU_CORRECTION_OPEN,
	MENU_CORRECTION_SAVE,
	MENU_CORRECTION_CLEAR,
	MENU_ERRORS,
	MENU_OSC,	
	MENU_SET_R,
	MENU_SET_R_AUTO,
	MENU_SET_R0,
	MENU_SET_R1,
	MENU_SET_R2,
	MENU_SET_R3,
	
	MENU_SET_U,
	MENU_SET_U_AUTO,
	MENU_SET_U0,
	MENU_SET_U1,
	MENU_SET_U2,
	MENU_SET_U3,	
	MENU_SET_U4,
	MENU_SET_U5,
	MENU_SET_U6,
	MENU_SET_U7,		
	
	MENU_SET_I,
	MENU_SET_I_AUTO,
	MENU_SET_I0,
	MENU_SET_I1,
	MENU_SET_I2,
	MENU_SET_I3,	
	MENU_SET_I4,
	MENU_SET_I5,
	MENU_SET_I6,
	MENU_SET_I7,		
	
	
	MENU_FILTR,	
	MENU_FILTR_OFF,	
	MENU_FILTR_ON,	
	MENU_FILTR_ON_1,		
	MENU_FILTR_ON_5,		
	MENU_FILTR_ON_10,	
	MENU_FILTR_WINDOW,
	MENU_FILTR_WINDOW_OFF,	
	MENU_FILTR_WINDOW_HAN,
	MENU_FILTR_WINDOW_BLACKMAN,	
	MENU_FILTR_LCD,
	MENU_FILTR_LCD_OFF,	
	MENU_FILTR_LCD_ON,
	MENU_DAC,
	MENU_DAC_OFF,	
	MENU_DAC_ON,	
	MENU_SETTINGS_DOP,
	MENU_MAIN_RETURN_SETTINGS_DOP,
	MENU_MAIN_RETURN_SETTINGS,	
} MenuEnum;

typedef struct MenuElem {
	char* text;
	MenuEnum command;
} MenuElem;

static MenuElem g_main_menu[]={
	{"..", MENU_MAIN_RETURN},
	//{"Частота сигнала", MENU_MAIN_FREQUENCY},
	//{"Амплитуда сигн.", MENU_MAIN_DAC},	
	//{"Схема замещения", MENU_MAIN_SER_PAR},
	//{"L/C или R.imag", MENU_MAIN_VIEW_PARAM},
	{"Настройки", MENU_MAIN_SETTINGS},		
	//{"Настройки экрана", MENU_MAIN_LCD},		
	{"Калибровка", MENU_MAIN_CORRECTION},
	//{"Задать R", MENU_SET_R},
	//{"Ошибка измерения", MENU_ERRORS},
	//{"Графики U, I", MENU_OSC},
	//{"Фильтр вкл/выкл", MENU_FILTR},	
};

static MenuElem g_settings[]={
	{"..", MENU_RETURN},
	//{"Частота сигнала", MENU_MAIN_FREQUENCY},
	//{"Амплитуда сигн.", MENU_MAIN_DAC},	
	{"Схема замещения", MENU_MAIN_SER_PAR},
	{"L/C или R.imag", MENU_MAIN_VIEW_PARAM},	
	{"Настройки экрана", MENU_MAIN_LCD},	
	{"Доп. настройки ", MENU_SETTINGS_DOP},		
};

static MenuElem g_settings_dop[]={
	{"..", MENU_MAIN_RETURN_SETTINGS},
	{"Задать R", MENU_SET_R},
	{"Задать U", MENU_SET_U},	
	{"Задать I", MENU_SET_I},		
	{"RC фильтр", MENU_FILTR},
	{"Оконный Фильтр", MENU_FILTR_WINDOW},	
	{"Усредн. Фильтр ", MENU_FILTR_LCD},		
	{"187кHz вкл/выкл", MENU_F_187_5KHz_on},	
	{"DAC вкл/выкл", MENU_DAC},			
};

static MenuElem g_lcd_dac[]={
	{"..", MENU_RETURN},
	{"Амплитуда 1.00 В", MENU_DAC_1},
	{"Амплитуда 0.33 В", MENU_DAC_2},	
	//{"Амплитуда 0.1 В", MENU_DAC_3},	
};

static MenuElem g_filtr[]={
	{"..", MENU_MAIN_RETURN_SETTINGS_DOP},
	{"Выкл", MENU_FILTR_OFF},
	{"Вкл.", MENU_FILTR_ON},	
	//{"Вкл. на 1%", MENU_FILTR_ON_1},		
	//{"Вкл. на 5%", MENU_FILTR_ON_5},	
	//{"Вкл. на 10%", MENU_FILTR_ON_10},		
	//{"Амплитуда 0.1 В", MENU_DAC_3},	
};
static MenuElem g_filtr_window[]={
	{"..", MENU_MAIN_RETURN_SETTINGS_DOP},
	{"Выкл", MENU_FILTR_WINDOW_OFF},
	{"Хан", MENU_FILTR_WINDOW_HAN},	
	{"Блекман", MENU_FILTR_WINDOW_BLACKMAN},		
};
static MenuElem g_filtr_lcd[]={
	{"..", MENU_MAIN_RETURN_SETTINGS_DOP},
	{"Выкл", MENU_FILTR_LCD_OFF},
	{"Вкл", MENU_FILTR_LCD_ON},	
};
static MenuElem g_dac[]={
	{"..", MENU_MAIN_RETURN_SETTINGS_DOP},
	{"Выкл", MENU_DAC_OFF},
	{"Вкл", MENU_DAC_ON},	
};
static MenuElem g_lcd_menu[]={
	{"..", MENU_MAIN_RETURN_SETTINGS},
	{"Перевернуть", MENU_LCD_TOGGLE_ROTATE},
	{"Инвертировать", MENU_LCD_TOGGLE_INVERS},
	{"Яркость", MENU_LCD_SET_LIGHT},	
	{"Автовыключение", MENU_LCD_AUTO_OFF},	
	{"Тип", MENU_LCD_TYPE},	
};

static MenuElem g_lcd_timer[]={
	{"..", MENU_RETURN},
	{"Таймер 2 мин", MENU_LCD_AUTO_OFF_2},
	{"Таймер 5 мин", MENU_LCD_AUTO_OFF_5},
	{"Таймер 10 мин", MENU_LCD_AUTO_OFF_10},	
	{"Таймер выкл.", MENU_LCD_AUTO_OFF_0},		
};

static MenuElem g_lcd_light[]={
	{"..", MENU_RETURN},
	{"Яркость 5 %", MENU_LCD_SET_LIGHT_5},	
	{"Яркость 25 %", MENU_LCD_SET_LIGHT_25},
	{"Яркость 50 %", MENU_LCD_SET_LIGHT_50},
	{"Яркость 100 %", MENU_LCD_SET_LIGHT_100},	
};

static MenuElem g_lcd_type[]={
	{"..", MENU_RETURN},
	{"SSD1306", MENU_LCD_SSD1306},	
	{"SH1106", MENU_LCD_SH1106},
};

static MenuElem g_f_menu[]={
	{"..", MENU_RETURN},
	{"100 Hz", MENU_F_100Hz},
	{"1 kHz", MENU_F_1KHz},
	{"10 kHz", MENU_F_10KHz},
	{"93.75 kHz", MENU_F_93_75KHz},
	{"187.5 kHz", MENU_F_187_5KHz},
};

static MenuElem g_sp_menu[]={
	{"..", MENU_MAIN_RETURN_SETTINGS},
	{"Последовательно", MENU_SP_SERIAL},
	{"Параллельно", MENU_SP_PARALLEL},
};

static MenuElem g_f187_menu[]={
	{"..", MENU_MAIN_RETURN_SETTINGS_DOP},
	{"Вкл.  187.5 кГц", MENU_F_187_5KHz_on_1},
	{"Выкл. 187.5 кГц", MENU_F_187_5KHz_on_0},
};


static MenuElem g_v_menu[]={
	{"..", MENU_MAIN_RETURN_SETTINGS},
	{"Показать R.imag", MENU_V_RIM},
	{"Показать L/C", MENU_V_LC},
};

static MenuElem g_correction_menu[]={
	{"..", MENU_RETURN},
	{"Разомкнутый", MENU_CORRECTION_OPEN},	
	{"Замкнутый", MENU_CORRECTION_SHORT},
	{"1 Om", MENU_CORRECTION_1_Om},
	{"100 Om", MENU_CORRECTION_100_Om},
	{"1 kOm", MENU_CORRECTION_1_KOm},
	{"10 kOm", MENU_CORRECTION_10_KOm},
	{"100 kOm", MENU_CORRECTION_100_KOm},
	{"Сохранить", MENU_CORRECTION_SAVE},
	{"Сбросить", MENU_CORRECTION_CLEAR},
};

static MenuElem g_set_r_menu[]={
	{"..", MENU_MAIN_RETURN_SETTINGS_DOP},
	{"Авто", MENU_SET_R_AUTO},
	{"R0 100 Om", MENU_SET_R0},
	{"R1 1 kOm", MENU_SET_R1},
	{"R2 10 kOm", MENU_SET_R2},
	{"R3 100 kOm", MENU_SET_R3},
};

static MenuElem g_set_u_menu[]={
	{"..", MENU_MAIN_RETURN_SETTINGS_DOP},
	{"Авто", MENU_SET_U_AUTO},
	{"U0 ", MENU_SET_U0},
	{"U1 ", MENU_SET_U1},
	{"U2 ", MENU_SET_U2},
	{"U3 ", MENU_SET_U3},
	{"U4 ", MENU_SET_U4},
	{"U5 ", MENU_SET_U5},
	{"U6 ", MENU_SET_U6},
	{"U7 ", MENU_SET_U7},	

};
static MenuElem g_set_i_menu[]={
	{"..", MENU_MAIN_RETURN_SETTINGS_DOP},
	{"Авто", MENU_SET_I_AUTO},
	{"I0 ", MENU_SET_I0},
	{"I1 ", MENU_SET_I1},
	{"I2 ", MENU_SET_I2},
	{"I3 ", MENU_SET_I3},
	{"I4 ", MENU_SET_I4},
	{"I5 ", MENU_SET_I5},
	{"I6 ", MENU_SET_I6},
	{"I7 ", MENU_SET_I7},	

};



static MenuElem* g_cur_menu = NULL;
static uint8_t g_menu_size = 0;
static uint8_t g_menu_pos = 0;
static volatile bool g_update = false;
static char* message_line1 = NULL;
static char* message_line2 = NULL;

static MenuEnum g_last_main_command = MENU_MAIN_FREQUENCY;
static MenuEnum g_last_f_command = MENU_F_100Hz;
static MenuEnum g_last_correction_command = MENU_NONE;
extern bool type_screen ;

#define MENU_START(menu) \
	g_cur_menu = menu; \
	g_menu_size = SIZEOF(menu); \
	g_menu_pos = 0;

#define MENU_CLEAR() \
	g_cur_menu = NULL; \
	g_menu_size = 0;


bool is_menu( void ){
	if (g_cur_menu!=NULL) return true;
	else return false;
}
void menu_clear( void ){
	MENU_CLEAR() 
}

void MenuSetF(uint32_t period);
void MenuSetSerial(bool ser, uint8_t save);
void MenuSetPos(MenuEnum pos);
void MenuSetPrinRim(bool pr);
void ToggleInvers(void);
void ToggleRotate(void);
void MenuOnCommand(MenuEnum command);
void MenuClearFlash(void);
void MenuOnCorrection(MenuEnum command);
void OnNumberEditEnd(void);
void MenuSetR(uint8_t Ridx);
void MenuSetU(uint8_t Uidx);
void MenuSetI(uint8_t Iidx);
void DAC_Frq(int8_t updown);

void OnButtonPressed()
{
	Reset_Timer_off();
	g_update = true;
	
	if(NumberEditStarted())
	{
		NumberEditOnButtonPressed();
		return;
	}

	if(message_line1)
	{//MessageBox suport
		message_line1 = NULL;
		return;
	}

	if(g_cur_menu==NULL)
	{
		MENU_START(g_main_menu);
		MenuSetPos(g_last_main_command);
		return;
	}

	if(g_menu_pos>=g_menu_size)
		return;
	MenuEnum command = g_cur_menu[g_menu_pos].command;
	MenuOnCommand(command);
}

extern uint8_t usrednenie;
void OnWeel(int16_t delta)
{
	Reset_Timer_off();	
	usrednenie=0;
	if(NumberEditStarted())
	{
		NumberEditOnWeel(delta);
	  g_update = true;
		return;
	}

	if(message_line1)
	{//MessageBox suport
		
		return;
	}

	if(g_cur_menu==NULL){
		DAC_Frq(-delta);
		g_update = true;
		return;
	}
	g_menu_pos = (g_menu_pos+g_menu_size+delta)%g_menu_size;
  g_update = true;
}

void OnTimer()
{
	if(NumberEditCompleted())
	{
		NumberEditEnd();
		OnNumberEditEnd();
		g_update = true;
	}

	if(g_update)
	{
		g_update = false;	
		LcdRepaint();			
	}
}

void MenuOnCommand(MenuEnum command)
{
	switch(command)
	{
	case MENU_NONE:
		break;
	case MENU_MAIN_RETURN:
		MENU_CLEAR();
		break;
	
	case MENU_MAIN_RETURN_SETTINGS:	
     MENU_START(g_settings);
		break;	
	
	case MENU_MAIN_RETURN_SETTINGS_DOP:	
     MENU_START(g_settings_dop);
			 if (pos_dop_menu==0)MenuSetPos(MENU_SET_R);
		 else 	 if (pos_dop_menu==1)MenuSetPos(MENU_SET_U);	
	 	 else 	 if (pos_dop_menu==2)MenuSetPos(MENU_SET_I);	
		 else if (pos_dop_menu==3)MenuSetPos(MENU_FILTR);
		 else if (pos_dop_menu==4)MenuSetPos(MENU_FILTR_WINDOW);
		 else if (pos_dop_menu==5)MenuSetPos(MENU_FILTR_LCD);
		 else if (pos_dop_menu==6)MenuSetPos(MENU_F_187_5KHz_on);
		 else if (pos_dop_menu==7)MenuSetPos(MENU_DAC);
		break;		
	
	case MENU_MAIN_SETTINGS:
		MENU_START(g_settings);
		MenuSetPos(MENU_MAIN_SER_PAR);		
		break;	
	
	case MENU_MAIN_FREQUENCY:
		MENU_START(g_f_menu);
		MenuSetPos(g_last_f_command);
		g_last_main_command = command;
		break;
	case MENU_MAIN_SER_PAR:
		MENU_START(g_sp_menu);
		MenuSetPos(isSerial?MENU_SP_SERIAL:MENU_SP_PARALLEL);
		g_last_main_command = command;
		break;
	case MENU_MAIN_VIEW_PARAM:
		g_last_main_command = command;
		MENU_START(g_v_menu);
		MenuSetPos(printRim?MENU_V_RIM:MENU_V_LC);
		break;
	case MENU_MAIN_LCD:
		MENU_START(g_lcd_menu);
		MenuSetPos(MENU_LCD_TOGGLE_ROTATE);
		break;
	case MENU_LCD_TOGGLE_ROTATE:
		 ToggleRotate();
		break;		
	case MENU_LCD_TOGGLE_INVERS:
		 ToggleInvers();
		break;	
	
	case MENU_LCD_TYPE:
		MENU_START(g_lcd_type);
		if (SSD1306_GetType()==1) MenuSetPos(MENU_LCD_SSD1306);		
		else MenuSetPos(MENU_LCD_SH1106);	
	break;	
	case MENU_LCD_SSD1306:
    SSD1306_Init(128, 64); 
 		SSD1306_SetType(1, 1);				
	break;		
	case MENU_LCD_SH1106:
    SSD1306_Init(128, 64); 
		SSD1306_SetType(0, 1);				
	break;			
	
	case MENU_LCD_SET_LIGHT:
		MENU_START(g_lcd_light);
		if (SSD1306_GetBrightness()==10) MenuSetPos(MENU_LCD_SET_LIGHT_5);		
		else if (SSD1306_GetBrightness()==64) MenuSetPos(MENU_LCD_SET_LIGHT_25);	
		else if (SSD1306_GetBrightness()==128) MenuSetPos(MENU_LCD_SET_LIGHT_50);	
		else MenuSetPos(MENU_LCD_SET_LIGHT_100);		
		break;	
	case MENU_LCD_SET_LIGHT_5:
    	SSD1306_SetBrightness(10,  1);	
		break;		
	case MENU_LCD_SET_LIGHT_25:
    	SSD1306_SetBrightness(64,  1);	
		break;			
	case MENU_LCD_SET_LIGHT_50:
    	SSD1306_SetBrightness(128,  1);	
		break;			
	case MENU_LCD_SET_LIGHT_100:
    	SSD1306_SetBrightness(255,  1);	
		break;		
	 
	  case MENU_SETTINGS_DOP:		
     MENU_START(g_settings_dop);
			 if (pos_dop_menu==0)MenuSetPos(MENU_SET_R);
		 else 	 if (pos_dop_menu==1)MenuSetPos(MENU_SET_U);	
	 	 else 	 if (pos_dop_menu==2)MenuSetPos(MENU_SET_I);	
		 else if (pos_dop_menu==3)MenuSetPos(MENU_FILTR);
		 else if (pos_dop_menu==4)MenuSetPos(MENU_FILTR_WINDOW);
		 else if (pos_dop_menu==5)MenuSetPos(MENU_FILTR_LCD);
		 else if (pos_dop_menu==6)MenuSetPos(MENU_F_187_5KHz_on);
		 else if (pos_dop_menu==7)MenuSetPos(MENU_DAC);
		
		break;	

	 case MENU_FILTR:
		 pos_dop_menu=3;
		MENU_START(g_filtr);
		if (filtr_on==0) MenuSetPos(MENU_FILTR_OFF);		
		else if (filtr_on==1) MenuSetPos(MENU_FILTR_ON);	
		else if (filtr_on==2) MenuSetPos(MENU_FILTR_ON_1);
		else if (filtr_on==3) MenuSetPos(MENU_FILTR_ON_5);
		else if (filtr_on==4) MenuSetPos(MENU_FILTR_ON_10);	  
    	//filtr_on=!filtr_on;
	    //SetLowPassFilter(DacPeriod(), filtr_on); // задается фильтр в зависимости от частоты
	    //MENU_CLEAR();
		break;		  
	 case MENU_FILTR_OFF:
    	filtr_on=0;
	    SetLowPassFilter(DacPeriod(), filtr_on); // задается фильтр в зависимости от частоты
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_on], (uint16_t)filtr_on );
      FLASH_Lock();	
	    MENU_CLEAR();
		break;		 
	 case MENU_FILTR_ON:
    	filtr_on=1;
	    SetLowPassFilter(DacPeriod(), filtr_on); // задается фильтр в зависимости от частоты
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_on], (uint16_t)filtr_on );
      FLASH_Lock();	
	    MENU_CLEAR();
		break;		 
	 case MENU_FILTR_ON_1:
    	filtr_on=2;
	    SetLowPassFilter(DacPeriod(), filtr_on); // задается фильтр в зависимости от частоты
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_on], (uint16_t)filtr_on );
      FLASH_Lock();	
	    MENU_CLEAR();
		break;		 	 
	 case MENU_FILTR_ON_5:
    	filtr_on=3;
	    SetLowPassFilter(DacPeriod(), filtr_on); // задается фильтр в зависимости от частоты
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_on], (uint16_t)filtr_on );
      FLASH_Lock();	
	    MENU_CLEAR();
		break;			 
	 case MENU_FILTR_ON_10:
    	filtr_on=4;
	    SetLowPassFilter(DacPeriod(), filtr_on); // задается фильтр в зависимости от частоты
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_on], (uint16_t)filtr_on );
      FLASH_Lock();	
	    MENU_CLEAR();
		break;			 
	 
	 case MENU_FILTR_WINDOW:
		 pos_dop_menu=4;
		MENU_START(g_filtr_window);
		if (filter_window==0) MenuSetPos(MENU_FILTR_WINDOW_OFF);		
		else if (filter_window==1) MenuSetPos(MENU_FILTR_WINDOW_HAN);	
		else if (filter_window==2) MenuSetPos(MENU_FILTR_WINDOW_BLACKMAN);
		break;		  
	 case MENU_FILTR_WINDOW_OFF:
    	filter_window=0;	
      DacSinusCalculate();
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_window_on], (uint16_t)filter_window );
      FLASH_Lock();	
	    MENU_CLEAR();	 
		break;	
	 case MENU_FILTR_WINDOW_HAN:
    	filter_window=1;	
      DacSinusCalculate();   
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_window_on], (uint16_t)filter_window );
      FLASH_Lock();	
	    MENU_CLEAR();
		break;	
	 case MENU_FILTR_WINDOW_BLACKMAN:
    	filter_window=2;	 
      DacSinusCalculate();
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_window_on], (uint16_t)filter_window );
      FLASH_Lock();	
	    MENU_CLEAR();
		break;	
	 
	 case MENU_FILTR_LCD:
		 pos_dop_menu=5;
		MENU_START(g_filtr_lcd);
		if (filter_lcd==0) MenuSetPos(MENU_FILTR_LCD_OFF);		
		else if (filter_lcd==1) MenuSetPos(MENU_FILTR_LCD_ON);	

		break;		  
	 case MENU_FILTR_LCD_OFF:
    	filter_lcd=0;	
      
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_lcd_on], (uint16_t)filter_lcd );
      FLASH_Lock();	
	    MENU_CLEAR();	 
		break;	
	 case MENU_FILTR_LCD_ON:
    	filter_lcd=1;	
        
	 		FLASH_Unlock();	
	    EE_WriteVariable(VirtAddVarTab[adres_eeprom_filtr_lcd_on], (uint16_t)filter_lcd );
      FLASH_Lock();	
	    MENU_CLEAR();
		break;		 
	 
	 case MENU_DAC:
		 pos_dop_menu=7;
		MENU_START(g_dac);
		if (dac_on==0) MenuSetPos(MENU_DAC_OFF);		
		else if (dac_on==1) MenuSetPos(MENU_DAC_ON);	

		break;		  
	 case MENU_DAC_OFF:
    	dac_on=0;	
      DacSinusCalculate();	
	    MENU_CLEAR();	 
		break;	
	 case MENU_DAC_ON:
    	dac_on=1;	
      DacSinusCalculate();  
	    MENU_CLEAR();
		break;		 
	case MENU_MAIN_DAC:
		MENU_START(g_lcd_dac);
		if (DacGetDelitel()==1) MenuSetPos(MENU_DAC_1);	
		else  MenuSetPos(MENU_DAC_2);		
		//else MenuSetPos(MENU_DAC_3);	
		break;	
	case MENU_DAC_1:
	    	DacSetDelitel(1,  1);
    		MENU_CLEAR();
		break;		
	case MENU_DAC_2:
		   DacSetDelitel(3,  1);
       MENU_CLEAR();
		break;			
//	case MENU_DAC_3:
//		   DacSetDelitel(9,  1);
//       MENU_CLEAR();
//		break;	
	case MENU_OSC:
		   if (type_screen!=0)type_screen=0;
	     else type_screen=1;
       MENU_CLEAR();
		break;		
	case MENU_LCD_AUTO_OFF:
		MENU_START(g_lcd_timer);
		if (Get_Timer_off()==2*60) MenuSetPos(MENU_LCD_AUTO_OFF_2);		
		else if (Get_Timer_off()==5*60) MenuSetPos(MENU_LCD_AUTO_OFF_5);	
		else if (Get_Timer_off()==10*60) MenuSetPos(MENU_LCD_AUTO_OFF_10);	
		else MenuSetPos(MENU_LCD_AUTO_OFF_0);		
		break;	
	case MENU_LCD_AUTO_OFF_2:
    	Set_Timer_off(2*60,  1);
      MENU_CLEAR();	
		break;		
	case MENU_LCD_AUTO_OFF_5:
    	Set_Timer_off(5*60,  1);	
	    MENU_CLEAR();
		break;			
	case MENU_LCD_AUTO_OFF_10:
    	Set_Timer_off(10*60,  1);	
	    MENU_CLEAR();
		break;			
	case MENU_LCD_AUTO_OFF_0:
    	Set_Timer_off(0,  1);	
	    MENU_CLEAR();
		break;	

	case MENU_MAIN_CORRECTION:
		g_last_main_command = command;
		MENU_START(g_correction_menu);
		break;
	case MENU_RETURN:
		MENU_START(g_main_menu);
		MenuSetPos(g_last_main_command);
		break;
	case MENU_F_100Hz:
		MenuSetF(720000);
		g_last_f_command = command;
		break;
	case MENU_F_1KHz:
		MenuSetF(72000);
		g_last_f_command = command;
		break;
	case MENU_F_10KHz:
		MenuSetF(7200);
		g_last_f_command = command;
		break;
	case MENU_F_93_75KHz:
		MenuSetF(dac_period_100k);
		g_last_f_command = command;
		break;
	case MENU_F_187_5KHz:
		MenuSetF(dac_period_200k);
		g_last_f_command = command;
		break;
	
	case MENU_F_187_5KHz_on:
		pos_dop_menu=6;
		MENU_START(g_f187_menu);
    if (MENU_F_187_5KHz_on_off==1) MenuSetPos(MENU_F_187_5KHz_on_1);		
		else MenuSetPos(MENU_F_187_5KHz_on_0);		
		g_last_f_command = command;
		break;	
	case MENU_F_187_5KHz_on_1:
    MENU_F_187_5KHz_on_off=1;
		FLASH_Unlock();	
	  EE_WriteVariable(VirtAddVarTab[adres_eeprom_187_5KHz_on_off], ((uint16_t)MENU_F_187_5KHz_on_off) );
    FLASH_Lock();	
    MENU_CLEAR();	
		break;		
	case MENU_F_187_5KHz_on_0:
    MENU_F_187_5KHz_on_off=0;	
		FLASH_Unlock();	
	  EE_WriteVariable(VirtAddVarTab[adres_eeprom_187_5KHz_on_off], ((uint16_t)MENU_F_187_5KHz_on_off) );
    FLASH_Lock();		
	  MENU_CLEAR();
		break;	
	
	case MENU_SP_SERIAL:
		MenuSetSerial(true, 1);
		break;
	case MENU_SP_PARALLEL:
		MenuSetSerial(false, 1);
		break;
	case MENU_V_RIM:
		MenuSetPrinRim(true);
		break;
	case MENU_V_LC:
		MenuSetPrinRim(false);
		break;
	case MENU_CORRECTION_1_Om:
	case MENU_CORRECTION_100_Om:
	case MENU_CORRECTION_1_KOm:
	case MENU_CORRECTION_10_KOm:
	case MENU_CORRECTION_100_KOm:
	case MENU_CORRECTION_SHORT:
	case MENU_CORRECTION_OPEN:
	case MENU_CORRECTION_SAVE:
	case MENU_CORRECTION_CLEAR:
		MenuOnCorrection(command);
		break;

	case MENU_SET_R:
		//g_last_main_command = command;
		pos_dop_menu=0;

		MENU_START(g_set_r_menu);
		break;
	case MENU_SET_U:
		//g_last_main_command = command;
		pos_dop_menu=1;

		MENU_START(g_set_u_menu);
		break;	
	
	case MENU_SET_I:
		//g_last_main_command = command;
		pos_dop_menu=2;

		MENU_START(g_set_i_menu);
		break;	
	case MENU_SET_R_AUTO:
		MenuSetR(255);
		break;
	case MENU_SET_R0:
		MenuSetR(0);
		break;
	case MENU_SET_R1:
		MenuSetR(1);
		break;
	case MENU_SET_R2:
		MenuSetR(2);
		break;
	case MENU_SET_R3:
		MenuSetR(3);
		break;
	
	case MENU_SET_U_AUTO:
		MenuSetU(255);
		break;
	case MENU_SET_U0:
		MenuSetU(0);
		break;
	case MENU_SET_U1:
		MenuSetU(1);
		break;
	case MENU_SET_U2:
		MenuSetU(2);
		break;
	case MENU_SET_U3:
		MenuSetU(3);
		break;	
	case MENU_SET_U4:
		MenuSetU(4);
		break;		
	case MENU_SET_U5:
		MenuSetU(5);
		break;		
	case MENU_SET_U6:
		MenuSetU(6);
		break;		
	case MENU_SET_U7:
		MenuSetU(7);
		break;		
	
	case MENU_SET_I_AUTO:
		MenuSetI(255);
		break;
	case MENU_SET_I0:
		MenuSetI(0);
		break;
	case MENU_SET_I1:
		MenuSetI(1);
		break;
	case MENU_SET_I2:
		MenuSetI(2);
		break;
	case MENU_SET_I3:
		MenuSetI(3);
		break;	
	case MENU_SET_I4:
		MenuSetI(4);
		break;		
	case MENU_SET_I5:
		MenuSetI(5);
		break;		
	case MENU_SET_I6:
		MenuSetI(6);
		break;		
	case MENU_SET_I7:
		MenuSetI(7);
		break;		
	

	case MENU_ERRORS:
		printError = !printError;
		MENU_CLEAR();
		break;
	}
}

void MenuRepaint()
{
	if(NumberEditStarted())
	{
		NumberEditRepaint();
		return;
	}

	if(message_line1)
	{//MessageBox suport
		LcdGotoXYFont( 3, 3 );
		LcdStr(FONT_1X, message_line1);
		if(message_line2)
		{
			LcdGotoXYFont( 3, 4 );
			LcdStr(FONT_1X, message_line2);
		}
		return;
	}

	if(g_menu_size==0 || g_cur_menu==NULL)
		return;
	
	const uint8_t height = 4;
	const uint8_t font_height = 16;
	uint8_t ystart = 1;

	if(g_menu_size<height)
	{
		ystart = (height-g_menu_size)/2+1;
	}

	uint8_t istart = 0;
	uint8_t iend = g_menu_size;
	if(g_menu_size>=height)
	{
		if(g_menu_pos<height)
		{
			iend = height;
		} else
		{
			istart = g_menu_pos-height+1;
			iend = g_menu_pos+1;
			if(iend>g_menu_size)
				iend = g_menu_size;
		}
	}
	
  static point_t pointXY;
	
	for(uint8_t i=istart; i<iend; i++)
	{
		//LcdGotoXYFont( 2, ystart+i-istart );
		//LcdStr(FONT_1X, g_cur_menu[i].text);				
	  pointXY.x_point=2;
	  pointXY.y_point = (ystart-istart+i-1)*font_height-1;
	  vram_put_str(pointXY, &tahoma_10ptFontInfo, g_cur_menu[i].text, PIXEL_SET);
	}

	uint8_t higlight_pos = ystart-istart+g_menu_pos;
	if(higlight_pos<=height)
	{
		//if(higlight_pos<5)
		//	LcdSingleBar( 4, higlight_pos*font_height+1, font_height+1, 128-4, PIXEL_XOR );
		//else
			LcdSingleBar( 0, higlight_pos*font_height-1, font_height, LCD_X_RES, PIXEL_XOR );
	}	
	//LcdUpdate();
	
}

bool MenuIsOpen()
{
	return g_menu_size!=0 && g_cur_menu!=NULL && !bCalibration;
}

void MenuSetPos(MenuEnum command)
{
	if(g_menu_size==0 || g_cur_menu==NULL)
		return;
	for(uint8_t i=0; i<g_menu_size; i++)
	{
		if(g_cur_menu[i].command==command)
		{
			g_menu_pos = i;
			break;
		}
	}
}


void DAC_Frq(int8_t updown)
{
	if (updown>0){
	  if (DacPeriod()==720000) MenuSetF(72000);
    else if (DacPeriod()==72000) MenuSetF(7200);
    else if (DacPeriod()==7200) MenuSetF(dac_period_100k);		
    else if (DacPeriod()==dac_period_100k) {			
			if (MENU_F_187_5KHz_on_off==1) MenuSetF(dac_period_200k);	
			else MenuSetF(720000);
		}
    else if (DacPeriod()==dac_period_200k) MenuSetF(720000);	
		
	}
	else {
	  if (DacPeriod()==720000) {
			if (MENU_F_187_5KHz_on_off==1) MenuSetF(dac_period_200k);
			else MenuSetF(dac_period_100k);	
		}
    else if (DacPeriod()==72000) MenuSetF(720000);
    else if (DacPeriod()==7200) MenuSetF(72000);		
    else if (DacPeriod()==dac_period_100k) MenuSetF(7200);	
    else if (DacPeriod()==dac_period_200k) MenuSetF(dac_period_100k);			
	}
	
		FLASH_Unlock();	
	  EE_WriteVariable(VirtAddVarTab[adres_eeprom_dac_period_h], ((uint16_t)(DacPeriod()>>16)) );
	  EE_WriteVariable(VirtAddVarTab[adres_eeprom_dac_period_l], ((uint16_t)(DacPeriod()&0xFFFF)) );	
    FLASH_Lock();	
}
void MenuSetF(uint32_t period)
{	
	bContinuousMode = true;
	AdcDacStartSynchro(period, DEFAULT_DAC_AMPLITUDE);
	ProcessStartComputeX(0/*count*/, 
			255/*predefinedResistorIdx*/,
			255/*predefinedGainVoltageIdx*/,
			255/*uint8_t predefinedGainCurrentIdx*/,
			true/*useCorrector*/
		);
	MENU_CLEAR();
}

void MenuSetSerial(bool ser, uint8_t save)
{
	isSerial = ser;
	MENU_CLEAR();
	
	if (save){
	  FLASH_Unlock();
	  EE_WriteVariable(VirtAddVarTab[adres_eeprom_ser_par], ((uint16_t)(ser)) );	
    FLASH_Lock();		
	}
}

void MenuSetPrinRim(bool pr)
{
	printRim = pr;
	MENU_CLEAR();
}

void MessageBox(char* line1)
{
	message_line1 = line1;
	message_line2 = NULL;
	g_update = true;
}

void MessageBox2(char* line1, char* line2)
{
	message_line1 = line1;
	message_line2 = line2;
	g_update = true;
}

void MenuClearFlash()
{
	if(CorrectorFlashClearCurrent())
		MessageBox("Очистка завершена");
	else
		MessageBox("Ошибка при стирании");
	ClearCorrector();
	//delay_ms(500);	
}

void MenuSaveFlash()
{
	if(!CorrectorFlashClearCurrent())
		MessageBox("Ошибка при стирании");

	delay_ms(10);

	if(CorrectorFlashCurrentData())
		MessageBox("    Сохранено");
	else
		MessageBox("Ошибка сохранения");
	//delay_ms(500);	
}


typedef struct CalibrationJob
{
	uint8_t resistorIndex;
	uint8_t VIndex;
	uint8_t IIndex;
	uint8_t ampDiv;
} CalibrationJob;

void OnCalibrationStart(CalibrationJob* job, uint8_t jobCount);
void OnOpenFirstPass(void);


static CalibrationJob* calJob;
static uint8_t calJobCount;
static uint8_t calCurIndex;
static complexf calResult[28];
static bool calFirstPassOpen = false;

static CalibrationJob calibrateShort[]=
{//resistorIndex, VIndex, IIndex, ampDiv
    {0, 0, 0, 1},
    {0, 1, 0, 1},
    {0, 2, 0, 1},
    {0, 3, 0, 1},		
    {0, 4, 0, 1},
    {0, 5, 0, 1},		
    {0, 6, 0, 1},
    {0, 7, 0, 1},
};

static CalibrationJob calibrate1Om[]=
{//resistorIndex, VIndex, IIndex, ampDiv

    {0, 5, 0, 1},		
    {0, 6, 0, 1},
    {0, 7, 0, 1},
};

static CalibrationJob calibrate100Om[]=
{//resistorIndex, VIndex, IIndex, ampDiv
	  {0, 0, 0, 1}, //1,2,5,10,20,50,100,200 
    {0, 0, 1, 1}, //2 // 1,2,4, 5, 8,10, 16, 32;
	
    {0, 1, 0, 1},
    {0, 2, 0, 2},
    {0, 3, 0, 5},		
    {0, 4, 0, 10},
		
};

static CalibrationJob calibrate1KOm[]=
{//resistorIndex, VIndex, IIndex, ampDiv
    {0, 0, 2, 1},
		
    {1, 0, 0, 1},
    {1, 0, 1, 2},
};

static CalibrationJob calibrate10KOm[]=
{//resistorIndex, VIndex, IIndex, ampDiv
    {1, 0, 2, 1},
		
    {2, 0, 0, 1},
    {2, 0, 1, 1},
};

static CalibrationJob calibrate100KOm[]=
{//resistorIndex, VIndex, IIndex, ampDiv
    {2, 0, 2, 1},

    {3, 0, 0, 1},
    {3, 0, 1, 2},
    {3, 0, 2, 5},
    {3, 0, 3, 10},
		
    {3, 0, 4, 20},
    {3, 0, 5, 50},	
		
    {3, 0, 6, 100},
    {3, 0, 7, 200},
};

static CalibrationJob calibrate1000KOm[]=
{//resistorIndex, VIndex, IIndex, ampDiv
		
    {3, 0, 4, 20},
    {3, 0, 5, 50},	
		
    {3, 0, 6, 100},
    {3, 0, 7, 200},
};

static CalibrationJob calibrateOpen[]=
{//resistorIndex, VIndex, IIndex, ampDiv
    {0, 0, 0, 1},
    {0, 0, 1, 1},
    {0, 0, 2, 1},

    {1, 0, 0, 1},
    {1, 0, 1, 1},
    {1, 0, 2, 1},

    {2, 0, 0, 1},
    {2, 0, 1, 1},
    {2, 0, 2, 1},

    {3, 0, 0, 1},
    {3, 0, 1, 1},
    {3, 0, 2, 1},
    {3, 0, 3, 1},		
    {3, 0, 4, 1},
    {3, 0, 5, 1},		
    {3, 0, 6, 1},
    {3, 0, 7, 1},
};

void MenuOnCorrection(MenuEnum command)
{
	g_last_correction_command = command;

	CoeffCorrector* corr = GetCorrector();
	if(corr->period==0)
	{
		ClearCorrector();
		corr->period = DacPeriod();
	}

	if(PredefinedPeriodIndex()==255)
	{
		MessageBox2("Ошибка", "Bad frequency");
		return;
	}

	switch(command)
	{
	case MENU_CORRECTION_1_Om:
		NumberEditSetText("Сопротивление 1 Om");
		NumberEditSetValue(corr->cshort.R1,
			-3, -1);
		NumberEditStart();
		break;	
	case MENU_CORRECTION_100_Om:
		NumberEditSetText("Сопротивление 100 Om");
		NumberEditSetValue(corr->cshort.R100,
			-1, 1);
		NumberEditStart();
		break;
	case MENU_CORRECTION_1_KOm:
		NumberEditSetText("Сопротивление 1 kOm");
		NumberEditSetValue(corr->x2x[1].R[0], 0, 2);
		NumberEditStart();
		break;
	case MENU_CORRECTION_10_KOm:
		NumberEditSetText("Сопротивление 10 kOm");
		NumberEditSetValue(corr->x2x[2].R[0], 1, 3);
		NumberEditStart();
		break;
	case MENU_CORRECTION_100_KOm:
		NumberEditSetText("Сопротивление 100 kOm");
		NumberEditSetValue(corr->open.R, 2, 4);
		NumberEditStart();
		break;
	case MENU_CORRECTION_SHORT:
		OnCalibrationStart(calibrateShort, sizeof(calibrateShort)/sizeof(calibrateShort[0]));
		break;
	case MENU_CORRECTION_OPEN:
		OnOpenFirstPass();
		break;	
	case MENU_CORRECTION_SAVE:
		MenuSaveFlash();
	  //MENU_CLEAR();
		break;
	case MENU_CORRECTION_CLEAR:
		MenuClearFlash();
	  //MENU_CLEAR();	
		break;

	default:;
	}
}

void CalNextJob()
{
	uint32_t period = DacPeriod();
	CalibrationJob* job = calJob+calCurIndex;
	//===============================================================
	uint8_t resistorIdx_max=3;
	if ((int)DacFrequency()>93000) { //ограничение r2
		resistorIdx_max=2;	
	}
	else {
		resistorIdx_max=3;	
  }
	
  if (job->resistorIndex>resistorIdx_max){
		 job->resistorIndex=resistorIdx_max;
		 if (job->IIndex<4) job->ampDiv=1;
		 else job->ampDiv=2;
	}	
	//===============================================================		
	AdcDacStartSynchro(period, DEFAULT_DAC_AMPLITUDE/job->ampDiv);
	ProcessStartComputeX(0/*count*/, 
			job->resistorIndex/*predefinedResistorIdx*/,
			job->VIndex/*predefinedGainVoltageIdx*/,
			job->IIndex/*predefinedGainCurrentIdx*/,
			false/*useCorrector*/
		);
}

void OnCalibrationStart(CalibrationJob* job, uint8_t jobCount)
{
	calJob = job;
	calJobCount = jobCount;
	calCurIndex = 0;

	if(jobCount>sizeof(calResult)/sizeof(calResult[0]))
	{
		MessageBox2("OnCalibrationStart", "ERR jobCount");
		return;
	}

	isSerial = true;
	bCalibration = true;
	//printRim = true;
	CalNextJob();
}

static bool FindResult(	uint8_t resistorIndex, uint8_t VIndex, uint8_t IIndex, complexf* result)
{
	for(uint8_t i=0; i<calJobCount; i++)
	{
		CalibrationJob* p = calJob+i;
		if(p->resistorIndex==resistorIndex && p->VIndex==VIndex && p->IIndex==IIndex)
		{
			*result = calResult[i];
			return true;
		}
	}

	return false;
}

void OnOpenFirstPass()
{
	calFirstPassOpen = true;
	isSerial = true;
	bCalibration = true;
	//printRim = true;

	GetCorrector()->open.maxGainIndex = 7;
	//	OnCalibrationStart(calibrateOpen, sizeof(calibrateOpen)/sizeof(calibrateOpen[0]));	
	AdcDacStartSynchro(GetCorrector()->period, DEFAULT_DAC_AMPLITUDE);
	ProcessStartComputeX(ProcessCalcOptimalCount()*2/*count*/, 
			255/*predefinedResistorIdx*/,
			255/*predefinedGainVoltageIdx*/,
			255/*uint8_t predefinedGainCurrentIdx*/,
			false/*useCorrector*/
		);
}

//void OnSaveCalibrationResult()
//{
//	CoeffCorrector* corr = GetCorrector();
//	if(g_last_correction_command==MENU_CORRECTION_SHORT)
//	{
//		CoeffCorrectorShort* p = &corr->cshort;
//		if(!FindResult(0, 0, 0, &p->Zm[0].Zsm))
//			MessageBox2("ОШИБКА Cal", "Open 0");
//		if(!FindResult(0, 1, 0, &p->Zm[1].Zsm))
//			MessageBox2("ОШИБКА Cal", "Open 1");
//		if(!FindResult(0, 2, 0, &p->Zm[2].Zsm))
//			MessageBox2("ОШИБКА Cal", "Open 2");
//		if(!FindResult(0, 4, 0, &p->Zm[3].Zsm))
//			MessageBox2("ОШИБКА Cal", "Open 4");
//		if(!FindResult(0, 6, 0, &p->Zm[4].Zsm))
//			MessageBox2("ОШИБКА Cal", "Open 6");
//		if(!FindResult(0, 7, 0, &p->Zm[5].Zsm))
//			MessageBox2("ОШИБКА Cal", "Open 7");
//		return;
//	}

//	if(g_last_correction_command==MENU_CORRECTION_1_Om)
//	{
//		CoeffCorrectorShort* p = &corr->cshort;
//		if(!FindResult(0, 7, 0, &p->Zm[5].Zstdm))
//			MessageBox2("ОШИБКА Cal", "1Om 7");
//		return;
//	}

//	if(g_last_correction_command==MENU_CORRECTION_100_Om)
//	{
//		CoeffCorrectorShort* p = &corr->cshort;
//		if(!FindResult(0, 0, 0, &p->Zm[0].Zstdm))
//			MessageBox2("ОШИБКА Cal", "100Om 0");
//		if(!FindResult(0, 1, 0, &p->Zm[1].Zstdm))
//			MessageBox2("ОШИБКА Cal", "100Om 1");
//		if(!FindResult(0, 2, 0, &p->Zm[2].Zstdm))
//			MessageBox2("ОШИБКА Cal", "100Om 2");
//		if(!FindResult(0, 4, 0, &p->Zm[3].Zstdm))
//			MessageBox2("ОШИБКА Cal", "100Om 4");
//		if(!FindResult(0, 6, 0, &p->Zm[4].Zstdm))
//			MessageBox2("ОШИБКА Cal", "100Om 6");

//		if(!FindResult(0, 0, 0, &corr->x2x[0].Zm[0].Zstdm))
//			MessageBox2("ОШИБКА Cal", "100Om 00");
//		if(!FindResult(0, 0, 1, &corr->x2x[0].Zm[1].Zstdm))
//			MessageBox2("ОШИБКА Cal", "100Om 01");
//		return;
//	}

//	if(g_last_correction_command==MENU_CORRECTION_1_KOm)
//	{
//		if(!FindResult(0, 0, 2, &corr->x2x[0].Zm[2].Zstdm))
//			MessageBox2("ОШИБКА Cal", "1kOm 00");
//		if(!FindResult(1, 0, 0, &corr->x2x[1].Zm[0].Zstdm))
//			MessageBox2("ОШИБКА Cal", "1kOm 00");
//		if(!FindResult(1, 0, 1, &corr->x2x[1].Zm[1].Zstdm))
//			MessageBox2("ОШИБКА Cal", "1kOm 01");
//	}

//	if(g_last_correction_command==MENU_CORRECTION_10_KOm)
//	{
//		if(!FindResult(1, 0, 2, &corr->x2x[1].Zm[2].Zstdm))
//			MessageBox2("ОШИБКА Cal", "10kOm 00");
//		if(!FindResult(2, 0, 0, &corr->x2x[2].Zm[0].Zstdm))
//			MessageBox2("ОШИБКА Cal", "10kOm 00");
//		if(!FindResult(2, 0, 1, &corr->x2x[2].Zm[1].Zstdm))
//			MessageBox2("ОШИБКА Cal", "10kOm 01");
//	}

//	if(g_last_correction_command==MENU_CORRECTION_100_KOm)
//	{
//		if(!FindResult(2, 0, 2, &corr->x2x[2].Zm[2].Zstdm))
//			MessageBox2("ОШИБКА Cal", "100kOm 00");

//		for(uint8_t IIndex=0; IIndex<8; IIndex++)
//		{
//			int8_t idx = GetGainValidIdx(IIndex);
//			if(idx<0)
//				continue;
//			if(!FindResult(3, 0, IIndex, &corr->open.Zm[idx].Zstdm))
//				MessageBox2("ОШИБКА Cal", "100 kOm 1");
//		}
//	}

//	if(g_last_correction_command==MENU_CORRECTION_OPEN)
//	{
//		for(uint8_t resistorIndex=0; resistorIndex<3; resistorIndex++)
//		for(uint8_t IIndex=0; IIndex<3; IIndex++)
//		{
//			if(!FindResult(resistorIndex, 0, IIndex, &corr->x2x[resistorIndex].Zm[IIndex].Zom))
//				MessageBox2("ОШИБКА Cal", "Open");
//		}

//		for(uint8_t IIndex=0; IIndex<8; IIndex++)
//		{
//			int8_t idx = GetGainValidIdx(IIndex);
//			if(idx<0)
//				continue;
//			if(!FindResult(3, 0, IIndex, &corr->open.Zm[idx].Zom))
//				MessageBox2("ОШИБКА Cal", "Open 1");
//		}
//	}
//	
//}
void OnSaveCalibrationResult()
{
	CoeffCorrector* corr = GetCorrector();
	if(g_last_correction_command==MENU_CORRECTION_SHORT)
	{
		CoeffCorrectorShort* p = &corr->cshort;
		if(!FindResult(0, 0, 0, &p->Zm[0].Zsm))
			MessageBox2("Ошибка Cal", "Open 0");
		if(!FindResult(0, 1, 0, &p->Zm[1].Zsm))
			MessageBox2("Ошибка Cal", "Open 1");
		if(!FindResult(0, 2, 0, &p->Zm[2].Zsm))
			MessageBox2("Ошибка Cal", "Open 2");
		if(!FindResult(0, 3, 0, &p->Zm[3].Zsm))
			MessageBox2("Ошибка Cal", "Open 3");		
		if(!FindResult(0, 4, 0, &p->Zm[4].Zsm))
			MessageBox2("Ошибка Cal", "Open 4");
		if(!FindResult(0, 5, 0, &p->Zm[5].Zsm))
			MessageBox2("Ошибка Cal", "Open 5");		
		if(!FindResult(0, 6, 0, &p->Zm[6].Zsm))
			MessageBox2("Ошибка Cal", "Open 6");
		if(!FindResult(0, 7, 0, &p->Zm[7].Zsm))
			MessageBox2("Ошибка Cal", "Open 7");
		return;
	}

	if(g_last_correction_command==MENU_CORRECTION_1_Om)
	{
		CoeffCorrectorShort* p = &corr->cshort;
		
//		if(!FindResult(0, 0, 0, &p->Zm[0].Zstdm))
//			MessageBox2("Ошибка Cal", "1Om 0");
//		if(!FindResult(0, 1, 0, &p->Zm[1].Zstdm))
//			MessageBox2("Ошибка Cal", "1Om 1");			
//		if(!FindResult(0, 2, 0, &p->Zm[2].Zstdm))
//			MessageBox2("Ошибка Cal", "1Om 2");
//		if(!FindResult(0, 3, 0, &p->Zm[3].Zstdm))
//			MessageBox2("Ошибка Cal", "1Om 3");		
//		if(!FindResult(0, 4, 0, &p->Zm[4].Zstdm))
//			MessageBox2("Ошибка Cal", "1Om 4");			
		if(!FindResult(0, 5, 0, &p->Zm[5].Zstdm))
			MessageBox2("Ошибка Cal", "1Om 5");
		if(!FindResult(0, 6, 0, &p->Zm[6].Zstdm))
			MessageBox2("Ошибка Cal", "1Om 6");		
		if(!FindResult(0, 7, 0, &p->Zm[7].Zstdm))
			MessageBox2("Ошибка Cal", "1Om 7");		
		return;
	}

	if(g_last_correction_command==MENU_CORRECTION_100_Om)
	{
		CoeffCorrectorShort* p = &corr->cshort;
		if(!FindResult(0, 0, 0, &p->Zm[0].Zstdm))
			MessageBox2("Ошибка Cal", "100Om 0");
		if(!FindResult(0, 1, 0, &p->Zm[1].Zstdm))
			MessageBox2("Ошибка Cal", "100Om 1");
		if(!FindResult(0, 2, 0, &p->Zm[2].Zstdm))
			MessageBox2("Ошибка Cal", "100Om 2");
		if(!FindResult(0, 3, 0, &p->Zm[3].Zstdm))
			MessageBox2("Ошибка Cal", "100Om 3");		
		if(!FindResult(0, 4, 0, &p->Zm[4].Zstdm))
			MessageBox2("Ошибка Cal", "100Om 4");
//		if(!FindResult(0, 5, 0, &p->Zm[5].Zstdm))
//			MessageBox2("Ошибка Cal", "100Om 5");		
//		if(!FindResult(0, 6, 0, &p->Zm[6].Zstdm))
//			MessageBox2("Ошибка Cal", "100Om 6");
//		if(!FindResult(0, 7, 0, &p->Zm[7].Zstdm))
//			MessageBox2("Ошибка Cal", "100Om 7");
		
		if(!FindResult(0, 0, 0, &corr->x2x[0].Zm[0].Zstdm))
			MessageBox2("Ошибка Cal", "100Om 00");
		if(!FindResult(0, 0, 1, &corr->x2x[0].Zm[1].Zstdm))
			MessageBox2("Ошибка Cal", "100Om 01");
		return;
	}

	if(g_last_correction_command==MENU_CORRECTION_1_KOm)
	{
		if(!FindResult(0, 0, 2, &corr->x2x[0].Zm[2].Zstdm))
			MessageBox2("Ошибка Cal", "1KOm 00");
		if(!FindResult(1, 0, 0, &corr->x2x[1].Zm[0].Zstdm))
			MessageBox2("Ошибка Cal", "1KOm 00");
		if(!FindResult(1, 0, 1, &corr->x2x[1].Zm[1].Zstdm))
			MessageBox2("Ошибка Cal", "1KOm 01");
	}

	if(g_last_correction_command==MENU_CORRECTION_10_KOm)
	{
		if(!FindResult(1, 0, 2, &corr->x2x[1].Zm[2].Zstdm))
			MessageBox2("Ошибка Cal", "10KOm 00");
		if(!FindResult(2, 0, 0, &corr->x2x[2].Zm[0].Zstdm))
			MessageBox2("Ошибка Cal", "10KOm 00");
		if(!FindResult(2, 0, 1, &corr->x2x[2].Zm[1].Zstdm))
			MessageBox2("Ошибка Cal", "10KOm 01");
	}

	if(g_last_correction_command==MENU_CORRECTION_100_KOm)
	{
				
		if(!FindResult(2, 0, 2, &corr->x2x[2].Zm[2].Zstdm))
			MessageBox2("Ошибка Cal", "100KOm 00");
		
			uint8_t resistorIdx_max=3;
			uint8_t resistorIdx_start=0;		
	    if ((int)DacFrequency()>93000){
				resistorIdx_max=2;
				resistorIdx_start=2;
			}
	    else {
				resistorIdx_max=3;
				resistorIdx_start=0;
			}				
		
		for(uint8_t IIndex=resistorIdx_start; IIndex<8; IIndex++)
		{
			int8_t idx = GetGainValidIdx(IIndex);
			if(idx<0)
				continue;
			
			if(!FindResult(resistorIdx_max, 0, IIndex, &corr->open.Zm[idx].Zstdm))
				MessageBox2("Ошибка Cal", "100 KOm 1");
		}
	}

	if(g_last_correction_command==MENU_CORRECTION_OPEN)
	{
			uint8_t resistorIdx_max=3;
	    if ((int)DacFrequency()>93000) resistorIdx_max=2;
	    else resistorIdx_max=3;	  		
		
		for(uint8_t resistorIndex=0; resistorIndex<resistorIdx_max; resistorIndex++)
		for(uint8_t IIndex=0; IIndex<resistorIdx_max; IIndex++)
		{
			if(!FindResult(resistorIndex, 0, IIndex, &corr->x2x[resistorIndex].Zm[IIndex].Zom))
				MessageBox2("Ошибка Cal", "Open");
		}

		for(uint8_t IIndex=0; IIndex<8; IIndex++)
		{
			int8_t idx = GetGainValidIdx(IIndex);
			if(idx<0)
				continue;
			
			if(!FindResult(resistorIdx_max, 0, IIndex, &corr->open.Zm[idx].Zom))
				MessageBox2("Ошибка Cal", "Open 1");
		}
	}
	
}


void OnCalibrationComplete()
{
	if(calFirstPassOpen)
	{
		calFirstPassOpen = false;
		GetCorrector()->open.maxGainIndex = gainCurrentIdx;

		if(false)
		{//debug code
			static char buf[]="0";
			buf[0]='0'+gainCurrentIdx;
			MessageBox2("  OPEN", buf);
			bCalibration = false;
		} else
		{
			OnCalibrationStart(calibrateOpen, sizeof(calibrateOpen)/sizeof(calibrateOpen[0]));
		}
		return;
	}

	calResult[calCurIndex] = Rre + Rim*I;

	calCurIndex++;
	if(calCurIndex<calJobCount)
	{
		CalNextJob();
	} else
	{
		OnSaveCalibrationResult();
		bCalibration = false;
		MessageBox("    Выполнено");
	//=============================	
	//ClearCorrector();	// я добавил
	
	bContinuousMode = true;
	AdcDacStartSynchro(DacPeriod(), DEFAULT_DAC_AMPLITUDE);// я добавил
	ProcessStartComputeX(0/*count*/, 
			255/*predefinedResistorIdx*/,
			255/*predefinedGainVoltageIdx*/,
			255/*uint8_t predefinedGainCurrentIdx*/,
			true/*useCorrector*/
		);
	//=============================			
	}
}

void OnNumberEditEnd()
{
	CoeffCorrector* corr = GetCorrector();
	switch(g_last_correction_command)
	{
	case MENU_CORRECTION_1_Om:
		corr->cshort.R1 = NumberEditGetValue();
		OnCalibrationStart(calibrate1Om, sizeof(calibrate1Om)/sizeof(calibrate1Om[0]));
		break;	
	case MENU_CORRECTION_100_Om:
		corr->cshort.R100 =
		corr->x2x[0].R[0] =
		corr->x2x[0].R[1] =
			NumberEditGetValue();
		OnCalibrationStart(calibrate100Om, sizeof(calibrate100Om)/sizeof(calibrate100Om[0]));
		break;
	case MENU_CORRECTION_1_KOm:
		corr->x2x[0].R[2] =
		corr->x2x[1].R[0] =
		corr->x2x[1].R[1] =
			NumberEditGetValue();
		OnCalibrationStart(calibrate1KOm, sizeof(calibrate1KOm)/sizeof(calibrate1KOm[0]));
		break;
	case MENU_CORRECTION_10_KOm:
		corr->x2x[1].R[2] =
		corr->x2x[2].R[0] =
		corr->x2x[2].R[1] =
			NumberEditGetValue();
		OnCalibrationStart(calibrate10KOm, sizeof(calibrate10KOm)/sizeof(calibrate10KOm[0]));
		break;
	case MENU_CORRECTION_100_KOm:
		corr->x2x[2].R[2] =
		corr->open.R = 
			NumberEditGetValue();
		OnCalibrationStart(calibrate100KOm, sizeof(calibrate100KOm)/sizeof(calibrate100KOm[0]));
		break;
	default:
		MessageBox2("NumberEditEnd", "Bad command");	
		break;
	}
}

extern uint8_t predefinedGainVoltageIdx; //напряжения
extern uint8_t predefinedGainCurrentIdx; //тока

void MenuSetR(uint8_t Ridx)
{
	predefinedResistorIdx = Ridx;
	gainVoltageIdx = 0;
	gainCurrentIdx = 0;
	g_update = true;
	MENU_CLEAR();
}
void MenuSetU(uint8_t Uidx)
{
	predefinedGainVoltageIdx = Uidx;
	gainVoltageIdx = 0;
	gainCurrentIdx = 0;
	g_update = true;
	MENU_CLEAR();
}
void MenuSetI(uint8_t Iidx)
{
	predefinedGainCurrentIdx = Iidx;
	gainVoltageIdx = 0;
	gainCurrentIdx = 0;
	g_update = true;
	MENU_CLEAR();
}

