/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*
*****************************************************************************/

#ifndef _MM_TRANS_H
#define _MM_TRANS_H

/* ----------------------- Modbus includes ----------------------------------*/
#include "mmerror.h"

        /* Performing send and receive */
        eMMException    eMMTransaction(USHORT usSlaveID,UCHAR * ucSendPDU,USHORT usSendPDUCount, UCHAR ** ucReceivePDU, USHORT * usReceivePDUCount);

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif

