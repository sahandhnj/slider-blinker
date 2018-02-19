/*
 * main.h
 *
 *  Created on: Apr 21, 2015
 *      Author: sahand
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "driverlib/rom.h"

#include "utils/uartstdio.c"
#include "CY8C201A0.h"

#define SLAVE_ADDR 0x00
#define delay(x) SysCtlDelay(BORDFRQ/1000000/3*x)
#define BORDFRQ 120000000

#define I2C_MRIS_STOPRIS        0x00000040  // STOP Detection Raw Interrupt Status
#define I2C_MRIS_STARTRIS       0x00000020  // START Detection Raw Interrupt Status


void init_board_leds();
void init_i2c();
void init_campsense();
void blink(int ledNumber, int brightness);
void slideBlinker(int status,int brightness);


int8_t I2C_Read_consecutive(uint32_t ui32Base, uint8_t ui8SlaveAddr, uint8_t ui8SlaveRegAddr, uint32_t* data, unsigned int len);
int32_t I2C_Write(uint32_t ui32Base, uint8_t ui8SlaveAddr , uint8_t ui8Data,uint8_t ui8SlaveRegAddr);
int32_t I2C_Read_Single(uint32_t ui32Base, uint8_t ui8SlaveAddr ,uint8_t ui8SlaveRegAddr);
void wait_BUS(uint32_t ui32Base);
int8_t* getBtnsStatus();
int32_t getSliderStatus();

#endif /* MAIN_H_ */
