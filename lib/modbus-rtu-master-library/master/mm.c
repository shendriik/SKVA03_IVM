/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/


/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "porting.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmconfig.h"
#include "mmporting.h"
#include "mmerror.h"
#include "mmcrc.h"
#include "mmfunc.h"

/* ----------------------- Defines ------------------------------------------*/
#define MM_SER_PDU_SIZE_MIN     4       /*!< Minimum size of a Modbus RTU frame. */
#define MM_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MM_SER_PDU_SIZE_CRC     2       /*!< Size of CRC field in PDU. */
#define MM_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */



#define MM_T35_EXP              1       /*!< Timer for 3.5*ch time expired */
#define MM_SLAVETIMEOUT_EXP     2       /*!< Timer for slave timeout time expired */
/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    STATE_RX_OFF,               /*!< Receiver is in off state. */
    STATE_RX_IDLE,              /*!< Receiver is in idle state.*/
    STATE_RX_RCV,               /*!< Frame is beeing received. */
    STATE_RX_ERROR              /*!< If the frame is invalid.  */
} eMMRcvState;

typedef enum
{
    STATE_TX_OFF,               /*!< Transmitter is in off state. */
    STATE_TX_IDLE,              /*!< Transmitter is in idle state. */
    STATE_TX_XMIT,              /*!< Transmitter is in transfer state. */ 
    STATE_TX_SNT                /*!< Transmitter is in sent state. */
} eMMSndState;

/* ----------------------- Static variables ---------------------------------*/
/* Send and Receive _ButtonInfo */
static volatile  eMMSndState eSndState;
static volatile  eMMRcvState eRcvState;

/* Receive buffer */
static volatile UCHAR  ucRTUBuf[MM_SER_PDU_SIZE_MAX];

/* Send buffer and it's stuff */
static volatile UCHAR *pucSndBufferCur;
static volatile UCHAR usSndBufferCount;
static volatile USHORT usRcvBufferPos;

/* Current state */
static enum
{
    STATE_ENABLED,
    STATE_DISABLED,
    STATE_NOT_INITIALIZED
} eMMState = STATE_NOT_INITIALIZED;


/* ----------------------- Start implementation -----------------------------*/
eMMErrorCode eMMInit( ULONG ulBaudRate, eMMParity eParity )
{
    eMMErrorCode    eStatus = MM_ENOERR;             
    ULONG           usTimerT35_50us;
    
    ENTER_CRITICAL_SECTION(  );

    /* Initialization serial port */
    if( xMMPortSerialInit( ulBaudRate, 8, eParity ) != TRUE )
    {
        eStatus = MM_EPORTERR;
    }
    else
    {   
        /* Initialization timer
         * If baudrate > 19200 then we should use the fixed timer values
         * t35 = 1750us. Otherwise t35 must be 3.5 times the character time.
         */
        if( ulBaudRate > 19200 )
        {
            usTimerT35_50us = 35;       /* 1800us. */
        }
        else
        {
            /* The timer reload value for a character is given by:
             *
             * ChTimeValue = Ticks_per_1s / ( Baudrate / 11 )
             *             = 11 * Ticks_per_1s / Baudrate
             *             = 220000 / Baudrate
             * The reload for t3.5 is 1.5 times this value and similary
             * for t3.5.
             */
            usTimerT35_50us = ( 7UL * 220000UL ) / ( 2UL * ulBaudRate );
        }
        if( xMMPortTimersInit( ( USHORT ) usTimerT35_50us ) != TRUE )
        {
            eStatus = MM_EPORTERR;
        } 
    } 
    
    if( eStatus == MM_ENOERR )
    {
        if( !xMMPortEventInit(  ) )
        {
            /* port dependent event module initalization failed. */
            eStatus = MM_EPORTERR;
        } 
        else
        {
            eMMState = STATE_DISABLED;
        }
    }   
    
    EXIT_CRITICAL_SECTION(  ); 
    
    return eStatus;
}

eMMErrorCode
eMMEnable( void )
{
    eMMErrorCode    eStatus = MM_ENOERR;

    if( eMMState == STATE_DISABLED )
    {
        /* Activate the protocol stack. */
        vMMRTUStart();
        eMMState = STATE_ENABLED;
    }
    else
    {
        eStatus = MM_EILLSTATE;
    }
    return eStatus;
}


