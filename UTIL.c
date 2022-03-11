#include "UTIL.h"


// Initialize a 320x240-pixel ILI9341 TFT display.
void ili9341_sspi_init(void) {
//SOFTWARE RESET
sspi_cmd(0x01);
for (int i=0;i<200000;i++); // Some Delay
	
//POWER CONTROL A
sspi_cmd(0xCB);
sspi_w(0x39);
sspi_w(0x2C);
sspi_w(0x00);
sspi_w(0x34);
sspi_w(0x02);

//POWER CONTROL B
sspi_cmd(0xCF);
sspi_w(0x00);
sspi_w(0xC1);
sspi_w(0x30);

//DRIVER TIMING CONTROL A
sspi_cmd(0xE8);
sspi_w(0x85);
sspi_w(0x00);
sspi_w(0x78);

//DRIVER TIMING CONTROL B
sspi_cmd(0xEA);
sspi_w(0x00);
sspi_w(0x00);

//POWER ON SEQUENCE CONTROL
sspi_cmd(0xED);
sspi_w(0x64);
sspi_w(0x03);
sspi_w(0x12);
sspi_w(0x81);

//PUMP RATIO CONTROL
sspi_cmd(0xF7);
sspi_w(0x20);

//POWER CONTROL,VRH[5:0]
sspi_cmd(0xC0);
sspi_w(0x23);

//POWER CONTROL,SAP[2:0];BT[3:0]
sspi_cmd(0xC1);
sspi_w(0x10);

//VCM CONTROL
sspi_cmd(0xC5);
sspi_w(0x3E);
sspi_w(0x28);

//VCM CONTROL 2
sspi_cmd(0xC7);
sspi_w(0x86);

//MEMORY ACCESS CONTROL
sspi_cmd(0x36);
sspi_w(0x48);

//PIXEL FORMAT
sspi_cmd(0x3A);
sspi_w(0x55);

//FRAME RATIO CONTROL, STANDARD RGB COLOR
sspi_cmd(0xB1);
sspi_w(0x00);
sspi_w(0x18);

//DISPLAY FUNCTION CONTROL
sspi_cmd(0xB6);
sspi_w(0x08);
sspi_w(0x82);
sspi_w(0x27);

//3GAMMA FUNCTION DISABLE
sspi_cmd(0xF2);
sspi_w(0x00);

//GAMMA CURVE SELECTED
sspi_cmd(0x26);
sspi_w(0x01);

//POSITIVE GAMMA CORRECTION
sspi_cmd(0xE0);
sspi_w(0x0F);
sspi_w(0x31);
sspi_w(0x2B);
sspi_w(0x0C);
sspi_w(0x0E);
sspi_w(0x08);
sspi_w(0x4E);
sspi_w(0xF1);
sspi_w(0x37);
sspi_w(0x07);
sspi_w(0x10);
sspi_w(0x03);
sspi_w(0x0E);
sspi_w(0x09);
sspi_w(0x00);

//NEGATIVE GAMMA CORRECTION
sspi_cmd(0xE1);
sspi_w(0x00);
sspi_w(0x0E);
sspi_w(0x14);
sspi_w(0x03);
sspi_w(0x11);
sspi_w(0x07);
sspi_w(0x31);
sspi_w(0xC1);
sspi_w(0x48);
sspi_w(0x08);
sspi_w(0x0F);
sspi_w(0x0C);
sspi_w(0x31);
sspi_w(0x36);
sspi_w(0x0F);

//EXIT SLEEP
sspi_cmd(0x11);
for (int i=0;i<200000;i++); // Some Delay

//TURN ON DISPLAY
sspi_cmd(0x29);
}

void ili9341_hspi_init() {
	ILI9341_Enable();
	ILI9341_RESET();
	
//SOFTWARE RESET
hspi_cmd(0x01);
for (int i=0;i<200000;i++); // Some Delay
	
//POWER CONTROL A
hspi_cmd(0xCB);
hspi_w8(0x39);
hspi_w8(0x2C);
hspi_w8(0x00);
hspi_w8(0x34);
hspi_w8(0x02);

//POWER CONTROL B
hspi_cmd(0xCF);
hspi_w8(0x00);
hspi_w8(0xC1);
hspi_w8(0x30);

//DRIVER TIMING CONTROL A
hspi_cmd(0xE8);
hspi_w8(0x85);
hspi_w8(0x00);
hspi_w8(0x78);

//DRIVER TIMING CONTROL B
hspi_cmd(0xEA);
hspi_w8(0x00);
hspi_w8(0x00);

//POWER ON SEQUENCE CONTROL
hspi_cmd(0xED);
hspi_w8(0x64);
hspi_w8(0x03);
hspi_w8(0x12);
hspi_w8(0x81);

//PUMP RATIO CONTROL
hspi_cmd(0xF7);
hspi_w8(0x20);

//POWER CONTROL,VRH[5:0]
hspi_cmd(0xC0);
hspi_w8(0x23);

//POWER CONTROL,SAP[2:0];BT[3:0]
hspi_cmd(0xC1);
hspi_w8(0x10);

//VCM CONTROL
hspi_cmd(0xC5);
hspi_w8(0x3E);
hspi_w8(0x28);

//VCM CONTROL 2
hspi_cmd(0xC7);
hspi_w8(0x86);

//MEMORY ACCESS CONTROL
hspi_cmd(0x36);
hspi_w8(0x48);

//PIXEL FORMAT
hspi_cmd(0x3A);
hspi_w8(0x55);

//FRAME RATIO CONTROL, STANDARD RGB COLOR
hspi_cmd(0xB1);
hspi_w8(0x00);
hspi_w8(0x18);

//DISPLAY FUNCTION CONTROL
hspi_cmd(0xB6);
hspi_w8(0x08);
hspi_w8(0x82);
hspi_w8(0x27);

//3GAMMA FUNCTION DISABLE
hspi_cmd(0xF2);
hspi_w8(0x00);

//GAMMA CURVE SELECTED
hspi_cmd(0x26);
hspi_w8(0x01);

//POSITIVE GAMMA CORRECTION
hspi_cmd(0xE0);
hspi_w8(0x0F);
hspi_w8(0x31);
hspi_w8(0x2B);
hspi_w8(0x0C);
hspi_w8(0x0E);
hspi_w8(0x08);
hspi_w8(0x4E);
hspi_w8(0xF1);
hspi_w8(0x37);
hspi_w8(0x07);
hspi_w8(0x10);
hspi_w8(0x03);
hspi_w8(0x0E);
hspi_w8(0x09);
hspi_w8(0x00);

//NEGATIVE GAMMA CORRECTION
hspi_cmd(0xE1);
hspi_w8(0x00);
hspi_w8(0x0E);
hspi_w8(0x14);
hspi_w8(0x03);
hspi_w8(0x11);
hspi_w8(0x07);
hspi_w8(0x31);
hspi_w8(0xC1);
hspi_w8(0x48);
hspi_w8(0x08);
hspi_w8(0x0F);
hspi_w8(0x0C);
hspi_w8(0x31);
hspi_w8(0x36);
hspi_w8(0x0F);

//EXIT SLEEP
hspi_cmd(0x11);
for (int i=0;i<200000;i++); // Some Delay

//TURN ON DISPLAY
hspi_cmd(0x29);
}
