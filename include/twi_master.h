 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _TWI_MASTER_
#define _TWI_MASTER_

#include "porting.h"

#define READ                1
#define WRITE               0
#define MAX_BUFFER          0x20    /* Received buffer max count */


/* Result of Send operation */
typedef enum 
{
    TRANSMITTED=0,
    RECEIVED,
    NO_RESPONSE,
    ABORTED, 
    BUSERROR    

} TWIState;

      /* Initialization of the TWI in master mode */
      void vTWI_Init_Master();                 
      
       /* Twi finite state machine */  
      void vTWI_FSM();                         
      
      /* Send data to slave */
      TWIState tsTWI_Write(UCHAR ucSlave, UCHAR* adr,UCHAR adrsize,UCHAR* uData, UCHAR uSize); 
      
      /* Receive data from slave */     
      TWIState tsTWI_Read(UCHAR ucSlave, UCHAR* adr,UCHAR adrsize,UCHAR* uData, UCHAR uSize);
  

#endif