 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _POWER_
#define _POWER_

#include "porting.h"




/* On battery power supply pin specify*/
#define ON_BAT_PIN       2
#define ON_BAT_DDR       DDRB
#define ON_BAT_PINPORT   PINB
#define ON_BAT_INIT()       \
    {\                
        ON_BAT_DDR &= ~_BV( ON_BAT_PIN ); \ 
    }             
                        
    
    /* func prototypes */     
     
    void vPowerControl_Init();      /* init power control   */
    UCHAR ucOnBattery();            /* return TRUE if only battery power supply used */ 
    UCHAR ucIsSleeping();           /* Is MCU sleeping? */
    void vGoSleep();                /* go to sleep    */
    void vWakingUp();               /* waking up    */

    
#endif