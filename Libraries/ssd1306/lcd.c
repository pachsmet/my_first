/*
 *
 * Name         :  lcd.c
 *
 * Description  :  This is a driver for the lcd graphic LCD.
 *                 Based on the code written by Sylvain Bissonette
 *                 This driver is buffered in 504 bytes memory be sure
 *                 that your MCU having bigger memory
 *
 * Author       :  Fandi Gunawan <fandigunawan@gmail.com>
 * Website      :  http://fandigunawan.wordpress.com
 *
 * Credit       :  Sylvain Bissonette (2003)
 *
 * License      :  GPL v. 3
 *
 * Compiler     :  WinAVR, GCC for AVR platform
 *                 Tested version :
 *                 - 20070525 (avr-libc 1.4)
 *                 - 20071221 (avr-libc 1.6)
 *                 - 20081225 tested by Jakub Lasinski
 *                 - other version please contact me if you find out it is working
 * Compiler note:  Please be aware of using older/newer version since WinAVR
 *                 is under extensive development. Please compile with parameter -O1
 *
 * History      :
 * Version 0.2.6 (March 14, 2009) additional optimization by Jakub Lasinski
 * + Optimization using Memset and Memcpy
 * + Bug fix and sample program reviewed
 * + Commented <stdio.h>
 * Version 0.2.5 (December 25, 2008) x-mas version :)
 * + Boundary check is added (reported by starlino on Dec 20, 2008)
 * + Return value is added, it will definitely useful for error checking
 * Version 0.2.4 (March 5, 2008)
 * + Multiplier was added to LcdBars to scale the bars
 * Version 0.2.3 (February 29, 2008)
 * + Rolled back LcdFStr function because of serious bug
 * + Stable release
 * Version 0.2.2 (February 27, 2008)
 * + Optimizing LcdFStr function
 * Version 0.2.1 (January 2, 2008)
 * + Clean up codes
 * + All settings were migrated to lcd.h
 * + Using _BV() instead of << to make a better readable code
 * Version 0.2 (December 11-14, 2007)
 * + Bug fixed in LcdLine() and LcdStr()
 * + Adding new routine
 *    - LcdFStr()
 *    - LcdSingleBar()
 *    - LcdBars()
 *    - LcdRect()
 *    - LcdImage()
 * + PROGMEM used instead of using.data section
 * Version 0.1 (December 3, 2007)
 * + First stable driver
 *
 * Note         :
 * Font will be displayed this way (16x6)
 * 1 2 3 4 5 6 7 8 9 0 1 2 3 4
 * 2
 * 3
 * 4
 * 5
 * 6
 *
 * Contributor : 
 * + Jakub Lasinski
 

 */

#include <stdbool.h>
#include <string.h>
#include "lcd.h"
//#include "hw_lcd.h"
#include "fonts.h"
#include <ssd1306.h>
 /*
 * Character lookup table code was taken from the work of Sylvain Bissonette
 * This table defines the standard ASCII characters in a 5x7 dot format.
 */
