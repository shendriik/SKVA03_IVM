/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/

/* ----------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmporting.h"

/* ----------------------- Variables ----------------------------------------*/
static eMMEventType eQueuedEvent;
static BOOL     xEventInQueue;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMMPortEventInit( void )
{
    xEventInQueue = FALSE;
    return TRUE;
}

BOOL
xMMPortEventPost( eMMEventType eEvent )
{
    xEventInQueue = TRUE;
    eQueuedEvent = eEvent;
    return TRUE;
}

BOOL
xMMPortEventGet( eMMEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;

    if( xEventInQueue )
    {
        *eEvent = eQueuedEvent;
        xEventInQueue = FALSE;
        xEventHappened = TRUE;
    }
    return xEventHappened;
}
