/*
 * author:	������ ���������
 * date:	08.04.2014	19:00:00
 * file:	i2cSoft.c
 * �������� ���������� I2C
 */

#include "i2cSoft.h"
#include <stdbool.h>
#include "stm32f30x.h"
//--------------------��������� ��������� �������-----------------------------
static void i2cSoft_Delay (void);

static void i2cSoft_Ack (void);
static void i2cSoft_NoAck (void);
static bool i2cSoft_WaitAck (void);
static void i2cSoft_PutByte ( uint8_t data );
static uint8_t i2cSoft_GetByte (void);

//---------------------���������� �������--------------------------------------

/**
 *	@brief	������������� ������ i2c, � ������ ������ �����/������
 *	@param	void
 *	@return	void
 */
void i2cSoft_Init (void)
{
  // ����������� ���� GPIO
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
  GPIO_InitTypeDef InitStruct;
	InitStruct.GPIO_Pin = GPIO_Pin_SDA | GPIO_Pin_SCL;
  InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	InitStruct.GPIO_OType = GPIO_OType_OD;
	
	GPIO_Init( I2C_GPIO, &InitStruct );
}

/**
 *	@brief	������ ������ �� ���� � ����� buffer, �������� sizeOfBuffer
 *			� ������ � ������� chipAddress.
 *	@param	uint8_t chipAddress		- ����� ������������
 *			uint8_t *buffer			- ��������� �� �����, ���� ������
 *									  ����������� ������
 *			uint32_t sizeOfBuffer	- ���������� ���� ��� ������
 *	@return	int - ��������� ���������� ������:
 *			true � ������ ������
 *			false � ������ ������
 */
int i2cSoft_ReadBuffer (  uint8_t *buffer, uint32_t sizeOfBuffer )
{

	while ( sizeOfBuffer != 0 ) {
		*buffer = i2cSoft_GetByte();

		buffer++;
		sizeOfBuffer--;
		if ( sizeOfBuffer == 0 ) {
			i2cSoft_NoAck();
			break;
		}
		else
			i2cSoft_Ack();
	}

	return true;
}
// ������������ ��������
void __delay_ms(uint32_t ms)
{
    volatile uint32_t nCount;
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq (&RCC_Clocks);

    nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
    for (; nCount!=0; nCount--);
}

/**
 *	@brief	������ ������ � ���� �� ������ buffer, �������� sizeOfBuffer
 *			� ������ � ������� chipAddress.
 *	@param	uint8_t chipAddress		- ����� ������������
 *			uint8_t *buffer			- ��������� �� �����, ������ ������
 *									  ������������ ������
 *			uint32_t sizeOfBuffer	- ���������� ���� ��� ������
 *	@return	int - ��������� ���������� ������:
 *			true � ������ ������
 *			false � ������ ������
 */
int i2cSoft_WriteBuffer ( uint8_t *buffer, uint32_t sizeOfBuffer )
{
	
	while ( sizeOfBuffer != 0 ) {
		i2cSoft_PutByte( *buffer );

		if ( !i2cSoft_WaitAck() ) {
			i2cSoft_Stop();
			return false;
		}

		buffer++;
		sizeOfBuffer--;
	}
	
	return true;
}

//---------------------------��������� �������-------------------------------

/**
 *	@brief	���������� ������� ��������
 *	@param	void
 *	@return	void
 */
static void i2cSoft_Delay (void)
{
//	volatile uint16_t i = I2C_DELAY_VALUE;
//	while ( i ) {
//		i--;
//	}
	__NOP();
	__NOP();	
	__NOP();
	__NOP();		
	__NOP();
	__NOP();		
	__NOP();
	__NOP();		
	__NOP();
	__NOP();		
	
	__NOP();
	__NOP();	
	__NOP();
	__NOP();		
	__NOP();
	__NOP();		
	__NOP();
	__NOP();		
	__NOP();
	__NOP();	//1 MGz

	__NOP();
	__NOP();	
	__NOP();
	__NOP();		
	__NOP();
	__NOP();		
	__NOP();
	__NOP();		
	__NOP();
	__NOP();	//524


	__NOP();
	__NOP();	
	__NOP();
	__NOP();	
	__NOP();
	__NOP();	
	__NOP();	
	__NOP();
	__NOP();	
	__NOP();


	__NOP();
	__NOP();	
	__NOP();
	__NOP();	
	__NOP();//394 ���



}
static void i2cSoft_DelayRead (void)
{
	volatile uint16_t i = I2C_DELAY_VALUE*1.5;
	while ( i ) {
		i--;
	}
}
/**
 *	@brief	�������� ������������������ ����� � ����
 *	@param	void
 *	@return	bool - ��������� ���������� �������:
 *			true � ������ ������
 *			false � ������ ������
 */
 int8_t i2cSoft_Start ( uint8_t slave_addr, uint8_t IsRead)
{
	SDAH;						// ��������� ��� �����, �� ������
	SCLH;						// �� ������, ���� ��� ���� �������
	i2cSoft_Delay();
	if ( !(SDAread) )			// ���� ����� SDA ������� �������,
		return false;			// �� ������������ ����� ����������, ����� � �������
	SDAL;						// ��������� SDA � �����
	i2cSoft_Delay();
	if ( SDAread )				// ���� �� ���������, �� ���� ����������
		return false;			// ����� � �������
	i2cSoft_Delay();
					            // ����� ������� �����������

	 // ����� ����� ������ � ������� ��������� ������
  if (IsRead)
  {
	i2cSoft_PutByte( (slave_addr<<1)+1 );
	if ( !i2cSoft_WaitAck() ) {
		i2cSoft_Stop();
		return false;
	}
  }
  else
  {
	i2cSoft_PutByte( slave_addr<<1 );
	if ( !i2cSoft_WaitAck() ) {
		i2cSoft_Stop();
		return false;
	}
  }    
  return true;	
}

