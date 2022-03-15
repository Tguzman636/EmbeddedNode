#include "stm32l476xx.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "GPIO.h"
#include "SSPI.h"
#include "UTIL.h"
#include "LCD_Draw.h"
#include "I2C.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "game.h"

extern uint8_t BLACK;
extern uint8_t WHITE;
extern uint8_t RED;
extern uint8_t LIGHTBLUE;
extern uint8_t BLUE;
extern uint8_t LIGHTGREEN;
extern uint8_t DARKGREEN;
extern uint8_t GREEN;
extern uint8_t YELLOW;
extern uint8_t ORANGE;
extern uint8_t PURPLE;

#define NBMODE 		0x01
#define DIRECT		0x80
#define SWITCH		0x07
#define PURCHASE 	0x02

uint8_t direction;

/*Modes {Node = 1, Bridge = 2}*/ 
int MODE = 1;

/*Turn {Player 1 = 1, Player 2 = 2}*/ 
int TURN = 1;

int LOCK = 1;

/*Pointers*/
int NodePointX = 2;
int NodePointY = 0;
int BridgePointX = 2;
int BridgePointY = 0;

/*Box Randomized*/
int box_color[13]={0,1,1,1,2,2,2,3,3,3,4,4,4};	// Keep track of color of box
int box_count[13]={0,1,1,1,1,2,2,2,2,3,3,3,3};	// Keep track of color dots
int ack_box[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};

/*Ownership {Player X}*/
int node_owner[6][6]={0};
int bridge_owner[11][6]={0};

/*ResourceTracker Red/Blue/Yellow/Green */
int Player1Res[4] = {1,1,3,3};
int Player2Res[4] = {2,2,6,6};

int Initial_state = 1;

/*Score Tracker*/
int Score[2] = {0,0}; //Player 1 : Player 2
int BridgeScore[2] = {0,0};

struct ArrayCoords {
	int X;
	int Y;
};

struct resourcenode {
	int NumOfNodes;
	int dots;
	int color;
	int owner;
};

struct ArrayBridgeCoords {
	int XS;
	int XF;
	int YS;
	int YF;
};

