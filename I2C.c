#include "I2C.h"

extern void Error_Handler(void);

// Inter-integrated Circuit Interface (I2C)
// up to 100 Kbit/s in the standard mode, 
// up to 400 Kbit/s in the fast mode, and 
// up to 3.4 Mbit/s in the high-speed mode.

// Recommended external pull-up resistance is 
// 4.7 kOmh for low speed, 
// 3.0 kOmh for the standard mode, and 
// 1.0 kOmh for the fast mode
	
//===============================================================================
//                        I2C GPIO Initialization
//===============================================================================
void I2C_GPIO_Init(void) {
	// Enable GPIO clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	// Set Pins PB6 & PB7 to Alternative Functions
	GPIOB->MODER &= ~GPIO_MODER_MODE6;
	GPIOB->MODER &= ~GPIO_MODER_MODE7;
	GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
	
	// Set to AF4
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6;
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL7;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_2;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL7_2;
	
	// Very High Speed
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR6;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR7;
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7);
	
	// Open-Drain Output
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT6;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT7;
	GPIOB->OTYPER |= (GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
	
	// Pull up Resistors
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7;
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0);
}
	
#define I2C_TIMINGR_PRESC_POS	28
#define I2C_TIMINGR_SCLDEL_POS	20
#define I2C_TIMINGR_SDADEL_POS	16
#define I2C_TIMINGR_SCLH_POS	8
#define I2C_TIMINGR_SCLL_POS	0

//===============================================================================
//                          I2C Initialization
//===============================================================================
void I2C_Initialization(void){
	uint32_t OwnAddr = 0x52;
	// I2C1 Clock Enable
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
	
	// System clock as clock source
	RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;
	RCC->CCIPR |= RCC_CCIPR_I2C1SEL_0;
	
	// Peripheral Reset Register
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;
	
	// Disable I2C1
	I2C1->CR1 &= ~I2C_CR1_PE;
	
	// Enable Analog Noise Filter
	I2C1->CR1 &= ~I2C_CR1_ANFOFF;
	
	// Disable Digital Noise Filter
	I2C1->CR1 &= ~I2C_CR1_DNF;
	
	// Enable Error Interrupts
	I2C1->CR1 |= I2C_CR1_ERRIE;
	
	// Enable Clock Stretching
	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;
	
	// Set master to operate in 7-bit addressing mode
	I2C1->CR2 &= ~I2C_CR2_ADD10;
	
	// Enable automatic end mode
	I2C1->CR2 |= I2C_CR2_AUTOEND;
	
	// Enable NACK generation
	I2C1->CR2 |= I2C_CR2_NACK;
	
	// Prescalar = 7, ClkSpeed = 80MHz / 8 = 10MHz
	// tSCLDEL = 1us, 		tSDADEL = 1.25us, 	tSCLH = 4us, 			tSCLL = 4.7us
	// SCLDEL = 19, 			SDADEL = 22, 				SCLH = 49, 				SCLL = 56
	// SCLDELnew = 2us, SDADELnew = 2.3us,	SCLHnew = 5us,	SCLLnew = 5.7us
	
	#define TIMINGR_PRESC		7
	#define TIMINGR_SCLDEL	15 // Max
	#define TIMINGR_SDADEL	15 // Max
	#define TIMINGR_SCLH		49
	#define TIMINGR_SCLL		56

	I2C1->TIMINGR |= TIMINGR_PRESC<<I2C_TIMINGR_PRESC_POS;
	I2C1->TIMINGR |= TIMINGR_SCLDEL<<I2C_TIMINGR_SCLDEL_POS;
	I2C1->TIMINGR |= TIMINGR_SDADEL<<I2C_TIMINGR_SDADEL_POS;
	I2C1->TIMINGR |= TIMINGR_SCLH<<I2C_TIMINGR_SCLH_POS;
	I2C1->TIMINGR |= TIMINGR_SCLL<<I2C_TIMINGR_SCLL_POS;
	
	// Disable Own Address
	I2C1->OAR1 &= ~I2C_OAR1_OA1EN;
	I2C1->OAR2 &= ~I2C_OAR2_OA2EN;
	
	// Setting Address to 7 bit mode
	I2C1->OAR1 &= ~I2C_OAR1_OA1MODE;
	
	// Set Address to OwnAddr
	I2C1->OAR1 |= OwnAddr;
	
	// Enable Own1 Address
	I2C1->OAR1 |= I2C_OAR1_OA1EN;
	
	// Enable I2C1
	I2C1->CR1 |= I2C_CR1_PE;
}

