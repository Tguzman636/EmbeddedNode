#ifndef _VVC_SSPI_H
#define _VVC_SSPI_H

#include "stm32l476xx.h"
#include "UTIL.h"
#include "GPIO.h"

void sspi_w(uint8_t dat);
// Write a 'command' byte for 4-wire SPI interfaces.
void sspi_cmd(uint8_t cdat);

/* Hardware SPI functions. */
void hspi_init(SPI_TypeDef *SPIx);
void hspi_w8(uint8_t dat);
void hspi_w16(uint16_t dat);
void hspimod_w16(uint8_t dat);
void hspi_cmd(uint8_t cmd);
void ILI9341_RESET();
void ILI9341_Enable();
#endif