static const byte FontLookup [][5] =
{
	

	{0x00, 0x00, 0x00, 0x00, 0x00},// (space)
	{0x00, 0x00, 0x5F, 0x00, 0x00},// !
	{0x00, 0x07, 0x00, 0x07, 0x00},// "
	{0x14, 0x7F, 0x14, 0x7F, 0x14},// #
	{0x24, 0x2A, 0x7F, 0x2A, 0x12},// $
	{0x23, 0x13, 0x08, 0x64, 0x62},// %
	{0x36, 0x49, 0x55, 0x22, 0x50},// &
	{0x00, 0x05, 0x03, 0x00, 0x00},// '
	{0x00, 0x1C, 0x22, 0x41, 0x00},// (
	{0x00, 0x41, 0x22, 0x1C, 0x00},// )
	{0x08, 0x2A, 0x1C, 0x2A, 0x08},// *
	{0x08, 0x08, 0x3E, 0x08, 0x08},// +
	{0x00, 0x50, 0x30, 0x00, 0x00},// ,
	{0x08, 0x08, 0x08, 0x08, 0x08},// -
	{0x00, 0x60, 0x60, 0x00, 0x00},// .
	{0x20, 0x10, 0x08, 0x04, 0x02},// /
	{0x3E, 0x51, 0x49, 0x45, 0x3E},// 0
	{0x00, 0x42, 0x7F, 0x40, 0x00},// 1
	{0x42, 0x61, 0x51, 0x49, 0x46},// 2
	{0x21, 0x41, 0x45, 0x4B, 0x31},// 3
	{0x18, 0x14, 0x12, 0x7F, 0x10},// 4
	{0x27, 0x45, 0x45, 0x45, 0x39},// 5
	{0x3C, 0x4A, 0x49, 0x49, 0x30},// 6
	{0x01, 0x71, 0x09, 0x05, 0x03},// 7
	{0x36, 0x49, 0x49, 0x49, 0x36},// 8
	{0x06, 0x49, 0x49, 0x29, 0x1E},// 9
	{0x00, 0x36, 0x36, 0x00, 0x00},// :
	{0x00, 0x56, 0x36, 0x00, 0x00},// ;
	{0x00, 0x08, 0x14, 0x22, 0x41},// <
	{0x14, 0x14, 0x14, 0x14, 0x14},// =
	{0x41, 0x22, 0x14, 0x08, 0x00},// >
	{0x02, 0x01, 0x51, 0x09, 0x06},// ?
	{0x32, 0x49, 0x79, 0x41, 0x3E},// @
	{0x7E, 0x11, 0x11, 0x11, 0x7E},// A
	{0x7F, 0x49, 0x49, 0x49, 0x36},// B
	{0x3E, 0x41, 0x41, 0x41, 0x22},// C
	{0x7F, 0x41, 0x41, 0x22, 0x1C},// D
	{0x7F, 0x49, 0x49, 0x49, 0x41},// E
	{0x7F, 0x09, 0x09, 0x01, 0x01},// F
	{0x3E, 0x41, 0x41, 0x51, 0x32},// G
	{0x7F, 0x08, 0x08, 0x08, 0x7F},// H
	{0x00, 0x41, 0x7F, 0x41, 0x00},// I
	{0x20, 0x40, 0x41, 0x3F, 0x01},// J
	{0x7F, 0x08, 0x14, 0x22, 0x41},// K
	{0x7F, 0x40, 0x40, 0x40, 0x40},// L
	{0x7F, 0x02, 0x04, 0x02, 0x7F},// M
	{0x7F, 0x04, 0x08, 0x10, 0x7F},// N
	{0x3E, 0x41, 0x41, 0x41, 0x3E},// O
	{0x7F, 0x09, 0x09, 0x09, 0x06},// P
	{0x3E, 0x41, 0x51, 0x21, 0x5E},// Q
	{0x7F, 0x09, 0x19, 0x29, 0x46},// R
	{0x46, 0x49, 0x49, 0x49, 0x31},// S
	{0x01, 0x01, 0x7F, 0x01, 0x01},// T
	{0x3F, 0x40, 0x40, 0x40, 0x3F},// U
	{0x1F, 0x20, 0x40, 0x20, 0x1F},// V
	{0x7F, 0x20, 0x18, 0x20, 0x7F},// W
	{0x63, 0x14, 0x08, 0x14, 0x63},// X
	{0x03, 0x04, 0x78, 0x04, 0x03},// Y
	{0x61, 0x51, 0x49, 0x45, 0x43},// Z
	{0x00, 0x00, 0x7F, 0x41, 0x41},// [
	{0x02, 0x04, 0x08, 0x10, 0x20},// "\"
	{0x41, 0x41, 0x7F, 0x00, 0x00},// ]
	{0x04, 0x02, 0x01, 0x02, 0x04},// ^
	{0x40, 0x40, 0x40, 0x40, 0x40},// _
	{0x00, 0x01, 0x02, 0x04, 0x00},// `
	{0x20, 0x54, 0x54, 0x54, 0x78},// a
	{0x7F, 0x48, 0x44, 0x44, 0x38},// b
	{0x38, 0x44, 0x44, 0x44, 0x20},// c
	{0x38, 0x44, 0x44, 0x48, 0x7F},// d
	{0x38, 0x54, 0x54, 0x54, 0x18},// e
	{0x08, 0x7E, 0x09, 0x01, 0x02},// f
	{0x08, 0x14, 0x54, 0x54, 0x3C},// g
	{0x7F, 0x08, 0x04, 0x04, 0x78},// h
	{0x00, 0x44, 0x7D, 0x40, 0x00},// i
	{0x20, 0x40, 0x44, 0x3D, 0x00},// j
	{0x00, 0x7F, 0x10, 0x28, 0x44},// k
	{0x00, 0x41, 0x7F, 0x40, 0x00},// l
	{0x7C, 0x04, 0x18, 0x04, 0x78},// m
	{0x7C, 0x08, 0x04, 0x04, 0x78},// n
	{0x38, 0x44, 0x44, 0x44, 0x38},// o
	{0x7C, 0x14, 0x14, 0x14, 0x08},// p
	{0x08, 0x14, 0x14, 0x18, 0x7C},// q
	{0x7C, 0x08, 0x04, 0x04, 0x08},// r
	{0x48, 0x54, 0x54, 0x54, 0x20},// s
	{0x04, 0x3F, 0x44, 0x40, 0x20},// t
	{0x3C, 0x40, 0x40, 0x20, 0x7C},// u
	{0x1C, 0x20, 0x40, 0x20, 0x1C},// v
	{0x3C, 0x40, 0x30, 0x40, 0x3C},// w
	{0x44, 0x28, 0x10, 0x28, 0x44},// x
	{0x0C, 0x50, 0x50, 0x50, 0x3C},// y
	{0x44, 0x64, 0x54, 0x4C, 0x44},// z
	//{0x00, 0x08, 0x36, 0x41, 0x00},// {
	{0x07, 0x05, 0x07, 0x00, 0x00 },    /* { -> GRADUS */
	{0x00, 0x00, 0x7F, 0x00, 0x00},// |
	{0x00, 0x41, 0x36, 0x08, 0x00},// }
	{0x08, 0x08, 0x2A, 0x1C, 0x08},// ->
	{0x08, 0x1C, 0x2A, 0x08, 0x08},// <- (0x7f)


	{0x7E, 0x11, 0x11, 0x11, 0x7E},// A  0x80
	{0x7F, 0x49, 0x49, 0x49, 0x30},// Б
	{0x7F, 0x49, 0x49, 0x49, 0x36},// B
	{0x7F, 0x01, 0x01, 0x01, 0x01},// Г
	{0x60, 0x3E, 0x21, 0x3F, 0x60},// Д
	{0x7F, 0x49, 0x49, 0x49, 0x41},// Е
	{0x67, 0x18, 0x7F, 0x18, 0x67},// Ж
	{0x22, 0x41, 0x49, 0x49, 0x36},// 3
	{0x7F, 0x10, 0x08, 0x04, 0x7F},// И
	{0x7F, 0x10, 0x09, 0x04, 0x7F},// Й
	{0x7F, 0x08, 0x14, 0x22, 0x41},// K
	{0x40, 0x3E, 0x01, 0x01, 0x7E},// Л
	{0x7F, 0x02, 0x04, 0x02, 0x7F},// M
	{0x7F, 0x08, 0x08, 0x08, 0x7F},// H
	{0x3E, 0x41, 0x41, 0x41, 0x3E},// O
	{0x7F, 0x01, 0x01, 0x01, 0x7F},// П
	{0x7F, 0x09, 0x09, 0x09, 0x06},// P
	{0x3E, 0x41, 0x41, 0x41, 0x22},// C
	{0x01, 0x01, 0x7F, 0x01, 0x01},// T
	{0x27, 0x48, 0x48, 0x48, 0x3F},// У
	{0x1E, 0x21, 0x7F, 0x21, 0x1E},// Ф
	{0x63, 0x14, 0x08, 0x14, 0x63},// X
	{0x3F, 0x20, 0x20, 0x3F, 0x40},// Ц
	{0x1F, 0x10, 0x10, 0x10, 0x7F},// Ч
	{0x7F, 0x40, 0x7F, 0x40, 0x7F},// Ш
	{0x3F, 0x20, 0x3F, 0x20, 0x7F},// Щ
	{0x01, 0x7F, 0x44, 0x44, 0x38},// Ъ
	{0x7F, 0x44, 0x7C, 0x00, 0x7F},// Ы
	{0x7F, 0x44, 0x44, 0x44, 0x38},// Ь
	{0x22, 0x41, 0x49, 0x49, 0x3E},// Э
	{0x7F, 0x08, 0x7F, 0x41, 0x7F},// Ю
	{0x46, 0x29, 0x19, 0x09, 0x7F},// Я

	{0x20, 0x54, 0x54, 0x54, 0x78},// a
	{0x3C, 0x4A, 0x4A, 0x4B, 0x30},// б
	{0x7C, 0x54, 0x54, 0x58, 0x20},// в
	{0x7C, 0x04, 0x04, 0x04, 0x04},// г
	{0x60, 0x38, 0x24, 0x3C, 0x60},// д
	{0x38, 0x54, 0x54, 0x54, 0x18},// e
	{0x74, 0x08, 0x7C, 0x08, 0x74},// ж
	{0x28, 0x44, 0x54, 0x54, 0x28},// з
	{0x7C, 0x20, 0x10, 0x08, 0x7C},// и
	{0x7C, 0x21, 0x12, 0x09, 0x7C},// й
	{0x7C, 0x10, 0x10, 0x28, 0x44},// к
	{0x40, 0x78, 0x04, 0x04, 0x7C},// л
	{0x7C, 0x08, 0x10, 0x08, 0x7C},// м
	{0x7C, 0x10, 0x10, 0x10, 0x7C},// н
	{0x38, 0x44, 0x44, 0x44, 0x38},// o
	{0x7C, 0x04, 0x04, 0x04, 0x7C},// п

	{0x7C, 0x14, 0x14, 0x14, 0x08},// p
	{0x38, 0x44, 0x44, 0x44, 0x28},// c
	{0x04, 0x04, 0x7C, 0x04, 0x04},// т
	{0x0C, 0x50, 0x50, 0x50, 0x3C},// y
	{0x18, 0x24, 0x7C, 0x24, 0x18},// ф
	{0x44, 0x28, 0x10, 0x28, 0x44},// х
	{0x3C, 0x20, 0x20, 0x3C, 0x40},// ц
	{0x1C, 0x20, 0x20, 0x20, 0x7C},// ч
	{0x7C, 0x40, 0x7C, 0x40, 0x7C},// ш
	{0x3C, 0x20, 0x3C, 0x20, 0x7C},// щ
	{0x04, 0x7C, 0x48, 0x48, 0x30},// ъ
	{0x7C, 0x48, 0x48, 0x30, 0x7C},// ы
	{0x7C, 0x48, 0x48, 0x48, 0x30},// ь
	{0x28, 0x44, 0x54, 0x54, 0x38},// э
	{0x7C, 0x38, 0x44, 0x44, 0x38},// ю
	{0x48, 0x34, 0x14, 0x14, 0x7C},// я

	{0x7E, 0x4B, 0x4A, 0x4B, 0x42},// Ё
	{0x38, 0x55, 0x54, 0x55, 0x18},// ё
};

