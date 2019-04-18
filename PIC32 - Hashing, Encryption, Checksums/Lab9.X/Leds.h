/* 
 * File:   Leds.h
 * Author: Brian Tong
 * No collaboration 
 * 
 * Created on May 16, 2017, 5:35 PM
 */

#ifndef LEDS_H
#define	LEDS_H

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>

// Sets the TRISE and LATE registers to 0
#define LEDS_INIT() do { \
    TRISE = 0x00; \
    LATE = 0x00; \
} while(0) 

// Sets LATE register to x
#define LEDS_SET(x) do { \
    LATE = (x); \
} while(0)

// Returns the value of the LATE register
#define LEDS_GET LATE 

#endif

