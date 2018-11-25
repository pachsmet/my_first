//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include "stm32f30x.h"
#include "spim.h"


//==============================================================================
// Процедура инициализации spi (SPI1 или SPI2) в режиме master
//==============================================================================
void spim_init(SPI_TypeDef* SPIx, uint8_t WordLen)
{
		GPIO_InitTypeDef GPIO_InitStruct;
	  SPI_InitTypeDef SPI_InitStruct;
	
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		
		// configure pins used by SPI
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		// connect SPI1 pins to SPI alternate function
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_5);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_5);

		// enable peripheral clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		
		// configure SPI1 in Mode 0 
		// CPOL = 0 --> clock is low when idle
		// CPHA = 0 --> data is sampled at the first edge
		SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
		SPI_InitStruct.SPI_DataSize = (WordLen == 16) ? SPI_DataSize_16b : SPI_DataSize_8b;
		SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
		SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;//SPI_BaudRatePrescaler_8
		SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_Init(SPI1, &SPI_InitStruct); 
		
		SPI_Cmd(SPI1, ENABLE);
}
//==============================================================================


//==============================================================================
// Процедура отправляет массив 16-битных слов
//==============================================================================
void SPI_send16b(SPI_TypeDef* SPIx, uint16_t *pBuff, uint16_t Len)
{
	SPI_DataSizeConfig(SPI1, SPI_DataSize_16b);	
  for (uint16_t i = 0; i < Len; i++)
  {
    SPI_I2S_SendData16(SPIx, *(pBuff++));
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET) ;
  }
	SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);	
}
//==============================================================================


//==============================================================================
// Процедура отправляет массив 8-битных слов
//==============================================================================
void SPI_send8b(SPI_TypeDef* SPIx, uint8_t *pBuff, uint16_t Len)
{
	SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);	
  for (uint16_t i = 0; i < Len; i++)
  {
    SPI_SendData8(SPIx, *(pBuff++));
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET) ;
  }
	//SPI_DataSizeConfig(SPI1, SPI_DataSize_16b);	
}
//==============================================================================


//==============================================================================
// Процедура принимает массив 16-битных слов
//==============================================================================
void SPI_recv16b(SPI_TypeDef* SPIx, uint16_t *pBuff, uint16_t Len)
{
	SPI_DataSizeConfig(SPI1, SPI_DataSize_16b);	
  for (uint16_t i = 0; i < Len; i++)
  {
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET) ;
    *(pBuff++) = SPI_I2S_ReceiveData16(SPIx);
  }
	SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);	
}//==============================================================================


//==============================================================================
// Процедура принимает массив 8-битных слов
//==============================================================================
void SPI_recv8b(SPI_TypeDef* SPIx, uint8_t *pBuff, uint16_t Len)
{
  for (uint16_t i = 0; i < Len; i++)
  {
    while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET) ;
    *(pBuff++) = SPI_ReceiveData8(SPIx);
  }
}
//==============================================================================
