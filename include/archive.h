 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _ARCHIVE_
#define _ARCHIVE_

#include "porting.h"
#include "twi_master.h"


#define DEV_ADDRESS             0b01010001
#define EVENT_SIZE              32
#define EVENT_SIZE_DEG2         5
#define FIRST_EVENT_NUM         1
#define LAST_EVENT_NUM          500//1023
#define SETTINGS_SIZE           4
#define SETTINGS_ADDRESS        0x0000
#define EVENT_NUM_STR_SIZE      4

/* Event data type */
typedef struct
{
    UCHAR Datestr[15];
    UCHAR Eventstr[15];
    UCHAR Reserved[1];
    UCHAR crc;     
    
} Event;

/* Archive settings */
typedef struct
{
    USHORT usNextEventNum;              
    UCHAR ucCycled;  
    UCHAR crc;
      
}ArchiveSettings;

/* Archive operation errorcodes */
typedef enum 
{
  ARCH_NOERROR=0,
  ARCH_WRITERROR,
  ARCH_READERROR,
  ARCH_WRONGNUM,
  ARCH_DAMAGED
  
} ArchiveErrorCode;


                /* init archive, get settings from eeprom */
                void vArchive_Init();                         
                          
                /* store archive settings to eeprom */
                ArchiveErrorCode aeArchive_StoreSettings();
                
                /* reset settings */
                void vArchive_ResetSettings();
                
                /* add event to archive */
                ArchiveErrorCode aeArchive_AddEvent(Event *event); 
                
                /* get number of events in archive */
                USHORT usArchive_NumOfEvents();
                
                /* get event from archive */
                ArchiveErrorCode aeArchive_GetEvent(USHORT num, Event* event);   
                
                /* get last archive page number  [0..page] */
                USHORT usGetLastArchivePage();   
                
                /* return pointer to string with even num in format N000 -> NXYZ */
                UCHAR* EventNum_ToString(USHORT event_num,UCHAR* valbuf);

#endif



