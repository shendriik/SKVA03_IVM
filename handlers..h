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
void ButtonClick_handler(UCHAR num,ButtonClick state)
{
    
    
    if(state==SHORT_CLICK || state==LONG_CLICK)
    {                   
        lcd_gotoxy(x,0); 
        lcd_putchar(0x30+x);
        x++;
        if(x>=20) 
        {
            x=0;
            lcd_clear();    
        }   
    }

}