void
vMMRTUStart( void )
{
    ENTER_CRITICAL_SECTION(  );                                    
    
    eRcvState = STATE_RX_OFF;  
    eSndState = STATE_TX_IDLE;  
    vMMPortSerialEnable( FALSE, FALSE );
    vMMPortTimersDisable( MM_T35_EXP );   
    vMMPortTimersDisable( 2 );   
    
    EXIT_CRITICAL_SECTION(  );
}

void
vMMRTUStop( void )
{
    ENTER_CRITICAL_SECTION(  ); 
    
    eRcvState = STATE_RX_OFF;  
    eSndState = STATE_TX_OFF;  
    vMMPortSerialEnable( FALSE, FALSE );
    vMMPortTimersDisable( MM_T35_EXP );
    vMMPortTimersDisable( 2 );      
    
    EXIT_CRITICAL_SECTION(  );
}



void
vMMRTUTransmitFSM( void )
{           
    switch ( eSndState )
    {

        case STATE_TX_IDLE:
            /* disable receiver/disable transmitter. */
            vMMPortSerialEnable( FALSE, FALSE );
            break;

        case STATE_TX_XMIT:
            /* check if we are finished. */
            if( usSndBufferCount != 0 )                  
            {      
                xMMPortSerialPutByte( (CHAR) *pucSndBufferCur );
                pucSndBufferCur++;  /* next byte in sendbuffer. */
                usSndBufferCount--;                    
            }
            else
            {
                xMMPortEventPost( EV_mmFRAME_SENT );
               /* Disable transmitter. This prevents another transmit buffer empty interrupt. */
                vMMPortSerialEnable( FALSE, FALSE );              
                eSndState = STATE_TX_SNT;
            }
            break;
    }
}


eMMErrorCode 
eMMSendPDU(USHORT usSlaveID,UCHAR * ucPDU,USHORT usLength) 
{ 

    eMMErrorCode    eStatus = MM_ENOERR;     
    eMMEventType event;   
    USHORT          usCRC16;

    ENTER_CRITICAL_SECTION(  );
      
                                         
    /* Available only in tx idle state then rx is off */
    if(eSndState == STATE_TX_IDLE && eRcvState == STATE_RX_OFF)
    {                
        /* Copy ucPDU and SlaveID to the send buffer */
        pucSndBufferCur = ( UCHAR * )ucPDU - 1; 
        pucSndBufferCur[0] = usSlaveID;
        usSndBufferCount = usLength+1;

        /* Calculate CRC16 checksum for the frame. */
        usCRC16 = usMMCRC16( ( UCHAR * ) pucSndBufferCur, usSndBufferCount-2 ); 
        
        pucSndBufferCur[usSndBufferCount-2] = ( UCHAR )( usCRC16 & 0xFF );
        pucSndBufferCur[usSndBufferCount-1] = ( UCHAR )( usCRC16 >> 8 );

        
        /* Activate the transmitter. */
        eSndState = STATE_TX_XMIT;
        vMMPortSerialEnable( FALSE, TRUE );  
        EXIT_CRITICAL_SECTION(  );
        
        /* waiting while sent */
        while(eSndState!=STATE_TX_SNT); 
        vMMPortSerialEnable( FALSE, FALSE ); 
                     
        /* Check events */   
        xMMPortEventGet(&event);
        
        switch(event)
        {               
                /* Frame have been send */    
            case(EV_mmFRAME_SENT):
                eStatus=MM_ENOERR;      
                break;           

            default:
                break; 
        }      
    } 
    else
    {               
        eStatus = MM_EBUSY;
    }          
    
    EXIT_CRITICAL_SECTION(  );
    return eStatus;  
}



void
vMMRTUReceiveFSM( void )
{

    UCHAR ucByte=0;

    /* Always read the character. */
    ( void )xMMPortSerialGetByte( (CHAR*)&ucByte );
            
    
    switch ( eRcvState )
    {
        /* In the error or off state we wait until all characters in the
         * damaged frame are transmitted.
         */            
    case STATE_RX_OFF:
    case STATE_RX_ERROR:
        vMMPortTimersEnable( MM_T35_EXP );
        break;

        /* We have received out first byte 
         * Turning slave timeout time off, T35 - on
         */
    case STATE_RX_IDLE:
        usRcvBufferPos = 0;
        ucRTUBuf[usRcvBufferPos++] = ucByte;
        eRcvState = STATE_RX_RCV;
                                 
        /* Disable slave timeout timer. */
        vMMPortTimersDisable( MM_SLAVETIMEOUT_EXP );       
        /* Enable t3.5 timer. */
        vMMPortTimersEnable( MM_T35_EXP );         
        break;

        /* We are currently receiving a frame. Reset the timer after
         * every character received. If more than the maximum possible
         * nuMMer of bytes in a modbus frame is received the frame is
         * ignored.
         */
    case STATE_RX_RCV:
        if( usRcvBufferPos < MM_SER_PDU_SIZE_MAX )
        {
            ucRTUBuf[usRcvBufferPos++] = ucByte;
        }
        else
        {
            eRcvState = STATE_RX_ERROR;
        }
        vMMPortTimersEnable( MM_T35_EXP );
        break;
    }
}



