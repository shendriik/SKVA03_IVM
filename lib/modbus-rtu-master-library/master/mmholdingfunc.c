
/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/
 

/* ----------------------- AVR includes -------------------------------------*/

/* ----------------------- Platform includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmconfig.h"
#include "mmfunc.h" 
#include "mmtrans.h"
#include "stdlib.h"

/* ----------------------- Defines ------------------------------------------*/

//eMMReadHoldingRegister
#define MM_PDU_FUNC_READ_SIZE                               ( 5 + 2 )
#define MM_PDU_FUNC_READ_REG_OFS                            ( 1 )
#define MM_PDU_FUNC_READ_COUNT_OFS                          ( 3 )
#define MM_PDU_FUNC_READ_ANSWER_BYTESCOUNT_OFS              ( 1 )
#define MM_PDU_FUNC_READ_ANSWER_REG_OFS                     ( 2 )

//eMMWriteHoldingRegister
#define MM_PDU_FUNC_WRITE_SIZE                              ( 5 + 2 )
#define MM_PDU_FUNC_WRITE_REG_OFS                           ( 1 )
#define MM_PDU_FUNC_WRITE_VALUE_OFS                         ( 3 )

//eMMFuncWriteMultipleHoldingRegister
#define MM_PDU_FUNC_WRITE_MULTIPLE_SIZE                     ( 6 + 2 )
#define MM_PDU_FUNC_WRITE_MULTIPLE_REG_OFS                  ( 1 )
#define MM_PDU_FUNC_WRITE_MULTIPLE_COUNT_OFS                ( 3 )
#define MM_PDU_FUNC_WRITE_MULTIPLE_DATABYTES_COUNT_OFS      ( 5 )
#define MM_PDU_FUNC_WRITE_MULTIPLE_HOLDREGS_OFS             ( 6 )

/* ----------------------- Static variables ---------------------------------*/
/* Input PDU, length  */
static UCHAR * ucReceivePDU;
static USHORT * usReceivePDULength;    
/* Output PDU */    
static UCHAR ucPDU [ MM_PDU_MAX_SIZE ];


#if MM_FUNC_READ_HOLDING_ENABLED > 0
eMMException    eMMReadHoldingRegister(USHORT usSlaveID,USHORT usStartReg,USHORT usCount,USHORT *ucHoldRegs)
{
    
    eMMException ex = MM_EX_NONE;
    UCHAR ucRegBytesCount;
    UCHAR i;     
              
    /* creating output PDU */
    ucPDU[ MM_PDU_FUNC_OFF ] = MM_FUNC_READ_HOLDING_REGISTER;
    ucPDU[ MM_PDU_FUNC_WRITE_REG_OFS ] = usStartReg >> 8;
    ucPDU[ MM_PDU_FUNC_WRITE_REG_OFS+1 ] = usStartReg & 0xFF;  
    ucPDU[ MM_PDU_FUNC_READ_COUNT_OFS ] = usCount >> 8;
    ucPDU[ MM_PDU_FUNC_READ_COUNT_OFS+1 ] = usCount & 0xFF;  
           
    /* Performing the transaction */       
    ex = eMMTransaction(usSlaveID, ucPDU, MM_PDU_FUNC_READ_SIZE, &ucReceivePDU, usReceivePDULength);

    /* slave return right answer to our request */
    if( ex == MM_EX_NONE )
    {              
        ucRegBytesCount = ucReceivePDU [ MM_PDU_FUNC_READ_ANSWER_BYTESCOUNT_OFS ];  
        
        for( i=0; i < ucRegBytesCount; i+= 2 )
        {    
            ucHoldRegs[i/2] = ((USHORT)ucReceivePDU [ MM_PDU_FUNC_READ_ANSWER_REG_OFS+i ])<<8 | ucReceivePDU [ MM_PDU_FUNC_READ_ANSWER_REG_OFS+i+1 ];   
        }           
    } 
    
    /* else return current exception */
    return ex;

}
#endif


#if MM_FUNC_WRITE_HOLDING_ENABLED > 0
eMMException    eMMWriteHoldingRegister(USHORT usSlaveID,USHORT usStartReg, USHORT value)
{

    eMMException ex = MM_EX_NONE;
              
    /* creating output PDU */
    ucPDU[ MM_PDU_FUNC_OFF ] = MM_FUNC_WRITE_HOLDING_REGISTER;
    ucPDU[ MM_PDU_FUNC_WRITE_REG_OFS ] = usStartReg >> 8;
    ucPDU[ MM_PDU_FUNC_WRITE_REG_OFS+1 ] = usStartReg & 0xFF;  
    ucPDU[ MM_PDU_FUNC_WRITE_VALUE_OFS ] = value >> 8;
    ucPDU[ MM_PDU_FUNC_WRITE_VALUE_OFS+1 ] = value & 0xFF;  

           
    /* Performing the transaction */       
    ex = eMMTransaction(usSlaveID, ucPDU, MM_PDU_FUNC_WRITE_SIZE, &ucReceivePDU, usReceivePDULength);

    /* return current exception */
    return ex;
    
}
#endif

#if MM_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
eMMException    eMMFuncWriteMultipleHoldingRegister( USHORT usSlaveID, USHORT usStartReg, USHORT usCount, USHORT * ucHoldRegs )
{

    eMMException ex = MM_EX_NONE;
    UCHAR i; 
        
    /* creating output PDU */
    ucPDU[ 0 ] = MM_FUNC_WRITE_MULTIPLE_REGISTERS;
    ucPDU[ MM_PDU_FUNC_WRITE_MULTIPLE_REG_OFS ] = usStartReg >> 8;
    ucPDU[ MM_PDU_FUNC_WRITE_MULTIPLE_REG_OFS + 1 ] = usStartReg & 0xFF;  
    ucPDU[ MM_PDU_FUNC_WRITE_MULTIPLE_COUNT_OFS ] = usCount >> 8;
    ucPDU[ MM_PDU_FUNC_WRITE_MULTIPLE_COUNT_OFS + 1 ] = usCount & 0xFF;    
    ucPDU[ MM_PDU_FUNC_WRITE_MULTIPLE_DATABYTES_COUNT_OFS ] = usCount*2;
     
    for( i = 0; i < usCount; i++ )
    {   
        ucPDU[ MM_PDU_FUNC_WRITE_MULTIPLE_HOLDREGS_OFS + i * 2 ] = ucHoldRegs[i] >> 8;   
        ucPDU[ MM_PDU_FUNC_WRITE_MULTIPLE_HOLDREGS_OFS + i * 2 + 1 ] = ucHoldRegs[i] & 0xFF; 
    }

    /* Performing the transaction */       
    ex = eMMTransaction(usSlaveID, ucPDU, MM_PDU_FUNC_WRITE_MULTIPLE_SIZE + (usCount*2) , &ucReceivePDU, usReceivePDULength);

    /* return current exception */  free(ucPDU);
    return ex;

}
#endif
