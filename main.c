#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "GPIO.h"
#include "SSPI.h"
#include "UTIL.h"
#include "game.h"
#include "LCD_Draw.h"
#include "I2C.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//76200 Pixels

extern int MODE;
extern int TURN;

extern int NodePointX;
extern int NodePointY;
extern int BridgePointX;
extern int BridgePointY;

extern int box_color[13];
extern int box_count[13];

extern int node_owner[6][6];
extern int bridge_owner[11][6];

extern int Player1Res[4];
extern int Player2Res[4];

extern int Score[2];

int Debug = 1;

uint8_t SlaveAddress1 = 0b10100101;
uint8_t SlaveAddress2 = 0b10100100;
uint8_t Data_Receive[6] = {0, 0, 0, 0, 0, 0};
uint8_t Data_Start[2] = {0x40, 0x00};
uint8_t Data_Conversion;

struct ArrayCoords {
	int X;
	int Y;
};
struct ArrayBridgeCoords {
	int XS;
	int XF;
	int YS;
	int YF;
};
extern struct ArrayCoords NodeCoord[6][6];
extern struct ArrayBridgeCoords BridgeCoord[11][6];
uint8_t BLACK = 0;
uint8_t WHITE = 255;
uint8_t RED = 224;
uint8_t LIGHTBLUE = 215;
uint8_t BLUE = 35;
uint8_t LIGHTGREEN = 165;
uint8_t DARKGREEN = 20;
uint8_t GREEN = 20;
uint8_t YELLOW = 248;
uint8_t ORANGE = 240;
uint8_t PURPLE = 130;

void shuffle(int *array, int n) {
	srand(600);
  //srand(RNG->DR);
  for (int i = 0; i < n - 1; i++) {
		size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
    int t = array[j];
    array[j] = array[i];
    array[i] = t;
	}
}

void DrawCircles(int XS, int XF, int YS, int YF, int num) {
	int Offset = 14;
	if (num == 1) {
		LCD_Circle(XS+Offset, YS+Offset, 7, BLACK);
	} else if (num == 2){
		LCD_Circle(XS+(Offset/2), YS+Offset, 7, BLACK);
		LCD_Circle(XS+(3*Offset/2), YS+Offset, 7, BLACK);
	} else if (num == 3){
		LCD_Circle(XS+(Offset/2), YS+(3*Offset/2), 7, BLACK);
		LCD_Circle(XS+(3*Offset/2), YS+(3*Offset/2), 7, BLACK);
		LCD_Circle(XS+Offset, YS+(Offset/2), 7, BLACK);
	}
}

