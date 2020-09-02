 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"

/* -------------------------- Own includes ------------------------------------*/
#include "data.h"



static UCHAR RelaisDelay[MAX_RELAIS] = {0};         /* Relais delay counters */                            
static UCHAR RelaisKvitState[MAX_RELAIS] = {0};     /* Relay's sensor states at the moment of the kvitation */  


void GetSensors(USHORT *sensors_mA_x100,SensorData* sensors,eeprom SensorConfig* sensors_conf)
{
    UCHAR i=0;
    for(i=0;i<MAX_SENSORS;i++)
    {   
        /* Reset sensor value */
        sensors[i].Value_x10 = 0;     
        
        /* Sensor zone */
        sensors[i].Zone=sensors_conf[i].Zone;
        /* Sensor id */
        sensors[i].Id=i;
         
        /* if sensor is in config, use it */
        if(sensors_conf[i].Max_x10!=0)
        {               
            /* if sensor unavailable ( <2.5 mA ) */
            if (sensors_mA_x100[i] <= _3mA*100)  
            {
                sensors[i].State=OTKAZ; 
                continue;
            }  
            if (sensors_mA_x100[i] <= _4mA*100)  
            {                        
                sensors[i].State=NORMA; 
                continue;
            }
            
            /* calculate value x10 for the sensor in its dimmension */
            sensors[i].Value_x10=( (ULONG)(sensors_mA_x100[i]-_4mA*100) * sensors_conf[i].Max_x10 )/((_20mA-_4mA)*100);
            
            /* calculate state */
            if( sensors[i].Value_x10 >= sensors_conf[i].P2_x10 )
            {
                sensors[i].State=POROG2;
            }                           
            else 
            if(sensors[i].Value_x10 >= sensors_conf[i].P1_x10)
            {
                sensors[i].State=POROG1;
            }  
            else 
            if(sensors[i].Value_x10 >= sensors_conf[i].P0_x10 && sensors_conf[i].P0_x10!=0)
            {
                sensors[i].State=POROG0_NH3;
            }   
            else
            {
                sensors[i].State=NORMA;
            }
        }
        else
        {   
            sensors[i].State=NONE;
        }
    }
    
}


void GetZones(UCHAR *zones,SensorData* sensors, UCHAR check_button)
{
 
    UCHAR z,s=0;
    UCHAR max=0;

    for(z=1;z<=MAX_ZONES;z++)
    {   
        if(!check_button)
        {        
            max=NONE;    
        }  
        else
        {   
            max=TEST;
        }
        
        for(s=0;s<MAX_SENSORS && !check_button;s++)
        {  
            if(sensors[s].Zone==z)
            { 
                if(sensors[s].State>max) max=sensors[s].State;
            }
        }
        zones[z-1]=max;
    }   
}   

                           




void GetRelais(UCHAR *relais,SensorData* sensors,eeprom RelaisConfig* relais_conf,UCHAR check_button,UCHAR off_button)
{

    UCHAR i,j=0;
    ULONG mask1=0;
    ULONG mask2=0;  
    UCHAR sens_minstate=0;    
    UCHAR sens_count=0; 
    UCHAR num=0;
    UCHAR sens_summstate=0;
                          
    /* reset  state */
    for(i=0;i<MAX_RELAIS/4;i++) relais[i]=0;
                   
    /* main algorithm */
    for(i=0;i<MAX_RELAIS;i++)
    { 
        mask1=relais_conf[i].SensorMask[0];
        mask2=relais_conf[i].SensorMask[1];
        sens_count=0;                    
        sens_summstate=0;
        num=(i-((i/4)*4)); 

        for(j=0;j<MAX_SENSORS;j++)
        {   
            /* if we have any sensors in this mask */     
            if(mask1 || mask2)
            {   
                /* current sensor min state to turn relay on */                                    
                sens_minstate = ((mask2 & 0x01)<< 1 | (mask1 & 0x01)) + OTKAZ;  
                
                if(sens_minstate>OTKAZ)
                {   
                    /* check sensor */
                    if(sensors[j].State>=sens_minstate) 
                    {
                        sens_count++;
                        sens_summstate+= sensors[j].State;        
                    }
                }    
            }else
            {
                break;  /* no more sensors for this mask */ 
            } 
        
            /* next sensor in mask */
            mask1 = mask1 >> 1;
            mask2 = mask2 >> 1;
        }     
                       
        /* set relay new state */ 
        if(sens_count>=relais_conf[i].Union) 
        {
            /* check if the state is higher then the state at the previous kvitation */
            if(sens_summstate>RelaisKvitState[i])
            {   
                RelaisKvitState[i]=0;       //kvitation disabled
                 
                /* if delay passed */
                if(RelaisDelay[i]>=relais_conf[i].Delay)
                {   
                    /* if sound off button is pressed  */ 
                    if(relais_conf[i].Type==SOUND && off_button)
                    { 
                        RelaisKvitState[i]=sens_summstate;  // kvitation enabled */ 
                        RelaisDelay[i]=0;
                    }
                    else
                    {             
                        relais[i/4]|= 1<<num;   // turn relay on    
                    }
                } 
                else
                {
                    RelaisDelay[i]++;   
                }   
            }  
            else
            {  
                RelaisDelay[i]=0;
            } 
        } 
        else
        {
            RelaisDelay[i]=0;
            RelaisKvitState[i]=0;
        }

        /* if check button is pressed  */  
        if(relais_conf[i].Type==LIGHT && check_button)
        {
            relais[i/4]|= 1<<num;
        }  
    }   
}





                            
                            
                            
                            
                            
                             
                            