struct resourcenode resourceavailable[7][7] = {
{{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
{{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
{{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
{{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
{{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
{{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}},
{{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}}
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
{{0,0,0,0},						{0,0,0,0},						{109, 133, 69, 72},		{0,0,0,0},						{0,0,0,0},						{0,0,0,0}},						//H
{{0,0,0,0},						{0,0,0,0},						{101, 104, 77, 101},	{137, 140, 77, 101},	{0,0,0,0},						{0,0,0,0}},						//V
{{0,0,0,0},						{73, 97, 105, 108}, 	{109, 133, 105, 108}, {145, 169, 105, 108},	{0,0,0,0},						{0,0,0,0}},						//H
{{0,0,0,0},						{65, 68, 113, 137}, 	{101, 104, 113, 137}, {137, 140, 113, 137}, {173, 176, 113, 137},	{0,0,0,0}},						//V
{{37, 61, 141, 144}, 	{73, 97, 141, 144}, 	{109, 133, 141, 144}, {145, 169, 141, 144}, {181, 205, 141, 144},	{0,0,0,0}},						//H
{{29, 32, 149, 173}, 	{65, 68, 149, 173}, 	{101, 104, 149, 173}, {137, 140, 149, 173}, {173, 176, 149, 173}, {209, 212, 149, 173}},//V
{{37, 61, 177, 180}, 	{73, 97, 177, 180}, 	{109, 133, 177, 180}, {145, 169, 177, 180}, {181, 205, 177, 180},	{0,0,0,0}},						//H
{{0,0,0,0},						{65, 68, 185, 209}, 	{101, 104, 185, 209}, {137, 140, 185, 209}, {173, 176, 185, 209},	{0,0,0,0}},						//V
{{0,0,0,0},						{73, 97, 213, 216}, 	{109, 133, 213, 216}, {145, 169, 213, 216},	{0,0,0,0},						{0,0,0,0}},						//H
{{0,0,0,0},						{0,0,0,0},						{101, 104, 221, 245}, {137, 140, 221, 245},	{0,0,0,0},						{0,0,0,0}},						//V
{{0,0,0,0},						{0,0,0,0},						{109, 133, 249, 252},	{0,0,0,0},						{0,0,0,0},						{0,0,0,0}}						//H										
};

void initialize_arrays() {
	for (int i = 0; i<6; i++) {
		for (int j=0; j<6;j++) {
			node_owner[i][j] = 0;
		}
	}
	for (int i = 0; i<11; i++) {
		for (int j=0; j<6;j++) {
			bridge_owner[i][j] = 0;
		}
	}
	for (int i = 0; i<7; i++) {
		for (int j = 0; j < 7; j++) {
			resourceavailable[i][j].NumOfNodes = 0;
			resourceavailable[i][j].owner = 0;
		}
	}
	resourceavailable[3][1].color = box_color[0];
	resourceavailable[2][2].color = box_color[1];
	resourceavailable[3][2].color = box_color[2];
	resourceavailable[4][2].color = box_color[3];
	resourceavailable[1][3].color = box_color[4];
	resourceavailable[2][3].color = box_color[5];
	resourceavailable[3][3].color = box_color[6];
	resourceavailable[4][3].color = box_color[7];
	resourceavailable[5][3].color = box_color[8];
	resourceavailable[2][4].color = box_color[9];
	resourceavailable[3][4].color = box_color[10];
	resourceavailable[4][4].color = box_color[11];
	resourceavailable[3][5].color = box_color[12];
}

void TurnIndicator() {
	int YS = 70;
	int YF = 90;
	if (TURN == 1) {
		LCD_FillRect(200, 220, YS, YF, BLACK);
		int XS = 20;
		int XF = 40;
		LCD_FillRect(XS, XF, YS, YF, ORANGE);
	} else if (TURN == 2) {
		LCD_FillRect(20, 40, YS, YF, BLACK);
		int XS = 200;
		int XF = 220;
		LCD_FillRect(XS, XF, YS, YF, PURPLE);
	}
}

void ScoreHandler() {
	LOCK = 1;
	LCD_DrawNumTitle(140, 10, 20, 40, 4, 10, BLACK);
	LCD_DrawNumTitle(160, 10, 20, 40, 4, 10, BLACK);
	LCD_DrawNumTitle(200, 10, 20, 40, 4, 10, BLACK);
	LCD_DrawNumTitle(220, 10, 20, 40, 4, 10, BLACK);
	LCD_DrawNumTitle(140, 10, 20, 40, 4, floor((Score[0]+BridgeScore[0])/10), ORANGE);
	LCD_DrawNumTitle(160, 10, 20, 40, 4, (Score[0]+BridgeScore[0])%10, ORANGE);
	LCD_DrawNumTitle(200, 10, 20, 40, 4, floor((Score[1]+BridgeScore[1])/10), PURPLE);
	LCD_DrawNumTitle(220, 10, 20, 40, 4, (Score[1]+BridgeScore[1])%10, PURPLE);
	if ((Score[0]+BridgeScore[0]) >= 15 | (Score[1]+BridgeScore[1]) >= 15) {
		while ((Score[0]+BridgeScore[0]) >= 15) {
			LCD_DrawNumTitle(140, 10, 20, 40, 4, 10, BLACK);
			LCD_DrawNumTitle(160, 10, 20, 40, 4, 10, BLACK);
			for (int i=0;i<200000;i++);
			LCD_DrawNumTitle(140, 10, 20, 40, 4, floor((Score[0]+BridgeScore[0])/10), ORANGE);
			LCD_DrawNumTitle(160, 10, 20, 40, 4, (Score[0]+BridgeScore[0])%10, ORANGE);
			for (int i=0;i<(2000000);i++);
		}
	}
	LOCK = 0;
}

void RefreshResourceBoard(int mode) {
	int thickness = 2;
	int RightPixel = 70;
	int LeftPixel = 15;
	int height = 20;
	int length = 14;
	if (TURN == 1) {
		if (mode == 1) {
		LCD_DrawNumTitle(10+LeftPixel, 270, 2*length, 2*height+5, thickness, 10, BLACK);
		LCD_DrawNumTitle(10+LeftPixel, 270, length, height, thickness, floor(Player1Res[0]/10), ORANGE);
		LCD_DrawNumTitle(10+LeftPixel+length, 270, length, height, thickness, Player1Res[0]%10, ORANGE);
		LCD_DrawNumTitle(10+LeftPixel, 295, length, height, thickness, floor(Player1Res[1]/10), ORANGE);
		LCD_DrawNumTitle(10+LeftPixel+length, 295, length, height, thickness, Player1Res[1]%10, ORANGE);
		} else {
		LCD_DrawNumTitle(10+RightPixel, 270, 2*length, 2*height+5, thickness, 10, BLACK);
		LCD_DrawNumTitle(10+RightPixel, 270, length, height, thickness, floor(Player1Res[2]/10), ORANGE);
		LCD_DrawNumTitle(10+RightPixel+length, 270, length, height, thickness, Player1Res[2]%10, ORANGE);
		LCD_DrawNumTitle(10+RightPixel, 295, length, height, thickness, floor(Player1Res[3]/10), ORANGE);
		LCD_DrawNumTitle(10+RightPixel+length, 295, length, height, thickness, Player1Res[3]%10, ORANGE);
		}
	} else if (TURN == 2) {
		if (mode == 1) {
		LCD_DrawNumTitle(140+LeftPixel, 270, 2*length, 2*height+5, thickness, 10, BLACK);
		LCD_DrawNumTitle(140+LeftPixel, 270, length, height, thickness, floor(Player2Res[0]/10), PURPLE);
		LCD_DrawNumTitle(140+LeftPixel+length, 270, length, height, thickness, Player2Res[0]%10, PURPLE);
		LCD_DrawNumTitle(140+LeftPixel, 295, length, height, thickness, floor(Player2Res[1]/10), PURPLE);
		LCD_DrawNumTitle(140+LeftPixel+length, 295, length, height, thickness, Player2Res[1]%10, PURPLE);
		} else {
		LCD_DrawNumTitle(140+RightPixel, 270, 2*length, 2*height+5, thickness, 10, BLACK);
		LCD_DrawNumTitle(140+RightPixel, 270, length, height, thickness, floor(Player2Res[2]/10), PURPLE);
		LCD_DrawNumTitle(140+RightPixel+length, 270, length, height, thickness, Player2Res[2]%10, PURPLE);
		LCD_DrawNumTitle(140+RightPixel, 295, length, height, thickness, floor(Player2Res[3]/10), PURPLE);
		LCD_DrawNumTitle(140+RightPixel+length, 295, length, height, thickness, Player2Res[3]%10, PURPLE);
		}
	}
}

void GatherResource() {
	if (TURN == 1) {
		for (int i=0; i<6;i++) {
			for (int j=0; j<6;j++) {
				if (node_owner[i][j] == 1) {
					for (int x=0; x<2;x++) {
						for (int y = 0; y<2;y++) {
							if ((resourceavailable[i+x][j+y].owner != 2)) {
								Player1Res[(resourceavailable[i+x][j+y].color)-1]++;
							}
						}
					}
				}
			}
		}
	} else if (TURN == 2) {
		for (int i=0; i<6;i++) {
			for (int j=0; j<6;j++) {
				if (node_owner[i][j] == 2) {
					for (int x=0; x<2;x++) {
						for (int y = 0; y<2;y++) {
							if ((resourceavailable[i+x][j+y].owner != 1)) {
								Player2Res[(resourceavailable[i+x][j+y].color)-1]++;
							}
						}
					}
				}
			}
		}
	}
	RefreshResourceBoard(1);
	RefreshResourceBoard(2);
}

void PurchaseNode() {
	if (node_owner[NodePointX][NodePointY] == 0) {
		if (TURN == 1) {
			if (Player1Res[2] >= 3 && Player1Res[3] >= 3) {
				node_owner[NodePointX][NodePointY] = 1;
				Player1Res[2]-=3;
				Player1Res[3]-=3;
				LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
							NodeCoord[NodePointX][NodePointY].Y,
							10,
							ORANGE);
				LCD_Circle(	NodeCoord[NodePointX][NodePointY].X+2,
							NodeCoord[NodePointX][NodePointY].Y+2,
							6,
							DARKGREEN);
				Score[0]++;
				RefreshResourceBoard(2);
			}
		} else if (TURN == 2) {
			if (Player2Res[2] >= 3 && Player2Res[3] >= 3) {
				node_owner[NodePointX][NodePointY] = 2;
				Player2Res[2]-=3;
				Player2Res[3]-=3;
				LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
							NodeCoord[NodePointX][NodePointY].Y,
							10,
							PURPLE);
				LCD_Circle(	NodeCoord[NodePointX][NodePointY].X+2,
							NodeCoord[NodePointX][NodePointY].Y+2,
							6,
							DARKGREEN);
				Score[1]++;
				RefreshResourceBoard(2);
			}
		}
	}
}

void DrawDot(int player, int x, int y) {
	int offset = 18;
	if (player == 1) {
	LCD_Circle(NodeCoord[x-1][y-1].X+offset,
						 NodeCoord[x-1][y-1].Y+offset,
						 10,
						 ORANGE);
	} else {
	LCD_Circle(NodeCoord[x-1][y-1].X+offset,
						 NodeCoord[x-1][y-1].Y+offset,
						 10,
						 PURPLE);	
	}
}

void BoxCheck() {
	if ((bridge_owner[0][2] == TURN) &
			(bridge_owner[2][2] == TURN) &
			(bridge_owner[1][2] == TURN) &
			(bridge_owner[1][3] == TURN) &
			(ack_box[0] != 1)) {
		resourceavailable[3][1].owner = TURN;
		ack_box[0] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 3, 1);
	}
	if ((bridge_owner[2][1] == TURN) &
			(bridge_owner[3][1] == TURN) &
			(bridge_owner[4][1] == TURN) &
			(bridge_owner[3][2] == TURN) &
			(ack_box[1] != 1)) {
		resourceavailable[2][2].owner = TURN;
		ack_box[1] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 2, 2);
	}
	if ((bridge_owner[2][2] == TURN) &
			(bridge_owner[3][2] == TURN) &
			(bridge_owner[4][2] == TURN) &
			(bridge_owner[3][3] == TURN) &
			(ack_box[2] != 1)) {
		resourceavailable[3][2].owner = TURN;
		ack_box[2] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 3, 2);
	}
	if ((bridge_owner[2][3] == TURN) &
			(bridge_owner[3][3] == TURN) &
			(bridge_owner[4][3] == TURN) &
			(bridge_owner[3][4] == TURN) &
			(ack_box[3] != 1)) {
		resourceavailable[4][2].owner = TURN;
		ack_box[3] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 4, 2);
	}
	if ((bridge_owner[4][0] == TURN) &
			(bridge_owner[5][0] == TURN) &
			(bridge_owner[6][0] == TURN) &
			(bridge_owner[5][1] == TURN) &
			(ack_box[4] != 1)) {
		resourceavailable[1][3].owner = TURN;
		ack_box[4] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 1, 3);
	}
	if ((bridge_owner[4][1] == TURN) &
			(bridge_owner[5][1] == TURN) &
			(bridge_owner[6][1] == TURN) &
			(bridge_owner[5][2] == TURN) &
			(ack_box[5] != 1)) {
		resourceavailable[2][3].owner = TURN;
		ack_box[5] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 2, 3);
	}
	if ((bridge_owner[4][2] == TURN) &
			(bridge_owner[5][2] == TURN) &
			(bridge_owner[6][2] == TURN) &
			(bridge_owner[5][3] == TURN) &
			(ack_box[6] != 1)) {
		resourceavailable[3][3].owner = TURN;
		ack_box[6] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 3, 3);
	}
	if ((bridge_owner[4][3] == TURN) &
			(bridge_owner[5][3] == TURN) &
			(bridge_owner[6][3] == TURN) &
			(bridge_owner[5][4] == TURN) &
			(ack_box[7] != 1)) {
		resourceavailable[4][3].owner = TURN;
		ack_box[7] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 4, 3);
	}
	if ((bridge_owner[4][4] == TURN) &
			(bridge_owner[5][4] == TURN) &
			(bridge_owner[6][4] == TURN) &
			(bridge_owner[5][5] == TURN) &
			(ack_box[8] != 1)) {
		resourceavailable[5][3].owner = TURN;
		ack_box[8] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 5, 3);
	}
	if ((bridge_owner[6][1] == TURN) &
			(bridge_owner[7][1] == TURN) &
			(bridge_owner[8][1] == TURN) &
			(bridge_owner[7][2] == TURN) &
			(ack_box[9] != 1)) {
		resourceavailable[2][4].owner = TURN;
		ack_box[9] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 2, 4);
	}
	if ((bridge_owner[6][2] == TURN) &
			(bridge_owner[7][2] == TURN) &
			(bridge_owner[8][2] == TURN) &
			(bridge_owner[7][3] == TURN) &
			(ack_box[10] != 1)) {
		resourceavailable[3][4].owner = TURN;
		ack_box[10] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 3, 4);
	}
	if ((bridge_owner[6][3] == TURN) &
			(bridge_owner[7][3] == TURN) &
			(bridge_owner[8][3] == TURN) &
			(bridge_owner[7][4] == TURN) &
			(ack_box[11] != 1)) {
		resourceavailable[4][4].owner = TURN;
		ack_box[11] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 4, 4);
	}
	if ((bridge_owner[8][2] == TURN) &
			(bridge_owner[9][2] == TURN) &
			(bridge_owner[10][2] == TURN) &
			(bridge_owner[9][3] == TURN) &
			(ack_box[12] != 1)) {
		resourceavailable[3][5].owner = TURN;
		ack_box[12] = 1;
		Score[TURN-1]+=3;
		DrawDot(TURN, 3, 5);
	}
}

