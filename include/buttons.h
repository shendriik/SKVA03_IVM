 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _BUTTONS_
#define _BUTTONS_

#include "porting.h"

#define MAX_BUTTONS       5
#define CLICK_CYCLES      1                     /* number of 20ms intervals for first click [20 ms]   */ 
#define HOLD_CYCLES       50                    /* number of 20ms intervals for first click in hold mode [1000 ms]   */ 
#define HOLD_CYCLES_NEXT  3                     /* number of 20ms intervals for next clicks in hold mode [150 ms]   */ 


/* Button click  */
typedef enum
{
    NO_CLICK=0, 
    RELEASED=1,
    CLICK=2, 
    HOLD=3, //doesn't returned
        
} ButtonClick;


/* Button  state */
typedef struct 
{
    uint8_t*        pinport;
    UCHAR           pin;
    ButtonClick     previous;
    USHORT          cnt; 
    
} ButtonInfo;



      /* func prototypes */    
      
      /*  Initialization of the button click handler */ 
      void vInit_Button(UCHAR number,uint8_t* ddrport,uint8_t* pinport, UCHAR pin, void (*ButtonClick_handler)(UCHAR ,ButtonClick));

      /* turn polling on/off */
      void vButtons_EnablePolling();
      void vButtons_DisablePolling();
      
      /* Butttons main handler */  
      void vButtonsPoll();             
   
#endif