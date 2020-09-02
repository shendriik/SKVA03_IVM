/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/

#ifndef _MM_CRC_H
#define _MM_CRC_H

USHORT          usMMCRC16( UCHAR * pucFrame, USHORT usLen );
USHORT          usMMEepromCRC16(eeprom UCHAR * pucFrame, USHORT usLen );
                      
#endif