void RefreshBridgePoint() {
	int temp[2] = {0,0};
	for (int i=0; i<10; i++) {
		for (int j=0; j<5; j++) {
			if (bridge_owner[i][j] == 1) {
				temp[0]++;
			} else if (bridge_owner[i][j] == 2) {
				temp[1]++;
			}
		}
	}
	if (temp[1] > temp[0]) {
		BridgeScore[1] = 2;
	} else if (temp[0] > temp[1]) {
		BridgeScore[0] = 2;
	} else {
		BridgeScore[0] = 0;
		BridgeScore[1] = 0;
	}
}
void PurchaseBridge() {
	if (bridge_owner[BridgePointY][BridgePointX] == 0) {
		if (TURN == 1) {
			if (Player1Res[0] >= 1 && Player1Res[1]>=1) {
				bridge_owner[BridgePointY][BridgePointX] = 1;
				Player1Res[0]--;
				Player1Res[1]--;
				LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS,
								BridgeCoord[BridgePointY][BridgePointX].XF,
								BridgeCoord[BridgePointY][BridgePointX].YS,
								BridgeCoord[BridgePointY][BridgePointX].YF,
								ORANGE);
				LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS+1,
								BridgeCoord[BridgePointY][BridgePointX].XF-1,
								BridgeCoord[BridgePointY][BridgePointX].YS+1,
								BridgeCoord[BridgePointY][BridgePointX].YF-1,
								DARKGREEN);
				RefreshResourceBoard(1);
			}
		} else if (TURN == 2) {
			if (Player2Res[0] >= 1 && Player2Res[1]>=1) {
				bridge_owner[BridgePointY][BridgePointX] = 2;
				Player2Res[0]--;
				Player2Res[1]--;
				LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS,
									BridgeCoord[BridgePointY][BridgePointX].XF,
									BridgeCoord[BridgePointY][BridgePointX].YS,
									BridgeCoord[BridgePointY][BridgePointX].YF,
									PURPLE);
				LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS+1,
									BridgeCoord[BridgePointY][BridgePointX].XF-1,
									BridgeCoord[BridgePointY][BridgePointX].YS+1,
									BridgeCoord[BridgePointY][BridgePointX].YF-1,
									DARKGREEN);
				RefreshResourceBoard(1);
			}
		}
		BoxCheck();
	}
}

