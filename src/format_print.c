
#include "stm32f30x.h"
#include <stdio.h>
#include "lcd.h"

#include "hw_config.h"

char Buffer[32];
extern bool printError;

LcdFontSize g_printX2size = FONT_2X;

void addCommaToBuffer(uint8_t posComma)
{
	uint8_t size = 0;
	for(;size<sizeof(Buffer) && Buffer[size]; size++);

	if(size>=posComma)
	{
		for(uint8_t i=size; i>size-posComma; i--)
		{
			Buffer[i] = Buffer[i-1];
		}

		Buffer[size+1] = 0;
		Buffer[size-posComma] = '.';
	}
}

void sprintIntFormat(int32_t value, uint8_t aMinDigits, uint8_t aEmptyChar)
{
	char* buf = Buffer;
    uint32_t valueUnsigned;
	if(value<0)
	{
		*buf++ = '-';
		valueUnsigned = -value;
	} else
    {
        valueUnsigned = value;
    }
    
	int8_t digits = 0;
	for(uint32_t v=valueUnsigned; v>0; digits++)
	{
		v/=10;
	}
    
	if(value==0)
	{
		digits = 1;
	}
    
	for(int8_t i=digits; i<aMinDigits; i++)
	{
		*buf++ = aEmptyChar;
	}
	
	buf[digits--]=0;
	for(uint32_t v=valueUnsigned; digits>=0; digits--)
	{
		buf[digits] = (v%10)+'0';
		v /= 10;
	}
}

void printIntFormat(int32_t value, uint8_t font, uint8_t aMinDigits, uint8_t aEmptyChar)
{
	sprintIntFormat(value, aMinDigits, aEmptyChar);
	LcdStr(font, Buffer);
}

void printIntFixed(int32_t value, uint8_t font, uint8_t aMinDigits, uint8_t aFixedPoint)
{
	sprintIntFormat(value, aMinDigits, '0');
	addCommaToBuffer(aFixedPoint);
	LcdStr(font, Buffer);
}

void printInt(int32_t value, uint8_t font)
{
	printIntFormat(value, font, 1, ' ');
}
static const char*  const strQuality[] ={"m","" };
static const char*  const strGradus[] ={"m","" };
static const char*  const strD[] ={"m","" };
static const char*  const strResistor[] ={"mR", "R", "kR", "MR", "GR"};
static const char*  const strCapacitor[] = {"pF", "nF","uF","mF","F"};
static const char*  const strInductor[] = {"nH","uH","mH","H"};

//static const char*  const strVoltage[] ={"mkV", "mV", "V"};
//static const char*  const strTime[] ={"us", "ms", "s"};
//static const char*  const strFrequency[] ={"mHz", "Hz", "kHz", "MHz"};


void formatPrint(char* aBuffer, float aValue, const char*const* aSuffix, uint8_t aSuffixCount, int8_t aSuffixMin)
{
	float mul = 1;
	int8_t iSuffix = 0;

	if(aValue<0)
	{
		*aBuffer++ = '-';
		aValue = - aValue;
	}

	if(aValue>=1)
	{
		for(iSuffix = -aSuffixMin; iSuffix<(int8_t)aSuffixCount; iSuffix++)
		{
			float mul1 = mul*1000;
			if(aValue<mul1)
				goto FormatValue;
			mul = mul1; 
		}

		*aBuffer++ = 'i';
		*aBuffer++ = 'n';
		*aBuffer++ = 'f';
		iSuffix = -aSuffixMin;
		goto AddSufix;
	}else
	{
		for(iSuffix = -aSuffixMin; iSuffix>0; iSuffix--)
		{
			if(aValue>=mul)
				break;
			mul *= 1e-3f;
		}
	}


FormatValue:;
	aValue /= mul;

	{
		int value;

		if(aValue>=200)
		{
			value = (int)(aValue+0.5f);
			char c = (value/1000);
			if(c)
				*aBuffer++ = '0'+c;
			*aBuffer++ = '0'+((value/100)%10);
			*aBuffer++ = '0'+(char)((value/10)%10);
			*aBuffer++ = '0'+(char)(value%10);
		} else
		if(aValue>=20)
		{
			value = (int)(aValue*10+0.5f);
			char c = (value/1000);
			if(c)
				*aBuffer++ = '0'+c;
			*aBuffer++ = '0'+(value/100)%10;
			*aBuffer++ = '0'+(char)((value/10)%10);
			*aBuffer++ = '.';
			*aBuffer++ = '0'+(char)(value%10);
		} else
		{
			value = (int)(aValue*100+0.5f);
			char c = (value/1000);
			if(c)
				*aBuffer++ = '0'+c;
			*aBuffer++ = '0'+(value/100)%10;
			*aBuffer++ = '.';
			*aBuffer++ = '0'+(char)((value/10)%10);
			*aBuffer++ = '0'+(char)(value%10);
		}
	}

AddSufix:;
	const char* s = aSuffix[iSuffix];
	//*aBuffer++ = ' ';
	while(*s)
		*aBuffer++ = *s++;
	*aBuffer = 0;
}

