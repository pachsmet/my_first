/**
  ******************************************************************************
  * @file    hw_config.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Hardware Configuration & Setup
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32_it.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "ssd1306.h"


#include "main.h"
#include "lcd.h"
#include "systick.h"
//#include "segoe16.h"
#include "lcd_interface.h"
ErrorStatus HSEStartUpStatus;
EXTI_InitTypeDef EXTI_InitStructure;

/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Private functions ---------------------------------------------------------*/
void Delay_ms(uint32_t ms)
{
// � stm32f10x_conf.h ����������� ������ ������ ��� ��������
volatile uint32_t nCount;
RCC_ClocksTypeDef RCC_Clocks;
RCC_GetClocksFreq (&RCC_Clocks);

nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
for (; nCount!=0; nCount--);
}
/**
  * @brief  Configures Main system clocks & power.
  * @param  None
  * @retval None
  */
void Set_System(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f30x.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f30x.c file
     */ 
//#if !defined(STM32L1XX_MD) && !defined(STM32L1XX_HD) && !defined(STM32L1XX_MD_PLUS)
//  GPIO_InitTypeDef GPIO_InitStructure;	
//#endif /* STM32L1XX_MD && STM32L1XX_XD */  

//#if defined(USB_USE_EXTERNAL_PULLUP)
//  GPIO_InitTypeDef  GPIO_InitStructure;
//#endif /* USB_USE_EXTERNAL_PULLUP */ 
//  
//  /*!< At this stage the microcontroller clock setting is already configured, 
//       this is done through SystemInit() function which is called from startup
//       file (startup_stm32f10x_xx.s) before to branch to application main.
//       To reconfigure the default setting of SystemInit() function, refer to
//       system_stm32f10x.c file
//     */   
//#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS) || defined(STM32F37X) || defined(STM32F30X)
//  /* Enable the SYSCFG module clock */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//#endif /* STM32L1XX_XD */ 
//   
//#if !defined(STM32L1XX_MD) && !defined(STM32L1XX_HD) && !defined(STM32L1XX_MD_PLUS) && !defined(STM32F37X) && !defined(STM32F30X)
//  /* Enable USB_DISCONNECT GPIO clock */
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

//  /* Configure USB pull-up pin */
//  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
//#endif /* STM32L1XX_MD && STM32L1XX_XD */
//   
//#if defined(USB_USE_EXTERNAL_PULLUP)
//  /* Enable the USB disconnect GPIO clock */
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_DISCONNECT, ENABLE);

//  /* USB_DISCONNECT used as USB pull-up */
//  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);  
//#endif /* USB_USE_EXTERNAL_PULLUP */ 
//  
//#if defined(STM32F37X) || defined(STM32F30X)
//  
//  /* Enable the USB disconnect GPIO clock */
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_DISCONNECT, ENABLE);
//  
//  /*Set PA11,12 as IN - USB_DM,DP*/ 
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  
//  /*SET PA11,12 for USB: USB_DM,DP*/
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_14);
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_14);
//  
//  /* USB_DISCONNECT used as USB pull-up */
//  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
//#endif /* STM32F37X  && STM32F30X)*/
// 
//   /* Configure the EXTI line 18 connected internally to the USB IP */
//  EXTI_ClearITPendingBit(EXTI_Line18);
//  EXTI_InitStructure.EXTI_Line = EXTI_Line18;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&EXTI_InitStructure); 
	
	
 /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Enable the SYSCFG module clock (used for the USB disconnect feature) */
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


 /*Set PA11,12 as IN - USB_DM,DP*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
      
  /*SET PA11,12 for USB: USB_DM,DP*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_14);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_14);
  
  //PA15 used as USB pull-up
  /* Enable the USB disconnect GPIO clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIO_DISCONNECT, ENABLE);
  GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  /* USB_DISCONNECT used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
   
  /* Configure the EXTI line 18 connected internally to the USB IP */
  EXTI_ClearITPendingBit(EXTI_Line18);
  EXTI_InitStructure.EXTI_Line = EXTI_Line18; /*USB resume from suspend mode*/
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

//EXTI->IMR = 0x00040000;
//EXTI->EMR = 0x00000000;
//EXTI->RTSR = 0x00040000;
//EXTI->FTSR = 0x00000000;

  //Set resistor pins
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  SetResistor(0);

  //Set Analog Switch pin
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  SetLowPassFilter(0, 0);
		
/*
	Init Power pin
*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOA, GPIO_Pin_10);
/*
	Init �������������� ��� ������ �� �����
*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);	
	
}

void Power_off(uint8_t low_akb)
{
  static point_t pointXY;
	
	if (low_akb==1){
	 LcdClear();
//   LcdGotoXYFont(4,3);
//   LcdStr(FONT_1X, "������ ����� ���");	
//   LcdGotoXYFont(7, 6);
//   LcdStr(FONT_1X, "����������");			
		pointXY.x_point=2*11;
	  pointXY.y_point = 1*16;		
	  vram_put_str(pointXY, &tahoma_10ptFontInfo, "������ �����", PIXEL_SET);		
	  pointXY.y_point = 2*16;
	  vram_put_str(pointXY, &tahoma_10ptFontInfo, "����������", PIXEL_SET);
	  LcdFillRect(1, 126, 1, 62, 1);	
	}
	else if (low_akb==0){
	 LcdClear();
   //LcdGotoXYFont(7, 4);
   //LcdStr(FONT_1X, "����������");	
	  LcdFillRect(1, 126, 1, 62, 1);			
		pointXY.x_point=2*11;
	  pointXY.y_point = 1*16;
	  vram_put_str(pointXY, &tahoma_10ptFontInfo, "����������", PIXEL_SET);
	}
	else if (low_akb==2){
	 LcdClear();
//   LcdGotoXYFont(4,3);
//   LcdStr(FONT_1X, "�������� ������");	
//   LcdGotoXYFont(7, 6);
//   LcdStr(FONT_1X, "����������");	
		pointXY.x_point=6;
	  pointXY.y_point = 1*16;		
	  vram_put_str(pointXY, &tahoma_10ptFontInfo, "�������� ������", PIXEL_SET);	
		pointXY.x_point=2*11;		
	  pointXY.y_point = 2*16;
	  vram_put_str(pointXY, &tahoma_10ptFontInfo, "����������", PIXEL_SET);		
	  LcdFillRect(1, 126, 1, 62, 1);	
	}	
	 delay_ms(500);	
   LcdUpdate();	
   GPIO_ResetBits(GPIOA, GPIO_Pin_10);
	 while(1);
	
}


/**
  * @brief  Configures all IOs as AIN to reduce the power consumption.
  * @param  None
  * @retval None
  */
void GPIO_AINConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  
  /* Enable all GPIOs Clock*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ALLGPIO, ENABLE);  
  
 /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Disable all GPIOs Clock*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ALLGPIO, DISABLE); 
}



/**
  * @brief  Convert Hex 32Bits value into char.
  * @param  value: Data to be converted.
  * @param  pbuf: pointer to buffer.  
  * @param  len: Data length.   
  * @retval None
  */
void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }	
}



void SetResistor(uint8_t idx)
{
	static  uint8_t idx_old;
	if 	(idx!=idx_old){
    GPIO_WriteBit(GPIOC, GPIO_Pin_14, (idx&1)?Bit_SET:Bit_RESET);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13,  ((idx>>1)&1)?Bit_SET:Bit_RESET);
	}
	idx_old=idx;
}

