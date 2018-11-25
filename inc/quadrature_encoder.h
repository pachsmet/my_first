

#include <stdbool.h>

void QuadEncInit(void);

/*
Раз в 20 ms проверяется не нажата ли кнопка и не повернулось ли колесико.
Если событие произошло, то вызывается соответсвующий обработчик.
*/
void OnButtonPressed(void);
void OnWeel(int16_t delta);
void OnTimer(void);
void Set_Timer_off(uint16_t sec, uint8_t save);
void Reset_Timer_off(void);
uint16_t Get_Timer_off(void);
