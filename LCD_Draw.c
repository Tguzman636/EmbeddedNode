#include "LCD_Draw.h"
#include <stdio.h>

#define WIDTH 240
#define HEIGHT 320
#define ALL 76800

uint8_t screen[ALL];

void Refresh() {
	for (int i=0;i<ALL;i++) {
		hspimod_w16(screen[i]);
	}
}

void LCD_Screen(uint8_t color) {
	for (int i=0;i<ALL;i++) {
		screen[i] = color;
	}
	Refresh();
}

void LCD_HLine(int X, int Y, int length, uint8_t color) {
	int Pixel_Start = X + Y*WIDTH;
	for (int i = Pixel_Start; i<Pixel_Start+length; i++) {
		screen[i] = color;
	}
	Refresh();
}

void LCD_VLine(int X, int Y, int length, uint8_t color) {
	int Pixel_Start = X + Y*WIDTH;
	for (int i = Pixel_Start; i<Pixel_Start+(length*WIDTH); i=i+WIDTH) {
		screen[i] = color;
	}
	Refresh();
}

void LCD_Rect(int XStart, int XFinish, int YStart, int YFinish, uint8_t color) {
	int TopLeft = XStart + YStart*WIDTH;
	int BottomLeft = TopLeft + ((YFinish-YStart)*WIDTH);
	int TopRight = TopLeft + (XFinish-XStart);
	for (int i = TopLeft; i<TopLeft+(XFinish-XStart); i++) {
		screen[i] = color;
	}
	for (int i = TopLeft; i<TopLeft+(YFinish-YStart)*WIDTH; i=i+WIDTH) {
		screen[i] = color;
	}
	for (int i = BottomLeft; i<BottomLeft+(XFinish-XStart); i++) {
		screen[i] = color;
	}
	for (int i = TopRight; i<TopRight+((YFinish-YStart)*WIDTH); i=i+WIDTH) {
		screen[i] = color;
	}
	Refresh();
}

void LCD_FillRect(int XStart, int XFinish, int YStart, int YFinish, uint8_t color) {
	for (int i=YStart; i<=YFinish; i++) {
		for (int j=XStart;j<=XFinish;j++) {
			screen[j+(i*WIDTH)] = color;
		}
	}
	Refresh();
}

void LCD_Circle(int X, int Y, int radius, uint8_t color) {
	for (int i=Y; i<=Y+radius; i++) {
		for (int j=X+2;j<=X+radius-2;j++) {
			screen[j+(i*WIDTH)] = color;
		}
	}
	for (int i=Y+2; i<=Y+radius-2; i++) {
		for (int j=X;j<=X+radius;j++) {
			screen[j+(i*WIDTH)] = color;
		}
	}
	Refresh();
}
