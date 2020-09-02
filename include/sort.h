 /*************************************************
 *    Sort funcs
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
 
#ifndef _SORT_
#define _SORT_

#include "porting.h"


#define MAX_BUFFER_SIZE     6
#define EMPTY               -1
#define ASC                 1   //sort like 0,1,2,3,4
#define DESC                0   //sort like 4,3,2,1,0

    /* Sort array by 3 key params. Key param is defined by offset */
    void bubble_sort(unsigned char *arr, 
                     char key,
                     char prev_key1,
                     char prev_key2,
                     char item_length,
                     char length,
                     char low_first); 
    
#endif