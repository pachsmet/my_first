#ifndef _MENU_H_
#define _MENU_H_

void MenuRepaint(void);
bool MenuIsOpen(void);

void MessageBox(char* line1);
void MessageBox2(char* line1, char* line2);
void OnCalibrationComplete(void);
void MenuSetSerial(bool ser, uint8_t save);
bool is_menu( void );
void menu_clear( void );

#endif//_MENU_H_
