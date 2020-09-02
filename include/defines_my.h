 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _DEFINES_MY_
#define _DEFINES_MY_

#include "avr/io.h"
#include "porting.h"


/* Buttons */
#define BUTTONS_COUNT               5

#define UP_BUTTON                   0
#define OK_BUTTON                   1
#define DOWN_BUTTON                 2
#define CHECK_BUTTON                3
#define SOUND_OFF_BUTTON            4


/* Modbus slave devices start reg */
#define DEV_START_REG               999

/* Own Modbus Slave specify */
#define REG_HOLD_START                  1000
#define REG_HOLD_NREGS                  76
#define MODBUS_SLAVE_BAUDRATE           19200
#define MODBUS_SLAVE_ID                 10
/*                           */
/* Modbus Register map       */
/*                           */
/* Function: 0x03            */
/* Start register: 1000      */
/* Registers count: 76       */
/*                           */
/* 0-7: Zone state           */
/* 8-11: Board relais states */ 
/* 12-43: Sensor value x 10  */
/* 44-75: Sensor state       */
/*                           */

/* Offset for relay boards id */
#define RELAY_ID_OFFS               10

#define INACTION_TIMELIMIT          30

/* Error pin specify */
#define ERROR_PIN       2
#define ERROR_DDR       DDRC
#define ERROR_PORT      PORTC
#define ERROR_INIT()       \
    {\                
        ERROR_DDR |= _BV( ERROR_PIN ); \ 
    } 
    
    
#endif


