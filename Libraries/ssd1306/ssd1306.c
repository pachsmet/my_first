//------------------------------------------------------------------------------
// This is Open source software. You can place this code on your site, but don't
// forget a link to my YouTube-channel: https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Это программное обеспечение распространяется свободно. Вы можете размещать
// его на вашем сайте, но не забудьте указать ссылку на мой YouTube-канал 
// "Электроника в объектике" https://www.youtube.com/channel/UChButpZaL5kUUl_zTyIDFkQ
// Автор: Надыршин Руслан / Nadyrshin Ruslan
//------------------------------------------------------------------------------
#include "stm32f30x.h"
//#include <delay.h>
#include "systick.h"
#include <ssd1306.h>
#include <gpio.h>
#include <i2cm.h>
#include <spim.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <i2cSoft.h>
#include <main.h>
#include "eeprom.h"	
extern uint16_t VirtAddVarTab[NB_OF_VAR];
static uint8_t SSD1306_invers;
#include "segoe16.h"

#define SSD1306_RESET_HIGH()    GPIO_WriteBit(SSD1306_Reset_Port, SSD1306_Reset_Pin, Bit_SET)
#define SSD1306_RESET_LOW()     GPIO_WriteBit(SSD1306_Reset_Port, SSD1306_Reset_Pin, Bit_RESET)
#define SSD1306_DC_HIGH()       GPIO_WriteBit(SSD1306_DC_Port, SSD1306_DC_Pin, Bit_SET)
#define SSD1306_DC_LOW()        GPIO_WriteBit(SSD1306_DC_Port, SSD1306_DC_Pin, Bit_RESET)

#if (SSD1306_CSPinUsed)
  #define SSD1306_CS_HIGH()     GPIO_WriteBit(SSD1306_CS_Port, SSD1306_CS_Pin, Bit_SET)
  #define SSD1306_CS_LOW()      GPIO_WriteBit(SSD1306_CS_Port, SSD1306_CS_Pin, Bit_RESET)
#else
  #define SSD1306_CS_HIGH()     ;
  #define SSD1306_CS_LOW()      ;
#endif


// Размеры дисплея в пикселях
uint8_t SSD1306_Height;
uint8_t SSD1306_Width;
uint8_t Brightness;
uint8_t SSD1306_on;
//==============================================================================
// Процедура передаёт команду с аргументами в ssd1306
//==============================================================================
void SSD1306_Send(uint8_t Cmd)
{
#if (SSD1306_IfMode == SSD1306_IfMode_I2C)
  uint8_t tmpBuff[2];
  tmpBuff[0] = 0x00;    // Control byte (Co = 0, D/A = 0)
  tmpBuff[1] = Cmd;     // Команда
	
	  if (i2cSoft_Start(SSD1306_I2C_addr, 0)){	
		  i2cSoft_WriteBuffer( tmpBuff, 2);
		  i2cSoft_Stop();
		}

	
#elif (SSD1306_IfMode == SSD1306_IfMode_SPI_4w)       // Параметры работы по SPI-4w
  SSD1306_DC_LOW();
  SSD1306_CS_LOW();
  SPI_send8b(SSD1306_SPI_periph, &Cmd, 1);
  SPI_send8b(SSD1306_SPI_periph, pBuff, BuffLen);
  SSD1306_CS_HIGH();
#else   
  // Остальные интерфейсы работы с ssd1306 пока не поддерживаются
#endif
}
//==============================================================================
//==============================================================================
// Процедура передаёт команду с аргументами в ssd1306
//==============================================================================
void SSD1306_SendCommand(uint8_t Cmd, uint8_t *pBuff, uint16_t BuffLen)
{
#if (SSD1306_IfMode == SSD1306_IfMode_I2C)
  uint8_t tmpBuff[2];
  tmpBuff[0] = 0x00;    // Control byte (Co = 0, D/A = 0)
  tmpBuff[1] = Cmd;     // Команда
	
	  if (i2cSoft_Start(SSD1306_I2C_addr, 0)){	
		  i2cSoft_WriteBuffer( tmpBuff, 2);
	    i2cSoft_WriteBuffer( pBuff, BuffLen);	
		  i2cSoft_Stop();
		}

	
#elif (SSD1306_IfMode == SSD1306_IfMode_SPI_4w)       // Параметры работы по SPI-4w
  SSD1306_DC_LOW();
  SSD1306_CS_LOW();
  SPI_send8b(SSD1306_SPI_periph, &Cmd, 1);
  SPI_send8b(SSD1306_SPI_periph, pBuff, BuffLen);
  SSD1306_CS_HIGH();
#else   
  // Остальные интерфейсы работы с ssd1306 пока не поддерживаются
#endif
}
//==============================================================================


