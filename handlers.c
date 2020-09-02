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
#include "porting.h"

/* ------------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmfunc.h"
#include "mmcrc.h"

/* -------------------------- Own includes ------------------------------------*/
#include "defines_my.h"
#include "twi_master.h"
#include "buttons.h";
#include "algorithm.h"
#include "config.h"
#include "display.h"
#include "calendar.h"
#include "perifery.h"
#include "sort.h"
#include "archive.h"
#include "usbloader.h"
#include "delay.h"
#include "power.h"


#define MAX_DELAY_SEC   3                      /* maximum delay in sec before sensor will set in OTKAZ */

/* extern var */
extern __eeprom Config* config;
extern flash UCHAR ARCHIVE_EMPTY[];
extern flash UCHAR WRITING_CONF[]; 
extern flash UCHAR READING_CONF[];  
extern flash UCHAR SETTINGS_SCREEN[];           
extern DateTime dDateTime;
extern USHORT usRegHoldingBuf[REG_HOLD_NREGS];


/* arrays */
USHORT sensors_mA_x100[MAX_SENSORS];            /* raw values from sensors */
SensorData sensors[MAX_SENSORS];                /* current measurements and states for sensors */
SensorState LastStates[MAX_SENSORS];            /* Sensors last states for archive */
UCHAR ucButtons[BUTTONS_COUNT];                 /* buttons state */
UCHAR buffer[MAX_SENSORS];                      /* buffer for different operations */
UCHAR ucRetries[BOARDS_COUNT];                  /* requests retries counter for the sensor boards */

/* primitives */
USHORT ucArchiveDisplayPage;                    /* Archive display page number */
UCHAR ucMainDisplayPage;                        /* Main display page number */
UCHAR ucLastDisplayMode;                        /* Last display mode */
UCHAR ucInactionTime;                           /* Inaction time */

/* primmitives as flags */
UCHAR ucNeedUpdateArchiveDisplayPage;           /* flag to update archive display page */
UCHAR ucNeedUpdateMainDisplayPage;              /* flag to update main display page */
UCHAR ucNeedUpdateSettingsDisplayPage;          /* flag to update settings display page */
UCHAR ucBlockMainActivity = TRUE;               /* block main activity flag */ 
UCHAR ucNeedRestart = FALSE;                    /* need restart after programming flag */  

void vHandlers_ResetVariables()
{
    UCHAR i;
    
    /* reset all sensors last state to NORMA */
    for(i=0;i<MAX_SENSORS;i++) LastStates[i] = NORMA;                    
           
    /* reset current main page to 0 */ 
    ucMainDisplayPage=0;
     
    /* reset current archive page to last */ 
    ucArchiveDisplayPage=usGetLastArchivePage(); 

    /* reset last display mode to MAIN */
    ucLastDisplayMode=MAIN;             
                     
    /* reset inaction time to 0 */
    ucInactionTime=0;        
    
    /* reset arrays */
    memset(sensors,0x00,MAX_SENSORS*sizeof(SensorData)); 
    memset(sensors_mA_x100,0x00,MAX_SENSORS*sizeof(USHORT));         
    memset(ucButtons,0x00,BUTTONS_COUNT); 
    memset(ucRetries,0x00,BOARDS_COUNT); 
    
    /* reset flags */
    ucNeedUpdateArchiveDisplayPage=FALSE;          
    ucNeedUpdateMainDisplayPage=FALSE; 
    ucNeedUpdateSettingsDisplayPage=FALSE; 
    ucNeedRestart = FALSE;            
}



UCHAR ucGetLastMainPage()
{
    return (ucGetSensorsCount()>>2)+((ucGetSensorsCount()%4==0)?0:1)-1;    
}
    
/* Button click handler */ 
void ButtonClick_handler(UCHAR sender,ButtonClick state)
{
    if(state==CLICK && !ucBlockMainActivity)
    {
        switch(sender)
        {                  
            case(OK_BUTTON): 
                
                /* changing display to next mode */
                dNextDisplayMode();
                break;      
                       
            case(UP_BUTTON): 
                
                if(dGetDisplayMode()==MAIN) 
                {
                   if(ucMainDisplayPage>0) 
                   {
                        ucMainDisplayPage--;
                        ucNeedUpdateMainDisplayPage=TRUE;  
                   }  
                   ucInactionTime=0;
                }                 
                if(dGetDisplayMode()==ARCHIVE) 
                {  
                   if(ucArchiveDisplayPage<usGetLastArchivePage()) 
                   {
                        ucArchiveDisplayPage++;
                        ucNeedUpdateArchiveDisplayPage=TRUE;
                   }
                   ucInactionTime=0;
                }
                break;
                
            case(DOWN_BUTTON): 
                
                if(dGetDisplayMode()==MAIN) 
                {
                   if(ucMainDisplayPage<ucGetLastMainPage()) 
                   {
                        ucMainDisplayPage++;
                        ucNeedUpdateMainDisplayPage=TRUE;
                   }    
                   ucInactionTime=0;
                }           
                if(dGetDisplayMode()==ARCHIVE) 
                { 
                    if(ucArchiveDisplayPage>0) 
                    {
                        ucArchiveDisplayPage--; 
                        ucNeedUpdateArchiveDisplayPage=TRUE;
                    }
                    ucInactionTime=0;
                }         
                break;
            
            default: 
                break;
        }  
    } 
    
    /* save button changed mode */
    ucButtons[sender]=state;        
}




/* MainWork event callback handler */
void MainWork_handler()
{
     eMMException ex;
     UCHAR i = 0;
     UCHAR crc;  
     UCHAR kvit = FALSE;
     UCHAR error = FALSE;
     Event* event = (Event*)buffer;
     SensorData* sens;
     UCHAR ucArchiveUpdated = FALSE; 
     UCHAR ucLastSensNum = 0; 
     UCHAR int_relay = FALSE;    
     UCHAR usb_conn = ucUSB_Conn_IsConnected();
        
   
    /* if writing config */
    if(ucBlockMainActivity) return;
            
    
    /* Getting sensors current from boards */       
    for(i=0;i<BOARDS_COUNT;i++)
    {    
        if(config->MR_Boards[i])
        {
            ex=eMMReadHoldingRegister(i+1,DEV_START_REG,SENSORS_IN_BOARD,sensors_mA_x100+(i<<SENSORS_IN_BOARD_2n));
            if(ex==MM_ENOERR) 
            {
                ucRetries[i]=0;          
            }
            else
            {
                error=TRUE;  
                if(ucRetries[i]==MAX_DELAY_SEC) 
                {   
                    memset(sensors_mA_x100+(i<<SENSORS_IN_BOARD_2n),0x00,sizeof(USHORT)<<SENSORS_IN_BOARD_2n); 
                }   
                else
                {
                    ucRetries[i]++;
                }
            }
        }
    } 
               
    /* Getting sensors data */                                      
    ucAlgorithm_GetSensors(sensors_mA_x100,sensors,config->_Sensors);  
    for(i=0;i<MAX_SENSORS;i++)
    {    
        sens=&sensors[i];                  
        
        /* Modbus Slave registers update stuff */ 
        if(!usb_conn)
        {   
            if(sens->State==OTKAZ || sens->State==NONE)
            {
                usRegHoldingBuf[12+i]=0xFFFF;
            }
            else
            {
                usRegHoldingBuf[12+i]=sens->Value_x10;
            }
            usRegHoldingBuf[44+i]=sens->State;
        }
                  
        /* internal relay update stuff */
        if(sens->State>OTKAZ) int_relay=TRUE;
        
        /* Archive stuff */ 
        if(sens->State!=NONE)
        {
            if(sens->State!=LastStates[i])
            {   
                /* create event*/
                vCalendar_ToString(event->Datestr); 
                vSensorToString(event->Eventstr,sens,config->_Sensors[i].Text,&config->_Sensors[i].Dim,15);
                
                /* write event to eeprom */
                if(aeArchive_AddEvent(event)==ARCH_NOERROR)
                {
                    LastStates[i]=sens->State;
                    ucArchiveUpdated=TRUE;
                }  
                else { } //TODO: handle write archive errors 
            }       
            ucLastSensNum=i;
        }                    
    }    
    /* Deleting archive */
    if(dGetDisplayMode()==ARCHIVE && ucButtons[UP_BUTTON]==CLICK && ucButtons[DOWN_BUTTON]==CLICK)
    {
        vArchive_ResetSettings();   /* delete archive */
        ucArchiveUpdated=TRUE;
    }       
    /* On Archive update */    
    if(ucArchiveUpdated) 
    {   
        delay_ms(5); 
        aeArchive_StoreSettings();  /* store archive settings*/
        ucArchiveDisplayPage=usGetLastArchivePage();  /* set current archive page to last */  
        ucNeedUpdateArchiveDisplayPage=TRUE;
    } 
    
    
    /* Getting  data for relais */  
    vAlgorithm_GetRelais(buffer,sensors,config->_Relais,ucButtons[CHECK_BUTTON]==CLICK,ucButtons[SOUND_OFF_BUTTON]==CLICK);  
    /* Sending data to relais boards */
    for(i=0;i<BOARDS_COUNT;i++)
    {
        if(config->Relay_Boards[i])
        {
            ex=eMMWriteHoldingRegister(RELAY_ID_OFFS+i+1,DEV_START_REG,(USHORT)buffer[i]); 
            if(ex!=MM_ENOERR) error=TRUE;
        }     
        
        /* Modbus Slave registers update stuff*/ 
        if(!usb_conn)
        {
            usRegHoldingBuf[8+i]=(USHORT)buffer[i];
        }
    }
    
    /* Getting data for zones */
    kvit=ucAlgorithm_GetZones(buffer,sensors,ucButtons[CHECK_BUTTON]==CLICK,ucButtons[SOUND_OFF_BUTTON]==CLICK);
    /* Sending data to LED MCu */
    crc=usMMCRC16(buffer,8);  
    buffer[8]=kvit; 
    buffer[9]=(UCHAR)crc; 
    buffer[10]=(UCHAR)(crc>>8); 
    tsTWI_Write(0x01, buffer,0,buffer,11);   
    /* Modbus Slave registers update */ 
    for(i=0;i<MAX_ZONES && !usb_conn;i++)
    {      
        usRegHoldingBuf[0+i]=(USHORT)buffer[i];
    }
       
    
    /* Prepare data for main display */   
    bubble_sort((UCHAR*)sensors,BY_STATE,EMPTY,EMPTY,SENSOR_STATE_SIZE,ucLastSensNum+1,DESC);   
    bubble_sort((UCHAR*)sensors,BY_ZONE,BY_STATE,EMPTY,SENSOR_STATE_SIZE,ucGetSensorsCount(),ASC);  
    bubble_sort((UCHAR*)sensors,BY_ID,BY_ZONE,BY_STATE,SENSOR_STATE_SIZE,ucGetSensorsCount(),ASC);         
    
    /* Error led */
    vSetError(error);
    /* Internal relais */
    vSet_InternalRelais(int_relay);       
    
    /* update main display and settigs  displays 1 time per second */
    ucNeedUpdateMainDisplayPage=TRUE;
    ucNeedUpdateSettingsDisplayPage=TRUE;  
    
    /* increment inaction time */
    ucInactionTime++;    
}


/* Display handler */
void Display_handler()
{
    SensorData* sens;  
    UCHAR i,row;   
    USHORT lastnum,eventnum;      
    Event* event=(Event*)buffer;
    UCHAR* numstr; 
     
    /* if writing config */
    if(ucBlockMainActivity) return;
    
    /* check if inaction time exceeds limit */
    if(ucInactionTime>INACTION_TIMELIMIT)
    {   
        /* set 0 main page */
        ucInactionTime=0;
        vSetDisplayMode(MAIN);
        ucMainDisplayPage=0;
    }
    
    /* if display mode changed */
    if(ucLastDisplayMode!=dGetDisplayMode())
    {   
        ucLastDisplayMode=dGetDisplayMode();
        /* enable update all displays */                              
        ucNeedUpdateMainDisplayPage=TRUE;
        ucNeedUpdateArchiveDisplayPage=TRUE; 
        ucNeedUpdateSettingsDisplayPage=TRUE;
        /* there is no inaction */
        ucInactionTime=0;  
    }
    
    switch(dGetDisplayMode()) 
    {   
        /* main windows with current measurements */
        case(MAIN): 
            
            /* check enable */
            if(!ucNeedUpdateMainDisplayPage) break;
            ucNeedUpdateMainDisplayPage=FALSE;

            for(i=0;i<ROWS;i++)
            {   
                sens=&sensors[i+ucMainDisplayPage*ROWS];
                vSensorToString(buffer,sens,config->_Sensors[sens->Id].Text,&config->_Sensors[sens->Id].Dim,20);   
                Display_Write(0,i,buffer,COLUMNS); 
            }  
            
            /* set current archive page to last */
            ucArchiveDisplayPage=usGetLastArchivePage();     
            break;   
             
            
        case(ARCHIVE): 

             /* check enable */
             if(!ucNeedUpdateArchiveDisplayPage) break;
             ucNeedUpdateArchiveDisplayPage=FALSE;
             
             lastnum = usArchive_NumOfEvents(); 
             if(lastnum!=0)
             {
                 eventnum=ucArchiveDisplayPage<<1;
                 for(i=1;i<=2;i++)
                 {          
                    if(i==1) row=2;else row=0; 
                    if(eventnum+i<=lastnum)
                    {   
                        if(aeArchive_GetEvent(eventnum+i, event)==ARCH_NOERROR)
                        {    
                            /* show event datetime */     
                            Display_Write(5,row,event->Datestr,15); 
                            
                            /* show event data */
                            Display_Write(0,row+1,event->Eventstr,15); 
                            memset(buffer,0x20,5);    
                            Display_Write(15,row+1,buffer,5);     
                            
                            /* show event num */
                            numstr=EventNum_ToString(eventnum+i,buffer);
                            numstr[4]=0x20;
                            Display_Write(0,row,numstr,5);                     
                        }
                        else
                        {
                            //TODO: handle archive read error
                        }   
                    } 
                    else
                    {  
                        /* there is no event, show empty lines */
                        memset(buffer,0x20,COLUMNS);
                        Display_Write(0,row,buffer,COLUMNS);
                        Display_Write(0,row+1,buffer,COLUMNS);
                    }
                 }
             }
             else
             {
                Display_SplashScreenf(ARCHIVE_EMPTY);
             }
             break;
           
        case(SETTINGS): 
            
            /* check enable */
             if(!ucNeedUpdateSettingsDisplayPage) break;
             ucNeedUpdateSettingsDisplayPage=FALSE; 
             
             vCalendar_ToString(buffer);   
             vDisplay_Clear();    
             
             if(ucUSB_Conn_IsConnected())
             {
                Display_Write(2,1,buffer,15);
                Display_Writef(0,3,SETTINGS_SCREEN+80,20);
             }  
             else
             {
                Display_Write(2,0,buffer,15);
                Display_Writef(0,1,SETTINGS_SCREEN+20,20);
                Display_Writef(0,2,SETTINGS_SCREEN+40,20); 
                Display_Writef(0,3,SETTINGS_SCREEN+60,20);
             }              
             
             break;     
    }
}



