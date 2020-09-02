 /*************************************************
 *    Math operations
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
 
#ifndef _MYMATH_
#define _MYMATH_
        
 
typedef struct
{
    unsigned long quot;
    unsigned char rem; 
    
} divmod10_t;

                                                                                          
        /* This functions available on                                                           */
        /* from http://we.easyelectronics.ru/Soft/preobrazuem-v-stroku-chast-1-celye-chisla.html */
                                                                                             
        
        /* fast div10 */
        inline divmod10_t divmodu10(unsigned long n);
        
        /* uint32 -> string */
        unsigned char* utoa_fast_div(unsigned long value, unsigned char *buffer);
        
        /* calculate xor checksumm */
        unsigned char CRC_Xor(unsigned char* array,unsigned short size);
        
#endif