//==============================================================================
// Процедура передаёт массив данных в ssd1306
//==============================================================================
void SSD1306_SendData(uint8_t *pBuff, uint16_t BuffLen)
{
#if (SSD1306_IfMode == SSD1306_IfMode_I2C)
  uint8_t tmpBuff = 0x40;    // Control byte (Co = 0, D/A = 1)
  
  if (i2cSoft_Start(SSD1306_I2C_addr, 0)){	
		  i2cSoft_WriteBuffer( &tmpBuff, 1);
	    i2cSoft_WriteBuffer( pBuff, BuffLen);	
		  i2cSoft_Stop();
		}	
	
#elif (SSD1306_IfMode == SSD1306_IfMode_SPI_4w)       // Параметры работы по SPI-4w
  SSD1306_DC_HIGH();
  SSD1306_CS_LOW();
  SPI_send8b(SSD1306_SPI_periph, pBuff, BuffLen);
  SSD1306_CS_HIGH();
  SSD1306_DC_LOW();
#else  
  // Остальные интерфейсы работы с ssd1306 пока не поддерживаются
#endif
}
//==============================================================================
uint8_t SSD1306_type;

void SSD1306_SetType(uint8_t type,uint8_t save){
	SSD1306_type=type; 
	if (save){
	FLASH_Unlock();	
	if	(SSD1306_type==0)EE_WriteVariable(VirtAddVarTab[adres_eeprom_lcd_type], ((uint16_t)1) );
	else EE_WriteVariable(VirtAddVarTab[adres_eeprom_lcd_type], ((uint16_t)0) );
  FLASH_Lock();	
	}
}

uint8_t SSD1306_GetType(void ){
	return SSD1306_type;
}

//==============================================================================
// Процедура передаёт в дисплей буфер кадра из массива pBuff
//==============================================================================
void SSD1306_DisplayFullUpdate(uint8_t *pBuff, uint16_t BuffLen)
{
	//SSD1306_Wake();

	
	if (SSD1306_type==1){	
		
  SSD1306_SetColumns(0, SSD1306_Width - 1);
  SSD1306_SetPages(0, (SSD1306_Height >> 3) - 1 );				
		
  SSD1306_SendData(pBuff, BuffLen);	
	}
	else {
		
	uint8_t Buff[] = {0x00, 0x7};
  SSD1306_SendCommand(SSD1306_CMD_SetPageAddr, Buff, 2);
		
	Buff[0] = 0x00;
	Buff[1] = 0x7F;
  SSD1306_SendCommand(SSD1306_CMD_SetColumnAddr, Buff, 2);				

  SSD1306_Send(SETSTARTLINE | 0x00);
	
	SSD1306_PageAddrMode_SetPage(0x00);
	SSD1306_PageAddrMode_StartColumn(2);
  SSD1306_SendData(pBuff+(BuffLen/8)*0, BuffLen/8);
	
	SSD1306_PageAddrMode_SetPage(0x01);
	SSD1306_PageAddrMode_StartColumn(2);
  SSD1306_SendData(pBuff+(BuffLen/8)*1, BuffLen/8);	
	
	SSD1306_PageAddrMode_SetPage(0x02);
	SSD1306_PageAddrMode_StartColumn(2);
  SSD1306_SendData(pBuff+(BuffLen/8)*2, BuffLen/8);	
		
	SSD1306_PageAddrMode_SetPage(0x03);
	SSD1306_PageAddrMode_StartColumn(2);
  SSD1306_SendData(pBuff+(BuffLen/8)*3, BuffLen/8);
	
	SSD1306_PageAddrMode_SetPage(0x04);
	SSD1306_PageAddrMode_StartColumn(2);
  SSD1306_SendData(pBuff+(BuffLen/8)*4, BuffLen/8);	
	
	SSD1306_PageAddrMode_SetPage(0x05);
	SSD1306_PageAddrMode_StartColumn(2);
  SSD1306_SendData(pBuff+(BuffLen/8)*5, BuffLen/8);		
	
	SSD1306_PageAddrMode_SetPage(0x06);
	SSD1306_PageAddrMode_StartColumn(2);
  SSD1306_SendData(pBuff+(BuffLen/8)*6, BuffLen/8);			
	
	SSD1306_PageAddrMode_SetPage(0x07);
	SSD1306_PageAddrMode_StartColumn(2);
  SSD1306_SendData(pBuff+(BuffLen/8)*7, BuffLen/8);		
 }	
	//delay_ms(1000);
	//SSD1306_Sleep();
}
//==============================================================================


