

/**** High level ****/
#include "stm32f30x.h"
#include "string.h"
/* Command or data */
typedef enum
{
	COMMAND = 0,
	DATA = 1
} lcd_cd_t;

/* Point str type */
typedef struct
{
	uint16_t x_point;
	uint16_t y_point;
} point_t;

/* Point mode type */
typedef enum
{
	PIXEL_CLEAR = 0,
	PIXEL_SET = 1,
	PIXEL_SETXOR = 2	// Note: PIXEL_XOR mode affects only to vram_put_point().
					// Graph functions only transmit parameter to vram_put_point() and result of graph functions
					// depends on it's implementation. However Line() and Bar() functions work well.
} point_mode_t;

/* Font descriptor type */
typedef struct
{
	uint16_t char_width;
	uint16_t char_offset;
} font_descriptor_t;

/* Font info type */
typedef struct
{
	uint16_t font_height;
	uint16_t start_char;
	uint16_t end_char;
	const font_descriptor_t* descr_array;
	const unsigned char* font_bitmap_array;
} font_info_t;

// Font information for Tahoma 10pt
extern const font_info_t tahoma_10ptFontInfo;