//===============================================================================
//                           I2C Start
// Master generates START condition:
//    -- Slave address: 7 bits
//    -- Automatically generate a STOP condition after all bytes have been transmitted 
// Direction = 0: Master requests a write transfer
// Direction = 1: Master requests a read transfer
//=============================================================================== 
int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction) {	
	
	// Direction = 0: Master requests a write transfer
	// Direction = 1: Master requests a read transfer
	
	uint32_t tmpreg = 0;
	
	// This bit is set by software, and cleared by hardware after the Start followed by the address
	// sequence is sent, by an arbitration loss, by a timeout error detection, or when PE = 0.
	tmpreg = I2Cx->CR2;
	
	tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));
	
	if (Direction == READ_FROM_SLAVE)
		tmpreg |= I2C_CR2_RD_WRN;  // Read from Slave
	else
		tmpreg &= ~I2C_CR2_RD_WRN; // Write to Slave
		
	tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES));
	
	tmpreg |= I2C_CR2_START;
	
	I2Cx->CR2 = tmpreg; 
	
   	return 0;  // Success
}

//===============================================================================
//                           I2C Stop
//=============================================================================== 
void I2C_Stop(I2C_TypeDef * I2Cx){
	// Master: Generate STOP bit after the current byte has been transferred 
	I2Cx->CR2 |= I2C_CR2_STOP;								
	// Wait until STOPF flag is reset
	while( (I2Cx->ISR & I2C_ISR_STOPF) == 0 ); 
}

//===============================================================================
//                           Wait for the bus is ready
//=============================================================================== 
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx){
	// Wait until I2C bus is ready
	while( (I2Cx->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY );	// If busy, wait
}

//===============================================================================
//                           I2C Send Data
//=============================================================================== 
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if (Size <= 0 || pData == NULL) return -1;
	
	I2C_WaitLineIdle(I2Cx);
	
	if (I2C_Start(I2Cx, DeviceAddress, Size, WRITE_TO_SLAVE) < 0 ) return -1;

	// Send Data
	// Write the first data in DR register
	// while((I2Cx->ISR & I2C_ISR_TXE) == 0);
	// I2Cx->TXDR = pData[0] & I2C_TXDR_TXDATA;  

	for (i = 0; i < Size; i++) {
		// TXE is set by hardware when the I2C_TXDR register is empty. It is cleared when the next
		// data to be sent is written in the I2C_TXDR register.
		// while( (I2Cx->ISR & I2C_ISR_TXE) == 0 ); 

		// TXIS bit is set by hardware when the I2C_TXDR register is empty and the data to be
		// transmitted must be written in the I2C_TXDR register. It is cleared when the next data to be
		// sent is written in the I2C_TXDR register.
		// The TXIS flag is not set when a NACK is received.
		while((I2Cx->ISR & I2C_ISR_TXIS) == 0 );
		I2Cx->TXDR = pData[i] & I2C_TXDR_TXDATA;  // TXE is cleared by writing to the TXDR register.
	}
	
	// Wait until TC flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0 && (I2Cx->ISR & I2C_ISR_NACKF) == 0);
	
	if( (I2Cx->ISR & I2C_ISR_NACKF) != 0 ) return -1;

	I2C_Stop(I2Cx);
	return 0;
}


//===============================================================================
//                           I2C Receive Data
//=============================================================================== 
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if(Size <= 0 || pData == NULL) return -1;

	I2C_WaitLineIdle(I2Cx);

	I2C_Start(I2Cx, DeviceAddress, Size, READ_FROM_SLAVE); // 0 = sending data to the slave, 1 = receiving data from the slave
						  	
	for (i = 0; i < Size; i++) {
		// Wait until RXNE flag is set 	
		while( (I2Cx->ISR & I2C_ISR_RXNE) == 0 );
		pData[i] = I2Cx->RXDR & I2C_RXDR_RXDATA;
	}
	
	// Wait until TCR flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0);
	
	I2C_Stop(I2Cx);
	
	return 0;
}
