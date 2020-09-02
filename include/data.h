 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _DATA_MY_
#define _DATA_MY_

#include "avr/io.h"
#include "porting.h"
#include "config.h"


#define _3mA            2.5
#define _4mA            4
#define _20mA           20

/* Fields offsets in SensorData type */ 
#define BY_ID       0
#define BY_STATE    3
#define BY_ZONE     5
                       


/* State of the Sensor */
typedef enum
{
    NONE=0,  
    NORMA,
    OTKAZ,  
    POROG0_NH3, 
    POROG1,
    POROG2, 
    TEST 
    
} SensorState;


/* Sensor data */
typedef struct
{
    UCHAR Id;
    USHORT Value_x10;
    SensorState State; 
    UCHAR Zone;

} SensorData;


        /* function prototypes*/         
        
        /* Converts mA values to SensorData  */ 
        void GetSensors(USHORT *sensors_mA_x100,SensorData* sensors,eeprom SensorConfig* sensors_conf);                
        
        /* Get zones state using SensorData and zones configuration */
        void GetZones(UCHAR *zones,SensorData* sensors,UCHAR check_button);  
        
        /* Get relais state using SensorData and relais configuration */
        void GetRelais(UCHAR *relais,SensorData* sensors,eeprom RelaisConfig* relais_conf,UCHAR check_button,UCHAR off_button);
        
        




#endif