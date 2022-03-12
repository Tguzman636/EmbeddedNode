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

#define NODE 			0x01
#define BRIDGE 		2
#define DIRECT		0x80
#define SWITCH		7
#define PURCHASE 	0x02

uint8_t direction;

/*Modes {Node = 1, Bridge = 2}*/ 
int MODE = 1;

/*Turn {Player 1 = 1, Player 2 = 2}*/ 
int TURN = 1;

/*Pointers*/
int NodePointX = 2;
int NodePointY = 0;
int BridgePointX = 2;
int BridgePointY = 0;

/*Box Randomized*/
int box_color[13]={0,1,1,1,2,2,2,3,3,3,4,4,4};	// Keep track of color of box
int box_count[13]={0,1,1,1,1,2,2,2,2,3,3,3,3};	// Keep track of color dots

/*Ownership {Player X}*/
int node_owner[6][6]={0};
int bridge_owner[11][6]={0};

/*ResourceTracker Red/Blue/Yellow/Green */
int Player1Res[4] = {0,0,20,20};
int Player2Res[4] = {0,0,4,4};

/*Score Tracker*/
int Score[2] = {0}; //Player 1 : Player 2

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

void ScoreHandler() {
	Score[0] = 0;
	Score[1] = 0;
	for (int i=0; i<10; i++) {
		for (int j=0; j<5; j++) {
			if (bridge_owner[i][j] == 1) {
				Score[0]+=2;
			} else if (bridge_owner[i][j] == 2) {
				Score[1]+=2;
			}
		}
	}
	for (int i = 0; i< 6; i++) {
		for (int j=0;i<6;j++){
			if (node_owner[i][j] == 1) {
				Score[0]++;
			} else if (node_owner[i][j] == 2) {
				Score[1]++;
			}
		}
	}
	for (int i=0; i<7;i++) {
		for (int j=0;j<7;j++) {
			if (resourceavailable[i][j].owner==1) {
				Score[0]++;
			} else if (resourceavailable[i][j].owner==2) {
				Score[1]++;
			}
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
							if ((resourceavailable[i+x][j+y].NumOfNodes<=resourceavailable[i+x][j+y].dots) |
									(resourceavailable[i+x][j+y].owner==1)) {
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
							if ((resourceavailable[i+x][j+y].NumOfNodes<=resourceavailable[i+x][j+y].dots) |
									(resourceavailable[i+x][j+y].owner==2)) {
								Player2Res[(resourceavailable[i+x][j+y].color)-1]++;
							}
						}
					}
				}
			}
		}
	}
}

void PurchaseNode() {
	if (node_owner[NodePointX][NodePointY] == 0) {
		if (TURN == 1) {
			if (Player1Res[2] >= 2 && Player1Res[3]>=2) {
				node_owner[NodePointX][NodePointY] = 1;
				resourceavailable[NodePointX][NodePointY].NumOfNodes++;
				resourceavailable[NodePointX+1][NodePointY+1].NumOfNodes++;
				resourceavailable[NodePointX+1][NodePointY].NumOfNodes++;
				resourceavailable[NodePointX][NodePointY+1].NumOfNodes++;
				Player1Res[2]-=2;
				Player1Res[3]-=2;
				LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
							NodeCoord[NodePointX][NodePointY].Y,
							10,
							RED);
				LCD_Circle(	NodeCoord[NodePointX][NodePointY].X+2,
							NodeCoord[NodePointX][NodePointY].Y+2,
							6,
							DARKGREEN);
			}
		} else if (TURN == 2) {
			if (Player2Res[2] >= 2 && Player2Res[3]>=2) {
				node_owner[NodePointX][NodePointY] = 2;
				resourceavailable[NodePointX][NodePointY].NumOfNodes++;
				resourceavailable[NodePointX+1][NodePointY+1].NumOfNodes++;
				resourceavailable[NodePointX+1][NodePointY].NumOfNodes++;
				resourceavailable[NodePointX][NodePointY+1].NumOfNodes++;
				Player2Res[2]-=2;
				Player2Res[3]-=2;
				LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
							NodeCoord[NodePointX][NodePointY].Y,
							10,
							LIGHTBLUE);
				LCD_Circle(	NodeCoord[NodePointX][NodePointY].X+2,
							NodeCoord[NodePointX][NodePointY].Y+2,
							6,
							DARKGREEN);
			}
		}
	}
}

//void BoxCheck() {
//	for (int i=0;i<4;i++) {
//		if (node_owner[
//			// Still Figuring this out
//	}
//}

void PurchaseBridge() {
	if (bridge_owner[BridgePointX][BridgePointY] == 0) {
		if (TURN == 1) {
			if (Player1Res[0] >= 1 && Player1Res[3]>=1) {
				bridge_owner[BridgePointX][BridgePointY] = 1;
				Player1Res[0]--;
				Player1Res[1]--;
			}
		} else if (TURN == 2) {
			if (Player2Res[0] >= 1 && Player2Res[3]>=1) {
				bridge_owner[BridgePointX][BridgePointY] = 2;
				Player2Res[0]--;
				Player2Res[1]--;
			}
		}
		//BoxCheck();
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
								RED);
	} else if (node_owner[NodePointX][NodePointY] == 2) {
		LCD_Circle(	NodeCoord[NodePointX][NodePointY].X,
								NodeCoord[NodePointX][NodePointY].Y,
								10,
								LIGHTBLUE);
	}
}

void NodePointer() {
	LCD_Circle(	NodeCoord[NodePointX][NodePointY].X+2,
							NodeCoord[NodePointX][NodePointY].Y+2,
							6,
							DARKGREEN);
}

void BridgePointerClear() {
	LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS,
								BridgeCoord[BridgePointY][BridgePointX].XF,
								BridgeCoord[BridgePointY][BridgePointX].YS,
								BridgeCoord[BridgePointY][BridgePointX].YF,
								WHITE);
}
void BridgePointer() {
	LCD_FillRect( BridgeCoord[BridgePointY][BridgePointX].XS,
								BridgeCoord[BridgePointY][BridgePointX].XF,
								BridgeCoord[BridgePointY][BridgePointX].YS,
								BridgeCoord[BridgePointY][BridgePointX].YF,
								DARKGREEN);
}

void JoystickHandler(uint8_t direction[]) {
	if (direction[5] == MODE) {
		MODE = (MODE%2)+1;
		if (MODE == 1) {
			BridgePointerClear();
			NodePointer();
		} else if (MODE == 2) {
			NodePointerClear();
			BridgePointer();
		}
	} else if ((direction[1] > DIRECT) && (direction[0] == DIRECT)) {
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
	} else if ((direction[1] < DIRECT) && (direction[0] == DIRECT)) {
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
	} else if (direction[5] == SWITCH) {
		NodePointerClear();
		//ScoreHandler();
		TURN = (TURN%2)+1;
		//GatherResource();
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
