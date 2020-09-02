 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
      
#ifndef _ALGORITHM_
#define _ALGORITHM_

#include "avr/io.h"
#include "porting.h"
#include "config.h"


#define _3mAx100            350
#define _4mAx100            400
#define _20mAx100           2000

#define O2_P0_x10           190
#define O2_P1_x10           230

#define KVIT_OFF_TIME       15

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
    KVIT, 
    TEST 
    
} SensorState;

#define SENSOR_STATE_SIZE   6

/* Sensor data */
typedef struct
{
    UCHAR Id;
    USHORT Value_x10;
    SensorState State;  
    UCHAR Zone;

} SensorData;


        /* Init algorothms vars */                                              
        void vAlgorithm_Init();
        
        /* Converts mA values to SensorData  */ 
        UCHAR ucAlgorithm_GetSensors(USHORT *sensors_mA_x100,SensorData* sensors,eeprom SensorConfig* sensors_conf);                
        
        /* Get zones state using SensorData and zones configuration */
        UCHAR ucAlgorithm_GetZones(UCHAR *zones,SensorData* sensors, UCHAR check_button,UCHAR off_button);; 
        
        /* Get relais state using SensorData and relais configuration, return sound kvitation state  */
        void vAlgorithm_GetRelais(UCHAR *relais,SensorData* sensors,eeprom RelaisConfig* relais_conf,UCHAR check_button,UCHAR off_button);
        
        /* Create string for sensor to show on LCD */
        void vSensorToString(UCHAR* str,SensorData* sensor,eeprom UCHAR* text,eeprom UCHAR* dim,UCHAR length);
        
        /* return available sensors count */
        UCHAR ucGetSensorsCount();


#endif