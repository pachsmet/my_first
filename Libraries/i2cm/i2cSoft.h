/*
 * author:	Елисей Равнюшкин
 * date:	08.04.2014	19:00:00
 * file:	i2cSoft.h
 * Софтовая реализация I2C
 */

#ifndef I2CSOFT_H_
#define I2CSOFT_H_

#include <stdint.h>

//I2C
#define I2C_GPIO					GPIOB
#define I2C_RCC_APB2Periph_GPIO		RCC_APB2Periph_GPIOB
#define GPIO_Pin_SDA				GPIO_Pin_7
#define GPIO_Pin_SCL				GPIO_Pin_6

#define SCLH						I2C_GPIO->BSRR = GPIO_Pin_SCL
#define SCLL						I2C_GPIO->BRR = GPIO_Pin_SCL

#define SDAH						I2C_GPIO->BSRR = GPIO_Pin_SDA
#define SDAL						I2C_GPIO->BRR = GPIO_Pin_SDA
#define SCLread						I2C_GPIO->IDR & GPIO_Pin_SCL
#define SDAread						I2C_GPIO->IDR & GPIO_Pin_SDA

#define I2C_DELAY_VALUE				2//22

void i2cSoft_Init (void);
int i2cSoft_ReadBuffer ( uint8_t *buffer, uint32_t sizeOfBuffer);
int i2cSoft_WriteBuffer ( uint8_t *buffer, uint32_t sizeOfBuffer);
int8_t i2cSoft_Start (uint8_t slave_addr, uint8_t IsRead);
void i2cSoft_Stop (void);
	
#endif