#define CHAR_MIN 0x20
#define CHAR_COUNT (sizeof(FontLookup)/sizeof(FontLookup[0]))

const unsigned char numbers[] =
{

    0x01,  0xFF,  0xF8,  //   ##############
		0x03,  0xFF,  0xFC,  //  ################
		0x07,  0x00,  0x0E,  // ###            ###
		0x06,  0xE0,  0x06,  // ## ###          ##
		0x06,  0x1C,  0x06,  // ##    ###       ##
		0x06,  0x03,  0x86,  // ##       ###    ##
		0x06,  0x00,  0x76,  // ##          ### ##
		0x07,  0x00,  0x0E,  // ###            ###
		0x03,  0xFF,  0xFC,  //  ################
		0x01,  0xFF,  0xF8,  //   ##############

		0x00,  0x00,  0x00,  //
		0x00,  0x00,  0x00,  //
		0x06,  0x00,  0x08,  // ##             #
		0x06,  0x00,  0x0C,  // ##             ##
		0x07,  0xFF,  0xFE,  // ##################
		0x07,  0xFF,  0xFE,  // ##################
		0x06,  0x00,  0x00,  // ##
		0x06,  0x00,  0x00,  // ##
		0x00,  0x00,  0x00,  //
		0x00,  0x00,  0x00,  //

		0x07,  0x00,  0x38,  // ###          ###
		0x07,  0x80,  0x3C,  // ####         ####
		0x07,  0xC0,  0x0E,  // #####          ###
		0x06,  0xE0,  0x06,  // ## ###          ##
		0x06,  0x70,  0x06,  // ##  ###         ##
		0x06,  0x38,  0x06,  // ##   ###        ##
		0x06,  0x1C,  0x06,  // ##    ###       ##
		0x06,  0x0F,  0x0E,  // ##     ####    ###
		0x06,  0x07,  0xFC,  // ##      #########
		0x06,  0x01,  0xF8,  // ##        ######

		0x01,  0xC0,  0x38,  //   ###        ###
		0x03,  0xC0,  0x3C,  //  ####        ####
		0x07,  0x06,  0x0E,  // ###     ##     ###
		0x06,  0x06,  0x06,  // ##      ##      ##
		0x06,  0x06,  0x06,  // ##      ##      ##
		0x06,  0x06,  0x06,  // ##      ##      ##
		0x06,  0x06,  0x06,  // ##      ##      ##
		0x07,  0x0F,  0x0E,  // ###    ####    ###
		0x03,  0xFF,  0xFC,  //  ################
		0x01,  0xF9,  0xF8,  //   ######  ######

		0x00,  0x78,  0x00,  //     ####
		0x00,  0x7E,  0x00,  //     ######
		0x00,  0x6F,  0x80,  //     ## #####
		0x00,  0x63,  0xE0,  //     ##   #####
		0x00,  0x60,  0xF0,  //     ##     ####
		0x00,  0x60,  0x3C,  //     ##       ####
		0x00,  0x60,  0x0E,  //     ##         ###
		0x07,  0xFF,  0xFE,  // ##################
		0x07,  0xFF,  0xFE,  // ##################
		0x00,  0x60,  0x00,  //     ##

		0x01,  0xC7,  0xFE,  //   ###   ##########
		0x03,  0xC7,  0xFE,  //  ####   ##########
		0x07,  0x06,  0x06,  // ###     ##      ##
		0x06,  0x03,  0x06,  // ##       ##     ##
		0x06,  0x03,  0x06,  // ##       ##     ##
		0x06,  0x03,  0x06,  // ##       ##     ##
		0x06,  0x03,  0x06,  // ##       ##     ##
		0x07,  0x07,  0x06,  // ###     ###     ##
		0x03,  0xFE,  0x06,  //  #########      ##
		0x01,  0xFC,  0x06,  //   #######       ##

		0x01,  0xFF,  0xF8,  //   ##############
		0x03,  0xFF,  0xFC,  //  ################
		0x06,  0x06,  0x0E,  // ##      ##     ###
		0x06,  0x03,  0x06,  // ##       ##     ##
		0x06,  0x03,  0x06,  // ##       ##     ##
		0x06,  0x03,  0x06,  // ##       ##     ##
		0x06,  0x03,  0x06,  // ##       ##     ##
		0x07,  0x07,  0x0E,  // ###     ###    ###
		0x03,  0xFE,  0x1C,  //  #########    ###
		0x01,  0xFC,  0x18,  //   #######     ##

		0x00,  0x00,  0x06,  //                 ##
		0x00,  0x00,  0x06,  //                 ##
		0x06,  0x00,  0x06,  // ##              ##
		0x07,  0x80,  0x06,  // ####            ##
		0x01,  0xF0,  0x06,  //   #####         ##
		0x00,  0x7E,  0x06,  //     ######      ##
		0x00,  0x0F,  0x86,  //        #####    ##
		0x00,  0x03,  0xF6,  //          ###### ##
		0x00,  0x00,  0x7E,  //             ######
		0x00,  0x00,  0x1E,  //               ####

		0x01,  0xF9,  0xF8,  //   ######  ######
		0x03,  0xFF,  0xFC,  //  ################
		0x07,  0x0F,  0x0E,  // ###    ####    ###
		0x06,  0x06,  0x06,  // ##      ##      ##
		0x06,  0x06,  0x06,  // ##      ##      ##
		0x06,  0x06,  0x06,  // ##      ##      ##
		0x06,  0x06,  0x06,  // ##      ##      ##
		0x07,  0x0F,  0x0E,  // ###    ####    ###
		0x03,  0xFF,  0xFC,  //  ################
		0x01,  0xF9,  0xF8,  //   ######  ######

		0x01,  0xC3,  0xF8,  //   ###    #######
		0x03,  0xC7,  0xFC,  //  ####   #########
		0x07,  0x0E,  0x0E,  // ###    ###     ###
		0x06,  0x0C,  0x06,  // ##     ##       ##
		0x06,  0x0C,  0x06,  // ##     ##       ##
		0x06,  0x0C,  0x06,  // ##     ##       ##
		0x06,  0x0C,  0x06,  // ##     ##       ##
		0x07,  0x06,  0x0E,  // ###     ##     ###
		0x03,  0xFF,  0xFC,  //  ################
		0x01,  0xFF,  0xF8,   //   ##############

//		0x03,  0x00,  0xC0,  //   ###    #######
//		0x00,  0x00,  0xC0,  //  ####   #########
//		0x00,  0x00,  0xC0,  // ###    ###     ###
//		0x00,  0x00,  0xC0,  // ##     ##       ##
//		0x00,  0x00,  0xC0,  // ##     ##       ##
//		0x00,  0xC1,  0x80,  // ##     ##       ##
//		0x00,  0x44,  0x00,  // ##     ##       ##
//		0x00,  0x63,  0x00,  // ###     ##     ###
//		0x00,  0x36,  0x00,  //  ################
//		0x00,  0x18,  0x00   //   ##############


};

