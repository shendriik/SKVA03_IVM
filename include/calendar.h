 /*************************************************
 *    Calendar Lib
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _CALENDAR_
#define _CALENDAR_

#define MONTHS_COUNT    12

/* Months of the year */ 
typedef enum
{

    FEBRUARY= 2,
    APRIL = 4,  
    JUNE = 6,
    SEPTEMBER = 9,
    NOVEMBER = 11   
    
} Months;

/* DateTime */
typedef struct
{
    char Hour;
    char Min;
    char Sec;
    char Day;
    char Month;
    char Year;
    
} DateTime;

#define DATETIME_SIZE 6


    /* Init calendar */
    void vCalendar_Init(DateTime* startDateTime);  
    
    /* get pointer to datetime value */
    DateTime* dCalendar_GetTime();

    /* Should be run in timer overfow ISR with 1 Hz interrupt frequency */
    void vCalendar_Poll();

    /* Check if it is not leap year */
    char not_leap(char year);  
    
    /* get date and time string */
    void vCalendar_ToString(unsigned char* str);
    
    
#endif