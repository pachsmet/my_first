

#include "hw_config.h"
#include "quadrature_encoder.h"
#include "lcd_interface.h"
#include <main.h>
#include "eeprom.h"	
#include "dac.h"	
#include "menu.h"	
extern uint16_t VirtAddVarTab[NB_OF_VAR];
extern uint8_t type_screen;

#define DEF_INTERFACE_BUTTONS

//-------------------------------------------#DEFINE------------------------------------------
#define Codeur_A           GPIO_Pin_2
#define Codeur_A_SOURCE    GPIO_PinSource2
#define Codeur_B           GPIO_Pin_10
#define Codeur_B_SOURCE    GPIO_PinSource10
#define Codeur_GPIO        GPIOB
#define Codeur_RCC         RCC_AHBPeriph_GPIOB
#define Codeur_AF          GPIO_AF_2
 
#define Codeur_TIMER       TIM3
#define Codeur_TIMER_RCC   RCC_APB1Periph_TIM3

#define BUTTON_PIN 			  GPIO_Pin_11
#define BUTTON_GPIO			  GPIOB

static bool g_lastButtonState = false;
uint16_t timer_of, timer_of_tiks;

#ifdef DEF_INTERFACE_BUTTONS
static bool g_lastButtonStateA = false;
static bool g_lastButtonStateB = false;
static void QuadInitAsButton(void);
#else
static uint16_t g_lastEncValue = 0;
static void QuadInitAsEncoder(void);
#endif

void QuadTimerButton(void);

void QuadEncInit(void)
{

#ifdef DEF_INTERFACE_BUTTONS
    QuadInitAsButton();
#else 
    QuadInitAsEncoder();
#endif

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(BUTTON_GPIO, &GPIO_InitStructure);

    QuadTimerButton();
}

#ifdef DEF_INTERFACE_BUTTONS
void QuadInitAsButton()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 2 Inputs for A and B Encoder Channels
    GPIO_InitStructure.GPIO_Pin = Codeur_A|Codeur_B;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(Codeur_GPIO, &GPIO_InitStructure);
}
#else //!DEF_INTERFACE_BUTTONS
static void QuadInitAsEncoder()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(Codeur_RCC, ENABLE);
    RCC_APB1PeriphClockCmd(Codeur_TIMER_RCC, ENABLE);
 
    // 2 Inputs for A and B Encoder Channels
    GPIO_InitStructure.GPIO_Pin = Codeur_A|Codeur_B;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(Codeur_GPIO, &GPIO_InitStructure);
 
    //Timer AF Pins Configuration
    GPIO_PinAFConfig(Codeur_GPIO, Codeur_A_SOURCE, Codeur_AF);
    GPIO_PinAFConfig(Codeur_GPIO, Codeur_B_SOURCE, Codeur_AF);
 
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
 
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_Period = 65535; // Maximal
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
 
    TIM_TimeBaseInit(Codeur_TIMER, &TIM_TimeBaseStructure);
 
    // TIM_EncoderMode_TI1: Counter counts on TI1FP1 edge depending on TI2FP2 level.
    TIM_EncoderInterfaceConfig(Codeur_TIMER, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
     
    TIM_Cmd(Codeur_TIMER, ENABLE);
}
#endif

uint16_t QuadEncValue()
{
	return TIM_GetCounter (Codeur_TIMER);
}

bool QuadEncButton()
{
	return GPIO_ReadInputDataBit(BUTTON_GPIO, BUTTON_PIN)==0;
}

void QuadTimerButton()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

    //TIM_TimeBaseStructure.TIM_Period = 2000; // 1 sec
    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock/10; // 100 msec
    TIM_TimeBaseStructure.TIM_Prescaler = 72-1; // 2000 tick per sec
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  

    TIM_TimeBaseInit(TIM7,&TIM_TimeBaseStructure);       
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    TIM_Cmd(TIM7, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Init(&NVIC_InitStructure);
}

void Set_Timer_off(uint16_t sec, uint8_t save)
{
	timer_of_tiks=0;
	timer_of=sec;
	if (save){
	  FLASH_Unlock();	
	  EE_WriteVariable(VirtAddVarTab[adres_eeprom_timer_off], ((uint16_t)timer_of) );
    FLASH_Lock();	
	}		
}
void Reset_Timer_off(void)
{
	timer_of_tiks=0;	
}
uint16_t Get_Timer_off(void)
{
	return timer_of;
}

extern uint8_t Measure_ok;
extern volatile uint8_t timer_of_Measure_ok;
extern uint32_t time_temp, time;

