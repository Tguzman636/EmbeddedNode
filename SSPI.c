#include "SSPI.h"
#define SCALE 256

uint16_t RGBLT[256] = {
    0x0000, 0x000a, 0x0015, 0x001f, 0x0120, 0x012a, 0x0135, 0x013f, 
    0x0240, 0x024a, 0x0255, 0x025f, 0x0360, 0x036a, 0x0375, 0x037f, 
    0x0480, 0x048a, 0x0495, 0x049f, 0x05a0, 0x05aa, 0x05b5, 0x05bf, 
    0x06c0, 0x06ca, 0x06d5, 0x06df, 0x07e0, 0x07ea, 0x07f5, 0x07ff, 
    0x2000, 0x200a, 0x2015, 0x201f, 0x2120, 0x212a, 0x2135, 0x213f, 
    0x2240, 0x224a, 0x2255, 0x225f, 0x2360, 0x236a, 0x2375, 0x237f, 
    0x2480, 0x248a, 0x2495, 0x249f, 0x25a0, 0x25aa, 0x25b5, 0x25bf, 
    0x26c0, 0x26ca, 0x26d5, 0x26df, 0x27e0, 0x27ea, 0x27f5, 0x27ff, 
    0x4800, 0x480a, 0x4815, 0x481f, 0x4920, 0x492a, 0x4935, 0x493f, 
    0x4a40, 0x4a4a, 0x4a55, 0x4a5f, 0x4b60, 0x4b6a, 0x4b75, 0x4b7f, 
    0x4c80, 0x4c8a, 0x4c95, 0x4c9f, 0x4da0, 0x4daa, 0x4db5, 0x4dbf, 
    0x4ec0, 0x4eca, 0x4ed5, 0x4edf, 0x4fe0, 0x4fea, 0x4ff5, 0x4fff, 
    0x6800, 0x680a, 0x6815, 0x681f, 0x6920, 0x692a, 0x6935, 0x693f, 
    0x6a40, 0x6a4a, 0x6a55, 0x6a5f, 0x6b60, 0x6b6a, 0x6b75, 0x6b7f, 
    0x6c80, 0x6c8a, 0x6c95, 0x6c9f, 0x6da0, 0x6daa, 0x6db5, 0x6dbf, 
    0x6ec0, 0x6eca, 0x6ed5, 0x6edf, 0x6fe0, 0x6fea, 0x6ff5, 0x6fff, 
    0x9000, 0x900a, 0x9015, 0x901f, 0x9120, 0x912a, 0x9135, 0x913f, 
    0x9240, 0x924a, 0x9255, 0x925f, 0x9360, 0x936a, 0x9375, 0x937f, 
    0x9480, 0x948a, 0x9495, 0x949f, 0x95a0, 0x95aa, 0x95b5, 0x95bf, 
    0x96c0, 0x96ca, 0x96d5, 0x96df, 0x97e0, 0x97ea, 0x97f5, 0x97ff, 
    0xb000, 0xb00a, 0xb015, 0xb01f, 0xb120, 0xb12a, 0xb135, 0xb13f, 
    0xb240, 0xb24a, 0xb255, 0xb25f, 0xb360, 0xb36a, 0xb375, 0xb37f, 
    0xb480, 0xb48a, 0xb495, 0xb49f, 0xb5a0, 0xb5aa, 0xb5b5, 0xb5bf, 
    0xb6c0, 0xb6ca, 0xb6d5, 0xb6df, 0xb7e0, 0xb7ea, 0xb7f5, 0xb7ff, 
    0xd800, 0xd80a, 0xd815, 0xd81f, 0xd920, 0xd92a, 0xd935, 0xd93f, 
    0xda40, 0xda4a, 0xda55, 0xda5f, 0xdb60, 0xdb6a, 0xdb75, 0xdb7f, 
    0xdc80, 0xdc8a, 0xdc95, 0xdc9f, 0xdda0, 0xddaa, 0xddb5, 0xddbf, 
    0xdec0, 0xdeca, 0xded5, 0xdedf, 0xdfe0, 0xdfea, 0xdff5, 0xdfff, 
    0xf800, 0xf80a, 0xf815, 0xf81f, 0xf920, 0xf92a, 0xf935, 0xf93f, 
    0xfa40, 0xfa4a, 0xfa55, 0xfa5f, 0xfb60, 0xfb6a, 0xfb75, 0xfb7f, 
    0xfc80, 0xfc8a, 0xfc95, 0xfc9f, 0xfda0, 0xfdaa, 0xfdb5, 0xfdbf, 
    0xfec0, 0xfeca, 0xfed5, 0xfedf, 0xffe0, 0xffea, 0xfff5, 0xffff 
};

