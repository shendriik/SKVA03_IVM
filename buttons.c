 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h""

/* -------------------------- Own includes ------------------------------------*/
#include "buttons.h"



static void (*_ButtonClick_handler[MAX_BUTTONS])(UCHAR,ButtonClick);  /* Array of pointers to ButtonClick handlers */
static ButtonInfo _ButtonInfo[MAX_BUTTONS] = {0};                     /* Array of button _ButtonInfo */
static UCHAR _ucEnablePolling = FALSE;                                /* Polling enable flag */

/* Init button pin for input */
void vInit_ButtonIO(uint8_t* ddrport, UCHAR pin)
{
    *ddrport&=~(1<<pin);
}



/* Get button pin value */
UCHAR ucGetButton(uint8_t* pinport, UCHAR pin)
{
    return ((*pinport)&(1<<pin))>>pin; 
}


void vInit_Button(UCHAR number, uint8_t* ddrport, uint8_t* pinport, UCHAR pin, void (*ButtonClick_handler)(UCHAR ,ButtonClick))
{
    /* Add handler */   
    if(number<MAX_BUTTONS)
    {
        _ButtonClick_handler[number]=ButtonClick_handler; 
    
        /* Add button info */
        _ButtonInfo[number].pinport=pinport;
        _ButtonInfo[number].pin=pin;
    
        /* Init button IO */  
        vInit_ButtonIO(ddrport,pin);
    }
}



void vButtons_EnablePolling()
{
    _ucEnablePolling=TRUE;    

}

void vButtons_DisablePolling()
{
    _ucEnablePolling=FALSE;
}
      

/* Buttons FSM, it should be run each 20 ms. */         
void vButtonsPoll()
{
    UCHAR i=0;  
    UCHAR val=0; 
    UCHAR click;
    
    
    /* Run FSM for each button in array */
    for(i=0;i<MAX_BUTTONS && _ucEnablePolling ;i++)
    { 
        /* check if no handler available */
        if(!_ButtonClick_handler[i]) continue;

        click=NO_CLICK;
        val=ucGetButton(_ButtonInfo[i].pinport,_ButtonInfo[i].pin); 
        
        /* FSM starts here  */     
        /* Pressed and Hold */
        if(val==0) 
        {   
            _ButtonInfo[i].cnt++; 
           
            /* Check if first click */   
            if(_ButtonInfo[i].previous==NO_CLICK && _ButtonInfo[i].cnt >= CLICK_CYCLES)
            {
                _ButtonInfo[i].cnt=0;  
                _ButtonInfo[i].previous=CLICK;          
                click=CLICK;
            }
            else
            if(_ButtonInfo[i].previous==CLICK && _ButtonInfo[i].cnt >= HOLD_CYCLES)
            {                 
                _ButtonInfo[i].cnt=0;  
                _ButtonInfo[i].previous=HOLD;          
                click=CLICK;  
            }    
            else
            if(_ButtonInfo[i].previous==HOLD && _ButtonInfo[i].cnt >= HOLD_CYCLES_NEXT)
            {                 
                _ButtonInfo[i].cnt=0;  
                _ButtonInfo[i].previous=HOLD;          
                click=CLICK;  
            }   
        }     
        
        /* Not pressed or Released */
        if(val==1) 
        {   
            if(_ButtonInfo[i].previous!=NO_CLICK)
            {   
                click=RELEASED;
                
                /* reset to initial state */
                _ButtonInfo[i].cnt=0;  
                _ButtonInfo[i].previous=NO_CLICK; 
            }      
        }
        
        /* call ButtonClick handler */
        if(click!=NO_CLICK) _ButtonClick_handler[i](i,click);
    }

}

