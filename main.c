#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "GPIO.h"
#include "SSPI.h"
#include "UTIL.h"
#include "LCD_Draw.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//76200 Pixels

uint8_t BLACK = 0;
uint8_t WHITE = 255;
uint8_t RED = 224;
uint8_t LIGHTBLUE = 215;
uint8_t BLUE = 35;
uint8_t LIGHTGREEN = 165;
uint8_t DARKGREEN = 20;
uint8_t GREEN = 20;
uint8_t YELLOW = 248;

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

struct ArrayCoords NodeCoord[6][6] = {  
{{0,0},     {0,0},      {26,138},   {26,174},   {0,0},      {0,0}},
{{0,0},     {62, 102},  {62, 138},  {62, 174},  {62, 210},  {0,0}},
{{98, 66},  {98, 102},  {98, 138},  {98,174},   {98,210},   {98,246}},
{{134, 66}, {134, 102}, {134, 138}, {134,174},  {134,210},  {134,246}},
{{0,0},     {170, 102}, {170, 138}, {170, 174}, {170, 210}, {0,0}},
{{0,0},     {0,0},      {206, 138}, {206, 174}, {0,0},      {0,0}}
};

struct ArrayBridgeCoords BridgeCoord[11][6] = {	
{{0,0,0,0},						{0,0,0,0},						{103, 138, 69, 72},		{0,0,0,0},						{0,0,0,0},						{0,0,0,0}},
{{0,0,0,0},						{0,0,0,0},						{101, 104, 73, 106},	{137, 140, 73, 106},	{0,0,0,0},						{0,0,0,0}},
{{0,0,0,0},						{67, 102, 105, 108}, 	{103, 138, 105, 108}, {139, 174, 105, 108},	{0,0,0,0},						{0,0,0,0}},
{{0,0,0,0},						{65, 68, 107, 142}, 	{101, 104, 107, 142}, {137, 140, 107, 142}, {173, 176, 107, 142},	{0,0,0,0}},
{{31, 66, 141, 144}, 	{67, 102, 141, 144}, 	{103, 138, 141, 144}, {139, 174, 141, 144}, {175, 210, 141, 144},	{0,0,0,0}},
{{29, 32, 143, 178}, 	{65, 68, 143, 178}, 	{101, 104, 143, 178}, {137, 140, 143, 178}, {173, 176, 143, 178}, {209, 212, 143, 178}},
{{31, 66, 177, 180}, 	{67, 102, 177, 180}, 	{103, 138, 177, 180}, {139, 174, 177, 180}, {175, 210, 177, 180},	{0,0,0,0}},
{{0,0,0,0},						{65, 68, 179, 214}, 	{101, 104, 179, 214}, {137, 140, 179, 214}, {173, 176, 179, 214},	{0,0,0,0}},
{{0,0,0,0},						{67, 102, 213, 216}, 	{103, 138, 213, 216}, {139, 174, 213, 216},	{0,0,0,0},						{0,0,0,0}},
{{0,0,0,0},						{0,0,0,0},						{101, 104, 215, 250}, {137, 140, 215, 250},	{0,0,0,0},						{0,0,0,0}},
{{0,0,0,0},						{0,0,0,0},						{103, 138, 249, 252},	{0,0,0,0},						{0,0,0,0},						{0,0,0,0}}																		
};
/*Ownership Tracker*/
int own_box[5][5] = {0};
int own_nodes[6][6] = {0};
int own_bridges[11][6] = {0};

/*ResourceTracker*/
/*Red/Blue/Yellow/Green*/
int Player1Res[4];
int Player2Res[4];
int Score[2] = {0}; //Player 1 : Player 2

int NodePointX = 0;
int NodePointY = 0;
int BridgePointX = 0;
int BridgePointY = 0;

/*Box Randomized*/
int box_color[13]={0,1,1,1,2,2,2,3,3,3,4,4,4};	// Keep track of color of box
int box_count[13]={0,1,1,1,1,2,2,2,2,3,3,3,3};	// Keep track of color count
int box_avail[13]={3,3,3,3,3,3,3,3,3,3,3,3,3};	// Keep track of available box

