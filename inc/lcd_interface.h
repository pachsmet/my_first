

#include "lcd.h"
#include "adc.h"

void LcdRepaint(void);
void LcdRepaint_RVI(void);

void printInt(int32_t value, uint8_t font);
float errorRelative(AdcSummaryChannel* ch);

extern int printD;
extern float printGradus;

extern double Rre;
extern double Rim;

extern bool printRim;
extern bool isSerial;
extern bool valueIsC;
extern bool calculatedValues;
extern double valueL;
extern double valueC;

extern bool printError;
