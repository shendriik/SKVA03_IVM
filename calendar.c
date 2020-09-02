 /*************************************************
 *    Calendar Lib
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
                          
 
/* -------------------------- Own includes ------------------------------------*/
#include "calendar.h"
#include "mymath.h"
#include "string.h"

static DateTime* dateTime;                  /* pointer to current date and time */


void vCalendar_Init(DateTime* startDateTime)
{
    dateTime=startDateTime;   
}

DateTime* dCalendar_GetTime()
{
    return dateTime;   
}




void vCalendar_Poll()
{

   if (++(dateTime->Sec)==60)
   {
      dateTime->Sec=0;
      if (++(dateTime->Min)==60)
      {
         dateTime->Min=0;
         if (++(dateTime->Hour)==24)
         {
            dateTime->Hour=0;
            if (++(dateTime->Day)==32)
            {
               dateTime->Month++;
               dateTime->Day=1;
            }
            else if (dateTime->Day==31)
            {
               if ((dateTime->Month==APRIL) || (dateTime->Month==JUNE) || (dateTime->Month==SEPTEMBER) || (dateTime->Month==NOVEMBER))
               {
                  dateTime->Month++;
                  dateTime->Day=1;
               }
            }
            else if (dateTime->Day==30)
            {
               if(dateTime->Month==FEBRUARY)
               {
                  dateTime->Month++;
                  dateTime->Day=1;
               }
            }
            else if (dateTime->Day==29)
            {
               if((dateTime->Month==FEBRUARY) && (not_leap(dateTime->Year)))
               {
                  dateTime->Month++;
                  dateTime->Day=1;
               }
            }
            if (dateTime->Month>MONTHS_COUNT)
            {
               dateTime->Month=1;
               dateTime->Year++;
            }
         }
      }
   }
}


char not_leap(char year)  // low 2 digits of the year
{
      return year%4;
}



void vCalendar_ToString(unsigned char* str)
{
    unsigned char valbuf[12]; 
    unsigned char* start;
    unsigned char* end; 
    unsigned char x;
    
    memset(str,0x30,15);
    x=14;
    end=valbuf+10; 
    start=utoa_fast_div(dateTime->Min,valbuf);
    while(end>=start) str[x--] = *(end--);   
    
    str[12]=':';
    
    x=11;
    end=valbuf+10; 
    start=utoa_fast_div(dateTime->Hour,valbuf);
    while(end>=start) str[x--] = *(end--); 
    
    str[9]=' ';
    str[8]=' ';
    
    x=7;
    end=valbuf+10; 
    start=utoa_fast_div(dateTime->Year,valbuf);
    while(end>=start) str[x--] = *(end--);  
    
    str[5]='.';
    
    x=4;
    end=valbuf+10; 
    start=utoa_fast_div(dateTime->Month,valbuf);
    while(end>=start) str[x--] = *(end--);   
    
    str[2]='.';
    
    x=1;
    end=valbuf+10; 
    start=utoa_fast_div(dateTime->Day,valbuf);
    while(end>=start) str[x--] = *(end--);
}