/*
 * Write a byte of data using software SPI. For each bit:
 * 1. Pull the clock pin low.
 * 2. Set the 'MOSI' data pin to the correct value.
 * 3. Pull the clock pin high.
 */
void sspi_w(uint8_t dat) {
  uint8_t sspi_i;
  // Send 8 bits, with the MSB first.
  for (sspi_i = 0x80; sspi_i != 0x00; sspi_i >>= 1) {
    GPIOB->ODR &= ~GPIO_ODR_OD3;
    if (dat & sspi_i) {
      GPIOB->ODR |=  GPIO_ODR_OD5;
    }
    else {
      GPIOB->ODR &= ~GPIO_ODR_OD5;
    }
    GPIOB->ODR |=  GPIO_ODR_OD3;
  }
}

/*
 * Write a 'Command byte' over software SPI.
 * "4-wire" SPI interfaces are common in devices like
 * screens. A 'Data/Command' pin determines whether
 * the SPI data is an instruction for the display,
 * or pixel data for the framebuffer RAM.
 * Here, '0' means 'Command' and '1' means 'Data'.
 */
void sspi_cmd(uint8_t cdat) {
  // Pull the 'D/C' pin low, write data, pull 'D/C' high.
  GPIOB->ODR &= ~GPIO_ODR_OD5;
  sspi_w(cdat);
  GPIOB->ODR |=  GPIO_ODR_OD5;
}


/*
 * Send a byte of data over hardware SPI.
 * This method does not wait for the communication to finish.
 * (The STM32 has an onboard FIFO queue, so we can check
 *  whether that has space for writing instead.)
 */
void hspi_w8(uint8_t dat) {
  // Wait for TXE.
  while (!(SPI1->SR & SPI_SR_TXE)) {};
  // Send the byte.
  *((volatile uint8_t*)&SPI1->DR) = dat;
//	while( (SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY );
}

/*
 * Send 16 bits of data over hardware SPI.
 * This method does not wait for the communication to finish.
 * It adds two bytes to the FIFO queue at once; that's one
 * pixel for a 16-bit color display.
 *
 * On L0 platforms, packing 2 data frames at once does
 * not appear to be supported.
 */
void hspi_w16(uint16_t dat) {

//  // Wait for TXE.
//  while (!(SPI1->SR & SPI_SR_TXE)) {};
//  // Send the data.
//  // (Flip the bytes for the little-endian ARM core.)
//  dat = (((dat & 0x00FF) << 8) | ((dat & 0xFF00) >> 8));
//  *((volatile uint16_t*)&SPI1->DR) = dat;
//	while( (SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY );

  hspi_w8((uint8_t)(dat >> 8));
  hspi_w8((uint8_t)(dat & 0xFF));
}

void hspimod_w16(uint8_t dat) {

//  // Wait for TXE.
//  while (!(SPI1->SR & SPI_SR_TXE)) {};
//  // Send the data.
//  // (Flip the bytes for the little-endian ARM core.)
//  dat = (((dat & 0x00FF) << 8) | ((dat & 0xFF00) >> 8));
//  *((volatile uint16_t*)&SPI1->DR) = dat;
//	while( (SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY );
	uint16_t color = RGBLT[(int)dat];
  hspi_w8((uint8_t)(color >> 8));
  hspi_w8((uint8_t)(color & 0xFF));
}


/*
 * Send a 'command' byte over hardware SPI.
 * Pull the 'D/C' pin low, send the byte, then pull the pin high.
 * Wait for the transmission to finish before changing the
 * 'D/C' pin value.
 */
void hspi_cmd(uint8_t cmd) {
  while ((SPI1->SR & SPI_SR_BSY)) {};
  GPIOB->ODR &= ~GPIO_ODR_OD4;
  hspi_w8(cmd);
  while ((SPI1->SR & SPI_SR_BSY)) {};
  GPIOB->ODR |=  GPIO_ODR_OD4;
}

void ILI9341_Enable(){
	GPIOA->ODR |= GPIO_ODR_OD15;
}

void ILI9341_RESET(){
	GPIOA->ODR &= ~GPIO_ODR_OD15;
	for (int i=0;i<200000;i++);
	GPIOA->ODR &= ~GPIO_ODR_OD12;
	for (int i=0;i<200000;i++);
	GPIOA->ODR |= GPIO_ODR_OD15;
}
