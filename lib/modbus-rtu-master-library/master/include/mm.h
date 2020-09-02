/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/


#ifndef _MM_H
#define _MM_H

#include "porting.h"

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

#include "mmporting.h"
#include "mmerror.h"


/* Function prototypes */

    /* Init */
    eMMErrorCode    eMMInit( ULONG ulBaudRate, eMMParity eParity );             /* Initialization of the stack */
    eMMErrorCode    eMMEnable( void );                                          /* Enabled stack */
    
    /* Start/Stop */
    void            vMMRTUStart( void );
    void            vMMRTUStop( void );   
    
    /* Send */           
    void            vMMRTUTransmitFSM( void );  
    eMMErrorCode    eMMSendPDU(USHORT usSlaveID,UCHAR * ucPDU,USHORT usLength);     /* sent PDU to slaveID */
               
    
    /* Receive */
    void            vMMRTUReceiveFSM( void );    
    eMMErrorCode    eMMRTUReceivePDU( USHORT usSlaveID,UCHAR **ucPDU,USHORT * usLength); 
    
    /* Timers expired */
    void           vMMRTUT35TimerExpired( );
    void           vMMRTUSlaveTimeoutTimerExpired( );
    
    
#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
