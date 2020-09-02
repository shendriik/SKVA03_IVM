/*
 * FreeModbus Libary: ATMega168 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer.c,v 1.4 2006/09/03 11:53:10 wolti Exp $
 */

/* ----------------------- AVR includes -------------------------------------*/


/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_TIMER_PRESCALER      ( 1024UL )
#define MB_TIMER_TICKS          ( F_CPU / MB_TIMER_PRESCALER )
#define MB_50US_TICKS           ( 20000UL )

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usTimerOCRADelta;
static USHORT   usTimerOCRBDelta;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim3Timerout50us )
{

    /* Calculate overflow counter an OCR values for Timer3. */
    usTimerOCRADelta =
        ((ULONG)MB_TIMER_TICKS * usTim3Timerout50us ) / ( MB_50US_TICKS );
    
    TCCR3A = 0x00;
    TCCR3B = 0x00;
    TCCR3C = 0x00;
          
    vMBPortTimersDisable(  );
                
    return TRUE; 
    
}


inline void vMBPortTimersEnable( )
{    

    TCNT3 = 0x0000;
    if( usTimerOCRADelta > 0 )
    {
        TIMSK3 |= _BV( OCIE3A ); 
        OCR3A = usTimerOCRADelta;
    }

    TCCR3B |= _BV( CS12 ) | _BV( CS10 ); 
    
}


inline void
vMBPortTimersDisable(  )
{

    /* Disable the timer. */
    TCCR3B &= ~( _BV( CS12 ) | _BV( CS10 ) );

    /* Disable the output compare interrupts for channel A/B. */
    TIMSK3 &= ~( _BV( OCIE3A ) );  
    
    /* Clear output compare flags for channel A/B. */
    TIFR3 |= _BV( OCF3A ) ;                 
    
}

// Timer1 output compare A interrupt service routine
interrupt [TIMER3_COMPA_vect_num] void TIMER3_COMPA_vect(void)
{   
  
    ( void )pxMBPortCBTimerExpired(  );
               
}

