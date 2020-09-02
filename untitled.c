 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _CONFIG_LOADER_
#define _CONFIG_LOADER_

#include "porting.h"




/* On USB connected pin specify */
#define ON_USB_PIN       6
#define ON_USB_DDR       DDRD
#define ON_USB_PINPORT   PIND
#define ON_USB_INIT()       \
    {\                
        ON_USB_DDR &= ~_BV( ON_USB_PIN ); \ 
    }             
                        
    
    /* func prototypes */     
     
    void vUSB_Configurer();         /* init usb configurer  */
    UCHAR ucOnUSB();                /* return TRUE if usb connected */ 


    
#endif