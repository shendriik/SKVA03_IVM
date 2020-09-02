/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/

/* ----------------------- Platform includes --------------------------------*/
#include "porting.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmporting.h"

#define mmUART_BAUD_CALC(UART_BAUD_R,F_OSC) \
    ( ( F_OSC ) / ( ( UART_BAUD_R ) * 16UL ) - 1 )


void
vMMPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* TX is allways on */
    mmUCSRB |= _BV( mmTXEN );  
        
#ifdef  mmDERE_ENABLE

    mmUCSRB |= _BV(mmTXCIE);        
#endif

    if( xRxEnable )
    {    
        mmUCSRB |= _BV( mmRXEN ) | _BV( mmRXCIE );     
    }
    else
    {    
       mmUCSRB &= ~( _BV( mmRXEN ) | _BV( mmRXCIE ) );      
    }    
    
    if( xTxEnable )
    {                           
        mmUCSRB |=  _BV( mmUDRIE ); 
                  
#ifdef  mmDERE_ENABLE 

        mmDERE_HIGH;
#endif

#ifdef  mmRTS_LED_ENABLE 

        mmRTS_LED_HIGH;
#endif

    }
    else
    {          
        mmUCSRB &= ~( _BV( mmUDRIE ) ); 
        
#ifdef  mmRTS_LED_ENABLE  
            
        mmRTS_LED_LOW;         
#endif  

    }      
}


BOOL
xMMPortSerialInit(  ULONG ulBaudRate, UCHAR ucDataBits, eMMParity eParity )
{        
    UCHAR ucUCSRC = 0;
	
    mmUBRR = mmUART_BAUD_CALC( ulBaudRate, mmF_CPU ); 
    
    switch ( eParity )
    {
        case MM_PAR_EVEN:
            ucUCSRC |= _BV( mmUPM1 ); 
            break;
        case MM_PAR_ODD:
            ucUCSRC |= _BV( mmUPM1 ) | _BV( mmUPM0 ); 
            break;
        case MM_PAR_NONE:
            break;
    }

    switch ( ucDataBits )
    {
        case 8:
            ucUCSRC |= _BV( mmUCSZ0 ) | _BV( mmUCSZ1 ); 
            break;
        case 7:
            ucUCSRC |= _BV( mmUCSZ1 );
            break;               
    }

#if defined (__AVR_ATmega168__)
    mmUCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega169__)
    mmUCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega8__)
    mmUCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega16__)
    mmUCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega32__)
    mmUCSRC = _BV( URSEL ) | ucUCSRC;
#elif defined (__AVR_ATmega128__)
    mmUCSRC |= ucUCSRC;  
#elif defined (__AVR_ATmega324PA__)
    mmUCSRC |= ucUCSRC;
#elif defined (__AVR_ATmega1284P__)
    mmUCSRC |= ucUCSRC;
#endif
                          
#ifdef mmDERE_ENABLE   
    mmDERE_INIT;       
#endif

#ifdef  mmRTS_LED_ENABLE
    mmRTS_LED_INIT;   
#endif
    
    vMMPortSerialEnable(FALSE, FALSE);   
    return TRUE;
}


BOOL
xMMPortSerialPutByte( CHAR ucByte )
{

    mmUDR = ucByte;
    return TRUE;      
    
}

BOOL
xMMPortSerialGetByte( CHAR * pucByte )
{

    *pucByte = mmUDR; 
    return TRUE;       
    
}


/* USART Receiver interrupt service routine */
interrupt [USART0_RX_vect_num] void USART0_RX_vect(void)
{  

   vMMRTUReceiveFSM(); 
   
}              


/* USART Transmitter interrupt service routine */
#ifdef mmDERE_ENABLE
interrupt [USART0_TX_vect_num] void USART0_TX_vect(void)
{

    mmDERE_LOW;   
      
}
#endif


/* USART Data Empty */
interrupt [USART0_UDRE_vect_num] void USART0_UDRE_vect(void)
{
    vMMRTUTransmitFSM();         
}
