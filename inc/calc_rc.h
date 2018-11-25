

#ifndef _CALC_RC_H_
#define _CALC_RC_H_

#include <complex.h>
typedef complex double complexf;


#include "adc.h"
#include "process_measure.h"

void OnCalculate(bool useCorrector);

extern complexf lastZx;
extern bool lastZxFilled;


#endif//_CALC_RC_H_
