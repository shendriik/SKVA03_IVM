/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/



#ifndef _MM_ERROR_H
#define _MM_ERROR_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Type definitions ---------------------------------*/

/* Error codes */
typedef enum
{
    MM_ENOERR,                  /*!< no error. */
    MM_ENOREG,                  /*!< illegal register address. */
    MM_EINVAL,                  /*!< illegal argument. */
    MM_EPORTERR,                /*!< porting layer error. */
    MM_ENORES,                  /*!< insufficient resources. */
    MM_EIO,                     /*!< I/O error. */
    MM_EILLSTATE,               /*!< protocol stack in illegal state. */
    MM_ETIMEDOUT,               /*!< timeout error occurred. */
    
    /* new */                 
    MM_NULL,                    /*!< not initialized value */
    MM_RECFRAMERR,              /*!< Receive damage frame */
    MM_ENORESPONSE,             /*!< No response from device */
    MM_EBUSY,                   /*!< Device is busy. */
    MM_ESNDREQERR               /*!< Error sending request. */
} eMMErrorCode;

/* Exceptions */
typedef enum
{
    MM_EX_NONE = 0x00,
    
    /* From SLAVE (in PDU frame) */   
    MM_EX_ILLEGAL_FUNCTION = 0x01,
    MM_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MM_EX_ILLEGAL_DATA_VALUE = 0x03,
    MM_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MM_EX_ACKNOWLEDGE = 0x05,
    MM_EX_SLAVE_BUSY = 0x06,
    MM_EX_MEMORY_PARITY_ERROR = 0x08,
    
    /* From Master */
    MM_EX_BUSY = 0x0C,
    MM_EX_INVALIDFRAME = 0x0D,
    MM_EX_NORESPONSE = 0x0E,   
    MM_EX_UNKNOWN = 0x10,
} eMMException;


/* Function prototypes */
  
    eMMException   eMMGetErrorFromPDU( UCHAR * ucPDU );    
    
#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif