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
#include "power.h"

static UCHAR ucIsSleep = FALSE;    /* if MCU is in sleep mode */


void vPowerControl_Init()
{

    /* Enable On battery power supply pin */
    ON_BAT_INIT();   

}


UCHAR ucOnBattery()
{

    return (ON_BAT_PINPORT & _BV(ON_BAT_PIN))>>ON_BAT_PIN;  
    
}


UCHAR ucIsSleeping()
{

    return ucIsSleep;       
    
}


void vGoSleep()
{

    ucIsSleep=TRUE;    
    
    /* going to power save mode */
    SMCR=(1<<SM1)|(1<<SM0)|(1<<SE);
    #asm("sleep") 

}


void vWakingUp()
{

    ucIsSleep=FALSE;
    SMCR&=~(1<<SE);     
    
}