//==============================================================================
// Процедура устанавливает режим автосдвига указателя в буфере кадра ssd1306
//==============================================================================
void SSD1306_SetMemAdressingMode(uint8_t Mode)
{
  if (Mode > 2)
    return;
  
  SSD1306_SendCommand(SSD1306_CMD_SetMemAdressingMode | Mode , 0, 0);
}
//==============================================================================


//==============================================================================
// Процедура устанавливает начальный и конечный индекс колонки 
// для автосмещения указателя в памяти кадра при чтении записи.
//==============================================================================
void SSD1306_SetColumns(uint8_t Start, uint8_t End)
{
  Start &= 0x7F;
  End &= 0x7F;
  uint8_t Buff[] = {Start, End};
  SSD1306_SendCommand(SSD1306_CMD_SetColumnAddr, Buff, 2);
}
//==============================================================================


//==============================================================================
// Процедура устанавливает начальный и конечный индекс страницы 
// для автосмещения указателя в памяти кадра при чтении записи.
//==============================================================================
void SSD1306_SetPages(uint8_t Start, uint8_t End)
{
  Start &= 0x07;
  End &= 0x07;
  uint8_t Buff[] = {Start, End};
  SSD1306_SendCommand(SSD1306_CMD_SetPageAddr, Buff, 2);
}
//==============================================================================


//==============================================================================
// Процедура устанавливает страницу для режима страничной адрессациии, когда
// сдвиг указателя в памяти делается только по горизонтали (SSD1306_Adressing_Page).
//==============================================================================
void SSD1306_PageAddrMode_SetPage(uint8_t Page)
{
  Page &= 0x07;
  SSD1306_SendCommand(SSD1306_CMD_PageAddrMode_SetPage | Page, 0, 0);
}
//==============================================================================


//==============================================================================
// Процедура устанавливает № строки, по которой указатель в буфере кадра будет перемещаться
// Только для режима автосдвига указателя (SSD1306_Adressing_Page)
//==============================================================================
void SSD1306_PageAddrMode_StartColumn(uint8_t Start)
{
  Start &= 0x7F;
  SSD1306_SendCommand(SSD1306_CMD_PageAddrMode_StartColumnLo | (Start & 0x07), 0, 0);
  SSD1306_SendCommand(SSD1306_CMD_PageAddrMode_StartColumnHi | (Start >> 4), 0, 0);
}
//==============================================================================


//==============================================================================
// Set display RAM display start line register from 0-63
//==============================================================================
void SSD1306_SetDisplayStartLine(uint8_t Line)
{
  Line &= 0x3F;
  SSD1306_SendCommand(SSD1306_CMD_SetDisplayStartLine | Line, 0, 0);
}
//==============================================================================


//==============================================================================
// Value=0: column address 0 is mapped to SEG0 (RESET)
// Value=1: column address 127 is mapped to SEG0 
//==============================================================================
void SSD1306_SetSegmentRemap(uint8_t Value)
{
  Value = Value ? 1 : 0;
	SSD1306_SendCommand(0x80, 0, 0);
  SSD1306_SendCommand(SSD1306_CMD_SetSegmentRemap | Value, 0, 0);
}
//==============================================================================


//==============================================================================
void SSD1306_SetMultiplexRatio(uint8_t Mux)
{
  Mux--;
  Mux &= 0x3F;
  SSD1306_SendCommand(SSD1306_CMD_SetMultiplexRatio, &Mux, 1);
}
//==============================================================================


