/*
 * FreeModbus Libary: AVR Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *   - Initial version + ATmega168 support
 * Modfications Copyright (C) 2006 Tran Minh Hoang:
 *   - ATmega8, ATmega16, ATmega32 support
 *   - RS485 support for DS75176
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
 * File: $Id: port.h,v 1.6 2006/09/17 16:45:52 wolti Exp $
 */

#ifndef _PORT_H
#define _PORT_H

/* ----------------------- Platform includes --------------------------------*/
#include "avr/io.h"

/* ----------------------- Defines ------------------------------------------*/
#define __AVR_ATmega1284P__
#define DERE_ENABLE
#define RTS_LED_ENABLE
#define F_CPU 4000000UL


#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define ENTER_CRITICAL_SECTION( )   #asm("cli")
#define EXIT_CRITICAL_SECTION( )    #asm("sei")

#define assert( x )

#define _BV(b) (1 << (b)) // bit b mask

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
#define UCSRB           UCSR0B
#define TXEN            TXEN0
#define RXEN            RXEN0
#define RXCIE           RXCIE0
#define TXCIE           TXCIE0
#define UDRE            UDRE0
#define UBRR            UBRR0
#define UCSRC           UCSR0C
#define UPM1            UPM01
#define UPM0            UPM00
#define UCSZ0           UCSZ00
#define UCSZ1           UCSZ01
#define UDR             UDR0
#define SIG_UART_TRANS  SIG_USART_TRANS

#elif defined (__AVR_ATmega169__)
#define SIG_UART_TRANS  SIG_USART_TRANS

#elif defined (__AVR_ATmega8__)
#define UBRR            UBRRL
#define TCCR1C          TCCR1A  /* dummy */
#define TIMSK1          TIMSK
#define TIFR1           TIFR
//#define SIG_USART_DATA  SIG_UART_DATA
//#define SIG_USART_RECV  SIG_UART_RECV

#elif defined (__AVR_ATmega16__)
#define UBRR            UBRRL
#define TCCR1C          TCCR1A  /* dummy */
#define TIMSK1          TIMSK
#define TIFR1           TIFR

#elif defined (__AVR_ATmega32__)
#define UBRR            UBRRL
#define TCCR1C          TCCR1A  /* dummy */
#define TIMSK1          TIMSK
#define TIFR1           TIFR

#elif defined (__AVR_ATmega128__)
#define UCSRB           UCSR0B
#define UBRR            UBRR0L
#define UDR             UDR0
#define TIMSK1          TIMSK
#define TIFR1           TIFR
#define SIG_UART_TRANS  SIG_USART0_TRANS
#define SIG_USART_DATA  SIG_USART0_DATA
#define SIG_USART_RECV  SIG_USART0_RECV
#define UCSZ0           UCSZ00
#define UCSZ1           UCSZ01
#define UPM0            UPM00
#define UPM1            UPM01
#define UCSRC           UCSR0C

#elif defined (__AVR_ATmega1284P__)
#define UCSRB           UCSR1B
#define UBRR            UBRR1L
#define UDR             UDR1
#define SIG_UART_TRANS  SIG_USART1_TRANS
#define SIG_USART_DATA  SIG_USART1_DATA
#define SIG_USART_RECV  SIG_USART1_RECV
#define UCSZ0           UCSZ10
#define UCSZ1           UCSZ11
#define UPM0            UPM10
#define UPM1            UPM11
#define UCSRC           UCSR1C
#define RXEN            RXEN1
#define TXEN            TXEN1
#define UDRIE           UDRIE1
#define RXCIE           RXCIE1
#define TXCIE           TXCIE1

#endif  

/* ----------------------- RS485 specifics ----------------------------------*/
#ifdef  DERE_ENABLE
#define DERE_PIN         7
#define DERE_DDR         DDRD
#define DERE_PORT        PORTD

#define DERE_INIT        \
    do { \
        DERE_DDR |= _BV( DERE_PIN ); \
    } while( 0 );

#define DERE_HIGH        \
    do { \
        DERE_PORT |= _BV( DERE_PIN ); \
    } while( 0 );

#define DERE_LOW         \
    do { \
        DERE_PORT &= ~( _BV( DERE_PIN ) ); \
    } while( 0 );
#endif


/* ----------------------- Led indication specifics ----------------------------------*/
#ifdef  RTS_LED_ENABLE
#define RTS_LED_PIN         1
#define RTS_LED_DDR         DDRA
#define RTS_LED_PORT        PORTA

#define RTS_LED_INIT        \
    do { \
        RTS_LED_DDR |= _BV( RTS_LED_PIN ); \
    } while( 0 );

#define RTS_LED_HIGH        \
    do { \
        RTS_LED_PORT |= _BV( RTS_LED_PIN ); \
    } while( 0 );

#define RTS_LED_LOW         \
    do { \
        RTS_LED_PORT &= ~( _BV( RTS_LED_PIN ) ); \
    } while( 0 );
#endif




#endif