void shuffle(int *array, int n) {
	srand(300);
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
									11,
									BLACK);
		}
	}
}

void NodePointer() {
	LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
							NodeCoord[NodePointX][NodePointY].Y,
							11,
							DARKGREEN);
	delay(100);
	LCD_Circle( NodeCoord[NodePointX][NodePointY].X,
							NodeCoord[NodePointX][NodePointY].Y,
							11,
							BLACK);
	delay(100);
}
void BridgePointer() {
	LCD_FillRect( BridgeCoord[BridgePointX][BridgePointY].XS,
								BridgeCoord[BridgePointX][BridgePointY].XF,
								BridgeCoord[BridgePointX][BridgePointY].YS,
								BridgeCoord[BridgePointX][BridgePointY].XF,
								RED);
	delay(100);
	LCD_FillRect( BridgeCoord[BridgePointX][BridgePointY].XS,
								BridgeCoord[BridgePointX][BridgePointY].XF,
								BridgeCoord[BridgePointX][BridgePointY].YS,
								BridgeCoord[BridgePointX][BridgePointY].XF,
								WHITE);
	delay(100);
}
int main(void){
	// Clock Setup
	System_Clock_Init();
		
	// Initialize the SPI peripheral.
	GPIO_Init();
	SPI_Init();
	SysTick_Init();
	
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
	while(1) {
		for (int i=0;i<6;i++) {
			for (int j=0;j<6;j++) {
				NodePointer();
				NodePointX++;
			}
			NodePointX=0;
			NodePointY++;
		}
		NodePointX=0;
		NodePointY=0;
	}
}

void ScoreHandler() {
	Score[0] = 0;
	Score[1] = 0;
	for (int i=0; i<10; i++) {
		for (int j=0; j<5; j++) {
			if (own_bridges[i][j] == 1) {
				Score[0]++;
			} else if (own_bridges[i][j] == 2) {
				Score[1]++;
			}
		}
	}
}

