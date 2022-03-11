#include <stdio.h>

#include "game.h"

#define NODE 			1
#define BRIDGE 		2
#define UP 				3
#define DOWN 			4
#define LEFT 			5
#define RIGHT			6
#define SWITCH		7
#define PURCHASE 	8

int direction;

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
			}
		}
	}
}

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
	}
}

void JoystickHandler() {
	if (direction == NODE) {
		MODE = 1;
	} else if (direction == BRIDGE) {
		MODE = 2;
	} else if (direction == UP) {
		if (MODE == 1) {
			if ((NodePointY==2 && (NodePointX==0 | NodePointX==5)) |
					(NodePointY==1 && (NodePointX==1 | NodePointX==4)) |
					(NodePointY==0 && (NodePointX==2 | NodePointX==3))) {
				//N/A
			} else {
				NodePointY--;
			}
		} else if (MODE == 2) {
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
	} else if (direction == DOWN) {
		if (MODE == 1) {
			if ((NodePointY==3 && (NodePointX==0 | NodePointX==5)) |
					(NodePointY==4 && (NodePointX==1 | NodePointX==4)) |
					(NodePointY==5 && (NodePointX==2 | NodePointX==3))) {
				//N/A
			} else {
				NodePointY++;
			}
		} else if (MODE == 2) {
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
	} else if (direction == LEFT) {
		if (MODE == 1) {
			if ((NodePointX==2 && (NodePointY==0 | NodePointY==5)) | (NodePointX==1 && (NodePointY==1 | NodePointY==4)) | (NodePointX==0 && (NodePointY==2 | NodePointY==3))) {
				//N/A
			} else {
				NodePointX--;
			}
		} else if (MODE == 2) {
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
	} else if (direction == RIGHT) {
		if (MODE == 1) {
			if ((NodePointX==3 && (NodePointY==0 | NodePointY==5)) | (NodePointX==4 && (NodePointY==1 | NodePointY==4)) | (NodePointX==5 && (NodePointY==2 | NodePointY==3))) {
				//N/A
			} else {
				NodePointX++;
			}
		} else if (MODE == 2) {
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
	} else if (direction == SWITCH) {
		ScoreHandler();
		TURN = (TURN%2)+1;
		GatherResource();
		NodePointX = 2;
		NodePointY = 0;
		BridgePointX = 2;
		BridgePointY = 0;
		MODE = 1;
	} else if (direction == PURCHASE) {
		if (MODE == 1) {
			PurchaseNode();
		} else if (MODE == 2) {
			PurchaseBridge();
		}
	}
}
