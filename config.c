 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
                   
/* ----------------------- System includes ------------------------------------*/  
#include "string.h" 
 
/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"

/* -------------------------- Own includes ------------------------------------*/
#include "config.h"
#include "mmcrc.h"




/* Current config*/
static __eeprom Config _config;



eCFGError GetConfig(eeprom Config* config)
{
    USHORT usCRC16; 
    
    /* TODO: check base config crc in eeprom */   
    usCRC16=usMMEepromCRC16((eeprom UCHAR*)&_config,CONFIG_SIZE-2);
    if(_config.crc!=usCRC16)
    {         
        /* there is no config in Eeprom */
        return CFG_ECRCFAIL;
    }    
   
    config=&_config; 
    
    /* return OK */
    return CFG_ENOERR;    
}


eCFGError WriteConfigByte(UCHAR data,USHORT offset)
{

    if(offset>=CONFIG_SIZE) return CFG_EWRONGARG;
    
    //write config byte to eeprom
     *((eeprom UCHAR*)&_config+offset)=data;

    /* return OK */
    return CFG_ENOERR;    
}

eCFGError ReadConfigByte(eeprom UCHAR** data,USHORT offset)
{

    if(offset>=CONFIG_SIZE) return CFG_EWRONGARG;
    
    //read config byte to eeprom
     *data=((eeprom UCHAR*)&_config+offset); 

   
    /* return OK */
    return CFG_ENOERR;    
}
