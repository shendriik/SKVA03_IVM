 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"

/* -------------------------- Own includes ------------------------------------*/
#include "twi_master.h"


static UCHAR *uSendBuffer;                               /* Received buffer */
static UCHAR ptr = 0;                                   /* ptr to current received byte */
static UCHAR uSendCount = 0;                            /* num of bytes to send */
static void (*DataSend_callback)(UCHAR , SendState);    /* ptr to data received callback */
static UCHAR uSlaveID = 0;                              /* Slave id */

void vInit_TWI_Master()
{

    /* TWI initialization 250,000 kHz */
    TWBR=0x00;
    TWAR=0x00;              
    
    /* Generate Acknowledge Pulse: On, TWI: On, TWI Interrupt: On */
    TWCR=(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
    TWSR=0x00;
}


void vTWI_FSM()
{
    /* state of the TWI interface */
    UCHAR state = TWSR & 0xF8;
    
    switch(state)
    {             
        /* Start condition has been transmmited  */
        case (0x08):  
            
            /* Sending SLA+W */
            TWDR = (uSlaveID<<1) & 0xFE;
            break;    
            
        /* SLA+W has been transmmited, ACK has been received  */
        case (0x18):  
            
            ptr=0;                                                      /* reset indexer */
            TWDR = *(uSendBuffer+ptr++);                                /* Preparing first data byte */
            break;    
            
        /* Data byte has been transmmited, ACK has been received  */
        case (0x28):         
        
            TWDR = *(uSendBuffer+ptr++);                                /* Preparing next data byte */
            if(ptr==uSendCount)
            {
                TWCR=(1<<TWSTO);                                        /* Sending STOP condition */
            }  
            DataSend_callback(uSlaveID,TRANSMITTED);                    /* Calling DataSend event */
            break; 
            
        default:
            break;

    }          
    
    /* Clear Status flag */
    TWCR= (1<<TWINT);
    
}




void uSendData(UCHAR uSlave, UCHAR * uData, UCHAR uSize,void (*result_callback)(UCHAR , SendState))
{
    DataSend_callback = result_callback; 
    uSlaveID = uSlave;  
    uSendCount = uSize; 
    uSendBuffer = uData;
    
    /* Sending Start condition */
    TWCR|=(1<<TWSTA);

}


