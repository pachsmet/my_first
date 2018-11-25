// balmer@inbox.ru RLC Meter 303
// 2013-2014

#ifndef _HW_PCD8544_H_
#define _HW_PCD8544_H_

#include <stdbool.h>
#include <stdint.h>
//LCD Hardware routines

void HwLcdInit(void);
//Подождать несколько милисекунд
void DelaySome(void);

//Управление пинами 0 - low, 1 - high
void HwLcdPinCE(uint8_t on);
void HwLcdPinDC(uint8_t on);
void HwLcdPinRst(uint8_t on);
void HwLcdSend(uint16_t data); //send spi data

#endif//_HW_PCD8544_H_