const uint16_t numbers_idx[] =
{
		0, 		// 0
		30, 		// 1
		60, 		// 2
		90, 		// 3
		120, 		// 4
		150, 		// 5
		180, 		// 6
		210, 		// 7
		240, 		// 8
		270, 		// 9
		300,
		330,	
};

/* Global variables */

/* Cache buffer in SRAM 84*48 bits or 504 bytes */
byte  LcdCache [ LCD_CACHE_SIZE ];

/* Cache index */
static int   LcdCacheIdx;
static int   LcdRotate=1;

/* Private SSD1306 structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;
/* Private variable */
static SSD1306_t SSD1306;

void LcdClear ( void )
{
	memset(LcdCache,0x00,LCD_CACHE_SIZE);
    /* Set update flag to be true */
//    UpdateLcd = true;
}


//char mask;
//int xpos;
byte LcdGotoXY ( byte x, byte y )
{
    LcdCacheIdx = (x - 1) + ( y - 1 ) * LCD_X_RES;
    return OK;
}

byte LcdGotoXpicsel ( uint16_t x )
{
    LcdCacheIdx = (x - 1);
    return OK;
}
void  lcd_putnum (int x, int y,char *str){

	char c;
	int i,j;
	char* str2 = str;
	for(i =0;i<3;i++)
	{
		LcdGotoXY(x, y + 2 - i );

		str2 = str;
		while( (c = (*str2++)) )
		{
			if( (c>=0x30  &&  c<= 0x40) || (c<11) )
			{
				int n = c - 0x30;
				if(n < 0) n = c+10;

				//lcd_write(DATA,  mask);
        LcdCache[LcdCacheIdx++]= 0;//mask;
			
				for(j=numbers_idx[n]+i; j< numbers_idx[n]+13*3 ; j+=3)
					{
					int dd = 0;
					if(j< numbers_idx[n+1]) dd = numbers[j];

					if( (*str2 == '.') && (i==0) && (j>( numbers_idx[n]+13*3 - 9))){					
						dd |= 0x0C;					
					}							
					//if (xpos <= LCD_XMAX) lcd_write(DATA, dd ^ mask);
						LcdCache[LcdCacheIdx++]=dd;// ^ mask;
					}
			}
		}
	}
	//lcd_write(DATA,0);
}


/*
 * Name         :  LcdGotoXYFont
 * Description  :  Sets cursor location to xy location corresponding to basic
 *                 font size.
 * Argument(s)  :  x, y -> Coordinate for new cursor position. Range: 1,1 .. 14,6
 * Return value :  see return value in lcd.h
 * Note         :  Based on Sylvain Bissonette's code
 */
byte LcdGotoXYFont ( byte x, byte y )
{ 
    LcdCacheIdx = ( x - 1 ) * 6 + ( y - 1 ) * LCD_X_RES ;
    return OK;
}

