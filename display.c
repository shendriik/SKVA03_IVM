 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"
#include "alcd.h"
#include "delay.h"

/* -------------------------- Own includes ------------------------------------*/
#include "display.h"

flash UCHAR NOT_CONFIGURED[] = 
                      { 
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                        ' ',' ',' ',' ',' ',' ','C',I  ,'C','T','E','M','A',' ',' ',' ',' ',' ',' ',' ',
                        'H','E',' ',Z  ,'A',P  ,'P','O',G  ,'P','A','M','M',I  ,'P','O','B','A','H','A',
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                      };  
                      
flash UCHAR WRITING_CONF[] = 
                      { 
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                        Z  ,'A',P  ,I  ,'C','b',' ',' ','K','O','H',F  ,I  ,G  ,Y  ,'P','A',C  ,I  ,I  ,
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                      };   
                      
flash UCHAR READING_CONF[] = 
                      { 
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                        CH ,'T','E','H',I  ,'E',' ',' ','K','O','H',F  ,I  ,G  ,Y  ,'P','A',C  ,I  ,I  ,
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                      };
                      
flash UCHAR START_LOGO[] = 
                      { 
                        ' ',' ','C',I  ,'C','T','E','M','A',' ',' ','C','K','B','A','-','0','3',' ',' ',
                        ' ',' ',' ','O','O','O',' ','H',P  ,F  ,' ',I  ,'H','K','P','A','M',' ',' ',' ',
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                        z  ,'a',g  ,'p','y',z  ,'k','a','.','.','.',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                      }; 
                      
flash UCHAR ARCHIVE_EMPTY[] = 
                      { 
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                        ' ',' ',' ','A','P','X',I  ,'B',' ',' ','C',I  ,'C','T','E','M',bI ,' ',' ',' ',
                        ' ',' ',' ',' ',' ',' ',' ',' ',P  ,Y  ,'C','T',' ',' ',' ',' ',' ',' ',' ',' ',
                        ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
                      };  
                      
flash UCHAR SETTINGS_SCREEN[] = 
                      { 
                        ' ',' ','X','X','.','X','X','.','X','X',' ',' ','X','X',':','X','X',' ',' ',' ',
                        'M','o','d','b','u','s',' ',' ','I','D','=','1','0',',',' ',' ','F','=','4','x',
                        'S','t','a','r','t','=','1','0','0','0',',','C','o','u','n','t','=','7','6',' ',
                        'r','u','n','n','i','n','g',' ','o','n',' ','1','9','2','0','0','-','8','E','1',
                        'U','S','B',' ','i','s',' ','c','o','n','n','e','c','t','e','d',' ',' ',' ',' ',
                      };

                                      
static DisplayMode dDisplayMode = MAIN;







void vInit_Display()
{    

    lcd_init(COLUMNS);
    lcd_clear();
    lcd_gotoxy(0,0); 
    dDisplayMode = MAIN;  
    
}

void vDeInit_Display()
{    

    DATA_DDR&=  ~((1<<D7)|(1<<D6)|(1<<D5)|(1<<D4)|(1<<RD)|(1<<RS));   
    DATA_PORT&= ~((1<<D7)|(1<<D6)|(1<<D5)|(1<<D4)|(1<<RD)|(1<<RS));
    EN_DDR&=  ~(1<<EN);    
    EN_PORT&= ~(1<<EN);    
        
}

void Display_WriteMessagef(flash UCHAR* message)
{
    lcd_clear(); 
    lcd_gotoxy(0,0);
    lcd_putsf((flash CHAR*)message);
}



void Display_SplashScreenf(flash UCHAR* message)
{
    UCHAR dx,y;  
    UCHAR med1,med2;
    
    for(dx=0;dx<COLUMNS/2;dx++)
    {               
        med1=COLUMNS/2-1-dx;
        med2=COLUMNS/2+dx;
            
        y=0;
        lcd_gotoxy(med1,y);
        lcd_putchar(message[y*COLUMNS+med1]); 
        lcd_gotoxy(med2,y);
        lcd_putchar(message[y*COLUMNS+med2]); 
            
        y=1;
        lcd_gotoxy(med1,y);
        lcd_putchar(message[y*COLUMNS+med1]); 
        lcd_gotoxy(med2,y);
        lcd_putchar(message[y*COLUMNS+med2]); 
            
        y=2;
        lcd_gotoxy(med1,y);
        lcd_putchar(message[y*COLUMNS+med1]); 
        lcd_gotoxy(med2,y);
        lcd_putchar(message[y*COLUMNS+med2]); 
            
        y=3;
        lcd_gotoxy(med1,y);
        lcd_putchar(message[y*COLUMNS+med1]); 
        lcd_gotoxy(med2,y);
        lcd_putchar(message[y*COLUMNS+med2]); 
        delay_ms(70);
    }   
    
}

void Display_Write(UCHAR x,UCHAR y,UCHAR* message,UCHAR size)
{
    UCHAR i=0;
    
    if(y<ROWS && x+size<=COLUMNS)
    {
        lcd_gotoxy(x,y);
        for(i=0;i<size;i++)
        {
            lcd_putchar(message[i]);
        }
    }    
}

void Display_Writef(UCHAR x,UCHAR y,flash UCHAR* message,UCHAR size)
{
    UCHAR i=0;
    
    if(y<ROWS && x+size<=COLUMNS)
    {
        lcd_gotoxy(x,y);
        for(i=0;i<size;i++)
        {
            lcd_putchar(message[i]);
        }
    }    
}


void vDisplay_Clear()
{    
    lcd_clear();           
}


DisplayMode dGetDisplayMode()
{
    return dDisplayMode;
}

DisplayMode dNextDisplayMode()
{
    if(++dDisplayMode > SETTINGS ) dDisplayMode = MAIN;
    return dDisplayMode;
}

void vSetDisplayMode(DisplayMode mode)
{
    dDisplayMode = mode;
}