void DrawResource() {
	int colorArray[13];
	int RedPlaceholder[3] = {4,4,4};
	int BluePlaceholder[3] = {4,4,4};
	int GreenPlaceholder[3] = {4,4,4};
	int YellowPlaceholder[3] = {4,4,4};
	int dots = 0;
	// Resource Box
	for (int i=0;i<13;i++) {
		if (box_color[i] == 0) {
			colorArray[i]=BLACK;
			box_count[i] = 0;
		} else if (box_color[i] == 1) {
			colorArray[i]=RED;
			dots = (rand()%3)+1;
			if ((RedPlaceholder[0] != 4) & (RedPlaceholder[1] != 4)) {
				while((dots == RedPlaceholder[0]) | (dots == RedPlaceholder[1])) {
					dots = (rand()%3)+1;
				}
				box_count[i] = dots;
			} else if ((RedPlaceholder[0] != 4) & (RedPlaceholder[1] == 4)) {
				while(dots == RedPlaceholder[0]) {
					dots = (rand()%3)+1;
				}
				RedPlaceholder[1] = dots;
				box_count[i] = dots;
			} else {
				RedPlaceholder[0] = dots;
				box_count[i] = dots;
			}
		} else if (box_color[i] == 2) {
			colorArray[i]=BLUE;
			dots = (rand()%3)+1;
			if ((BluePlaceholder[0] != 4) & (BluePlaceholder[1] != 4)) {
				while((dots == BluePlaceholder[0]) | (dots == BluePlaceholder[1])) {
					dots = (rand()%3)+1;
				}
				box_count[i] = dots;
			} else if ((BluePlaceholder[0] != 4) & (BluePlaceholder[1] == 4)) {
				while(dots == BluePlaceholder[0]) {
					dots = (rand()%3)+1;
				}
				box_count[i] = dots;
				BluePlaceholder[1] = dots;
			} else {
				BluePlaceholder[0] = dots;
				box_count[i] = dots;
			}
		} else if (box_color[i] == 3) {
			colorArray[i]=YELLOW;
			dots = (rand()%3)+1;
			if ((YellowPlaceholder[0] != 4) & (YellowPlaceholder[1] != 4)) {
				while((dots == YellowPlaceholder[0]) | (dots == YellowPlaceholder[1])) {
					dots = (rand()%3)+1;
				}
				box_count[i] = dots;
			} else if ((YellowPlaceholder[0] != 4) & (YellowPlaceholder[1] == 4)) {
				while(dots == YellowPlaceholder[0]) {
					dots = (rand()%3)+1;
				}
				YellowPlaceholder[1] = dots;
				box_count[i] = dots;
			} else {
				YellowPlaceholder[0] = dots;
				box_count[i] = dots;
			}
		} else if (box_color[i] == 4) {
			colorArray[i]=GREEN;
			dots = (rand()%3)+1;
			if ((GreenPlaceholder[0] != 4) & (GreenPlaceholder[1] != 4)) {
				while((dots == GreenPlaceholder[0]) | (dots == GreenPlaceholder[1])) {
					dots = (rand()%3)+1;
				}
				box_count[i] = dots;
			} else if ((GreenPlaceholder[0] != 4) & (GreenPlaceholder[1] == 4)) {
				while(dots == GreenPlaceholder[0]) {
					dots = (rand()%3)+1;
				}
				GreenPlaceholder[1] = dots;
				box_count[i] = dots;
			} else {
				GreenPlaceholder[0] = dots;
				box_count[i] = dots;
			}
		}
	}
	LCD_FillRect(103,138,71,106,colorArray[0]);
	DrawCircles(103,138,71,106,box_count[0]);
	
	LCD_FillRect(67,102,107,142,colorArray[1]);
	DrawCircles(67,102,107,142,box_count[1]);
	LCD_FillRect(103,138,107,142,colorArray[2]);
	DrawCircles(103,138,107,142,box_count[2]);
	LCD_FillRect(139,174,107,142,colorArray[3]);
	DrawCircles(139,174,107,142,box_count[3]);
	
	LCD_FillRect(31,66,143,178,colorArray[4]);
	DrawCircles(31,66,143,178,box_count[4]);
	LCD_FillRect(67,102,143,178,colorArray[5]);
	DrawCircles(67,102,143,178,box_count[5]);
	LCD_FillRect(103,138,143,178,colorArray[6]);
	DrawCircles(103,138,143,178,box_count[6]);
	LCD_FillRect(139,174,143,178,colorArray[7]);
	DrawCircles(139,174,143,178,box_count[7]);
	LCD_FillRect(175,210,143,178,colorArray[8]);
	DrawCircles(175,210,143,178,box_count[8]);
	
	LCD_FillRect(67,102,179,214,colorArray[9]);
	DrawCircles(67,102,179,214,box_count[9]);
	LCD_FillRect(103,138,179,214,colorArray[10]);
	DrawCircles(103,138,179,214,box_count[10]);
	LCD_FillRect(139,174,179,214,colorArray[11]);
	DrawCircles(139,174,179,214,box_count[11]);
	
	LCD_FillRect(103,138,215,250,colorArray[12]);
	DrawCircles(103,138,215,250,box_count[12]);
}

void DrawWhiteBorder() {
	LCD_FillRect(29, 32, 141, 180, WHITE);
	LCD_FillRect(65, 68, 105, 216, WHITE);
	LCD_FillRect(101, 104, 69, 252, WHITE);
	LCD_FillRect(137, 140, 69, 252, WHITE);
	LCD_FillRect(173, 176, 105, 216, WHITE);
	LCD_FillRect(209, 212, 141, 180, WHITE);
	LCD_FillRect(101, 140, 69, 72, WHITE);
	LCD_FillRect(65, 176, 105, 108, WHITE);
	LCD_FillRect(29, 212, 141, 144, WHITE);
	LCD_FillRect(29, 212, 177, 180, WHITE);
	LCD_FillRect(65, 176, 213, 216, WHITE);
	LCD_FillRect(101, 140, 249, 252, WHITE);
	for (int i = 0; i<6; i++) {
		for (int j = 0; j<6; j++) {
			LCD_Circle( NodeCoord[j][i].X,
									NodeCoord[j][i].Y,
									10,
									BLACK);
		}
	}
}

void ScoreBoard() {
	// S
	LCD_Rect(2, 18, 10, 14, WHITE);
	LCD_Rect(2, 6, 10, 30, WHITE);
	LCD_Rect(2, 18, 28, 32, WHITE);
	LCD_Rect(14, 18, 30, 50, WHITE);
	LCD_Rect(2, 18, 46, 50, WHITE);
	// C
	LCD_Rect(22, 38, 10, 14, WHITE);
	LCD_Rect(22, 26, 10, 50, WHITE);
	LCD_Rect(22, 38, 46, 50, WHITE);
	// O
	LCD_Rect(42, 46, 10, 50, WHITE);
	LCD_Rect(54, 58, 10, 50, WHITE);
	LCD_Rect(42, 58, 10, 14, WHITE);
	LCD_Rect(42, 58, 46, 50, WHITE);
	// R
	LCD_Rect(62, 66, 10, 50, WHITE);
	LCD_Rect(62, 74, 10, 14, WHITE);
	LCD_Rect(62, 74, 28, 32, WHITE);
	LCD_Rect(74, 78, 15, 27, WHITE);
	LCD_Rect(74, 78, 33, 50, WHITE);
	// E
	LCD_Rect(82, 86, 10, 50, WHITE);
	LCD_Rect(82, 98, 10, 14, WHITE);
	LCD_Rect(82, 98, 28, 32, WHITE);
	LCD_Rect(82, 98, 46, 50, WHITE);
	// :
	LCD_Rect(107, 113, 15, 21, WHITE);
	LCD_Rect(107, 113, 39, 45, WHITE);
	// {Space}
	// 10th for Player 1
	LCD_DrawNumTitle(140, 10, 20, 40, 4, 0, ORANGE);
	// 1th for Player 1
	LCD_DrawNumTitle(160, 10, 20, 40, 4, 0, ORANGE);
	// :
	LCD_Rect(187, 193, 15, 21, WHITE);
	LCD_Rect(187, 193, 39, 45, WHITE);
	// 10th for Player 2
	LCD_DrawNumTitle(200, 10, 20, 40, 4, 0, PURPLE);
	// 1th for Player 2
	LCD_DrawNumTitle(220, 10, 20, 40, 4, 0, PURPLE);
}	