void NodePointerClear() {
	if ((node_owner[NodePointX][NodePointY] != 1) &&
			(node_owner[NodePointX][NodePointY] != 2)) {
		LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
								NodeCoord[NodePointX][NodePointY].Y,
								10,
								BLACK);
	} else if (node_owner[NodePointX][NodePointY] == 1) {
		LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
								NodeCoord[NodePointX][NodePointY].Y,
								10,
								ORANGE);
	} else if (node_owner[NodePointX][NodePointY] == 2) {
		LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
								NodeCoord[NodePointX][NodePointY].Y,
								10,
								PURPLE);
	}
}

void NodePointer() {
	LCD_Circle(	NodeCoord[NodePointX][NodePointY].X+2,
							NodeCoord[NodePointX][NodePointY].Y+2,
							6,
							DARKGREEN);
}

void BridgePointerClear() {
	if ((bridge_owner[BridgePointY][BridgePointX] != 1) &&
			(bridge_owner[BridgePointY][BridgePointX] != 2)) {
		LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS,
								BridgeCoord[BridgePointY][BridgePointX].XF,
								BridgeCoord[BridgePointY][BridgePointX].YS,
								BridgeCoord[BridgePointY][BridgePointX].YF,
								WHITE);
	} else if (bridge_owner[BridgePointY][BridgePointX] == 1) {
		LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS,
								BridgeCoord[BridgePointY][BridgePointX].XF,
								BridgeCoord[BridgePointY][BridgePointX].YS,
								BridgeCoord[BridgePointY][BridgePointX].YF,
								ORANGE);
	} else if (bridge_owner[BridgePointY][BridgePointX] == 2) {
		LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS,
								BridgeCoord[BridgePointY][BridgePointX].XF,
								BridgeCoord[BridgePointY][BridgePointX].YS,
								BridgeCoord[BridgePointY][BridgePointX].YF,
								PURPLE);
	}
}
void BridgePointer() {
	LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS+1,
								BridgeCoord[BridgePointY][BridgePointX].XF-1,
								BridgeCoord[BridgePointY][BridgePointX].YS+1,
								BridgeCoord[BridgePointY][BridgePointX].YF-1,
								DARKGREEN);
}

