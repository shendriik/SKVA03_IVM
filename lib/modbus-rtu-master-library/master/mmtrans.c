/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/

/* ----------------------- Platform includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmconfig.h"
#include "mmfunc.h"


/* This function should be used to create special request functions to Slave device (eMMReadHoldingRegister and others) */
eMMException    eMMTransaction(USHORT usSlaveID,UCHAR * ucSendPDU,USHORT usSendPDUCount, UCHAR ** ucReceivePDU, USHORT * usReceivePDUCount)
{
    UCHAR ucFunctionCode;
    eMMException ex;
    eMMErrorCode erRcv = MM_NULL; 
    UCHAR ucTries = 0;

    /* Performing the transaction */       
    while( ucTries++ < MM_RESPONSE_TRIES_COUNT && erRcv != MM_ENOERR )
    {                                                   
        /* Sending PDU */                               
        if (eMMSendPDU( usSlaveID, ucSendPDU, usSendPDUCount ) == MM_EBUSY ) return MM_EX_BUSY;          

        /* Receiving PDU */    
        erRcv = eMMRTUReceivePDU( usSlaveID,ucReceivePDU,usReceivePDUCount );      
        if( erRcv == MM_EBUSY ) return MM_EX_BUSY;         

        /* Set conditions to the new transaction */
        vMMRTUStart( );      
    } 

    switch ( erRcv )
    {         
        /* No Frame error - executing received pdu */ 
        case ( MM_ENOERR ):
            
            ucFunctionCode = (*ucReceivePDU)[ MM_PDU_FUNC_OFF ];
                         
            if( ucFunctionCode == ucSendPDU[ MM_PDU_FUNC_OFF ] )    /* slave return right answer to our request */ 
            {     
                ex = MM_EX_NONE;   
            }  
            else
            if( (ucFunctionCode  & 0x7F) == ucSendPDU[ MM_PDU_FUNC_OFF ] )  /* slave return error message */ 
            {
                ex = (eMMException)((*ucReceivePDU)[ MM_PDU_ERROR_OFF ]);
            } 
            else                                                    /* slave return answer to other function */
            {                                                        
                ex = MM_EX_INVALIDFRAME;
            } 
            break;
        
        /* Frame error */                                   
        case ( MM_RECFRAMERR ):  
                
            ex = MM_EX_INVALIDFRAME; 
            break;
        
        /* Slave is not responsed */    
        case ( MM_ENORESPONSE ): 
                 
            ex =  MM_EX_NORESPONSE;
            break;
              
        /* Unknown error */    
        default:               
        
            ex = MM_EX_UNKNOWN; 
            break;
    }
               
    return ex;    
}