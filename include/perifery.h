 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _PERIFERY_
#define _PERIFERY_


#define RELAY_PIN         5
#define RELAY_DDR         DDRA
#define RELAY_PORT        PORTA

#define RELAY_INIT        \
    do { \
        RELAY_DDR |= _BV( RELAY_PIN ); \
    } while( 0 );

#define RELAY_HIGH        \
    do { \
        RELAY_PORT |= _BV( RELAY_PIN ); \
    } while( 0 );

#define RELAY_LOW         \
    do { \
        RELAY_PORT &= ~( _BV( RELAY_PIN ) ); \
    } while( 0 );



#include "porting.h"


                /* Set system clock prescaler */
                void vInit_Clock();  
                
                /* Initialization of the Timer 0 */
                void vInit_Timer0();  
                
                /* Stop the Timer 0 */
                void vDeInit_Timer0(); 
                
                /* Initialization of the Timer 2 */
                void vInit_RTC_Timer2();  
                
                /* Initialization of the external interrupt INT2 */
                void vInit_ExtInt2();   
                
                /* Init internal relay port */
                void vInit_InternalRelais(); 
                
                /* Turn internat relais on/off */
                void vSet_InternalRelais(UCHAR on);
                
                /* Set error led */
				void vSetError(UCHAR error);


#endif