

#ifndef _FORMAT_PRINT_H_
#define _FORMAT_PRINT_H_

void printInt(int32_t value, uint8_t font);
void printIntFixed(int32_t value, uint8_t font, uint8_t aMinDigits, uint8_t aFixedPoint);

void printC(float aValue, uint8_t font);
void printR(float aValue, uint8_t font);
void printV(float aValue);
void printT(float aValue);
void printF(float aValue);
void printL(float aValue, uint8_t font);


void printX2size(LcdFontSize size);//Set FONT_1X or FONT_2X
void printRX2(float aValue,uint8_t x, uint8_t y);
void printLX2(float aValue,uint8_t x, uint8_t y);
void printCX2(float aValue,uint8_t x, uint8_t y);
void printG(float aValue, uint8_t x, uint8_t y);
void printQuality(float aValue, uint8_t x, uint8_t y);

#endif//_FORMAT_PRINT_H_
