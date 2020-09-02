 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 

/* ----------------------- Platform includes ----------------------------------*/    
#include "string.h" 

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"
/* -------------------------- Own includes ------------------------------------*/
#include "usbloader.h"

static UCHAR ucIsStarted=FALSE;                             /* main started flag */
static UCHAR ucCommand[COMMAND_SIZE];                       /* command chars */
static USHORT usOfs = 0;                                    /* offset in array */
static USBCommand usbCommand = NO;                          /* received command */
static void (*writeconfig_handler)(USHORT,UCHAR);           /* pointer to write config callback function */
static UCHAR (*readconfig_handler)(USHORT,eeprom UCHAR**);  /* pointer to read config callback function */
static UCHAR ucIsIdle=FALSE;                                /* idle state flag */   
static UCHAR prev_byte;                                     /* previously sent byte */
            

void vUSB_Conn_Init()
{
    /* Enable on usb pin */
    ON_USB_INIT();           
}

void vUSB_Conn_SetWriteConfig_callback(void (*callback_handler)(USHORT,UCHAR))
{
    writeconfig_handler=callback_handler;
}

void vUSB_Conn_SetReadConfig_callback(UCHAR (*callback_handler)(USHORT,eeprom UCHAR**))
{
    readconfig_handler=callback_handler;
}


UCHAR ucUSB_Conn_IsConnected()
{
    return (ON_USB_PINPORT & _BV(ON_USB_PIN))>>ON_USB_PIN;             
}

UCHAR ucUSB_Conn_IsIdle()
{
    return ucIsIdle;          
}

UCHAR ucUSB_Conn_SetIdle()
{
    ucIsIdle=TRUE;
}

UCHAR ucUSB_Conn_IsListening()
{

    return ucIsStarted; 
    
}


void vUSB_Conn_WaitForCommand()
{
    memset(ucCommand,0x00,COMMAND_SIZE);
    usbCommand=NO; 
    usOfs=0;
} 

USBCommand uUSB_Conn_GetCurrentCommand()
{

    return usbCommand;  
    
}

/* Transport layer */
inline void vUSB_Conn_Listen()
{
    UCSR1A=0x00;
    UCSR1B=0x98;
    UCSR1C=0x06;
    UBRR1H=0x00;
    UBRR1L=0x33;  
    ucIsStarted=TRUE;
    ucIsIdle=FALSE; 
}

inline void vUSB_Conn_Stop()
{
    UCSR1A=0x00;
    UCSR1B=0x00;
    UCSR1C=0x00;
    UBRR1H=0x00;
    UBRR1L=0x00;
    ucIsStarted=FALSE;
}

void vUSB_Conn_ISR(UCHAR data)
{
    eeprom UCHAR* read_data;
    UCHAR read_ok;
    
    /* if no command received */
    if(usbCommand==NO)
    {              
        /* get command next byte */
        if(usOfs==COMMAND_SIZE) 
        {   
            usOfs--;
            memmove(ucCommand,ucCommand+1,COMMAND_SIZE-1); 
        }
        ucCommand[usOfs++]=data;                
        
        /* $PING0 command received */
        if(ucCommand[0]=='$'&&ucCommand[1]=='P'&&ucCommand[2]=='I'&&ucCommand[3]=='N'&&ucCommand[4]=='G'&&ucCommand[5]=='0')
        {   
            /* setting command */
            memset(ucCommand,0x00,COMMAND_SIZE);
            usOfs=0;
            
            /* sending Ack */
            putchar('$');
            putchar('O');
            putchar('Z'); 
            putchar('P');
            putchar('O');
            putchar('N');
            putchar('G');            
        }
        
        /* $WRCFG command received */
        if(ucCommand[0]=='$'&&ucCommand[1]=='W'&&ucCommand[2]=='R'&&ucCommand[3]=='C'&&ucCommand[4]=='F'&&ucCommand[5]=='G')
        {        
            /* setting command */
            usbCommand=WRITE_CONFIG;
            memset(ucCommand,0x00,COMMAND_SIZE);
            usOfs=0;
                     
            /* sending Ack */
            putchar('$');
            putchar('O');
            putchar('K');         
        }
        
        /* $RDCFG command received */
        if(ucCommand[0]=='$'&&ucCommand[1]=='R'&&ucCommand[2]=='D'&&ucCommand[3]=='C'&&ucCommand[4]=='F'&&ucCommand[5]=='G')
        {        
            /* setting command */
            usbCommand=READ_CONFIG;
            memset(ucCommand,0x00,COMMAND_SIZE);
            usOfs=0;
                     
            /* sending Ack */
            putchar('$');
            putchar('O');
            putchar('K');         
        }  
        
        /* interface isn't idle */
        ucIsIdle=FALSE; 
    }  
    /* if we have received any command */
    else
    {    
        switch(usbCommand)
        {
            case(WRITE_CONFIG):  
                
                /* call handler to write config byte */ 
                if(writeconfig_handler) 
                {     
                    /* echo byte to PC */
                    putchar(data);  
        
                    writeconfig_handler(usOfs++,data); 
                                 
                    /* interface isn't idle */
                    ucIsIdle=FALSE;   
                }
                break;   
                
            case(READ_CONFIG):

                /* call handler to write config byte */ 
                if(readconfig_handler) 
                {    
                    if(usOfs==0 || data==prev_byte)
                    { 
                        read_ok=readconfig_handler(usOfs++,&read_data);
                        if(read_ok)
                        {                               
                            prev_byte=*read_data;
                            /* send config byte to PC */
                            putchar(prev_byte);  
                                
                            /* interface isn't idle */
                            ucIsIdle=FALSE;  
                        }
                    }         
                }  
                break;   
        }                  
    }    
}


/********************/
/* helper functions */
/********************/

inline void putchar(UCHAR c)
{
    while (!(UCSR1A & 1<<UDRE1));
    UDR1=c;
}