 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"
#include "porting.h"
#include "delay.h"

/* ------------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmfunc.h"
#include "mmcrc.h"

//
#include "mb.h"
#include "mbcrc.h"
#include "mbfunc.h"
#include "port.h" 

/* -------------------------- Own includes ------------------------------------*/
#include "defines_my.h"
#include "twi_master.h"
#include "buttons.h";
#include "calendar.h"
#include "power.h"
#include "display.h"
#include "config.h"
#include "algorithm.h"
#include "perifery.h"
#include "archive.h"
#include "usbloader.h"


/* extern functions from handlers.c */
extern void ButtonClick_handler(UCHAR sender,ButtonClick state);
extern void USB_Conn_CheckIdle_handler();
extern void MainWork_handler();
extern void Display_handler();
extern void WriteConfig_handler(USHORT offset,UCHAR data);
extern UCHAR ReadConfig_handler(USHORT offset,eeprom UCHAR** data);
extern void vHandlers_ResetVariables();
/***********************************/


/* extern var */
extern UCHAR ucIsSecondLeft;                        /* 1 second interval flag */
extern UCHAR ucIs20mSecondLeft;                     /* 1/4 second interval flag */                 
extern UCHAR ucNeedRestart;                         /* need restart after programming flag */ 
extern flash UCHAR NOT_CONFIGURED[];                /* Not configerd error message */
extern flash UCHAR START_LOGO[];                    /* Start logo */
extern UCHAR ucBlockMainActivity;                   /* block main activity flag */ 

eeprom Config* config;                              /* pointer to current config in eeprom */
DateTime dDateTime = { 0,0,0,01,01,00 };             /* Initial date and time */
USHORT usRegHoldingStart = REG_HOLD_START;           /* Modbus slave start register */
USHORT usRegHoldingBuf[REG_HOLD_NREGS];              /* Modbus slave holding registers array */
UCHAR  ucModbusSlave_IsListening = FALSE;            /* Flag if Modbus Slave is listening */






void main(void)
{   

    /* Start system clock */
    vInit_Clock();      
    
    /* Start timer 2 for RTC */  
    vInit_RTC_Timer2();    
    
    /* Start timer 0 for buttons,display update and algorithm */  
    vInit_Timer0();   
    
    /* Init relais port */
    vInit_InternalRelais();
    
    /* Init calendar */
    vCalendar_Init(&dDateTime);  //TODO: put it after config loaded 
        
    /* External Int2 for waking up on external power supply */   
    vPowerControl_Init();
    vInit_ExtInt2();      
    
    /* usb loader */
    vUSB_Conn_Init( );
    vUSB_Conn_SetWriteConfig_callback(WriteConfig_handler);   
    vUSB_Conn_SetReadConfig_callback(ReadConfig_handler); 
    
    /* init buttons handler */
    vInit_Button(OK_BUTTON,&DDRC,&PINC,5,ButtonClick_handler);
    vInit_Button(DOWN_BUTTON,&DDRC,&PINC,4,ButtonClick_handler);
    vInit_Button(UP_BUTTON,&DDRC,&PINC,3,ButtonClick_handler);  
    vInit_Button(CHECK_BUTTON,&DDRA,&PINA,2,ButtonClick_handler);
    vInit_Button(SOUND_OFF_BUTTON,&DDRA,&PINA,3,ButtonClick_handler); 
    vButtons_EnablePolling(); 
                            
    /* initialy main activity is blocked */
    ucBlockMainActivity = TRUE;
                       
    /* go sleep */  
    #asm("sei")   
    vGoSleep(); 

    while(1)
    {   
       
        /* if no external power supply or if restart needed */
        if(ucOnBattery() || ucNeedRestart)
        {    
            /* reset restart flag */ 
            ucNeedRestart=FALSE;
                   
            vDeInit_Display();
            vSetError(FALSE);  
            vSet_InternalRelais(FALSE); 
            /* go sleep */ 
            vGoSleep();  
        }   
        else
        {       
            /* if we was sleeping and external power released */
            if(ucIsSleeping())
            {    
                vWakingUp();    
                
                /* main initialization */          
                {          
                    /* init display */
                    vInit_Display();
                    Display_SplashScreenf(START_LOGO); 
                    delay_ms(2000); 
                    
                    /* Getting config*/  
                    if(GetConfig(config)==CFG_ECRCFAIL)
                    {     
                        /* block main activity */
                        ucBlockMainActivity = TRUE;
                        
                        /* show error message */
                        Display_SplashScreenf(NOT_CONFIGURED);      
                    } 
                    else
                    {   
                        /* reset variables from handlers.c */
                        vHandlers_ResetVariables(); 
                        
                        /* start Modbus Master */
                        if(!(eMMInit(4800,MM_PAR_EVEN)==MM_ENOERR && eMMEnable()==MM_ENOERR)) {while(1);}   
                        
                        /* init main algorithm */
                        vAlgorithm_Init();

                        /* init archive */
                        vArchive_Init();
                         
                        /* init I2C interface */
                        vTWI_Init_Master(); 
                                   
                        /* unblock main activity */
                        ucBlockMainActivity = FALSE;  
                        
                        /* reset modbus listenning flag !!TEST!! */
                        ucModbusSlave_IsListening = FALSE; 
                        
                    } 
                }
            } 
            
                                  
            /* USB connection handling */
            if (ucUSB_Conn_IsConnected() && !ucUSB_Conn_IsListening() || !ucUSB_Conn_IsConnected())
            {
	            /* if config was previously written */
	            if (uUSB_Conn_GetCurrentCommand() == WRITE_CONFIG)
	            {
                    /* resetting archive */
                    vArchive_ResetSettings();
                    aeArchive_StoreSettings();

                    /* emulate restart */
                    ucNeedRestart = TRUE;
	            }

	            /* if config was previously read */
	            if (uUSB_Conn_GetCurrentCommand() == READ_CONFIG)
	            {
		            /* emulate restart*/
		            ucNeedRestart = TRUE;
	            }

	            /* start listenning */
	            if (ucUSB_Conn_IsConnected()) vUSB_Conn_Listen();

	            /* reset current command */
	            vUSB_Conn_WaitForCommand();     
            }   
               
            
            /* Modbus Slave handling */  
            if (!ucUSB_Conn_IsConnected())
            {                        
                /* if modbus isn't running */    
                if(!ucModbusSlave_IsListening)
                {
                    eMBInit(MB_RTU, MODBUS_SLAVE_ID, 0x01 ,MODBUS_SLAVE_BAUDRATE , MB_PAR_EVEN);
                    eMBEnable();            
                    
                    ucModbusSlave_IsListening = TRUE;
                } 
                
                /* Running Modbus Protocol Stack if the system works fine */
                if(!ucBlockMainActivity)  
                {
                    if(eMBPoll()==MB_ENOERR) { }
                }         
            }     
            else
            {            
                /* Stop modbus slave */
                eMBClose();
                eMBDisable();      
                
                ucModbusSlave_IsListening = FALSE;
            }
            
 
            
            /* if one second interval flag is set */
            if(ucIsSecondLeft==TRUE) 
            {                
                ucIsSecondLeft=FALSE; 
                   
                /* run usb check idle handler */
                USB_Conn_CheckIdle_handler();
                
                /* run handler */
                MainWork_handler();  
            }
            
            /* if 20 milisecond interval flag is set */
            if(ucIs20mSecondLeft==TRUE) 
            {                
                ucIs20mSecondLeft=FALSE; 
                
                /* run handler */
                Display_handler();  
            }
        }
    }     
}

eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    return MB_ENOREG;
}



eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    
    if( ( usAddress >= REG_HOLD_START ) &&
        ( usAddress + usNRegs <= REG_HOLD_START + REG_HOLD_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );

        switch ( eMode )
        {
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}





