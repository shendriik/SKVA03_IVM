/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/

/* ----------------------- Platform includes --------------------------------*/

/* ----------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmfunc.h"
#include "mmerror.h"



eMMException   eMMGetErrorFromPDU( UCHAR * ucPDU )
{
           
    if( ( ucPDU[MM_PDU_FUNC_OFF] & 0xF0 ) == 0x80 )
    {
        return (eMMException)ucPDU[MM_PDU_ERROR_OFF];
    }   
    
    return MM_EX_NONE; 
}