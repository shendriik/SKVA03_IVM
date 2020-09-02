/*
 * FreeModbus Libary: ATMega168 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *   - Initial version and ATmega168 support
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
 * File: $Id: portserial.c,v 1.6 2006/09/17 16:45:53 wolti Exp $
 */
/* ----------------------- Platform includes ----------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


#define UART_BAUD_CALC(UART_BAUD_R,F_OSC) \
    ( ( F_OSC ) / ( ( UART_BAUD_R ) * 16UL ) - 1 )


//#define UART_UCSRB  UCSR0B


void            vMBPortClose( void ){}


void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{

#ifdef DERE_ENABLE
    UCSRB |= _BV( TXEN ) | _BV(TXCIE);     
#else
    UCSRB |= _BV( TXEN );     
#endif

#ifdef  RTS_LED_ENABLE
        RTS_LED_INIT;                
        RTS_LED_LOW;         
#endif

    if( xRxEnable )
    {    
        UCSRB |= _BV( RXEN ) | _BV( RXCIE );     
    }
    else
    {    
       UCSRB &= ~( _BV( RXEN ) | _BV( RXCIE ) );      
    }    
    
    if( xTxEnable )
    {                           
        UCSRB |= _BV( TXEN ) | _BV( UDRIE ); 
                  
#ifdef  DERE_ENABLE 
        DERE_HIGH;
#endif

#ifdef  RTS_LED_ENABLE 
        RTS_LED_HIGH;
#endif

    }
    else
    {          
        UCSRB &= ~( _BV( UDRIE ) );   
    }           
}


BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{        
    UCHAR ucUCSRC = 0;

    (void)ucPORT;
	
    UBRR = UART_BAUD_CALC( ulBaudRate, F_CPU );
            
    
    
    switch ( eParity )
    {
        case MB_PAR_EVEN:
            ucUCSRC |= _BV( UPM1 ); 
            break;
        case MB_PAR_ODD:
            ucUCSRC |= _BV( UPM1 ) | _BV( UPM0 ); 
            break;
        case MB_PAR_NONE:
            break;
    }

    switch ( ucDataBits )
    {
        case 8:
            ucUCSRC |= _BV( UCSZ0 ) | _BV( UCSZ1 ); 
            break;
        case 7:
            ucUCSRC |= _BV( UCSZ1 );
            break;               
    }

#if defined (__AVR_ATmega168__)
    UCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega169__)
    UCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega8__)
    UCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega16__)
    UCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega32__)
    UCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega128__)
    UCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega324PA__)
    UCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega1284P__)
    UCSRC |= ucUCSRC;
#endif
 
    vMBPortSerialEnable(FALSE, FALSE);   
                         
#ifdef DERE_ENABLE   
        DERE_INIT;
#endif

    return TRUE;
}

 BOOL
xMBPortSerialPutByte( CHAR ucByte )
{

    UDR = ucByte;
    return TRUE;      
    
}

 BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{

    *pucByte = UDR; 
    return TRUE;       
    
}


// USART Receiver interrupt service routine. Look in isr.c
//interrupt [USART1_RX_vect_num] void USART1_RX_vect(void)
//{  
//
//   pxMBFrameCBByteReceived(  );  
//       
//}             


// USART Transmitter interrupt service routine
#ifdef DERE_ENABLE
interrupt [USART1_TX_vect_num] void USART1_TX_vect(void)
{

    DERE_LOW;  
    
}
#endif


// USART Data Empty
interrupt [USART1_UDRE_vect_num] void USART1_UDRE_vect(void)
{
    pxMBFrameCBTransmitterEmpty(  );      
}