//==============================================================================
// Смещение по вертикали области отображения относительно памяти кадра
//==============================================================================
void SSD1306_SetDisplayOffset(uint8_t Offset)
{
  SSD1306_SendCommand(SSD1306_CMD_SetDisplayOffset, &Offset, 1);
}
//==============================================================================


//==============================================================================
void SSD1306_SetDisplayClockDivider(uint8_t DCLKdiv, uint8_t Fosc)
{
  DCLKdiv--;
  DCLKdiv &= 0x0F;
  DCLKdiv |= ((Fosc & 0x0F) << 4);
  SSD1306_SendCommand(SSD1306_CMD_SetDisplayClockDivider, &DCLKdiv, 1);
}
//==============================================================================


//==============================================================================
void SSD1306_ChargePumpSetting(uint8_t Value)
{
  Value = Value ? 0x14 : 0x10;
  SSD1306_SendCommand(SSD1306_CMD_ChargePumpSetting, &Value, 1);
}
//==============================================================================


//==============================================================================
// Направление сканирования
// Value=0: normal mode (RESET) Scan from COM0 to COM[N –1]
// Value=1: remapped mode. Scan from COM[N-1] to COM0
// Where N is the Multiplex ratio. 
//==============================================================================
void SSD1306_SetCOMoutScanDirection(uint8_t Value)
{
  Value = Value ? 0x08 : 0x00;
	SSD1306_SendCommand(0x80, 0, 0);
  SSD1306_SendCommand(SSD1306_CMD_SetCOMoutScanDirection | Value, 0, 0);
	
	
}
//==============================================================================


//==============================================================================
// AltCOMpinConfig=0: Sequential COM pin configuration
// AltCOMpinConfig=1(RESET): Alternative COM pinconfiguration
// LeftRightRemap=0(RESET): Disable COM Left/Right remap
// LeftRightRemap=1: Enable COM Left/Right remap 
//==============================================================================
void SSD1306_SetCOMPinsConfig(uint8_t AltCOMpinConfig, uint8_t LeftRightRemap)
{
  uint8_t tmpValue = (1 << 1);
  if (AltCOMpinConfig)
    tmpValue |= (1 << 4);
  if (LeftRightRemap)
    tmpValue |= (1 << 5);
  SSD1306_SendCommand(SSD1306_CMD_SetCOMPinsConfig, &tmpValue, 1);
}
//==============================================================================


//==============================================================================
 void SSD1306_SetPrechargePeriod(uint8_t Phase1period, uint8_t Phase2period)
{
  Phase1period &= 0x0F;
  Phase1period &= 0x0F;
  if (!Phase1period)
    Phase1period = 2;
  if (!Phase2period)
    Phase2period = 2;
  Phase1period |= (Phase2period << 4);
  SSD1306_SendCommand(SSD1306_CMD_SetPrechargePeriod, &Phase1period, 1);
}
//==============================================================================


//==============================================================================
void SSD1306_SetVCOMHDeselectLevel(uint8_t Code)
{
  Code &= 0x70;
  SSD1306_SendCommand(SSD1306_CMD_SetVCOMHDeselectLevel, &Code, 1);
}
//==============================================================================


//==============================================================================
// Процедура отключает скролл (средствами контроллера дисплея
//==============================================================================
void SSD1306_DeactivateScroll(void)
{
  SSD1306_SendCommand(SSD1306_CMD_DeactivateScroll, 0, 0);
}
//==============================================================================


//==============================================================================
// Процедура переводит дисплей в режим сна
//==============================================================================
void SSD1306_Sleep(void)
{
  SSD1306_SendCommand(SSD1306_CMD_Sleep, 0, 0);
	SSD1306_on=0;
}
//==============================================================================


//==============================================================================
// Процедура выводит дисплей из режима сна
//==============================================================================
void SSD1306_Wake(void)
{
  SSD1306_SendCommand(SSD1306_CMD_Wake, 0, 0);
	SSD1306_on=1;	
}
//==============================================================================


//==============================================================================
// Процедура включает все пиксели дисплея (Тест индикатора)
//==============================================================================
void SSD1306_AllPixOn(void)
{
  SSD1306_SendCommand(SSD1306_CMD_AllPixOn, 0, 0);
}
//==============================================================================


