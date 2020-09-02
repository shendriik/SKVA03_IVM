 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
 
/* ------------------------ System includes -----------------------------------*/ 
#include "string.h"

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"

/* -------------------------- Own includes ------------------------------------*/
#include "archive.h"
#include "mymath.h"


static ArchiveSettings settings;        /* archive settings */


void vArchive_Init()
{
    TWIState state;
    USHORT adr;  
    UCHAR adrbytes[2];

    /* address of settings in eeprom */
    adr=SETTINGS_ADDRESS;     
    adrbytes[0]=(UCHAR)(adr>>8);
    adrbytes[1]=(UCHAR)(adr&0x00FF);   
    
    /* read settings from eeprom */                            
    state=tsTWI_Read(DEV_ADDRESS,adrbytes,2,(UCHAR*)&settings,SETTINGS_SIZE);    
    
    if(state==RECEIVED)     
    {        
        /* check crc */    
        if(CRC_Xor((UCHAR*)&settings,SETTINGS_SIZE-1)==settings.crc)
        {               
            /* ok */    
            return;
        }
    }              
    
    /* reset settings on error */    
    settings.usNextEventNum = FIRST_EVENT_NUM;
    settings.ucCycled = FALSE;   
}

                       
ArchiveErrorCode aeArchive_StoreSettings()
{
    TWIState state;
    USHORT adr;  
    UCHAR adrbytes[2];

    /* address of settings in eeprom */
    adr=SETTINGS_ADDRESS;
    adrbytes[0]=(UCHAR)(adr>>8);
    adrbytes[1]=(UCHAR)(adr&0x00FF); 
    
    /* prepare data and write it */
    settings.crc=CRC_Xor((UCHAR*)&settings,SETTINGS_SIZE-1);
    state=tsTWI_Write(DEV_ADDRESS,adrbytes,2,(UCHAR*)&settings,SETTINGS_SIZE);    
  
    /* check operation status */
    if(state==TRANSMITTED)
    {                               
        return ARCH_NOERROR;         
    }
    else 
    {      
        return ARCH_WRITERROR;
    }  
}


void vArchive_ResetSettings()
{
    /* reset settings */    
    settings.usNextEventNum = FIRST_EVENT_NUM;
    settings.ucCycled = FALSE; 
}

ArchiveErrorCode aeArchive_AddEvent(Event *event)
{   
    TWIState state;
    USHORT adr;
    UCHAR adrbytes[2];
    
    /* calculate address to write */
    if(settings.usNextEventNum>LAST_EVENT_NUM) 
    {
        settings.usNextEventNum=FIRST_EVENT_NUM; 
        settings.ucCycled=TRUE;
    }     
             
    adr=settings.usNextEventNum<<EVENT_SIZE_DEG2;
    adrbytes[0]=(UCHAR)(adr>>8);
    adrbytes[1]=(UCHAR)(adr&0x00FF);                            
    
    /* prepare data and write it */
    event->crc=CRC_Xor((UCHAR*)event,EVENT_SIZE-1);
    state=tsTWI_Write(DEV_ADDRESS,adrbytes,2,(UCHAR*)event,EVENT_SIZE);    
    
    /* check operation status */
    if(state==TRANSMITTED)
    {   
        //TODO: check if write ok     
        settings.usNextEventNum++;
        return ARCH_NOERROR;         
    }
    else              
    {            
        return ARCH_WRITERROR;       
    }
}


USHORT usArchive_NumOfEvents()
{
    if(settings.ucCycled) 
        return LAST_EVENT_NUM;
    else 
        return settings.usNextEventNum-1;
}

ArchiveErrorCode aeArchive_GetEvent(USHORT num, Event* event)
{   
    TWIState state;
    USHORT adr;
    UCHAR adrbytes[2];

    if(num>=FIRST_EVENT_NUM && num<=usArchive_NumOfEvents())
    {   
        /* calculate address to read */
        if(settings.ucCycled)
        {
            num = settings.usNextEventNum+num-1;
            if(num>LAST_EVENT_NUM) num = num-LAST_EVENT_NUM; 
        }  
         
        adr=num<<EVENT_SIZE_DEG2; 
        adrbytes[0]=(UCHAR)(adr>>8);
        adrbytes[1]=(UCHAR)(adr&0x00FF);  

        /* read event */                                  
        state=tsTWI_Read(DEV_ADDRESS,adrbytes,2,(UCHAR*)event,EVENT_SIZE); 
        
        /* check status op operation */
        if(state==RECEIVED)     
        {
            /* check crc */    
            if(CRC_Xor((UCHAR*)event,EVENT_SIZE-1)==event->crc)
            {
                return ARCH_NOERROR; 
            }
            else 
            { 
                return ARCH_DAMAGED;  
            }
        }     
        else
        {
            return ARCH_READERROR;     
        }
    }
    else
    {
        return ARCH_WRONGNUM;
    } 
}


 
USHORT usGetLastArchivePage()
{
    SHORT page=usArchive_NumOfEvents()/2+usArchive_NumOfEvents()%2-1;
    if(page>=0) return (USHORT)page;
    return 0; 
}




UCHAR* EventNum_ToString(USHORT event_num,UCHAR* valbuf)
{
    UCHAR* start;
    UCHAR* end; 
    UCHAR i; 
    UCHAR size=EVENT_NUM_STR_SIZE;
    
    end=valbuf+10; 
    start=utoa_fast_div(event_num,valbuf);
    for(i=1;i<size-(end-start)-1;i++) *(start-i)='0';
    start-=i;
    *start='N';
    
    return start;
}
