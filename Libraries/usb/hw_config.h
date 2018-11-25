/**
  ******************************************************************************
  * @file    hw_config.h
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#ifdef STM32F30X
 #include "stm32f30x.h"
#endif /* STM32F30X */
#include <stdbool.h>


void Set_System(void);
void GPIO_AINConfig(void);
void Power_off(uint8_t low_akb);

/*
  bit 0 = PA
  bit 1 = PB
*/
void SetResistor(uint8_t idx);

//24000 == 3 KHz
#define LOW_PASS_PERIOD 24000 
/*
Add 3.3 nF capacitor to filter after DAC.
*/
void SetLowPassFilter(uint32_t dac_period, uint8_t filtr_on);
uint8_t GetLowPassFilter( void);



/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
//#define MASS_MEMORY_START     0x04002000
//#define BULK_MAX_PACKET_SIZE  0x00000040
//#define LED_ON                0xF0
//#define LED_OFF               0xFF

/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void Get_SerialNum(void);
void LCD_Control(void);
uint32_t CDC_Send_DATA (uint8_t *ptrBuffer, uint32_t Send_length);
uint32_t CDC_Receive_DATA(void);
/* External variables --------------------------------------------------------*/

#endif  /*__HW_CONFIG_H*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