//==============================================================================
// Процедура отключает тест дисплея и выводит на него картинку из буфера кадра в ssd1306
//==============================================================================
void SSD1306_AllPixRAM(void)
{
  SSD1306_SendCommand(SSD1306_CMD_AllPixRAM, 0, 0);
}
//==============================================================================


//==============================================================================
// Процедура включает инверсию дисплея
//==============================================================================
void SSD1306_SetInverseOn(void)
{
	SSD1306_invers=0;
  SSD1306_SendCommand(SSD1306_CMD_SetInverseOn, 0, 0);
}
//==============================================================================


//==============================================================================
// Процедура отключает инверсию дисплея
//==============================================================================
void SSD1306_SetInverseOff(void)
{
	SSD1306_invers=1;
  SSD1306_SendCommand(SSD1306_CMD_SetInverseOff, 0, 0);
}
//==============================================================================


//==============================================================================
// Процедура устанавливает параметр контрастности (0-255)
//==============================================================================
void SSD1306_SetContrast(uint8_t Value)
{
  SSD1306_SendCommand(SSD1306_CMD_SetContrast, &Value, 1);
}
//==============================================================================


//==============================================================================
// Процедура настройки ножек микроконтроллера для обмена с дисплеем
//==============================================================================
void SSD1306_GPIO_init(void)
{
#if (SSD1306_IfMode == SSD1306_IfMode_SPI_4w) 	
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;	
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	
  
#if (SSD1306_ResetPinUsed)      // Включено управление ножкой reset ssd1306
  gpio_PortClockStart(SSD1306_Reset_Port);
  GPIO_InitStruct.GPIO_Pin = SSD1306_Reset_Pin;
  GPIO_Init(SSD1306_Reset_Port, &GPIO_InitStruct);
#endif
  
#if (SSD1306_CSPinUsed)
  gpio_PortClockStart(SSD1306_CS_Port);
  GPIO_InitStruct.GPIO_Pin = SSD1306_CS_Pin;
  GPIO_Init(SSD1306_CS_Port, &GPIO_InitStruct);
#endif
  
      // Работаем по SPI-4w
  gpio_PortClockStart(SSD1306_DC_Port);
  GPIO_InitStruct.GPIO_Pin = SSD1306_DC_Pin;
  GPIO_Init(SSD1306_DC_Port, &GPIO_InitStruct);
#endif
}
//==============================================================================
//==============================================================================
// Процедура устанавливает яркость дисплея
//==============================================================================
void SSD1306_SetBrightness(uint8_t Value, uint8_t save)
{
	Brightness=Value;
  SSD1306_SetContrast(Value);
	if (save){
	FLASH_Unlock();	
	EE_WriteVariable(VirtAddVarTab[adres_eeprom_lcd_brightness], ((uint16_t)Brightness) );
  FLASH_Lock();	
	}
}
//==============================================================================
//==============================================================================
// Процедура устанавливает яркость дисплея
//==============================================================================
uint8_t SSD1306_GetBrightness(void)
{
	return Brightness;  
}
//==============================================================================
void sendCommand(uint8_t c)
{
  SSD1306_SendCommand(c, 0, 0);
}