void ResourceBoard() {
	int RightPixel = 70;
	int LeftPixel = 15;
	int height = 20;
	int length = 14;
	int thickness = 2;
	LCD_HLine(0, 259, 240, WHITE);
	LCD_HLine(0, 263, 240, WHITE);
	LCD_VLine(117, 259, 70, WHITE);
	LCD_VLine(121, 259, 70, WHITE);
	LCD_VLine(55, 259, 70, WHITE);
	LCD_VLine(185, 259, 70, WHITE);
	LCD_DrawNumTitle(10+LeftPixel, 270, length, height, thickness, 0, ORANGE);
	LCD_DrawNumTitle(10+LeftPixel+length, 270, length, height, thickness, 0, ORANGE);
	LCD_FillRect(LeftPixel-7, LeftPixel+2, 275, 285, RED);
	
	LCD_DrawNumTitle(10+LeftPixel, 295, length, height, thickness, 0, ORANGE);
	LCD_DrawNumTitle(10+LeftPixel+length, 295, length, height, thickness, 0, ORANGE);
	LCD_FillRect(LeftPixel-7, LeftPixel+2, 300, 310, BLUE);
	
	LCD_DrawNumTitle(10+RightPixel, 270, length, height, thickness, 0, ORANGE);
	LCD_DrawNumTitle(10+RightPixel+length, 270, length, height, thickness, 0, ORANGE);
	LCD_FillRect(RightPixel-7, RightPixel+2, 275, 285, YELLOW);
	
	LCD_DrawNumTitle(10+RightPixel, 295, length, height, thickness, 0, ORANGE);
	LCD_DrawNumTitle(10+RightPixel+length, 295, length, height, thickness, 0, ORANGE);
	LCD_FillRect(RightPixel-7, RightPixel+2, 300, 310, GREEN);
	
	LCD_DrawNumTitle(140+LeftPixel, 270, length, height, thickness, 0, PURPLE);
	LCD_DrawNumTitle(140+LeftPixel+length, 270, length, height, thickness, 0, PURPLE);
	LCD_FillRect(LeftPixel+123, LeftPixel+133, 275, 285, RED);
	
	LCD_DrawNumTitle(140+LeftPixel, 295, length, height, thickness, 0, PURPLE);
	LCD_DrawNumTitle(140+LeftPixel+length, 295, length, height, thickness, 0, PURPLE);
	LCD_FillRect(LeftPixel+123, LeftPixel+133, 300, 310, BLUE);
	
	LCD_DrawNumTitle(140+RightPixel, 270, length, height, thickness, 0, PURPLE);
	LCD_DrawNumTitle(140+RightPixel+length, 270, length, height, thickness, 0, PURPLE);
	LCD_FillRect(RightPixel+123, RightPixel+133, 275, 285, YELLOW);
	
	LCD_DrawNumTitle(140+RightPixel, 295, length, height, thickness, 0, PURPLE);
	LCD_DrawNumTitle(140+RightPixel+length, 295, length, height, thickness, 0, PURPLE);
	LCD_FillRect(RightPixel+123, RightPixel+133, 300, 310, GREEN);
}

int main(void){
	// Clock Setup
	System_Clock_Init();
		
	// Initialize the SPI peripheral.
	GPIO_Init();
	SPI_Init();
	SysTick_Init();
	
	if (Debug != 1) {
		I2C_GPIO_Init();
		I2C_Initialization();
		I2C_SendData(I2C1, SlaveAddress2, Data_Start, 2);
	}
	
  // Initialize the display.
	ili9341_hspi_init();
	
 // Set column range.
	hspi_cmd(0x2A); //Command to draw rectangle
	hspi_w16(0x0000); //where to start (width)
	hspi_w16((uint16_t)(239)); //set the end
	// Set row range.
	hspi_cmd(0x2B); 
	hspi_w16(0x0000); //where to start (width)
	hspi_w16((uint16_t)(319)); //Where to end
	// Set 'write to RAM'
	hspi_cmd(0x2C);
	
	LCD_Screen(BLACK);
	
	// Randomize Box
	shuffle(box_color, 13);
	DrawResource();
	DrawWhiteBorder();
	NodePointer();
	ScoreBoard();
	ResourceBoard();
	delay(500); 
	RefreshResourceBoard();
	while(1) {
	}
}
