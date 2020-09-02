 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _DISPLAY_
#define _DISPLAY_

#include "avr/io.h"
#include "porting.h"

#define COLUMNS     20
#define ROWS        4

#define EN_PORT     PORTD
#define EN_DDR      DDRD
#define EN          4

#define DATA_PORT   PORTB
#define DATA_DDR    DDRB
#define RS          0
#define RD          1
#define D4          4
#define D5          5
#define D6          6
#define D7          7


/* Cyrilic */
#define I           0xA5
#define Z           0xA4
#define P           0xA8
#define G           0xA1  
#define F           0xAA  
#define CH          0xAB 
#define B           0xA0 
#define Y           0xA9 
#define bI          0xAE
#define D           0xE0
#define C           0xE1
#define EE          0xA2



#define z           0xB7
#define g           0xB4  

#define NUM         0x17    
      
/* Display mode */
typedef enum
{
    MAIN=0,
    ARCHIVE, 
    SETTINGS,
} DisplayMode;


                void vInit_Display();               /* Init display */
                void vDeInit_Display();    
                                       
                
                void Display_WriteMessagef(flash UCHAR* message);
                void Display_SplashScreenf(flash UCHAR* message); 
                
                void Display_Write(UCHAR x,UCHAR y,UCHAR* message,UCHAR size);   
                void Display_Writef(UCHAR x,UCHAR y,flash UCHAR* message,UCHAR size);  
                void vDisplay_Clear();
                
                /* functions to incapsulate display mode */
                DisplayMode dGetDisplayMode();
                DisplayMode dNextDisplayMode();
                void vSetDisplayMode(DisplayMode mode);

#endif