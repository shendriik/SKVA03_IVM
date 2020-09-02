 /*************************************************
 *    Math operations
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 

#include "mymath.h"



inline divmod10_t divmodu10(unsigned long n)
{
    divmod10_t res;
    unsigned long qq;
    
    //  x0.8
    res.quot = n >> 1;
    res.quot += res.quot >> 1;
    res.quot += res.quot >> 4;
    res.quot += res.quot >> 8;
    res.quot += res.quot >> 16;
    qq = res.quot;
    //  /8
    res.quot >>= 3;
    // ostatok
    res.rem = (unsigned char)(n - ((res.quot << 1) + (qq & ~7ul)));
    // correct
    if(res.rem > 9)
    {
        res.rem -= 10;
        res.quot++;
    }
    return res;
}

unsigned char * utoa_fast_div(unsigned long value, unsigned char *buffer)
{
    divmod10_t res;
    buffer += 11;
    *buffer = 0; 

    do
    {
        res = divmodu10(value);
        *--buffer = res.rem + '0';
        value = res.quot;
    }
    while (value != 0);
    return buffer;
}


unsigned char CRC_Xor(unsigned char* array,unsigned short size)
{
    unsigned short i;
    unsigned char res=0;
    for(i=0;i<size;i++)
    {
        res^= (unsigned char)array[i];   
    }
    
    return res;
}