//void printC(float aValue, uint8_t font)
//{
//	formatPrint(Buffer, aValue, strCapacitor, sizeof(strCapacitor)/sizeof(strCapacitor[0]), -4);
//	LcdStr(font, Buffer);	
//}

void printR(float aValue, uint8_t font)
{
	formatPrint(Buffer, aValue, strResistor, sizeof(strResistor)/sizeof(strResistor[0]), -1);
	LcdStr(font, Buffer);	
}



//void printV(float aValue)
//{
//	formatPrint(Buffer, aValue, strVoltage, sizeof(strVoltage)/sizeof(strVoltage[0]), -2);
//	LcdStr(FONT_1X, Buffer);	
//}

//void printT(float aValue)
//{
//	formatPrint(Buffer, aValue, strTime, sizeof(strTime)/sizeof(strTime[0]), -2);
//	LcdStr(FONT_1X, Buffer);	
//}

//void printF(float aValue)
//{
//	formatPrint(Buffer, aValue, strFrequency, sizeof(strFrequency)/sizeof(strFrequency[0]), -1);
//	LcdStr(FONT_1X, Buffer);	
//}

//void printL(float aValue, uint8_t font)
//{
//	formatPrint(Buffer, aValue, strInductor, sizeof(strInductor)/sizeof(strInductor[0]), -3);
//	LcdStr(font, Buffer);	
//}



