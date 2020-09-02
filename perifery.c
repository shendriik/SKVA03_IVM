 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
 
 
/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"
#include "porting.h"

/* -------------------------- Own includes ------------------------------------*/
#include "defines_my.h"
#include "perifery.h"


void vInit_Clock()
{

    // Crystal Oscillator division factor: 1
    #pragma optsize-
    CLKPR=0x80;
    CLKPR=0x00;
    #ifdef _OPTIMIZE_SIZE_
    #pragma optsize+
    #endif          
}

/* Initialization of the Timer 0 */
void vInit_Timer0()
{

    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: 62,500 kHz
    // Mode: Normal top=0xFF
    // OC0A output: Disconnected
    // OC0B output: Disconnected
    TCCR0A=0x00;
    TCCR0B=0x03;
    TCNT0=0x00;
    OCR0A=0x00;
    OCR0B=0x00;    
    TIMSK0=0x01;
         
}

/* Stop the Timer 0 */
void vDeInit_Timer0()
{

    TCCR0A=0x00;
    TCCR0B=0x00;
    TCNT0=0x00;
    OCR0A=0x00;
    OCR0B=0x00;    
    TIMSK0=0x00;
         
}

/* Initialization of the Timer 2 */
void vInit_RTC_Timer2()
{

    // Timer/Counter 2 initialization
    // Clock source: Crystal on TOSC1 pin
    // Clock value: PCK2/128
    // Mode: Normal top=0xFF
    // OC2A output: Disconnected
    // OC2B output: Disconnected
    ASSR=0x20;
    TCCR2A=0x00;
    TCCR2B=0x05;
    TCNT2=0x00;
    OCR2A=0x00;
    OCR2B=0x00;  
    while (ASSR & ((1 << TCN2UB)|(1 << OCR2AUB)|(1 << OCR2BUB)|(1 << TCR2AUB)|(1 << TCR2BUB)));
    TIMSK2=0x01;

}


/* Initialization of the external interrupt INT2 */
void vInit_ExtInt2()
{

    // INT2 Mode: Falling Edge
    EICRA=0x20;
    EIMSK=0x04;
    EIFR=0x04;
    PCICR=0x00;      
    
}

void vInit_InternalRelais()
{
   RELAY_INIT;              
}

void vSet_InternalRelais(UCHAR on)
{
    if(on) 
    {
        RELAY_HIGH; 
    }
    else
    {
        RELAY_LOW;
    }
}


void vSetError(UCHAR error)
{
    ERROR_INIT();
    if(error)
    {
        ERROR_PORT |= _BV( ERROR_PIN );    
    } 
    else
    {
        ERROR_PORT &= ~_BV( ERROR_PIN );   
    }
}








