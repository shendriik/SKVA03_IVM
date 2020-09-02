/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/


#ifndef _MM_PORTING_H
#define _MM_PORTING_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif

/* ----------------------- Type definitions ---------------------------------*/

/* Event types */
typedef enum
{
    EV_mmSLAVETIMEOUT_EXPIRED,    /*!< Slave timeout expired. */
    EV_mmFRAME_RECEIVED,          /*!< Frame received. */ 
    EV_mmFRAME_RECEIVE_ERROR,      /*!< Frame receiving error */               
    EV_mmFRAME_SENT               /*!< Frame sent. */   
} eMMEventType;

/* USART Parity mode */
typedef enum
{
    MM_PAR_NONE,                /*!< No parity. */
    MM_PAR_ODD,                 /*!< Odd parity. */
    MM_PAR_EVEN                 /*!< Even parity. */
} eMMParity;


/* Function prototypes */


/* ---------------------- Event port functions ------------------------*/

    BOOL            xMMPortEventInit( void );
    BOOL            xMMPortEventPost( eMMEventType eEvent );
    BOOL            xMMPortEventGet(  /*@out@ */ eMMEventType * eEvent );

/* --------------------- Serial port functions ------------------------*/     

    BOOL            xMMPortSerialInit(ULONG ulBaudRate, UCHAR ucDataBits, eMMParity eParity );
    void            vMMPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable );
    BOOL            xMMPortSerialPutByte( CHAR ucByte );   
    BOOL            xMMPortSerialGetByte( CHAR * pucByte );

/* ------------------------ Timers functions --------------------------*/

    BOOL            xMMPortTimersInit( USHORT usTimeOut50us );
    INLINE void     vMMPortTimersEnable( UCHAR timer );
    INLINE void     vMMPortTimersDisable( UCHAR timer );   

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif