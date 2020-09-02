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

static UCHAR *ucBuffer;                                  /* buffer */
static UCHAR ucSendCount = 0;                            /* num of bytes to send */
static UCHAR ucRecvCount =0;                             /* num of bytes to receive */
static UCHAR sptr=0;                                     /* send stream pointer */
static UCHAR rptr=0;                                     /* receive stream pointer */

static UCHAR *ucAdrBuffer;                               /* adr buffer */
static UCHAR ucAdrCount = 0;                             /* num of address bytes to send */

static TWIState tsLastState;                             /* last operation state */
static UCHAR ucSlaveID = 0;                              /* slave id */
static UCHAR ucBusy = FALSE;                             /* busy flag */
static UCHAR ucMode = WRITE;                             /* interface mode */

UCHAR GetNextByteSend()
{
    UCHAR res=0;
    if(sptr>=ucAdrCount)
    {
        res=ucBuffer[sptr-ucAdrCount];
    }           
    else
    {
        res=ucAdrBuffer[sptr];
    }  
    sptr++;
    return res;
}

UCHAR GetSendLength()
{
    return ucAdrCount+ucSendCount; 
}

void SetSendPtr(UCHAR val)
{
    sptr=val;
}

UCHAR GetSendPtr()
{
    return sptr;
}


void SetNextByteReceived(UCHAR val)
{
    ucBuffer[rptr++]=val;
}

UCHAR GetReceiveLength()
{
    return ucRecvCount; 
}

void SetReceivePtr(UCHAR val)
{
    rptr=val;
}

UCHAR GetReceivePtr()
{
    return rptr;
}


void vTWI_Init_Master()
{
    /* TWI initialization 250,000 kHz */
    TWBR= 0x00;
    TWAR= 0x00;              
    TWSR= 0x00; 
    TWCR= 0x00;
       
    ucBusy = FALSE;
    ucMode = WRITE;  
}

void vTWI_FSM()
{

    /* state of the TWI interface */
    UCHAR state = TWSR & 0xF8;
     
    switch(state)
    {    
    
        /* Start condition has been transmmited  */
        case (0x08):  
            
            TWDR = (ucSlaveID<<1) | WRITE;                                   /* Sending SLA+W */  
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE); 
            break;   
         
        /* Repeated start condition has been transmmited  */
        case (0x10):  

            TWDR = (ucSlaveID<<1) | READ;                                    /* Sending SLA+R */  
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
            break;    
             
        /* SLA+W has been transmmited, ACK has been received  */
        case (0x18):   
  
            SetSendPtr(0);                                                  /* reset indexer */
            TWDR = GetNextByteSend();                                       /* Preparing first data byte */        
            TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
            break;  
              
        /* SLA+R has been transmmited, ACK has been received  */
        case (0x40):   

            SetReceivePtr(0);                                               /* reset indexer */      
            if(GetReceivePtr()==GetReceiveLength()-1)
            {                                       
                TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWIE);                       /* return NOT ACK after next received byte */ 
            }    
            else
            {      
                TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);            /* return ACK after next received byte */
            } 
            break; 
                                  
        /* SLA + WR has been transmmited, NOT ACK has been received */
        case (0x20): 
        case (0x48): 
        
            TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE);               /* stop */
            tsLastState=NO_RESPONSE;                                    
            ucBusy = FALSE; 
            break; 
        
        /* Data byte has been transmmited, ACK has been received  */
        case (0x28):         
            
            /* No more bytes to send */
            if(GetSendPtr()==GetSendLength())
            {   
                if(ucMode==WRITE)
                {
                    TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE);       /* stop */           
                    tsLastState=TRANSMITTED;                                 
                    ucBusy = FALSE; 
                }
                if(ucMode==READ)
                {
                    TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWSTA);        /* repeated start */    
                }
            }    
            else
            {      
                TWDR = GetNextByteSend();                                  /* Preparing next data byte */ 
                TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWIE); 
            }              
            break;  
        
        /* Data byte has been transmmited, NOT ACK has been received  */
        case (0x30):         
            
            /* No more bytes to send */
            if(GetSendPtr()==GetSendLength())
            {   
                if(ucMode==WRITE)
                {
                    TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE);        /* stop */          
                    tsLastState=TRANSMITTED;                                
                    ucBusy = FALSE; 
                }
                if(ucMode==READ)
                {
                    TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWSTA);        /* repeated start */  
                }
            }    
            else
            {                 
                tsLastState=ABORTED;                                     
                ucBusy = FALSE;       
                TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE);           /* error, stop */  
            }            
            break; 

        /* Data byte has been received, ACK has been returned  */
        case (0x50):         

            SetNextByteReceived(TWDR);  
            if(GetReceivePtr()==GetReceiveLength()-1)
            {                                       
                TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWIE);                       /* return NOT ACK after next received byte */ 
            }    
            else
            {      
                TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWEA);            /* return ACK after next received byte */
            }              
            break;
            
        /* Data byte has been received, NOT ACK has been returned  */
        case (0x58):         
                             
            TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE);               /* received ok, stop */
            SetNextByteReceived(TWDR);        
            tsLastState=RECEIVED;                                       
            ucBusy = FALSE;   
            break;  
        
        /* Unknown state */
        default: 
        
            TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN)|(1<<TWIE);              /* unknown error, stop */ 
            tsLastState=BUSERROR;                                       
            ucBusy = FALSE;             
            break;
    }             
}





TWIState tsTWI_Write(UCHAR ucSlave, UCHAR* adr,UCHAR adrsize,UCHAR* uData, UCHAR uSize)
{
    ucBuffer = uData;
    ucSendCount = uSize;
    ucRecvCount = 0;  
    
    ucAdrBuffer=adr;
    ucAdrCount=adrsize;    
    
    ucBusy = TRUE; 
    ucSlaveID = ucSlave;      
    ucMode = WRITE; 

    /* Start */
    TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWSTA);  
    
    /* Wait for complete */   
    while(ucBusy); 

    return tsLastState;
}

TWIState tsTWI_Read(UCHAR ucSlave, UCHAR* adr,UCHAR adrsize,UCHAR* uData, UCHAR uSize)
{       
    ucBuffer = uData;
    ucRecvCount = uSize; 
    ucSendCount = 0; 

    ucAdrBuffer=adr;
    ucAdrCount=adrsize;  
    
    ucBusy = TRUE; 
    ucSlaveID = ucSlave; 
    ucMode = READ; 

    /* Start */
    TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWIE)|(1<<TWSTA);  
    
    /* Wait for complete */   
    while(ucBusy); 
    
    return tsLastState;
}

