//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// ��� ����������� ����������� ���������������� ��������. �� ������ ���������
// ��� �� ����� �����, �� �� �������� ������� ������ �� ��� YouTube-����� 
// "����������� � ���������" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// �����: �������� ������ / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include "stm32f30x.h"
#include "i2cm.h"


////==============================================================================
//// ��������� ������������� i2c (I2C1 ��� I2C2) � ������ master � �������� �������� ����������
////==============================================================================
//void i2cm_init(I2C_TypeDef* I2Cx, uint32_t i2c_clock)
//{
//  // �������� ������������ GPIO � I2C
//  if (I2Cx == I2C1)
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
//  else
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
//	
//  RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

//  // ����������� I2C
//  I2C_Cmd(I2Cx, DISABLE); 
//  I2C_DeInit(I2Cx);
//	
////  I2C_InitTypeDef i2c_InitStruct;
////  i2c_InitStruct.I2C_Mode = I2C_Mode_I2C;
////  //i2c_InitStruct.I2C_ClockSpeed = i2c_clock;
////  i2c_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
////  //i2c_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
////  i2c_InitStruct.I2C_Ack = I2C_Ack_Enable;
////  i2c_InitStruct.I2C_OwnAddress1 = 0;
//	
//	I2C_InitTypeDef I2C_InitStructure;
//	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
//  I2C_InitStructure.I2C_DigitalFilter = 0x00;
//  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
//  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//  I2C_InitStructure.I2C_Timing = 0x00902025;
//	
//  I2C_Cmd(I2Cx, ENABLE); 
//  I2C_Init(I2Cx, &I2C_InitStructure);
//  
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_4); // SCL
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_4); // SDA
//	
//  // ����������� ���� GPIO
//  GPIO_InitTypeDef InitStruct;
//  InitStruct.GPIO_Mode = GPIO_Mode_AF;
//  InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//  InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
//	InitStruct.GPIO_OType = GPIO_OType_OD;
//				
//  if (I2Cx == I2C1)
//    InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
//  else
//    InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
//  
//  GPIO_Init(GPIOB, &InitStruct);
//}
////==============================================================================


////==============================================================================
//// ������� �������� �����. ����� ������� START, ����� ����� ������ � ��������� R/W
////==============================================================================
//int8_t i2cm_Start(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t IsRead, uint16_t TimeOut)
//{
//  uint16_t TOcntr;
//  
//  // ����� ������� START
//  I2C_GenerateSTART(I2Cx, ENABLE);
//  TOcntr = TimeOut;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) && TOcntr) {TOcntr--;}
//  if (!TOcntr)
//    return I2C_ERR_HWerr;
//  
//  // ����� ����� ������ � ������� ��������� ������
//  if (IsRead)
//  {
//    I2C_Send7bitAddress(I2Cx, slave_addr << 1, I2C_Direction_Receiver);
//    TOcntr = TimeOut;
//    while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && TOcntr) {TOcntr--;}
//  }
//  else
//  {
//    I2C_Send7bitAddress(I2Cx, slave_addr << 1, I2C_Direction_Transmitter);
//    TOcntr = TimeOut;
//    while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && TOcntr) {TOcntr--;}
//  }
//  
//  if (!TOcntr)
//      return I2C_ERR_NotConnect;
//  
//  return I2C_ERR_Ok;
//}
////==============================================================================


////==============================================================================
//// ������� ����� ������� STOP
////==============================================================================
//int8_t i2cm_Stop(I2C_TypeDef* I2Cx, uint16_t TimeOut)
//{
//  I2C_GenerateSTOP(I2Cx, ENABLE);
//  uint16_t TOcntr = TimeOut;
//  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) && TOcntr);
//  if (!TOcntr)
//    return I2C_ERR_HWerr;
//  
//  return I2C_ERR_Ok;
//}
////==============================================================================


////==============================================================================
//// ������� ����� �� ���� ������ ���� �� ������
////==============================================================================
//int8_t i2cm_WriteBuff(I2C_TypeDef* I2Cx, uint8_t *pbuf, uint16_t len, uint16_t TimeOut)
//{
//  uint16_t TOcntr;
//  
//  while (len--)
//  {
//    I2C_SendData(I2Cx, *(pbuf++));
//    TOcntr = TimeOut;
//    while((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && TOcntr) {TOcntr--;}
//    if (!TOcntr)
//      return I2C_ERR_NotConnect;
//  }
//  
//  return I2C_ERR_Ok;
//}
////==============================================================================


////==============================================================================
//// ������� ������ ������ ���� � ���� � ����� ������� STOP
////==============================================================================
//int8_t i2cm_ReadBuffAndStop(I2C_TypeDef* I2Cx, uint8_t *pbuf, uint16_t len, uint16_t TimeOut)
//{
//  uint16_t TOcntr;
//  
//  // ��������� ������ ������������� ACK
//  I2C_AcknowledgeConfig(I2Cx, ENABLE);

//  while (len-- != 1)
//  {
//    TOcntr = TimeOut;
//    while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) ) && TOcntr) {TOcntr--;}
//    *pbuf++ = I2C_ReceiveData(I2Cx);
//  }