/*
 * Name         :  LcdChr
 * Description  :  Displays a character at current cursor location and
 *                 increment cursor location.
 * Argument(s)  :  size -> Font size. See enum in lcd.h.
 *                 ch   -> Character to write.
 * Return value :  see lcd.h about return value
 */
byte LcdChr ( LcdFontSize size, char chr )
{
    byte i, c;
    byte b1, b2;
    int  tmpIdx;
    byte ch = (byte)chr;

//    if ( (ch < CHAR_MIN) || (ch >= (CHAR_MIN+CHAR_COUNT)) )
//    {
//        /* Convert to a printable character. */
//        ch = 92;
//    }			
  
	  if ((byte)chr  == 168){
        ch = 0xA0;
	  }
    else if ((byte)chr  == 184){
        ch = 0xA1;
		}
    else	if ((byte)chr >= 192){  //192 это русская 'А'
       ch -= 0x60;
		}
    else {
        ch -= 0x20;
		}
    
		 
    if ( size == FONT_1X )
    {									
        for ( i = 0; i < 5; i++ )
        {
            /* Copy lookup table from Flash ROM to LcdCache */
            LcdCache[LcdCacheIdx++] = ( FontLookup[ ch ][ i ] )  << 1;
        }
    }
    else if ( size == FONT_2X )
    {
        tmpIdx = LcdCacheIdx - LCD_X_RES;

        if ( tmpIdx < 0 ) return OUT_OF_BORDER;

        for ( i = 0; i < 5; i++ )
        {
            /* Copy lookup table from Flash ROM to temporary c */
            c = (FontLookup[ ch ][i]) << 1;
            /* Enlarge image */
            /* First part */
            b1 =  (c & 0x01) * 3;
            b1 |= (c & 0x02) * 6;
            b1 |= (c & 0x04) * 12;
            b1 |= (c & 0x08) * 24;

            c >>= 4;
            /* Second part */
            b2 =  (c & 0x01) * 3;
            b2 |= (c & 0x02) * 6;
            b2 |= (c & 0x04) * 12;
            b2 |= (c & 0x08) * 24;

            /* Copy two parts into LcdCache */
            LcdCache[tmpIdx++] = b1;
            LcdCache[tmpIdx++] = b1;
            LcdCache[tmpIdx + LCD_X_RES-2] = b2;
            LcdCache[tmpIdx + LCD_X_RES-1] = b2;
        }

        /* Update x cursor position. */
        /* Version 0.2.5 - Possible bug fixed on Dec 25,2008 */
        LcdCacheIdx = (LcdCacheIdx + 11) % LCD_CACHE_SIZE;
    }

    /* Horizontal gap between characters. */
    /* Version 0.2.5 - Possible bug fixed on Dec 25,2008 */
    LcdCache[LcdCacheIdx] = 0x00;
    /* At index number LCD_CACHE_SIZE - 1, wrap to 0 */
    if(LcdCacheIdx == (LCD_CACHE_SIZE - 1) )
    {
        LcdCacheIdx = 0;
        return OK_WITH_WRAP;
    }
    /* Otherwise just increment the index */
    LcdCacheIdx++;
    return OK;
}

/*
 * Name         :  LcdStr
 * Description  :  Displays a character at current cursor location and increment
 *                 cursor location according to font size. This function is
 *                 dedicated to print string laid in SRAM
 * Argument(s)  :  size      -> Font size. See enum.
 *                 dataArray -> Array contained string of char to be written
 *                              into cache.
 * Return value :  see return value on lcd.h
 */
byte LcdStr ( LcdFontSize size, const char* dataArray )
{
    byte tmpIdx=0;
    byte response;
    char chr;
    while(1)
	{
        chr = dataArray[ tmpIdx ];
        if(chr==0)
            break;

        if(size==FONT_2X && chr=='.' && tmpIdx>0)
            LcdCacheIdx = (LcdCacheIdx - 2) % LCD_CACHE_SIZE;
        /* Send char */
		     response = LcdChr( size, dataArray[ tmpIdx ] );

        if(size==FONT_2X)
        {
            if(chr=='.')
                LcdCacheIdx = (LcdCacheIdx - 5) % LCD_CACHE_SIZE;
        }
        /* Just in case OUT_OF_BORDER occured */
        /* Dont worry if the signal == OK_WITH_WRAP, the string will
        be wrapped to starting point */
        if( response == OUT_OF_BORDER)
            return OUT_OF_BORDER;
        /* Increase index */
		    tmpIdx++;
	}
    return OK;
}

//void SSD1306_GotoXY(uint16_t x, uint16_t y) {
//	/* Set write pointers */
//	SSD1306.CurrentX = x;
//	SSD1306.CurrentY = y;
//}


void SSD1306_DrawPixel(uint16_t x, uint16_t y, LcdPixelMode color) {
	if (
		x >= LCD_X_RES ||
		y >= LCD_Y_RES
	) {
		/* Error */
		return;
	}
	
	/* Check if pixels are inverted */
	if (SSD1306.Inverted) {
		color = !color;
	}
	
	/* Set color */
	if (color == PIXEL_ON) {
		LcdCache[x + (y / 8) * LCD_X_RES] |= 1 << (y % 8);
	} else {
		LcdCache[x + (y / 8) * LCD_X_RES] &= ~(1 << (y % 8));
	}
}

//void SSD1306_GotoXY(uint16_t x, uint16_t y) {
//	/* Set write pointers */
//	SSD1306.CurrentX = x;
//	SSD1306.CurrentY = y;
//}