void formatPrintX2(uint8_t x, uint8_t y,char* aBuffer, float aValue,
	 const char*const* aSuffix, uint8_t aSuffixCount, int8_t aSuffixMin)
{
	char* startBuffer = aBuffer;
	float mul = 1;
	int8_t iSuffix = 0;
  uint8_t minus=0;
	
	if(aValue<0)
	{
		if (g_printX2size==FONT_2X) LcdSingleBar  ( 1, (y+1)*8+2, 2, 6, PIXEL_ON);
		aValue = -aValue;
		minus=1;
	}

	if(aValue>=1)
	{
		for(iSuffix = -aSuffixMin; iSuffix<(int8_t)aSuffixCount; iSuffix++)
		{
			float mul1 = mul*1000;
			if(aValue<mul1)
				goto FormatValue;
			mul = mul1; 
		}
    return;
//		*aBuffer++ = 'i';
//		*aBuffer++ = 'n';
//		*aBuffer++ = 'f';
//		iSuffix = -aSuffixMin;
//		goto AddSufix;
	}else
	{
		for(iSuffix = -aSuffixMin; iSuffix>0; iSuffix--)
		{
			if(aValue>=mul)
				break;
			mul *= 1e-3f;
		}
	}


FormatValue:;
	aValue /= mul;

	{
		int value;
		
		if(aValue>=1000)
		{
			value = (int)(aValue);//+0.5f
			char c = (value/1000);
			if(c)
			*aBuffer++ = '0'+c;
			*aBuffer++ = '0'+((value/100)%10);
			*aBuffer++ = '0'+(char)((value/10)%10);
			*aBuffer++ = '0'+(char)(value%10);
		} else
		if(aValue>=100)
		{
			value = (int)(aValue*10);//+0.5f
			char c = (value/1000);
			if(c)
			*aBuffer++ = '0'+c;
			*aBuffer++ = '0'+(value/100)%10;
			*aBuffer++ = '0'+(char)((value/10)%10);
			*aBuffer++ = '.';
			*aBuffer++ = '0'+(char)(value%10);
		}
		else
		if(aValue>=10)
		{
			value = (int)(aValue*100);//+0.5f
			char c = (value/1000);
			if(c)
			*aBuffer++ = '0'+c;
			*aBuffer++ = '0'+(value/100)%10;
			*aBuffer++ = '.';
			*aBuffer++ = '0'+(char)((value/10)%10);
			*aBuffer++ = '0'+(char)(value%10);
		} else
		{
			value = (int)(aValue*1000);//+0.5f
			char c = (value/1000);
			*aBuffer++ = '0'+c;
			*aBuffer++ = '.';
			*aBuffer++ = '0'+(value/100)%10;
			*aBuffer++ = '0'+(char)((value/10)%10);
			*aBuffer++ = '0'+(char)(value%10);
		}
	}

AddSufix:;
	*aBuffer = 0;
	//LcdGotoXYFont(2, y+2);
	//LcdStr(g_printX2size, startBuffer);
	
	if (g_printX2size==FONT_2X){
	
  lcd_putnum( 10, y+1, startBuffer);
	
	if (printError == true){
	  LcdGotoXYFont(12, y+2);
	  LcdStr(FONT_1X, aSuffix[iSuffix]);
	}
	else{
	  if (y==2)LcdGoto(70, 20);
	  else LcdGoto(70, 44);	
	  LcdPuts(aSuffix[iSuffix], &Font_11x18, 1);
	}		
}
	else 	{
		LcdGotoXYFont(x, y);
		if (minus) LcdStr(FONT_1X, "-");
	  LcdStr(FONT_1X, startBuffer);		
	  LcdStr(FONT_1X, aSuffix[iSuffix]);		
	}
	

	
}

void printRX2(float aValue,uint8_t x, uint8_t y)
{
	formatPrintX2(x, y, Buffer, aValue, strResistor, sizeof(strResistor)/sizeof(strResistor[0]), -1);
}

void printLX2(float aValue,uint8_t x, uint8_t y)
{
	formatPrintX2(x, y, Buffer, aValue, strInductor, sizeof(strInductor)/sizeof(strInductor[0]), -3);
}

void printCX2(float aValue,uint8_t x, uint8_t y)
{
	formatPrintX2(x, y, Buffer, aValue, strCapacitor, sizeof(strCapacitor)/sizeof(strCapacitor[0]), -4);
}

void printG(float aValue, uint8_t x, uint8_t y)
{
	formatPrintX2(x, y, Buffer, aValue, strGradus, sizeof(strGradus)/sizeof(strGradus[0]), -1);
}

void printQuality(float aValue, uint8_t x, uint8_t y)
{
	formatPrintX2(x, y, Buffer, aValue, strD, sizeof(strD)/sizeof(strD[0]), -1);
}


void printX2size(LcdFontSize size)
{
	g_printX2size = size;
}

//void printR(float aValue, uint8_t y)
//{
//	formatPrint(y, Buffer, aValue, strResistor, sizeof(strResistor)/sizeof(strResistor[0]));
//}

//void printL(float aValue, uint8_t y)
//{
//	formatPrint(y, Buffer, aValue, strInductor, sizeof(strInductor)/sizeof(strInductor[0]));
//}

//void printC(float aValue, uint8_t y)
//{
//	formatPrint(y, Buffer, aValue, strCapacitor, sizeof(strCapacitor)/sizeof(strCapacitor[0]));
//}