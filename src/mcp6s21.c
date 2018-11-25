

#include "hw_config.h"
#include "spim.h"
#include "lcd_interface.h"
#include "process_measure.h"
/*
	Init SPI for operate MCP6S21 Opertional amplifier
*/
void MCPInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;		
	// enable clock for used IO pins
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	/*
	PB9 - CS_I
	PB8 - CS_U
	*/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
	
    //hardware SPI
  	SPI_InitTypeDef SPI_InitStruct;
		// enable clock for used IO pins
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		// connect SPI1 pins to SPI alternate function
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_5);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_5);		
		// configure pins used by SPI
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStruct);

		// enable peripheral clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		
		// configure SPI1 in Mode 0 
		// CPOL = 0 --> clock is low when idle
		// CPHA = 0 --> data is sampled at the first edge
		SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
		SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
		SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
		SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
		SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_Init(SPI1, &SPI_InitStruct); 
		
		SPI_Cmd(SPI1, ENABLE);		
}

/*
0 - Gain of +1 (Default)
1 - Gain of +2
2 - Gain of +4
3 - Gain of +5
4 - Gain of +8
5 - Gain of +10
6 - Gain of +16
7 - Gain of +32
*/

/*
0 - Gain of +1 (Default)
1 - Gain of +2
2 - Gain of +5
3 - Gain of +10
4 - Gain of +20
5 - Gain of +50
6 - Gain of +100
7 - Gain of +200
*/

void MCPSetGain(bool voltage, uint8_t gain)
{
	static uint8_t gain_v=255;
	static uint8_t gain_i=255;	
	
	if (voltage){
		gainVoltageIdx_temp=gain;
		if (gain_v!=gain){
	    GPIO_ResetBits(GPIOB, voltage?GPIO_Pin_8:GPIO_Pin_9);	
	    uint16_t  command[1];
	    command[0]= (0x2A<<8);// 0x40<<8;
	    command[0] |= (gain<<4)&0xF0;
      command[0] |= 0x01;// Первый канал
	    SPI_send16b(SPI1, command, 1);
	    GPIO_SetBits(GPIOB, voltage?GPIO_Pin_8:GPIO_Pin_9);

		}
		gain_v=gain;
	}
	else {	
    gainCurrentIdx_temp=gain;	
		if (gain_i!=gain){
	    GPIO_ResetBits(GPIOB, voltage?GPIO_Pin_8:GPIO_Pin_9);	
	    uint16_t  command[1];
	    command[0]= (0x2A<<8);// 0x40<<8;
	    command[0] |= (gain<<4)&0xF0;
      command[0] |= 0x01;// Первый канал
	    SPI_send16b(SPI1, command, 1);
	    GPIO_SetBits(GPIOB, voltage?GPIO_Pin_8:GPIO_Pin_9);

		}
		gain_i=gain;		
	}
	
}