void WriteConfig_handler(USHORT offset,UCHAR data)
{
    UCHAR progress,crc;  
    static CHAR old_progress;
    DateTime* time;
    
    
    /* on 1st byte writing */
    if(0==offset)
    {          
        /* block main activity */
        ucBlockMainActivity=TRUE;  
        
        old_progress=-1;
        
        /* message */
        Display_SplashScreenf(WRITING_CONF);
    } 
     
    
    if(offset<=CONFIG_SIZE-1)
    {        
        /* write byte to eeprom config */
        WriteConfigByte(data,offset);   
        
        /* set date */
        if(offset>=CONFIG_SIZE-DATETIME_SIZE-2 && offset<CONFIG_SIZE-2)
        {
            time=dCalendar_GetTime();
            *((UCHAR*)time+(offset-(CONFIG_SIZE-DATETIME_SIZE-2)))=data; 
        }
        
        
        /* Show progress value */      
        progress=(((ULONG)offset+1)*100)/(CONFIG_SIZE*5);
        if(progress!=old_progress)
        {       
            old_progress=progress;
            memset(buffer,0xFF,20);  
            Display_Write(0,3,buffer,progress);  
        }
    }
    
}

UCHAR ReadConfig_handler(USHORT offset,eeprom UCHAR** data)
{
    UCHAR progress;
    static CHAR old_progress;


    
    /* on 1st byte reading */
    if(0==offset)
    {          
        /* block main activity */
        ucBlockMainActivity=TRUE;  
        
        old_progress=-1;
        
        /* message */
        Display_SplashScreenf(READING_CONF);
    } 
     
    
    if(offset<=CONFIG_SIZE-1)
    {        
        /* write byte to eeprom config */
        ReadConfigByte(data,offset);   
        

        /* Show progress value */      
        progress=(((ULONG)offset+1)*100)/(CONFIG_SIZE*5);
        if(progress!=old_progress)
        {       
            old_progress=progress;
            memset(buffer,0xFF,20);  
            Display_Write(0,3,buffer,progress);  
        }
        
        return TRUE;
    } 
    else
        return FALSE;
    
}


void USB_Conn_CheckIdle_handler()
{
    /* if USB listening is runing */
    if(ucUSB_Conn_IsListening())
    {                
        /* check for idle state */
        if(ucUSB_Conn_IsIdle()==TRUE)
        {       
            /* turn USB listening off */
            vUSB_Conn_Stop();
        }
        else
        { 
           /* set idle to check next time */
           ucUSB_Conn_SetIdle();
        }
    }
}


