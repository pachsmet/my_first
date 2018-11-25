/*
 *
 * Name         :  lcd.h
 *
 * Description  :  This is header file for the lcd graphic LCD driver.
 *                 Based on the code written by Sylvain Bissonette
 *
 * Author       :  Fandi Gunawan <fandigunawan@gmail.com>
 * Website      :  http://fandigunawan.wordpress.com
 *
 * Credit       :  Sylvain Bissonette (2003)
 *                 Louis Frigon (2003)
 *
 * License      :  GPL v. 3
 *
 * Compiler     :  WinAVR, GCC for AVR platform
 *                 Tested version :
 *                 - 20070525
 *                 - 20071221
 *                 - 20081225
 * Compiler note:  Please be aware of using older/newer version since WinAVR
 *                 is in extensive development. Please compile with parameter -O1
 *
 * History      :
 * Please refer to lcd.c
 */

#ifndef _lcd_H_
#define _lcd_H_

#include "fonts.h"	
#include "segoe16.h"

/* For return value */
#define OK                         0
#define OUT_OF_BORDER              1
#define OK_WITH_WRAP               2


//#define LCD_X_RES                  84    /* x resolution */
//#define LCD_Y_RES                  48    /* y resolution */

#define LCD_X_RES                  128    /* x resolution */
#define LCD_Y_RES                  64    /* y resolution */

#define EMPTY_SPACE_BARS           2
#define BAR_X                      5
#define BAR_Y                      38

/* Cache size in bytes ( 84 * 48 ) / 8 = 504 bytes */
#define LCD_CACHE_SIZE             ( ( LCD_X_RES * LCD_Y_RES ) / 8)

/* Type definition */
//typedef char                       bool;
typedef unsigned char              byte;
typedef unsigned int               word;

/* Enumeration */

typedef enum
{
    PIXEL_OFF =  0,
    PIXEL_ON  =  1,
    PIXEL_XOR =  2

} LcdPixelMode;

typedef enum
{
    FONT_1X = 1,
    FONT_2X = 2

} LcdFontSize;

/* Function prototypes */
void LcdInit       ( void );

void LcdClear      ( void );
void LcdUpdate     ( void );
void LcdImage      ( const byte *imageData );
void LcdContrast   ( byte contrast);
byte LcdGotoXYFont ( byte x, byte y );
byte LcdChr        ( LcdFontSize size, char ch );
byte LcdStr        ( LcdFontSize size, const char* dataArray);
byte LcdPixel      ( byte x, byte y, LcdPixelMode mode );
byte LcdLine       ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode );
byte LcdRect       ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode );
byte LcdFillRect   (byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode ); 
byte LcdSingleBar  ( byte baseX, byte baseY, byte height, byte width, LcdPixelMode mode );
byte LcdBars       ( byte data[], byte numbBars, byte width, byte multiplier );
void lcd_putnum (int x, int y,char *str);
void LcdGoto(byte x, byte y);
void LcdGotoX(byte x);
void LcdGotoY(byte y);
void LcdAddX(byte x);
void LcdAddY(byte y);
char LcdPuts(const char* str, FontDef_t* Font, LcdPixelMode color);
byte LcdGotoXpicsel ( uint16_t x );


/* Graph error type */
typedef enum
{
	NO_GRAPH_ERROR = 0,
	GRAPH_ERROR = 1
} gr_error_t;

/* All graph functions return error if exceed boundaries.
 * p1 and p2 may be entered in any sequence */
/* Puts point with coordinates p to VRAM */
extern gr_error_t vram_put_point (point_t p, point_mode_t mode);
/* Draws line in VRAM from p1 to p2 */
extern gr_error_t vram_put_line (point_t p1, point_t p2, point_mode_t mode);
/* Draws rectangle in VRAM from p1 to p2 */
extern gr_error_t vram_put_rect (point_t p1, point_t p2, point_mode_t mode);
/* Draws filled rectangle in VRAM from p1 to p2 */
extern gr_error_t vram_put_bar (point_t p1, point_t p2, point_mode_t mode);
/* Draws circle in VRAM from center with radius */
extern gr_error_t vram_put_circle (point_t center, uint8_t radius, point_mode_t mode);

/* Puts character from font type pointer (begins from top left).
 * Returns error if exceeds boundaries or no char available */
extern gr_error_t vram_put_char (point_t p, const font_info_t * font, const char c, point_mode_t mode);
/* Puts string from font type pointer. Returns error if exceeds boundaries or no char available */
extern gr_error_t vram_put_str (point_t p, const font_info_t * font, const char* str, point_mode_t mode);

#endif  /*  _lcd_H_ */
