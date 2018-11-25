

#ifndef PARSER_H
#define PARSER_H

#include "stm32f30x.h"

#define SIZE_RECEIVE_BUF  32

#define AMOUNT_PAR 4

void PARS_Init(void);
void PARS_Parser(char sym);

extern void PARS_Handler(uint8_t argc, char *argv[]);

uint8_t PARS_EqualStr(char *s1, char *s2);
//uint8_t PARS_EqualStrFl(char *s1, char const *s2);
uint8_t PARS_StrToUchar(char *s);
uint16_t PARS_StrToUint(char *s);
uint32_t PARS_StrToUint32(char *s);


#endif //PARSER_H