char LcdPutc( char ch, FontDef_t* Font, LcdPixelMode color) {
	uint32_t i, b, j;
	
	/* Check available space in LCD */
	if (
		LCD_X_RES <= (SSD1306.CurrentX + Font->FontWidth) ||
		LCD_Y_RES <= (SSD1306.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}
	
	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (LcdPixelMode) color);
			} else {
				SSD1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (LcdPixelMode)!color);
			}
		}
	}
	
	/* Increase pointer */
	SSD1306.CurrentX += Font->FontWidth;
	
	/* Return character written */
	return ch;
}
void LcdGoto(byte x, byte y){
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;	 	
}
void LcdGotoX(byte x){
	SSD1306.CurrentX = x;	
}
void LcdGotoY(byte y){
	SSD1306.CurrentY = y;	 	
}
void LcdAddX(byte x){
	SSD1306.CurrentX += x;	
}
void LcdAddY(byte y){
	SSD1306.CurrentY += y;	 	
}
char LcdPuts(const char* str, FontDef_t* Font, LcdPixelMode color) {
	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (LcdPutc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}		
		/* Increase string pointer */
		str++;
	}	
	/* Everything OK, zero should be returned */
	return *str;
}
/*
 * Name         :  LcdPixel
 * Description  :  Displays a pixel at given absolute (x, y) location.
 * Argument(s)  :  x, y -> Absolute pixel coordinates
 *                 mode -> Off, On or Xor. See enum in lcd.h.
 * Return value :  see return value on lcd.h
 * Note         :  Based on Sylvain Bissonette's code
 */
byte LcdPixel ( byte x, byte y, LcdPixelMode mode )
{
    word  index;
    byte  offset;
    byte  data;

    /* Prevent from getting out of border */
    if ( x > LCD_X_RES ) return OUT_OF_BORDER;
    if ( y > LCD_Y_RES ) return OUT_OF_BORDER;

    /* Recalculating index and offset */
    index = ( ( y / 8 ) * LCD_X_RES ) + x;
    offset  = y - ( ( y / 8 ) * 8 );

    data = LcdCache[ index ];

    /* Bit processing */

	/* Clear mode */
    if ( mode == PIXEL_OFF )
    {
        data &= ( ~( 0x01 << offset ) );
    }

    /* On mode */
    else if ( mode == PIXEL_ON )
    {
        data |= ( 0x01 << offset );
    }

    /* Xor mode */
    else if ( mode  == PIXEL_XOR )
    {
        data ^= ( 0x01 << offset );
    }

    /* Final result copied to cache */
    LcdCache[ index ] = data;

    return OK;
}

/*
 * Name         :  LcdLine
 * Description  :  Draws a line between two points on the display.
 * Argument(s)  :  x1, y1 -> Absolute pixel coordinates for line origin.
 *                 x2, y2 -> Absolute pixel coordinates for line end.
 *                 mode   -> Off, On or Xor. See enum in lcd.h.
 * Return value :  see return value on lcd.h
 */
