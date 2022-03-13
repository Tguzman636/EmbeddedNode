#ifndef _VVC_LCD_DRAW_H
#define _VVC_LCD_DRAW_H

#include "stm32l476xx.h"
#include "SSPI.h"
#include "SysTimer.h"

/* Utility method declarations. */
void LCD_Screen(uint8_t color);
void LCD_HLine(int X, int Y, int length, uint8_t color);
void LCD_VLine(int X, int Y, int length, uint8_t color);
void LCD_Rect(int XStart, int XFinish, int YStart, int YFinish, uint8_t color);
void LCD_FillRect(int XStart, int XFinish, int YStart, int YFinish, uint8_t color);
void LCD_Circle(int X, int Y, int radius, uint8_t color);
void LCD_DrawNumTitle(int X, int Y, int Length, int Height, int num, uint8_t color);

#endif
