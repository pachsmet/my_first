//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include "stm32f30x.h"
#include "gpio.h"


//==============================================================================
// Процедура настройки пинов микроконтроллера (по масте) как входов 
//==============================================================================
void gpio_SetGPIOmode_In(GPIO_TypeDef* GPIOx, uint16_t Mask, uint8_t PullMode)
{
  GPIO_InitTypeDef InitStruct;
  InitStruct.GPIO_Pin = Mask;
  InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  InitStruct.GPIO_Mode = GPIO_Mode_IN;	
  switch (PullMode)
  {
  case gpio_NoPull:
    InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    break;
  case gpio_PullUp:
    InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    break;
  case gpio_PullDown:
    InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    break;
  }
  
  GPIO_Init(GPIOx, &InitStruct);
}
//==============================================================================


//==============================================================================
// Процедура настройки пинов микроконтроллера (по масте) как выходов 
//==============================================================================
void gpio_SetGPIOmode_Out(GPIO_TypeDef* GPIOx, uint16_t Mask)
{
  GPIO_InitTypeDef InitStruct;
  InitStruct.GPIO_Pin = Mask;
  InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	InitStruct.GPIO_Mode = GPIO_Mode_OUT;	
	InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	InitStruct.GPIO_OType = GPIO_OType_PP;

  GPIO_Init(GPIOx, &InitStruct);
}
//==============================================================================


//==============================================================================
// Процедура старта тактирования GPIO
//==============================================================================
void gpio_PortClockStart(GPIO_TypeDef *GPIOx)
{
  if (GPIOx == GPIOA)
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  else if (GPIOx == GPIOB)
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  else if (GPIOx == GPIOC)
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  else if (GPIOx == GPIOD)
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
  else if (GPIOx == GPIOE)
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
  else if (GPIOx == GPIOF)
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
  else if (GPIOx == GPIOG)
    RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOG, ENABLE);
}
//==============================================================================

