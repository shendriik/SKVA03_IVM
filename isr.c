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

/* ------------------------- Modbus Slave includes ----------------------------------*/
#include "mbport.h"


/* -------------------------- Own includes ------------------------------------*/
#include "twi_master.h"
#include "buttons.h"
#include "calendar.h"
#include "power.h"      
#include "usbloader.h"


//delete
#include "perifery.h"

static UCHAR tim0ovf_cnt=0;                 /* Timer 0 overflow counter */ 
static USHORT tim0ovf_cnt2=0;               /* Timer 0 overflow counter */ 
static USHORT tim0ovf_cnt3=0;               /* Timer 0 overflow counter */ 
UCHAR ucIsSecondLeft=FALSE;                 /* 1 second interval flag */
UCHAR ucIs20mSecondLeft=FALSE;              /* 20 milisecond interval flag */


/* Timer 0 overflow interrupt service routine */
interrupt [TIMER0_OVF_vect_num] void TIMER0_OVF_vect(void)
{
    

    /* Starts vButtonPoll FSM each 20 ms */
    if(++tim0ovf_cnt==6)
    {
        vButtonsPoll(); 
        tim0ovf_cnt=0;
    }  
    
    /* set 1 sec interval flag */
    if(++tim0ovf_cnt2==244)
    {
        ucIsSecondLeft=TRUE;
        tim0ovf_cnt2=0;
    } 
    
    /* set 20 milisec interval flag */
    if(++tim0ovf_cnt3==6)
    {
        ucIs20mSecondLeft=TRUE;
        tim0ovf_cnt3=0;
    }
     
}

/* Timer that runs from external 32768 Hz crystall */
interrupt [TIMER2_OVF_vect_num] void TIMER2_OVF_vect(void)
{

    /* run calendar poll */
    vCalendar_Poll();  
    
}




/* Two Wire bus interrupt service routine */
interrupt [TWI_vect_num] void TWI_vect(void)
{
    vTWI_FSM();
}



/* External Interrupt 2 service routine */
interrupt [INT2_vect_num] void INT2_vect(void)
{

}
 




// USART1 Receiver interrupt service routine
interrupt [USART1_RX_vect_num] void USART1_RX_vect(void)
{
    UCHAR data;
    
    if(ucUSB_Conn_IsConnected())
    {       
        data=UDR1;
        vUSB_Conn_ISR(data); 
    }
    else
    {
        pxMBFrameCBByteReceived(  ); 
    }     
}