byte LcdLine ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode )
{
    int dx, dy, stepx, stepy, fraction;
    byte response;

    /* Calculate differential form */
    /* dy   y2 - y1 */
    /* -- = ------- */
    /* dx   x2 - x1 */

    /* Take differences */
    dy = y2 - y1;
    dx = x2 - x1;

    /* dy is negative */
    if ( dy < 0 )
    {
        dy    = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    /* dx is negative */
    if ( dx < 0 )
    {
        dx    = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 1;
    dy <<= 1;

    /* Draw initial position */
    response = LcdPixel( x1, y1, mode );
    if(response)
        return response;

    /* Draw next positions until end */
    if ( dx > dy )
    {
        /* Take fraction */
        fraction = dy - ( dx >> 1);
        while ( x1 != x2 )
        {
            if ( fraction >= 0 )
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;

            /* Draw calculated point */
            response = LcdPixel( x1, y1, mode );
            if(response)
                return response;

        }
    }
    else
    {
        /* Take fraction */
        fraction = dx - ( dy >> 1);
        while ( y1 != y2 )
        {
            if ( fraction >= 0 )
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;

            /* Draw calculated point */
            response = LcdPixel( x1, y1, mode );
            if(response)
                return response;
        }
    }

    /* Set update flag to be true */
//    UpdateLcd = true;
    return OK;
}

//==============================================================================
// Процедура рисует прямоугольник в буфере кадра дисплея
//==============================================================================
byte LcdFillRect(byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode ) 
{
  LcdLine(x1, x2, y1, y1, mode);
  LcdLine(x1, x2, y2, y2, mode);
  LcdLine(x1, x1, y1, y2, mode);
  LcdLine(x2, x2, y1, y2, mode);
	return OK;
}
//==============================================================================

/*
 * Name         :  LcdSingleBar
 * Description  :  Display single bar.
 * Argument(s)  :  baseX  -> absolute x axis coordinate
 *                 baseY  -> absolute y axis coordinate
 *				   height -> height of bar (in pixel)
 *				   width  -> width of bar (in pixel)
 *				   mode   -> Off, On or Xor. See enum in lcd.h.
 * Return value :  see return value on lcd.h
 */
byte LcdSingleBar ( byte baseX, byte baseY, byte height, byte width, LcdPixelMode mode )
{
	byte tmpIdxX,tmpIdxY,tmp;

    byte response;

    /* Checking border */
	if ( ( baseX > LCD_X_RES ) || ( baseY > LCD_Y_RES ) ) return OUT_OF_BORDER;

	if ( height > baseY )
		tmp = 0;
	else
		tmp = baseY - height;

    /* Draw lines */
	for ( tmpIdxY = tmp; tmpIdxY < baseY; tmpIdxY++ )
	{
		for ( tmpIdxX = baseX; tmpIdxX < (baseX + width); tmpIdxX++ )
        {
			response = LcdPixel( tmpIdxX, tmpIdxY, mode );
            if(response)
                return response;

        }
	}

    /* Set update flag to be true */
//	UpdateLcd = true;
    return OK;
}

/*
 * Name         :  LcdBars
 * Description  :  Display multiple bars.
 * Argument(s)  :  data[] -> data which want to be plotted
 *                 numbBars  -> number of bars want to be plotted
 *				   width  -> width of bar (in pixel)
 * Return value :  see return value on lcd.h
 * Note         :  Please check EMPTY_SPACE_BARS, BAR_X, BAR_Y in lcd.h
 */
byte LcdBars ( byte data[], byte numbBars, byte width, byte multiplier )
{
	byte b;
	byte tmpIdx = 0;
    byte response;

	for ( b = 0;  b < numbBars ; b++ )
	{
        /* Preventing from out of border (LCD_X_RES) */
		if ( tmpIdx > LCD_X_RES ) return OUT_OF_BORDER;

		/* Calculate x axis */
		tmpIdx = ((width + EMPTY_SPACE_BARS) * b) + BAR_X;

		/* Draw single bar */
		response = LcdSingleBar( tmpIdx, BAR_Y, data[ b ] * multiplier, width, PIXEL_ON);
        if(response == OUT_OF_BORDER)
            return response;
	}

	/* Set update flag to be true */
//	UpdateLcd = true;
    return OK;

}
/*
 * Name         :  LcdRect
 * Description  :  Display a rectangle.
 * Argument(s)  :  x1   -> absolute first x axis coordinate
 *                 y1   -> absolute first y axis coordinate
 *				   x2   -> absolute second x axis coordinate
 *				   y2   -> absolute second y axis coordinate
 *				   mode -> Off, On or Xor. See enum in lcd.h.
 * Return value :  see return value on lcd.h.
 */
byte LcdRect ( byte x1, byte x2, byte y1, byte y2, LcdPixelMode mode )
{
	byte tmpIdxX,tmpIdxY;
    byte response;

	/* Checking border */
	if ( ( x1 > LCD_X_RES ) ||  ( x2 > LCD_X_RES ) || ( y1 > LCD_Y_RES ) || ( y2 > LCD_Y_RES ) )
		/* If out of border then return */
		return OUT_OF_BORDER;

	if ( ( x2 > x1 ) && ( y2 > y1 ) )
	{
		for ( tmpIdxY = y1; tmpIdxY < y2; tmpIdxY++ )
		{
			/* Draw line horizontally */
			for ( tmpIdxX = x1; tmpIdxX < x2; tmpIdxX++ )
            {
				/* Draw a pixel */
				response = LcdPixel( tmpIdxX, tmpIdxY, mode );
                if(response)
                    return response;
            }
		}

		/* Set update flag to be true */
//		UpdateLcd = true;
	}
    return OK;
}
/*
 * Name         :  LcdImage
 * Description  :  Image mode display routine.
 * Argument(s)  :  Address of image in hexes
 * Return value :  None.
 * Example      :  LcdImage(&sample_image_declared_as_array);
 */
void LcdImage ( const byte *imageData )
{
    memcpy(LcdCache,imageData,LCD_CACHE_SIZE);

	/* Set update flag to be true */
//    UpdateLcd = true;
}

/*
 * Name         :  LcdUpdate
 * Description  :  Copies the LCD cache into the device RAM.
 * Argument(s)  :  None.
 * Return value :  None.
 */
void LcdUpdate ( void )
{	
		SSD1306_DisplayFullUpdate(LcdCache, sizeof(LcdCache)); 
}

void ToggleInvers(void)
{
	if (SSD1306_Get_invers()==0){
		SSD1306_Set_invers(1, 1);
	}
	else {
		SSD1306_Set_invers(0, 1);
	}
}

void ToggleRotate(void)
{
	if (LcdRotate==0){
		LcdRotate=1;
		SSD1306_Rotate(LcdRotate, 1);
	}
	else {
		LcdRotate=0;	
		SSD1306_Rotate(LcdRotate, 1);
	}
}



/**************** HIGH LEVEL ************************************************/

/*
 * Puts point with coordinates p to VRAM, returns error if exceed boundaries
 */
gr_error_t vram_put_point (point_t p, point_mode_t mode)
{
	/* Assertion of point value */
	if(p.x_point > (LCD_X_RES-1)) return GRAPH_ERROR;
	if(p.y_point > (LCD_Y_RES-1)) return GRAPH_ERROR;

	/* Set point in video buffer by mode option */
	switch(mode)
	{
	case PIXEL_CLEAR: /* Clears pixel in VRAM */
		LcdCache[ ((p.y_point/8) * LCD_X_RES) + p.x_point] &= ~(1<<p.y_point%8);
		break;

	case PIXEL_SET: /* Sets pixel in VRAM */
		LcdCache[((p.y_point/8) * LCD_X_RES) + p.x_point] |= (1<<p.y_point%8);
		break;

	case PIXEL_XOR: /* Sets pixel inverted in VRAM */
		LcdCache[((p.y_point/8) * LCD_X_RES) + p.x_point] ^= (1<<p.y_point%8);
		break;

	default: break;
	}

	return NO_GRAPH_ERROR;
}


/*
 * Puts line from p1 to p2, returns error if exceed boundaries
 * (normal PIXEL_XOR mode implementation)
 */
gr_error_t vram_put_line (point_t p1, point_t p2, point_mode_t mode)
{
	register int32_t dx, dy, stepx, stepy, frac;
	gr_error_t err = NO_GRAPH_ERROR;


	/* Calculate DX, DY */
	dx = (int32_t)p2.x_point - (int32_t)p1.x_point;
	dy = (int32_t)p2.y_point - (int32_t)p1.y_point;

	if(dx<0) {dx = -dx; stepx = -1;} else stepx = 1;
	if(dy<0) {dy = -dy; stepy = -1;} else stepy = 1;

	dx *= 2; dy *= 2;

	/* Draw first point */
	if(vram_put_point(p1, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	/* Calculate and draw trace by Brezenhem algorithm */
	if (dx > dy)
	{
		frac = dy - (dx/2);
	    while(p1.x_point != p2.x_point)
	    {
	    	if(frac >= 0)
	        {
	            p1.y_point += stepy;
	            frac -= dx;
	        }
	        p1.x_point += stepx;
	        frac += dy;

	        if(vram_put_point(p1, mode) == GRAPH_ERROR) err = GRAPH_ERROR;
	    }
	}
	else
	{ /* dx < dy */
		frac = dx - (dy/2);
	    while(p1.y_point != p2.y_point)
	    {
	        if(frac >= 0)
	        {
	            p1.x_point += stepx;
	            frac -= dy;
	        }
	        p1.y_point += stepy;
	        frac += dx;

	        if(vram_put_point(p1, mode) == GRAPH_ERROR) err = GRAPH_ERROR;
	    }
	}

	return err;
}

/*
 * Draws rectangle in VRAM from p1 to p2, returns error if exceed boundaries
 * (PIXEL_XOR mode works not well)
 */
gr_error_t vram_put_rect (point_t p1, point_t p2, point_mode_t mode)
{
	register int32_t dx, dy;
	point_t __p2;
	static uint8_t rec_flag = 2;
	gr_error_t err = NO_GRAPH_ERROR;


	/* Calculate DX, DY */
	dx = (int32_t)p2.x_point - (int32_t)p1.x_point;
	dy = (int32_t)p2.y_point - (int32_t)p1.y_point;

	/* X line */
	__p2.x_point = p1.x_point + dx;
	__p2.y_point = p1.y_point;
	if(vram_put_line(p1, __p2, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	/* Y line */
	__p2.x_point = p1.x_point;
	__p2.y_point = p1.y_point + dy;
	if(vram_put_line(p1, __p2, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	rec_flag--;

	if(rec_flag)
	{ /* Swap p1<->p2 and draw second part of rectangle by recoursive call */
		if(vram_put_rect(p2, p1, mode) == GRAPH_ERROR) err = GRAPH_ERROR;
	} else rec_flag = 2;

	return err;
}

/*
 * Draws filled rectangle in VRAM from p1 to p2
 * (normal PIXEL_XOR mode implementation)
 */
gr_error_t vram_put_bar (point_t p1, point_t p2, point_mode_t mode)
{
	register int32_t dx, dy;
	point_t __p2;
	gr_error_t err = NO_GRAPH_ERROR;


	/* Calculate DX, DY */
	dx = (int32_t)p2.x_point - (int32_t)p1.x_point;
	dy = (int32_t)p2.y_point - (int32_t)p1.y_point;

	if(labs(dx) < labs(dy))
	{ /* dy > dx - fill lines along X axis */

		/* Set __p2 */
		__p2.x_point = p1.x_point;
		__p2.y_point = p1.y_point + dy;

		while( !((__p2.x_point == p2.x_point) && (__p2.y_point == p2.y_point)) )
		{ /* Draw lines until __p2 reaches p2 */
			if(vram_put_line(p1, __p2, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

			/* Select direction */
			if(dx>0) { p1.x_point++; __p2.x_point++;}
			else {p1.x_point--; __p2.x_point--;};
		}
		/* Put last line */
		if(vram_put_line(p1, __p2, mode) == GRAPH_ERROR) err = GRAPH_ERROR;
	}
	else
	{ /* dx > dy - fill lines along Y axis */

		/* Set __p2 */
		__p2.y_point = p1.y_point;
		__p2.x_point = p1.x_point + dx;

		while( !((__p2.x_point == p2.x_point) && (__p2.y_point == p2.y_point)) )
		{ /* Draw lines until __p2 reaches p2 */
			if(vram_put_line(p1, __p2, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

			/* Select direction */
			if(dy>0) { p1.y_point++; __p2.y_point++;}
			else {p1.y_point--; __p2.y_point--;};
		}
		/* Put last line */
		if(vram_put_line(p1, __p2, mode) == GRAPH_ERROR) err = GRAPH_ERROR;
	}

	return err;
}


/*
 * Draws circle in VRAM from center with radius
 * (PIXEL_XOR works not well)
 */
gr_error_t vram_put_circle (point_t center, uint8_t radius, point_mode_t mode)
{
	register int32_t xc = 0, yc, p;
	point_t __p;
	gr_error_t err = NO_GRAPH_ERROR;

	yc = radius;
	p = 3 - (radius*2);
	while (xc <= yc)
	{
		__p.x_point = center.x_point + xc;
		__p.y_point = center.y_point + yc;
	    if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	    __p.y_point = center.y_point - yc;
	    if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	    __p.x_point = center.x_point - xc;
	    if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	    __p.y_point = center.y_point + yc;
	    if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	    __p.x_point = center.x_point + yc;
	    __p.y_point = center.y_point + xc;
	    if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	    __p.y_point = center.y_point - xc;
	    if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	    __p.x_point = center.x_point - yc;
	    if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	    __p.y_point = center.y_point + xc;
	    if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

	    if(p < 0) p += ( (xc++)*4) + 6;
	    else p += (( (xc++) - (yc--))*4) + 10;
	}

	return err;
}

/*
 *
 */
volatile unsigned char cars;
gr_error_t vram_put_char (point_t p, const font_info_t* font, const char c, point_mode_t mode)
{
	const unsigned char* bitmap_ptr;
	const unsigned char* __bitmap_ptr;
	unsigned char tmp;
	register uint32_t i, j, k, font_height_bytes, current_char_width;
	point_t __p = p;
	gr_error_t err = NO_GRAPH_ERROR;
	
  cars=c;
//	if( (c < (font->start_char)) || (c > (font->end_char)) ) return GRAPH_ERROR;
	
	/* Get height in bytes */
	font_height_bytes = (font->font_height);
	//if( ((font->font_height)%8) ) font_height_bytes++;

	/* Get width of current character */
	current_char_width = font->descr_array[cars - (font->start_char)].char_width;

	/* Get pointer to data */
	bitmap_ptr = &( font->font_bitmap_array[(font->descr_array[cars - (font->start_char)].char_offset)] );
	/* Copy to temporary ptr */
	__bitmap_ptr = bitmap_ptr;

	for(k = 0; k < current_char_width; k++)
	{ /* Width of character */

		for(i = 0; i<font_height_bytes; i++)
		{ /* Height of character */
			/* Get data to "shift" */
			tmp = *__bitmap_ptr;
			__bitmap_ptr += current_char_width;

			for(j = 0; j<8; j++)
			{ /* Push byte */

				if( (tmp & 0x01) )
				{if(vram_put_point(__p, mode) == GRAPH_ERROR) err = GRAPH_ERROR;};
				tmp >>= 1;
				__p.y_point++;
			}
		}
		__p.x_point++;
		__p.y_point = p.y_point;
		__bitmap_ptr = ++bitmap_ptr;
	}

	return err;
}

/*
 *
 */
gr_error_t vram_put_str (point_t p, const font_info_t * font, const char* str, point_mode_t mode)
{
	unsigned char c;
	register uint32_t current_char_width;
	gr_error_t err = NO_GRAPH_ERROR;

	while( (c = *str++) )
	{
		/* Check if current symbol is printable */
		if( (c < (font->start_char)) || (c > (font->end_char)) ) {
	  	err = GRAPH_ERROR;
		  continue;
		};
		
		if (c >= 192){  //192 это русская 'А'
       c -= (192-96);	    
       c +=font->start_char;		
	  }		
		
		/* Get width of current character */
		current_char_width = font->descr_array[c - (font->start_char)].char_width;

		/* Put character */
		if(vram_put_char(p, font, c, mode) == GRAPH_ERROR) err = GRAPH_ERROR;

		/* Increment X by last character width + 1*/
		p.x_point += current_char_width + 1;
	}

	return err;
}