uint8_t LowPassFilter;
void SetLowPassFilter(uint32_t dac_period, uint8_t filtr_on)
{  	
	if(filtr_on==0){
		LowPassFilter=0;
    GPIO_WriteBit(GPIOA, GPIO_Pin_1,  Bit_SET);		
    GPIO_WriteBit(GPIOA, GPIO_Pin_2,  Bit_SET);	
		//GPIO_WriteBit(GPIOA, GPIO_Pin_3,  Bit_SET);	
  return;		
	}
		
	
	if (dac_period>=720000) {
		LowPassFilter=3;

    GPIO_WriteBit(GPIOA, GPIO_Pin_1,  Bit_RESET);		
    GPIO_WriteBit(GPIOA, GPIO_Pin_2,  Bit_RESET);	
    //GPIO_WriteBit(GPIOA, GPIO_Pin_3,  Bit_RESET);			
	}
	else if (dac_period>=72000){
		LowPassFilter=2;
    GPIO_WriteBit(GPIOA, GPIO_Pin_1,  Bit_RESET);		
    GPIO_WriteBit(GPIOA, GPIO_Pin_2,  Bit_SET);
    //GPIO_WriteBit(GPIOA, GPIO_Pin_3,  Bit_RESET);			
	}
	else if (dac_period>=7200){
		LowPassFilter=1;
    GPIO_WriteBit(GPIOA, GPIO_Pin_1,  Bit_SET);		
    GPIO_WriteBit(GPIOA, GPIO_Pin_2,  Bit_RESET);		
    //GPIO_WriteBit(GPIOA, GPIO_Pin_3,  Bit_RESET);			
	}
	else if (dac_period>=768){
		LowPassFilter=0;
    GPIO_WriteBit(GPIOA, GPIO_Pin_1,  Bit_SET);		
    GPIO_WriteBit(GPIOA, GPIO_Pin_2,  Bit_SET);	
    //GPIO_WriteBit(GPIOA, GPIO_Pin_3,  Bit_RESET);			
	}
	else if (dac_period>=384){
		LowPassFilter=0;
    GPIO_WriteBit(GPIOA, GPIO_Pin_1,  Bit_SET);		
    GPIO_WriteBit(GPIOA, GPIO_Pin_2,  Bit_SET);	
    //GPIO_WriteBit(GPIOA, GPIO_Pin_3,  Bit_SET);			
		
	}
}

uint8_t GetLowPassFilter( void)
{  	
  return LowPassFilter;
}


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
EXTI_InitTypeDef EXTI_InitStructure;
extern __IO uint32_t packet_sent;
extern __IO uint8_t Send_Buffer[VIRTUAL_COM_PORT_DATA_SIZE] ;
extern __IO  uint32_t packet_receive;
extern __IO uint8_t Receive_length;

uint8_t Receive_Buffer[64];
uint32_t Send_length;
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Extern variables ----------------------------------------------------------*/

extern LINE_CODING linecoding;

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
	SSD1306_Sleep();	
  bDeviceState = SUSPENDED;

}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
    /*Enable SystemCoreClock*/
  SystemInit();
	SSD1306_Wake();
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
NVIC_InitTypeDef NVIC_InitStructure;

  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 
  /* Enable the USB interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USB Wake-up interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_Init(&NVIC_InitStructure);   

}

/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
		
  }
  else
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }

}
#define         ID1          (0x1FFFF7AC)
#define         ID2          (0x1FFFF7B0)
#define         ID3          (0x1FFFF7B4)
/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(uint32_t*)ID1;
  Device_Serial1 = *(uint32_t*)ID2;
  Device_Serial2 = *(uint32_t*)ID3;
 
  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &Virtual_Com_Port_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &Virtual_Com_Port_StringSerial[18], 4);
  }
}


/*******************************************************************************
* Function Name  : Send DATA .
* Description    : send the data received from the STM32 to the PC through USB  
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t CDC_Send_DATA (uint8_t *ptrBuffer, uint32_t Send_length)
{
  /*if max buffer is Not reached*/
  if(Send_length < VIRTUAL_COM_PORT_DATA_SIZE)     
  {
    /*Sent flag*/
    packet_sent = 0;
    /* send  packet to PMA*/
    UserToPMABufferCopy((unsigned char*)ptrBuffer, ENDP1_TXADDR, Send_length);
    SetEPTxCount(ENDP1, Send_length);
    SetEPTxValid(ENDP1);
  }
  else
  {
    return 0;
  } 
  return 1;
}

/*******************************************************************************
* Function Name  : Receive DATA .
* Description    : receive the data from the PC to STM32 and send it through USB
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint32_t CDC_Receive_DATA(void)
{ 
  /*Receive flag*/
  packet_receive = 0;
  SetEPRxValid(ENDP3); 
  return 1 ;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
