/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/
 

/* ----------------------- AVR includes -------------------------------------*/

/* ----------------------- Platform includes --------------------------------*/
#include "porting.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmconfig.h"
#include "mmporting.h"

/* ----------------------- Defines ------------------------------------------*/
#define MM_TIMER_PRESCALER      ( 1024UL )
#define MM_TIMER_FREQ           ( mmF_CPU / MM_TIMER_PRESCALER )
#define MM_50US_FREQ            ( 20000UL )

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usTimerOCRADelta;
static USHORT   usTimerOCRBDelta;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMMPortTimersInit( USHORT usTimeOut50us )
{

    /* Calculate overflow counter an OCRA value for Timer 3.5 interval. */
    usTimerOCRADelta =
        ( (ULONG)MM_TIMER_FREQ  * usTimeOut50us ) / ( MM_50US_FREQ  );
    
    /* Calculate overflow counter an OCRB value for Timer slave timeout interval */
    usTimerOCRBDelta =
        ( (ULONG)MM_SLAVE_TIMEOUT_US * MM_TIMER_FREQ ) / ( 1000 ) ; 
    
    /* Timer configuration */
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCCR1C = 0x00; 
                         
    OCR1AH = usTimerOCRADelta >> 8; 
    OCR1AL = usTimerOCRADelta & 0xFF;
    OCR1BH = usTimerOCRBDelta >> 8; 
    OCR1BL = usTimerOCRBDelta & 0xFF; 
                
    return TRUE; 
    
}


inline void vMMPortTimersEnable( UCHAR timer )
{    
    /* Reset timer */
    TCNT1 = 0x0000; 
    
    /* Enable interrupt on compare match */
    switch (timer)
    {
        case(1):
            TIMSK1|= _BV( OCIE1A ); 
            break;
            
        case(2):
            TIMSK1|= _BV( OCIE1B ); 
            break;  
    }
    
    /* Enable timer clock */
    TCCR1B|= _BV( CS12 ) | _BV( CS10 );   
}


inline void
vMMPortTimersDisable( UCHAR timer )
{  
    /* Disable timer clock */
    TCCR1B &= ~( _BV( CS12 ) | _BV( CS10 ) );
    
    switch (timer)
    {
        case(1):
        
            /* Disable the output compare interrupts for channel A. */
            TIMSK1 &= ~( _BV( OCIE1A ) );  
            /* Clear output compare flags for channel A. */
            TIFR1 |= _BV( OCF1A ) ;
            break;    
            
        case(2):
            /* Disable the output compare interrupts for channel A. */
            TIMSK1 &= ~( _BV( OCIE1B ) );  
            /* Clear output compare flags for channel A. */
            TIFR1 |= _BV( OCF1B ) ;
            break;  
    }              
    
}

// Timer1 output compare A interrupt service routine
interrupt [TIMER1_COMPA_vect_num] void TIMER1_COMPA_vect(void)
{   
                       
    vMMRTUT35TimerExpired();
               
}

// Timer1 output compare B interrupt service routine
interrupt [TIMER1_COMPB_vect_num] void TIMER1_COMPB_vect(void)
{    
 
    vMMRTUSlaveTimeoutTimerExpired( );
               
}

