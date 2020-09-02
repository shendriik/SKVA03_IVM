 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _CONFIG_
#define _CONFIG_

#include "avr/io.h"
#include "porting.h"
#include "calendar.h"


/* Max sensors,zones and relais count */
#define MAX_SENSORS             32
#define MAX_ZONES               8
#define MAX_RELAIS              16

#define BOARDS_COUNT            4    
#define SENSORS_IN_BOARD        8  
#define RELAIS_IN_BOARD         4    

#define BOARDS_COUNT_2n         2    
#define SENSORS_IN_BOARD_2n     3  
#define RELAIS_IN_BOARD_2n      2 



/* sensor text string length */
#define TEXT_LENGTH     10

/* config size */
#define CONFIG_SIZE     880





/* Relay types */
typedef enum
{
    EQUIPMENT=0,
    LIGHT,
    SOUND    
} RelayType;


/* Sensor configuration */
typedef struct
{
    UCHAR  Unique;
    USHORT Max_x10;   
    USHORT P0_x10;
    USHORT P1_x10;
    USHORT P2_x10; 
    UCHAR  Dim;
    UCHAR  Zone;
    UCHAR  Text[TEXT_LENGTH];
} SensorConfig;



/* Relay configuration */
typedef struct
{
    UCHAR Delay;   
    RelayType Type;
    UCHAR Union; 
    ULONG SensorMask[2];  /* mask thresholds of the sensors for each relais */ 
} RelaisConfig;


/* All configuration */
typedef struct
{
    UCHAR MR_Boards[BOARDS_COUNT];
    UCHAR Relay_Boards[BOARDS_COUNT];
    SensorConfig _Sensors[MAX_SENSORS]; 
    RelaisConfig _Relais[MAX_RELAIS]; 
    DateTime _dateTime;
    USHORT crc;  
} Config;



/* typical errors for operations with config */
typedef enum
{
    CFG_ENOERR=0,
    CFG_ECRCFAIL,
    CFG_EWRONGARG
} eCFGError;


        /* Getting pointer to eeprom config */
        eCFGError GetConfig(eeprom Config* config);
                            
        /* Writing config byte to eeprom */
        eCFGError WriteConfigByte(UCHAR data,USHORT offset);   
                         
        /* Reading config byte from eeprom */
        eCFGError ReadConfigByte(eeprom UCHAR** data,USHORT offset);

#endif