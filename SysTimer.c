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

volatile uint32_t msTicks;
volatile uint32_t joystick;

extern uint8_t SlaveAddress1;
extern uint8_t SlaveAddress2;
extern uint8_t Data_Receive[6];
extern uint8_t Data_Start[2];
extern uint8_t Data_Conversion;
extern int Debug;
extern int LOCK;


//******************************************************************************************
// Initialize SysTick	
//******************************************************************************************	
void SysTick_Init(void){
	// TODO: generate 1ms interrupt
	// SysTick Control & Status Register
	SysTick->CTRL = 0; // Disable SysTick IRQ and SysTick Counter
	
	// SysTick Reload Value Register
	SysTick->LOAD = 9999; // [TODO] - Enter the correct LOAD value that will give us a 1 ms period
	
	// SysTick Current Value Register
	SysTick->VAL = 0;
	
	// Configure and Enable SysTick interrupt in NVIC
	NVIC_EnableIRQ(SysTick_IRQn);
	NVIC_SetPriority(SysTick_IRQn, 1); // Set Priority to 1
	
	// Enables SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	// 0 = counting down to zero does not assert the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Select clock source
	// If CLKSOURCE = 0, the external clock is used. The frequency of SysTick clock is the frequency of the AHB clock divided by 8.
	// If CLKSOURCE = 1, the processor clock is used.
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;		
	
	// Enable SysTick IRQ and SysTick Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  
}

//******************************************************************************************
// SysTick Interrupt Handler
//******************************************************************************************
void SysTick_Handler(void){
	msTicks++;
	joystick++;
	if (joystick > 200 && Debug != 1 && LOCK == 0) {
		Data_Conversion = 0;
		// Conversion Command
		I2C_SendData(I2C1, SlaveAddress2, &Data_Conversion, 1);
		// Read Nunchuck Data
		I2C_ReceiveData(I2C1, SlaveAddress1, Data_Receive, 6);

		JoystickHandler(Data_Receive);
		joystick = 0;
	}
}
	
//******************************************************************************************
// Delay in ms
//******************************************************************************************
void delay (uint32_t T){
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < T);
	
  msTicks = 0;
}
