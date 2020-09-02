/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*
*****************************************************************************/

#ifndef _MM_FUNC_H
#define _MM_FUNC_H

/* ----------------------- Modbus includes ----------------------------------*/
#include "mmconfig.h"
#include "mmerror.h"


#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Defines ------------------------------------------*/ 
#define MM_ADDRESS_BROADCAST    ( 0 )   /*! Modbus broadcast address. */
#define MM_ADDRESS_MIN          ( 1 )   /*! Smallest possible slave address. */
#define MM_ADDRESS_MAX          ( 247 ) /*! Biggest possible slave address. */
#define MM_PDU_MAX_SIZE         ( 253 ) /*! Maximum PDU size. */

/* Global offsets */
#define MM_PDU_FUNC_OFF         ( 0 )   /*!< Offset of Func in Modbus-PDU frame */
#define MM_PDU_ERROR_OFF        ( 1 )   /*!< Offset of Error code in Modbus-PDU frame */

/* Modbus functions */
#define MM_FUNC_NONE                          (  0 )
#define MM_FUNC_READ_COILS                    (  1 )
#define MM_FUNC_READ_DISCRETE_INPUTS          (  2 )
#define MM_FUNC_WRITE_SINGLE_COIL             (  5 )
#define MM_FUNC_WRITE_MULTIPLE_COILS          ( 15 )
#define MM_FUNC_READ_HOLDING_REGISTER         (  3 )
#define MM_FUNC_READ_INPUT_REGISTER           (  4 )
#define MM_FUNC_WRITE_HOLDING_REGISTER        (  6 )
#define MM_FUNC_WRITE_MULTIPLE_REGISTERS      ( 16 )
#define MM_FUNC_READWRITE_MULTIPLE_REGISTERS  ( 23 )
#define MM_FUNC_DIAG_READ_EXCEPTION           (  7 )
#define MM_FUNC_DIAG_DIAGNOSTIC               (  8 )
#define MM_FUNC_DIAG_GET_COM_EVENT_CNT        ( 11 )
#define MM_FUNC_DIAG_GET_COM_EVENT_LOG        ( 12 )
#define MM_FUNC_OTHER_REPORT_SLAVEID          ( 17 )
#define MM_FUNC_ERROR                         ( 128 )


#if MM_FUNC_READ_INPUT_ENABLED > 0
#endif

#if MM_FUNC_READ_HOLDING_ENABLED > 0
eMMException    eMMReadHoldingRegister(USHORT usSlaveID,USHORT usStartReg,USHORT usCount,USHORT * ucHoldRegs);   /* Defined! */
#endif

#if MM_FUNC_WRITE_HOLDING_ENABLED > 0
eMMException    eMMWriteHoldingRegister(USHORT usSlaveID,USHORT usStartReg, USHORT value);
#endif

#if MM_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
eMMException    eMMFuncWriteMultipleHoldingRegister( USHORT usSlaveID,USHORT usStartReg, USHORT usCount,USHORT * ucHoldRegs );
#endif

#if MM_FUNC_READ_COILS_ENABLED > 0
#endif

#if MM_FUNC_WRITE_COIL_ENABLED > 0
#endif

#if MM_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
#endif

#if MM_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
#endif




#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
