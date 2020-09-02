/*****************************************************************************
*
* Modbus RTU Master lib
* Andrey Shendrik andrey@findthis.ru (c) 2016
*
*****************************************************************************/

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"
#include "porting.h"


/* ------------------------- Modbus includes ----------------------------------*/
#include "mm.h"
#include "mmfunc.h"

USHORT regs[12];


void main(void)
{   
    
    eMMException ex;
    
    /* Initialize the ModbusRTU Master protocol stack, if error wait for WD to reset MCU */
    if(!(eMMInit(19200,MM_PAR_EVEN)==MM_ENOERR && eMMEnable()==MM_ENOERR))
    { 
       while(1); 
    }  

    while(1)
    {   
	    
	    //Requesting dev 0x01 for 12 holding registers from register 100
            ex = eMMReadHoldingRegister(0x01,100,12,regs);     
                            
            if(ex == MM_EX_NONE)
            {                   
                //successful
            } 
            else
            {
                //Error code = ex
            }
     
            //Writing 2 holding registers
            ex = eMMWriteHoldingRegister(0x01,100,0x00A8);     
            ex = eMMWriteHoldingRegister(0x01,101,0xF3A8);     

            
            regs[0]=0x0001; 
            regs[1]=0x0002;
            regs[2]=0x0003;
            regs[3]=0x0004;
            regs[4]=0x0005;
            regs[5]=0x0006;
            regs[6]=0x0007;
            regs[7]=0x0008;
            regs[8]=0x0009;
            regs[9]=0x000A;
              
	    //Writing 10 holding registers
            ex = eMMFuncWriteMultipleHoldingRegister( 0x01 , 102 , 10 , regs );                     

    }     

}