void sendCommand_p(uint8_t c, uint8_t p )
{
  SSD1306_SendCommand(c, &p, 1);
}
//==============================================================================
// Процедура инициализации дисплея на контроллере ssd1306
//==============================================================================
void SSD1306_Init(uint8_t Width, uint8_t Height)
{
  SSD1306_Width = Width;
  SSD1306_Height = Height;
  
  SSD1306_GPIO_init();
  SSD1306_on=1;	
  // Инициализация интерфейса
#if (SSD1306_IfMode == SSD1306_IfMode_I2C)      // I2C
  //i2cm_init(SSD1306_I2C_periph, SSD1306_I2C_clock);
	i2cSoft_Init();	
	
#elif (SSD1306_IfMode == SSD1306_IfMode_SPI_4w)       // Параметры работы по SPI-4w
  spim_init(SSD1306_SPI_periph, 8);
#else 
  // Остальные интерфейсы работы с ssd1306 пока не поддерживаются
#endif

  // Сброс контроллера дисплея ssd1306 ножкой Reset
#if (SSD1306_ResetPinUsed)      // Включено управление ножкой reset ssd1306
  SSD1306_RESET_HIGH();
  delay_ms(20);
  SSD1306_RESET_LOW();  // Роняем ножку reset в 0 на 10 мс
  delay_ms(150);
  SSD1306_RESET_HIGH();
#endif
  
//	#if SSD1306_SH1106
  // Шлём команды инициализации ssd1306
  SSD1306_Sleep();
  SSD1306_SetDisplayClockDivider(1, 8);
  SSD1306_SetMultiplexRatio(SSD1306_Height);
  SSD1306_SetDisplayOffset(0);
  SSD1306_SetDisplayStartLine(0);
  SSD1306_ChargePumpSetting(1); // умножитель
	if (SSD1306_type==0){		
    SSD1306_SetMemAdressingMode(SSD1306_Adressing_Horizontal);
	}
	else {	
  SSD1306_SetMemAdressingMode(SSD1306_Adressing_Page);
  }
	
  SSD1306_SetCOMoutScanDirection(1);    // *переворачивает оторбражение на матрице (только по вертикали)
  SSD1306_SetSegmentRemap(1 );          // *меняет направление заполнение матрицы из буфера кадра (вертикаль/горизонталь)
		
  if ((SSD1306_Width == 128) && (SSD1306_Height == 32))
    SSD1306_SetCOMPinsConfig(0, 0);
  else  if ((SSD1306_Width == 128) && (SSD1306_Height == 64))
    SSD1306_SetCOMPinsConfig(1, 0);
  else  if ((SSD1306_Width == 96) && (SSD1306_Height == 16))
    SSD1306_SetCOMPinsConfig(0, 0);
	
  //==============================================================================
// Процедура устанавливает страницу для режима страничной адрессациии, когда
// сдвиг указателя в памяти делается только по горизонтали (SSD1306_Adressing_Page).
//==============================================================================
 SSD1306_PageAddrMode_SetPage(0x0);
//==============================================================================
// Процедура устанавливает № строки, по которой указатель в буфере кадра будет перемещаться
// Только для режима автосдвига указателя (SSD1306_Adressing_Page)
//==============================================================================
 SSD1306_PageAddrMode_StartColumn(0);
	
  SSD1306_SetContrast(Brightness);
  SSD1306_SetPrechargePeriod(2, 2);
  SSD1306_SetVCOMHDeselectLevel(0x40);
  SSD1306_AllPixRAM();
  SSD1306_SetInverseOff();
  SSD1306_DeactivateScroll();
  SSD1306_Wake();	

}
//==============================================================================
void SSD1306_Rotate(uint8_t Value, uint8_t save){
	if (Value==0){
    SSD1306_SetCOMoutScanDirection(1);    // *переворачивает оторбражение на матрице (только по вертикали)
    SSD1306_SetSegmentRemap(1);          // *меняет направление заполнение матрицы из буфера кадра (вертикаль/горизонталь)	
	}
	else {
    SSD1306_SetCOMoutScanDirection(0);    // *переворачивает оторбражение на матрице (только по вертикали)
    SSD1306_SetSegmentRemap(0);          // *меняет направление заполнение матрицы из буфера кадра (вертикаль/горизонталь)			
	}
	if (save){
	FLASH_Unlock();	
	EE_WriteVariable(VirtAddVarTab[adres_eeprom_lcd_rotate], ((uint16_t)Value) );
  FLASH_Lock();	
	}
}

void SSD1306_Set_invers(uint8_t invers, uint8_t save)
{	
	if (invers==0){
		//SSD1306_SetBrightness(127);
		SSD1306_SetInverseOn();
	}
	else {
		//SSD1306_SetBrightness(255);
		SSD1306_SetInverseOff();
	}
	//SSD1306_invers=invers;
	if (save){
	FLASH_Unlock();	
	EE_WriteVariable(VirtAddVarTab[adres_eeprom_lcd_invers], ((uint16_t)SSD1306_invers) );
  FLASH_Lock();	
	}
}

uint8_t SSD1306_Get_invers(void)
{
	return  SSD1306_invers;	
	
}