void JoystickHandler(uint8_t direction[]) {
	if ((direction[1] == DIRECT) && (direction[0] == DIRECT) && (direction[5] == NBMODE)) {
		MODE = (MODE%2)+1;
		if (MODE == 1) {
			BridgePointerClear();
			NodePointer();
		} else if (MODE == 2) {
			NodePointerClear();
			BridgePointer();
		}
	} else if ((direction[1] > DIRECT) && (direction[0] == DIRECT)) { // Up
		if (MODE == 1) {
			if ((NodePointY==2 && (NodePointX==0 | NodePointX==5)) |
					(NodePointY==1 && (NodePointX==1 | NodePointX==4)) |
					(NodePointY==0 && (NodePointX==2 | NodePointX==3))) {
				//N/A
			} else {
				NodePointerClear();
				NodePointY--;
				NodePointer();
			}
		} else if (MODE == 2) {
			if ((BridgePointX==0 && BridgePointY==4) |
					(BridgePointX==1 && BridgePointY==2) |
					(BridgePointX==2 && BridgePointY==0) |
					(BridgePointX==3 && BridgePointY==1) |
					(BridgePointX==4 && BridgePointY==3) |
					(BridgePointX==5 && BridgePointY==5)) {
				BridgePointerClear();
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
				BridgePointer();
			} else {
				BridgePointerClear();
				BridgePointY--;
				BridgePointer();
			}
		}
	} else if ((direction[1] < DIRECT) && (direction[0] == DIRECT) && (direction[5] == 0x03)) { //Down
		if (MODE == 1) {
			if ((NodePointY==3 && (NodePointX==0 | NodePointX==5)) |
					(NodePointY==4 && (NodePointX==1 | NodePointX==4)) |
					(NodePointY==5 && (NodePointX==2 | NodePointX==3))) {
				//N/A
			} else {
				NodePointerClear();
				NodePointY++;
				NodePointer();
			}
		} else if (MODE == 2) {
			if ((BridgePointX==0 && BridgePointY==6) |
					(BridgePointX==1 && BridgePointY==8) |
					(BridgePointX==2 && BridgePointY==10) |
					(BridgePointX==3 && BridgePointY==9) |
					(BridgePointX==4 && BridgePointY==7) |
					(BridgePointX==5 && BridgePointY==5)) {
				BridgePointerClear();
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
				BridgePointer();
			} else {
				BridgePointerClear();
				BridgePointY++;
				BridgePointer();
			}
		}
	} else if ((direction[1] == DIRECT) && (direction[0] < DIRECT)) {
		if (MODE == 1) {
			if ((NodePointX==2 && (NodePointY==0 | NodePointY==5)) | (NodePointX==1 && (NodePointY==1 | NodePointY==4)) | (NodePointX==0 && (NodePointY==2 | NodePointY==3))) {
				//N/A
			} else {
				NodePointerClear();
				NodePointX--;
				NodePointer();
			}
		} else if (MODE == 2) {
			if ((BridgePointX==0 && (BridgePointY==4 | BridgePointY==5 | BridgePointY==6)) |
					(BridgePointX==1 && (BridgePointY==2 | BridgePointY==3 | BridgePointY==7 | BridgePointY==8)) |
					(BridgePointX==2 && (BridgePointY==0 | BridgePointY==1 | BridgePointY==9 | BridgePointY==10))){
				BridgePointerClear();
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
				BridgePointer();
			} else {
				BridgePointerClear();
				BridgePointX--;
				BridgePointer();
			}
		}
	} else if ((direction[1] == DIRECT) && (direction[0] > DIRECT)) {
		if (MODE == 1) {
			if ((NodePointX==3 && (NodePointY==0 | NodePointY==5)) | (NodePointX==4 && (NodePointY==1 | NodePointY==4)) | (NodePointX==5 && (NodePointY==2 | NodePointY==3))) {
				//N/A
			} else {
				NodePointerClear();
				NodePointX++;
				NodePointer();
			}
		} else if (MODE == 2) {
			if ((BridgePointX==2 && (BridgePointY==0 | BridgePointY==10)) |
					(BridgePointX==3 && (BridgePointY==1 | BridgePointY==2 | BridgePointY==8 | BridgePointY==9)) |
					(BridgePointX==4 && (BridgePointY==3 | BridgePointY==4 | BridgePointY==6 | BridgePointY==7)) |
					(BridgePointX==5 && BridgePointY==5)){
				BridgePointerClear();
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
				BridgePointer();
			} else {
				BridgePointerClear();
				BridgePointX++;
				BridgePointer(); 
			}
		}
	} else if ((direction[1] < DIRECT) && (direction[0] == DIRECT) && (direction[5] == NBMODE)) {
		NodePointerClear();
		BridgePointerClear();
		if (Initial_state == 0) {
			RefreshBridgePoint();
			ScoreHandler();
		}
		TURN = (TURN%2)+1;
		TurnIndicator();
		if (Initial_state == 0) {
			GatherResource();
		}
		if (Initial_state == 2) {
			Player1Res[0] = 1;
			Player1Res[1] = 1;
			Player1Res[2] = 3;
			Player1Res[3] = 3;
			RefreshResourceBoard(0);
			RefreshResourceBoard(1); 
			Initial_state = 0;
		}
		if (Initial_state == 1) {
			Initial_state = 2;
		}
		NodePointX = 2;
		NodePointY = 0;
		BridgePointX = 2;
		BridgePointY = 0;
		NodePointer();
		MODE = 1;
	} else if (direction[5] == PURCHASE) {
		if (MODE == 1) {
			PurchaseNode();
		} else if (MODE == 2) {
			PurchaseBridge();
		}
	}
}