/**
 *	@brief	�������� ������������������ ���� � ����
 *	@param	void
 *	@return	bool - ��������� ���������� �������:
 *			true � ������ ������
 *			false � ������ ������
 */
 void i2cSoft_Stop (void)
{
	SCLL;						// ������������������ ��� ������������ �����
	i2cSoft_Delay();
	SDAL;
	i2cSoft_Delay();
	SCLH;
	i2cSoft_Delay();
	SDAH;
	i2cSoft_Delay();
}

/**
 *	@brief	�������� ������������������ ACK � ����
 *	@param	void
 *	@return	void
 */
static void i2cSoft_Ack (void)
{
	SCLL;
	i2cSoft_Delay();
	SDAL;						// ��������� ����� SDA � �����
	i2cSoft_Delay();
	SCLH;						// � ������ ���� ���� ������ SCL
	i2cSoft_Delay();
	SCLL;
	i2cSoft_Delay();
}

/**
 *	@brief	�������� ������������������ NO ACK � ����
 *	@param	void
 *	@return	void
 */
static void i2cSoft_NoAck (void)	//
{
	SCLL;
	i2cSoft_Delay();
	SDAH;						// ��������� ����� SDA
	i2cSoft_Delay();
	SCLH;						// � ������ ���� ���� ������ SCL
	i2cSoft_Delay();
	SCLL;
	i2cSoft_Delay();
}

/**
 *	@brief	�������� ���� �� ������� ACK �� ������
 *	@param	void
 *	@return	bool - ��������� ���������� �������:
 *			true  - ���� ACK �������
 *			false - ���� ACK �� �������
 */
static bool i2cSoft_WaitAck (void)
{
	SCLL;
	i2cSoft_Delay();
	SDAH;
	i2cSoft_Delay();
	SCLH;						// ������ �������� ����� ������ SCL
	i2cSoft_Delay();
	if ( SDAread ) {			// � ���������, ������ �� ����� ����� SDA
		SCLL;
		return false;
	}
	SCLL;						// ��������� ���� ������ SCL
	return true;
}

/**
 *	@brief	�������� ������ ����� data � ����
 *	@param	uint8_t data - ���� ������ ��� ��������
 *	@return	void
 */
static void i2cSoft_PutByte ( uint8_t data )
{
	uint8_t i = 8;				// ����� ��������� 8 ��� ������
	while ( i-- ) {				// ���� �� ��������� ��� ����
		SCLL;					// ��������� ����� SCL � �����
		i2cSoft_Delay();
		if ( data & 0x80 )		// � ���������� �� ����� SDA ������ �������
			SDAH;
		else
			SDAL;
		data <<= 1;
		i2cSoft_Delay();
		SCLH;					// ��������� ����� SCL
		i2cSoft_Delay();		// ����� ����� ����� ����� �� ��������� �������� �� ����� SDA
	}
	SCLL;
}

/**
 *	@brief	������ ������ ����� data �� ����
 *	@param	void
 *	@return	uint8_t - ����������� ����
 */
static uint8_t i2cSoft_GetByte (void)
{
	volatile uint8_t i = 8;		// ����� ��������� 8 ��� ������
	uint8_t data = 0;

	SDAH;						// ��������� ����� SDA. ��������� �� ����� �����
	while ( i-- ) {				// ���� �� �������� ��� ����
		data <<= 1;
		SCLL;					// ������ ���� ������ SCL
		i2cSoft_DelayRead();
		SCLH;
		i2cSoft_DelayRead();
		if ( SDAread ) {		// ������ �������� �� ����� SDA
			data |= 0x01;
		}
	}
	SCLL;
	return data;				// ���������� ����������� ��������
}