//  // ��������� ������ ACK      
//  I2C_AcknowledgeConfig(I2Cx, DISABLE);
//  I2C_GenerateSTOP(I2Cx,ENABLE);               // ����� STOP

//  TOcntr = TimeOut;
//  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) ) && TOcntr) {TOcntr--;}
//  *pbuf++ = I2C_ReceiveData(I2Cx);             // ������ N-2 ����

//  i2cm_Stop(I2Cx, TimeOut);
//  
//  return I2C_ERR_Ok;
//}
////==============================================================================

void I2C1_Init()
{
//������� ��� I2C1 �� ���� B �� ���� PB8 � PB9
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);
	
GPIO_InitTypeDef GPIO_I2C1;
GPIO_I2C1.GPIO_Pin      =   I2C1_SCL | I2C1_SDA;
GPIO_I2C1.GPIO_Speed    =   GPIO_Speed_50MHz;
GPIO_I2C1.GPIO_Mode     =   GPIO_Mode_AF;
GPIO_I2C1.GPIO_OType    =   GPIO_OType_OD;
GPIO_I2C1.GPIO_PuPd     =   GPIO_PuPd_UP;
GPIO_Init(I2C1_PORT, &GPIO_I2C1);

GPIO_PinAFConfig(I2C1_PORT, I2C1_SCL, GPIO_AF_4);
GPIO_PinAFConfig(I2C1_PORT, I2C1_SDA, GPIO_AF_4);

I2C_InitTypeDef  I2C;

I2C_DeInit(I2C1);
//������������� I2C1
RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

I2C.I2C_AnalogFilter    =   I2C_AnalogFilter_Enable;
I2C.I2C_Timing          =   0x2000090E;//0x0020B; //0x00201D2B; //����� ������� �����, ����� ������� EXEL-����, ����� ����� � MXCube
I2C.I2C_OwnAddress1     =   MASTER_ADDR;
I2C.I2C_Ack             =   I2C_Ack_Enable;
I2C.I2C_Mode            =   I2C_Mode_I2C;
I2C.I2C_DigitalFilter   =   0;
I2C.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

I2C_Init(I2C1, &I2C);
I2C_Cmd(I2C1, ENABLE);
}
//������� ������ �����
void I2C1_WriteByte(unsigned char HordSlaveAddr, unsigned char NumBytes, unsigned char *Byte)
{
int DataNum = 0;
unsigned char *Buffer;
Buffer = Byte;

while(I2C_GetFlagStatus(I2C1, I2C_ISR_BUSY) != RESET){}; // ���� ���� ����������� ����
	
//I2C_TransferHandling(I2C1, HordSlaveAddr, 2, I2C_Reload_Mode, I2C_Generate_Start_Write); //�������� 2-�������� ������,
//while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXE)==RESET){};
//I2C_SendData(I2C1, (unsigned char)((ExtSlaveAddress & 0xFF00) << 8));
//while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXE)==RESET){};
//I2C_SendData(I2C1, (unsigned char)(ExtSlaveAddress & 0x00FF));
//while(I2C_GetFlagStatus(I2C1, I2C_ISR_TCR)==RESET){}; // 2 ����� �������� ���� ���� retood
I2C_TransferHandling(I2C1, HordSlaveAddr, NumBytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Write); //I2C_AutoEnd_Mode I2C_No_StartStop ������������� �� �������� N ����, ��� ����� ����, � ���� ����������
while ( DataNum != NumBytes)
{
while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXE)==RESET){};
I2C_SendData(I2C1, *Buffer++);
DataNum++;
}
while(I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF)==RESET){};//����� ����-���
I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);// ������� ����

}
//������� ������ �����
void I2C1_ReadByte(unsigned char HordSlaveAddr, unsigned char NumBytes, unsigned short ExtSlaveAddress, char* Destination)
{
int DataNum = 0;
while(I2C_GetFlagStatus(I2C1, I2C_ISR_BUSY) != RESET){};
I2C_TransferHandling(I2C1, HordSlaveAddr, 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write); //������� ������������� �� �������� 2 ���� ������
while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXE) == RESET){};
I2C_SendData(I2C1, (unsigned char)((ExtSlaveAddress & 0xFF00) << 8));
while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXE) == RESET){};
I2C_SendData(I2C1, (unsigned char)(ExtSlaveAddress & 0x00FF));
while(I2C_GetFlagStatus(I2C1, I2C_ISR_TC)==RESET){};
I2C_TransferHandling(I2C1, HordSlaveAddr, NumBytes, I2C_AutoEnd_Mode, I2C_Generate_Start_Read); // ������ ������ �� �����  N ���� � ����� ��������
while(DataNum != NumBytes)
{
while(I2C_GetFlagStatus(I2C1, I2C_ISR_RXNE) == RESET){};
*Destination = I2C_ReceiveData(I2C1);
while(I2C_GetFlagStatus(I2C1, I2C_ISR_RXNE) != RESET){};
DataNum++;
Destination++;
}

while(I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) == RESET){}; //����� ���� ���
I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
}