void TIM7_IRQHandler(void)
{
	  static uint16_t count_OnButtonPressed, count_OnButtonPressed_long;
	  static uint16_t count_OnButtonAPressed, count_OnButtonAPressed_long;	
		static uint16_t count_OnButtonBPressed, count_OnButtonBPressed_long;
		static uint16_t	count_time;
  	static uint16_t timer_of_tiks_ms;
	
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
			//if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10)) GPIO_ResetBits(GPIOA, GPIO_Pin_10);
			//else GPIO_SetBits(GPIOA, GPIO_Pin_10);
			
			if (count_time<100)count_time++;
			else {	
				count_time=0;
				time=time_temp;
				time_temp=0;				
			}
			
			if (timer_of_tiks_ms<10)timer_of_tiks_ms++;
			else {
				timer_of_tiks_ms=0;
				timer_of_tiks++;
				if ((timer_of_tiks>=timer_of)&&(timer_of>0)){
					timer_of_tiks=0;
					Power_off(2);
				}
				

				
				if (timer_of_Measure_ok<2) timer_of_Measure_ok++;
				else{
					timer_of_Measure_ok=0;
					Measure_ok=1;
				}				
				
			}
						
        bool buttonState = QuadEncButton();
			
        if(buttonState!=g_lastButtonState){
					count_OnButtonPressed_long=0;
					
					if (buttonState==1){
						count_OnButtonPressed++;
					}
					else if (count_OnButtonPressed>0){
						count_OnButtonPressed=0;
						OnButtonPressed(); 						
					}						  					
              
				}
				else {
					if (buttonState==1){
					//count_OnButtonPressed	=0;
					if (count_OnButtonPressed_long<10)count_OnButtonPressed_long++;
					else {
						count_OnButtonPressed_long=0;
						Power_off(0);
					}					
				}
					else count_OnButtonPressed_long=0;
			  }
        g_lastButtonState = buttonState;

				
#ifdef DEF_INTERFACE_BUTTONS
      //=======================================================================
        bool buttonStateA = (GPIO_ReadInputDataBit(Codeur_GPIO, Codeur_A)==0);
			
        if(buttonStateA!=g_lastButtonStateA){
					count_OnButtonAPressed_long=0;
					
					if (buttonStateA==1){
						count_OnButtonAPressed++;
					}
					else if (count_OnButtonAPressed>0){
						count_OnButtonAPressed=0;
						OnWeel(+1); 						
					}						  					
              
				}
				else {
					if (buttonStateA==1){
					//count_OnButtonAPressed	=0;
					if (count_OnButtonAPressed_long<10)count_OnButtonAPressed_long++;
					else {
						 count_OnButtonAPressed_long=0;
						 count_OnButtonAPressed=0;
						 count_OnButtonBPressed_long=0;
						 count_OnButtonBPressed=0;	
						if (is_menu())menu_clear();
						else{
						 if (type_screen<2)type_screen++;
	           else type_screen=0;
						}
						
						LcdRepaint();	
					}					
				}
					else count_OnButtonAPressed_long=0;
			  }
        g_lastButtonStateA = buttonStateA;				
				
				//=======================================================================
        bool buttonStateB = (GPIO_ReadInputDataBit(Codeur_GPIO, Codeur_B)==0);
			
        if(buttonStateB!=g_lastButtonStateB){
					count_OnButtonBPressed_long=0;
					
					if (buttonStateB==1){
						count_OnButtonBPressed++;
					}
					else if (count_OnButtonBPressed>0){
						count_OnButtonBPressed=0;
						OnWeel(-1); 						
					}						  					
              
				}
				else {
					if (buttonStateB==1){
					//count_OnButtonAPressed	=0;
					if (count_OnButtonBPressed_long<10)count_OnButtonBPressed_long++;
					else {
						 count_OnButtonAPressed_long=0;
						 count_OnButtonAPressed=0;
						 count_OnButtonBPressed_long=0;
						 count_OnButtonBPressed=0;
						if (is_menu())menu_clear();
						else{						
						if (DacGetDelitel()==3) DacSetDelitel(1,  1);
						else DacSetDelitel(3,  1);
						}
						LcdRepaint();
					}					
				}
					else count_OnButtonBPressed_long=0;
			  }
        g_lastButtonStateB = buttonStateB;					
				
				

#else
        uint16_t encValue = QuadEncValue();
        if(encValue!=g_lastEncValue)
        {
            //OnWeel(encValue-g_lastEncValue);
					if ((encValue-g_lastEncValue)<0) OnWeel(-1);   
					else OnWeel(1);  
        }

        g_lastEncValue = encValue;
#endif
        OnTimer();

        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}