void JoystickHandler() {
	if (direction == UP) {
		if (NodeMode == 1) {
			if ((NodePointY==2 && (NodePointX==0 | NodePointX==5)) |
					(NodePointY==1 && (NodePointX==1 | NodePointX==4)) |
					(NodePointY==0 && (NodePointX==2 | NodePointX==3))) {
				//N/A
			} else {
				NodePointY--;
			}
		}
		if (BridgeMode == 1) {
			if ((BridgePointX==0 && BridgePointY==4) |
					(BridgePointX==1 && BridgePointY==2) |
					(BridgePointX==2 && BridgePointY==0) |
					(BridgePointX==3 && BridgePointY==1) |
					(BridgePointX==4 && BridgePointY==3) |
					(BridgePointX==5 && BridgePointY==5)) {
				if ((BridgePointX==0 && BridgePointY==4) |
						(BridgePointX==1 && BridgePointY==2)) {
					BridgePointX++;
					BridgePointY--;
				} else if (	(BridgePointX==3 && BridgePointY==1) |
										(BridgePointX==4 && BridgePointY==3) |
										(BridgePointX==5 && BridgePointY==5)) {
					BridgePointX--;
					BridgePointY--;
				}
				//N/A
			} else {
				BridgePointY--;
			}
		}
	}
	if (direction == DOWN) {
		if (NodeMode == 1) {
			if ((NodePointY==3 && (NodePointX==0 | NodePointX==5)) |
					(NodePointY==4 && (NodePointX==1 | NodePointX==4)) |
					(NodePointY==5 && (NodePointX==2 | NodePointX==3))) {
				//N/A
			} else {
				NodePointY++;
			}
		}
		if (BridgeMode == 1) {
			if ((BridgePointX==0 && BridgePointY==6) |
					(BridgePointX==1 && BridgePointY==8) |
					(BridgePointX==2 && BridgePointY==10) |
					(BridgePointX==3 && BridgePointY==9) |
					(BridgePointX==4 && BridgePointY==7) |
					(BridgePointX==5 && BridgePointY==5)) {
				if ((BridgePointX==0 && BridgePointY==6) |
						(BridgePointX==1 && BridgePointY==8)) {
					BridgePointY++;
					BridgePointX++;
				} else if (	(BridgePointX==3 && BridgePointY==9) |
										(BridgePointX==4 && BridgePointY==7) |
										(BridgePointX==5 && BridgePointY==5)) {
					BridgePointX--;
					BridgePointY++;
				}
				//N/A
			} else {
				BridgePointY++;
			}
		}
	}
	if (direction == LEFT) {
		if (NodeMode == 1) {
			if ((NodePointX==2 && (NodePointY==0 | NodePointY==5)) | (NodePointX==1 && (NodePointY==1 | NodePointY==4)) | (NodePointX==0 && (NodePointY==2 | NodePointY==3))) {
				//N/A
			} else {
				NodePointX--;
			}
		}
		if (BridgeMode == 1) {
			if ((BridgePointX==0 && (BridgePointY==4 | BridgePointY==5 | BridgePointY==6)) |
					(BridgePointX==1 && (BridgePointY==2 | BridgePointY==3 | BridgePointY==7 | BridgePointY==8)) |
					(BridgePointX==2 && (BridgePointY==0 | BridgePointY==1 | BridgePointY==9 | BridgePointY==10))){
				if ((BridgePointX==1 && BridgePointY==2) |
						(BridgePointX==2 && BridgePointY==0)) {
					BridgePointY++;		
				} else if (	(BridgePointX==1 && BridgePointY==3) |
										(BridgePointX==2 && BridgePointY==1)) {
					BridgePointX--;
					BridgePointY++;
				} else if (	(BridgePointX==1 && BridgePointY==8) |
										(BridgePointX==2 && BridgePointY==10)) {
					BridgePointY--;
				} else if (	(BridgePointX==1 && BridgePointY==7) |
										(BridgePointX==2 && BridgePointY==9)) {
					BridgePointY--;
					BridgePointX--;
				}
				//N/A
			} else {
				BridgePointX--;
			}
		}
	}
	if (direction == RIGHT) {
		if (NodeMode == 1) {
			if ((NodePointX==3 && (NodePointY==0 | NodePointY==5)) | (NodePointX==4 && (NodePointY==1 | NodePointY==4)) | (NodePointX==5 && (NodePointY==2 | NodePointY==3))) {
				//N/A
			} else {
				NodePointX++;
			}
		}
		if (BridgeMode == 1) {
			if ((BridgePointX==2 && (BridgePointY==0 | BridgePointY==10)) |
					(BridgePointX==3 && (BridgePointY==1 | BridgePointY==2 | BridgePointY==8 | BridgePointY==9)) |
					(BridgePointX==4 && (BridgePointY==3 | BridgePointY==4 | BridgePointY==6 | BridgePointY==7)) |
					(BridgePointX==5 && BridgePointY==5)){
				if ((BridgePointX==2 && BridgePointY==0) |
						(BridgePointX==3 && BridgePointY==2) |
						(BridgePointX==4 && BridgePointY==4)) {
					BridgePointY++;
					BridgePointX++;
				} else if (	(BridgePointX==3 && BridgePointY==1) |
										(BridgePointX==4 && BridgePointY==3)) {
					BridgePointY++;
				} else if (	(BridgePointX==2 && BridgePointY==10) |
										(BridgePointX==3 && BridgePointY==8)	|
										(BridgePointX==4 && BridgePointY==6)) {
					BridgePointY--;
					BridgePointX++;
				} else if (	(BridgePointX==3 && BridgePointY==9) |
										(BridgePointX==4 && BridgePointY==7)) {
					BridgePointY--;
				}
				//N/A
			} else {
				BridgePointX++;
			}
		}
	}
}