eMMErrorCode    
eMMRTUReceivePDU(USHORT usSlaveID,UCHAR **ucPDU,USHORT *usLength)
{
    eMMErrorCode eStatus = MM_ENOERR;
    eMMEventType event;   
    USHORT usCRC16; 
       
               
    ENTER_CRITICAL_SECTION(  );
                                     

    /* Receive available only after previous operation of transmit */
    if( eSndState == STATE_TX_SNT && eRcvState == STATE_RX_OFF)
    {
        /* Activate the receiver. */
        eRcvState = STATE_RX_IDLE;
        vMMPortSerialEnable( TRUE, FALSE );  
        
        /* Starting slave timeout timer */
        vMMPortTimersEnable( MM_SLAVETIMEOUT_EXP );        
        EXIT_CRITICAL_SECTION(  ); 
        
        /* wait while receiving the frame */
        while(eRcvState!=STATE_RX_OFF);
        vMMPortSerialEnable( FALSE, FALSE );  
        
        /* Check events */
        xMMPortEventGet(&event);
        switch (event)
        {
                /* received frame*/                 
            case(EV_mmFRAME_RECEIVED):
                
                /* CRC & SlaveID OK */                
                usCRC16 = usMMCRC16( ( UCHAR * ) ucRTUBuf, usRcvBufferPos-2);
                    
                if(*(USHORT*)(ucRTUBuf+(usRcvBufferPos-2)) ==  usCRC16 && ucRTUBuf[0] == usSlaveID)
                {   
                    /* return pdu frame and length */ 
                    *ucPDU=(UCHAR *)ucRTUBuf+1;
                    *usLength=usRcvBufferPos-3; 
                    
                                                   
                }   
                    /* CRC or Slave ID failed */
                else
                {
                    eStatus=MM_RECFRAMERR;       
                }
                break;     
                
            case(EV_mmFRAME_RECEIVE_ERROR):
                eStatus=MM_RECFRAMERR;  
                break;                  
                
                /* slave timeout expired */  
            case(EV_mmSLAVETIMEOUT_EXPIRED):            
                eStatus=MM_ENORESPONSE;        
                break;
                               
                /* Unknown event */ 
            default:
                break; 
        }

    } 
    else
    {
        eStatus = MM_EBUSY;
    }          
    
    EXIT_CRITICAL_SECTION(  );
    
    return eStatus;    
}


void
vMMRTUT35TimerExpired( )
{   

    switch ( eRcvState )
    {  
    
        /* A frame was received and t35 expired. Notify the listener that
         * a new frame was received. */
    case STATE_RX_RCV:       
        xMMPortEventPost( EV_mmFRAME_RECEIVED );    
        break;

        /* An error occured while receiving the frame. */
    case STATE_RX_ERROR:
        xMMPortEventPost( EV_mmFRAME_RECEIVE_ERROR );
        break;

        /* Function called in an illegal state. */
    default:
        break;
    }

    vMMPortTimersDisable( MM_T35_EXP );
    eRcvState = STATE_RX_OFF;

}



void
vMMRTUSlaveTimeoutTimerExpired(  )
{      
   
    switch ( eRcvState )
    {  
        /* Timeout waiting for the first byte expired */
    case STATE_RX_IDLE:  
            
            xMMPortEventPost( EV_mmSLAVETIMEOUT_EXPIRED ); 
        break; 
     
        /* Function called in an illegal state. */
    default:
        break;
    }
    
    /* Disable slave timeout timer. */
    vMMPortTimersDisable( MM_SLAVETIMEOUT_EXP );
    
    /* Error state of the receive FSM */
    eRcvState = STATE_RX_OFF; 

}
