 /*************************************************
 *    Sort funcs
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 

#include "sort.h"
#include "string.h"
#include "stdlib.h"


unsigned char temp[MAX_BUFFER_SIZE] = {0};


void swap(unsigned char* x,unsigned char* y,unsigned char size)
{
    memmove(temp,x,size);
    memmove(x,y,size);
    memmove(y,temp,size); 
}


void bubble_sort(unsigned char *arr, 
                     char key,
                     char prev_key1,
                     char prev_key2,
                     char item_length,
                     char length,
                     char low_first)                                   
{

    char num = 0; 
    char i;
    char x,y; 
    char sorted;
    
    char xp1=EMPTY;
    char yp1=EMPTY;
    char xp2=EMPTY;
    char yp2=EMPTY;

    do
    {
        sorted = 0;
        for (i = 0; i < length - 1 - num; i++)
        {   
                x=(unsigned char)*(arr+item_length*i+key);
                y=(unsigned char)*(arr+item_length*(i+1)+key);
                
                if(prev_key1!=EMPTY)
                {
                    xp1=(unsigned char)*(arr+item_length*i+prev_key1);
                    yp1=(unsigned char)*(arr+item_length*(i+1)+prev_key1);
                }
                
                if(prev_key2!=EMPTY)
                {
                    xp2=(unsigned char)*(arr+item_length*i+prev_key2);
                    yp2=(unsigned char)*(arr+item_length*(i+1)+prev_key2);
                }       
                
                if(
                (xp1==yp1 && prev_key1!=EMPTY || prev_key1==EMPTY)
                &&
                (xp2==yp2 && prev_key2!=EMPTY || prev_key2==EMPTY) 
                &&
                (x > y && low_first || x < y && !low_first))
                {
                    swap(arr+item_length*i,arr+item_length*(i+1),item_length);
                    sorted = 1;
                } 
        }  
        num++;
    } while (sorted); 
    
}


