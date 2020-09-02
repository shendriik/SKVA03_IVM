/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*
**********************************************************************/

#ifndef _PORTING_H
#define _PORTING_H

/* ----------------------- Platform includes --------------------------------*/
#include "avr/io.h"

/* ----------------------- Defines ------------------------------------------*/



#define mmDERE_ENABLE
#define mmRTS_LED_ENABLE
#define mmF_CPU 4000000UL


#ifndef INLINE
#define	INLINE                      inline
#endif

#ifndef PR_BEGIN_EXTERN_C
#define PR_BEGIN_EXTERN_C           extern "C" {
#endif

#ifndef PR_END_EXTERN_C
#define	PR_END_EXTERN_C             }
#endif

#ifndef ENTER_CRITICAL_SECTION( )
#define ENTER_CRITICAL_SECTION( )   #asm("cli")
#endif

#ifndef EXIT_CRITICAL_SECTION( ) 
#define EXIT_CRITICAL_SECTION( )    #asm("sei")
#endif

#ifndef assert( x )
#define assert( x )
#endif

//#define _BV(b) (1 << (b)) // bit b mask

typedef char    BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

/* ----------------------- AVR platform specifics ---------------------------*/

 #if defined (__AVR_ATmega168__)
#define mmUCSRB           UCSR0B
#define mmTXEN            TXEN0
#define mmRXEN            RXEN0
#define mmRXCIE           RXCIE0
#define mmTXCIE           TXCIE0
#define mmUDRE            UDRE0
#define mmUBRR            UBRR0
#define mmUCSRC           UCSR0C
#define mmUPM1            UPM01
#define mmUPM0            UPM00
#define mmUCSZ0           UCSZ00
#define mmUCSZ1           UCSZ01
#define mmUDR             UDR0
#define mmSIG_UART_TRANS  SIG_USART_TRANS

#elif defined (__AVR_ATmega169__)
#define mmSIG_UART_TRANS  SIG_USART_TRANS

#elif defined (__AVR_ATmega8__)
#define mmUBRR            UBRRL
#define mmTCCR1C          TCCR1A  /* dummy */
#define mmTIMSK1          TIMSK
#define mmTIFR1           TIFR
//#define SIG_USART_DATA  SIG_UART_DATA
//#define SIG_USART_RECV  SIG_UART_RECV

#elif defined (__AVR_ATmega16__)
#define mmUBRR            UBRRL
#define mmTCCR1C          TCCR1A  /* dummy */
#define mmTIMSK1          TIMSK
#define mmTIFR1           TIFR

#elif defined (__AVR_ATmega32__)
#define mmUBRR            UBRRL
#define mmTCCR1C          TCCR1A  /* dummy */
#define mmTIMSK1          TIMSK
#define mmTIFR1           TIFR

#elif defined (__AVR_ATmega128__)
#define mmUCSRB           UCSR0B
#define mmUBRR            UBRR0L
#define mmUDR             UDR0
#define mmTIMSK1          TIMSK
#define mmTIFR1           TIFR
#define mmSIG_UART_TRANS  SIG_USART0_TRANS
#define mmSIG_USART_DATA  SIG_USART0_DATA
#define mmSIG_USART_RECV  SIG_USART0_RECV
#define mmUCSZ0           UCSZ00
#define mmUCSZ1           UCSZ01
#define mmUPM0            UPM00
#define mmUPM1            UPM01
#define mmUCSRC           UCSR0C

#elif defined (__AVR_ATmega324PA__)
#define mmUCSRB           UCSR0B
#define mmUBRR            UBRR0L
#define mmUDR             UDR0
#define mmSIG_UART_TRANS  SIG_USART0_TRANS
#define mmSIG_USART_DATA  SIG_USART0_DATA
#define mmSIG_USART_RECV  SIG_USART0_RECV
#define mmUCSZ0           UCSZ00
#define mmUCSZ1           UCSZ01
#define mmUPM0            UPM00
#define mmUPM1            UPM01
#define mmUCSRC           UCSR0C
#define mmRXEN            RXEN0
#define mmTXEN            TXEN0
#define mmUDRIE           UDRIE0
#define mmRXCIE           RXCIE0
#define mmTXCIE           TXCIE0

#elif defined (__AVR_ATmega1284P__)
#define mmUCSRB           UCSR0B
#define mmUBRR            UBRR0L
#define mmUDR             UDR0
#define mmSIG_UART_TRANS  SIG_USART0_TRANS
#define mmSIG_USART_DATA  SIG_USART0_DATA
#define mmSIG_USART_RECV  SIG_USART0_RECV
#define mmUCSZ0           UCSZ00
#define mmUCSZ1           UCSZ01
#define mmUPM0            UPM00
#define mmUPM1            UPM01
#define mmUCSRC           UCSR0C
#define mmRXEN            RXEN0
#define mmTXEN            TXEN0
#define mmUDRIE           UDRIE0
#define mmRXCIE           RXCIE0
#define mmTXCIE           TXCIE0
#endif  

/* ----------------------- RS485 specifics ----------------------------------*/
#ifdef  mmDERE_ENABLE
#define mmDERE_PIN         5
#define mmDERE_DDR         DDRD
#define mmDERE_PORT        PORTD

#define mmDERE_INIT        \
    do { \
        mmDERE_DDR |= _BV( mmDERE_PIN ); \
    } while( 0 );

#define mmDERE_HIGH        \
    do { \
        mmDERE_PORT |= _BV( mmDERE_PIN ); \
    } while( 0 );

#define mmDERE_LOW         \
    do { \
        mmDERE_PORT &= ~( _BV( mmDERE_PIN ) ); \
    } while( 0 );
#endif


/* ----------------------- Led indication specifics ----------------------------------*/
#ifdef  mmRTS_LED_ENABLE
#define mmRTS_LED_PIN         0
#define mmRTS_LED_DDR         DDRA
#define mmRTS_LED_PORT        PORTA

#define mmRTS_LED_INIT        \
    do { \
        mmRTS_LED_DDR |= _BV( mmRTS_LED_PIN ); \
    } while( 0 );

#define mmRTS_LED_HIGH        \
    do { \
        mmRTS_LED_PORT |= _BV( mmRTS_LED_PIN ); \
    } while( 0 );

#define mmRTS_LED_LOW         \
    do { \
        mmRTS_LED_PORT &= ~( _BV( mmRTS_LED_PIN ) ); \
    } while( 0 );
#endif




#endif
