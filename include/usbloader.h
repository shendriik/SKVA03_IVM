 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _USB_CONN_
#define _USB_CONN_

#include "porting.h"

/* Command size */
#define COMMAND_SIZE 6

/* On USB connected pin specify */
#define ON_USB_PIN       6
#define ON_USB_DDR       DDRD
#define ON_USB_PINPORT   PIND
#define ON_USB_INIT()       \
    {\                
        ON_USB_DDR &= ~_BV( ON_USB_PIN ); \ 
    }             


typedef enum
{
  NO,
  WRITE_CONFIG,  
  READ_CONFIG,

} USBCommand;
                        
    
    void vUSB_Conn_Init( );                     /* init usb connection */
    
    /* set event handlers callback  */
    void vUSB_Conn_SetWriteConfig_callback(void (*callback_handler)(USHORT,UCHAR));     
    void vUSB_Conn_SetReadConfig_callback(UCHAR (*callback_handler)(USHORT,eeprom UCHAR**));   

    /* states */                                                       
    UCHAR ucUSB_Conn_IsConnected();             /* check if usb cable is connected */ 
    UCHAR ucUSB_Conn_IsIdle();                  /* Check idle state */
    UCHAR ucUSB_Conn_SetIdle();                 /*Set idle flag */     
    UCHAR ucUSB_Conn_IsListening();             /* check if listen is running */
        
    /* commands */ 
    void vUSB_Conn_WaitForCommand();            /* starts waiting for command */
    USBCommand uUSB_Conn_GetCurrentCommand();   /* get current resolving command */
                   
    /* Transport layer */
    inline void vUSB_Conn_Listen();             /* start USART listening */    
    inline void vUSB_Conn_Stop();               /* stop USART listening */ 
    void vUSB_Conn_ISR(UCHAR data);             /* USART ISR callback */     
        
    
    /* helper functions */
    inline void putchar(UCHAR c);               /* send char via USART1 */

    
#endif