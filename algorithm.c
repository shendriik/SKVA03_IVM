 /*************************************************
 *    Firmware for SKVA-03 IVM
 *    Shendrik Andrey
 *    andrey@findthis.ru 
 *    2016 (c)
 * 
 *************************************************/ 
 
/* ------------------------ System includes -----------------------------------*/ 
#include "string.h"

/* ----------------------- Platform includes ----------------------------------*/ 
#include "avr/io.h"

/* -------------------------- Own includes ------------------------------------*/
#include "algorithm.h"
#include "mymath.h"


typedef struct
{
    ULONG Warn1; 
    ULONG Warn2;
           
} RelaySensWarnings;


static UCHAR RelaisDelay[MAX_RELAIS] = {0};                     /* Relais delay counters */                            
static RelaySensWarnings RelaisKvitWarn[MAX_RELAIS] = {0};      /* Relay's sensors warning at the moment of the kvitation */  
static UCHAR RelaisKvitTime[MAX_RELAIS] = {0};                  /* Relay's kvitation time counter*/ 
static UCHAR ZoneKvitStates[MAX_ZONES]={0};                     /* Zones state at the moment of kvitation */
static UCHAR was_kvit=FALSE;                                    /* sound kvit flag */ 
static UCHAR kvit_timer=0;                                      /* sound kvit timer */
static UCHAR ucSensorsCount=0;                                  /* Available sensors count */


void vAlgorithm_Init()
{
    memset(RelaisDelay, 0x00, MAX_RELAIS);
    memset(RelaisKvitWarn, 0x00, MAX_RELAIS*8);   
    memset(RelaisKvitTime,0x00,MAX_RELAIS);
    memset(ZoneKvitStates,0x00,MAX_ZONES); 
    was_kvit=FALSE; 
    kvit_timer=0;  
    ucSensorsCount=0;
}

UCHAR ucAlgorithm_GetSensors(USHORT *sensors_mA_x100,SensorData* sensors,eeprom SensorConfig* sensors_conf)
{
    UCHAR i;
    UCHAR count=0;    

    for(i=0;i<MAX_SENSORS;i++)
    {                   
        /* if sensor is in config */    
        if(sensors_conf[i].Max_x10!=0)   /* if(sensors_conf[i].Unique!=0) */
        {   
            /* increment sensors count */
            count++; 
            
            sensors[i].Id=i;         
            sensors[i].State=NORMA;
            sensors[i].Zone=sensors_conf[i].Zone;

            /* if sensor unavailable ( <= 3.5 mA ) */
            if (sensors_mA_x100[i] <= _3mAx100)  
            {
                sensors[i].State=OTKAZ; 
                continue;
            }  
             
            /* calculate value_x10 */
            if (sensors_mA_x100[i] <= _4mAx100)  
            {                        
                sensors[i].Value_x10=0;
            }  
            else
            {
                sensors[i].Value_x10=((ULONG)(sensors_mA_x100[i]-_4mAx100) * sensors_conf[i].Max_x10)/(_20mAx100-_4mAx100); 
            }               
             
            /* if O2 sensor */
            if(sensors_conf[i].P0_x10==O2_P0_x10  && sensors_conf[i].P1_x10==O2_P1_x10 )
            {
                if(sensors[i].Value_x10 <= sensors_conf[i].P0_x10)
                {
                    sensors[i].State=POROG0_NH3;
                }                           
                else 
                if(sensors[i].Value_x10 >= sensors_conf[i].P1_x10)
                {
                    sensors[i].State=POROG1;
                }      
            }
            /* others */
            else
            {
                if(sensors_conf[i].P2_x10!=0 && sensors[i].Value_x10 >= sensors_conf[i].P2_x10)
                {
                    sensors[i].State=POROG2;
                }                           
                else 
                if(sensors_conf[i].P1_x10!=0 && sensors[i].Value_x10 >= sensors_conf[i].P1_x10)
                {
                    sensors[i].State=POROG1;
                }  
                else 
                if(sensors_conf[i].P0_x10!=0 && sensors[i].Value_x10 >= sensors_conf[i].P0_x10)
                {
                    sensors[i].State=POROG0_NH3;
                }  
            }      
        }
        else
        {   
            sensors[i].State=NONE;
        }
    }
    
    ucSensorsCount=count;
    return count;  
}


UCHAR ucAlgorithm_GetZones(UCHAR *zones,SensorData* sensors, UCHAR check_button,UCHAR off_button)
{
 
    UCHAR z,s=0;
    UCHAR max=0;  
    UCHAR has_higher=FALSE;
    UCHAR has_lower=FALSE;
    UCHAR warning;  
    UCHAR res=FALSE;  

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
        
        
        
        /* sound kvitation algorithm */
        if(zones[z-1]>OTKAZ && zones[z-1]<TEST) warning = zones[z-1]-OTKAZ;
        else
            warning=0;
            
        if(off_button)
        {
            ZoneKvitStates[z-1]=warning;
            was_kvit=TRUE;
            kvit_timer=0;   
        } 
        
        if(was_kvit)
        {
            if(warning>ZoneKvitStates[z-1])
            {
                has_higher=TRUE;
            } 
            if(warning<ZoneKvitStates[z-1])
            {
                has_lower=TRUE;
            }
        }
    }
    
    if(was_kvit)
    {          
        /* warning is higher */
        if(has_higher==TRUE)
        {
            was_kvit=FALSE; 
            res=FALSE;
        } 
        
        /* warning is lower */
        if(has_lower==TRUE && has_higher==FALSE)
        {
            if(kvit_timer>KVIT_OFF_TIME)
            {
                for(z=1;z<=MAX_ZONES;z++)
                {   
                    if(zones[z-1]>OTKAZ && zones[z-1]<TEST) warning = zones[z-1]-OTKAZ;
                    else
                        warning=0;  
                         
                    ZoneKvitStates[z-1]=warning;
                }
                kvit_timer=0;
            }
            else
                kvit_timer++; 
            
            res=TRUE;
        }
         
        /* same warning */
        if(has_lower==FALSE && has_higher==FALSE)
        {
            kvit_timer=0;
            res=TRUE;
        }
    }
    else
        res=FALSE; 
    
    return res;  
}   


void vAlgorithm_GetRelais(UCHAR *relais,SensorData* sensors,eeprom RelaisConfig* relais_conf,UCHAR check_button,UCHAR off_button)
{
    UCHAR i,j,k;
    ULONG mask1,mask2;
    UCHAR sens_minstate;    
    UCHAR sens_count; 
    ULONG sens_warn1,sens_warn2; 
    UCHAR warning,kvit_warning;   
    UCHAR num;  
    UCHAR equals;
                          
    /* reset  state */
    for(i=0;i<MAX_RELAIS/4;i++) relais[i]=0;
                   
    /* main algorithm */
    for(i=0;i<MAX_RELAIS;i++)
    { 
        mask1=relais_conf[i].SensorMask[0];
        mask2=relais_conf[i].SensorMask[1];
        sens_count=0;                    
        sens_warn1=0; 
        sens_warn2=0; 
        num=(i-((i/4)*4)); 

        for(j=0;j<MAX_SENSORS;j++)
        {   
            /* if we still have sensors in the mask */     
            if(mask1 || mask2)
            {   
                /* sensor[j] min state to turn relay[i] on */                                    
                sens_minstate = ((mask2 & 0x01)<< 1 | (mask1 & 0x01)) + OTKAZ;  
                
                if(sens_minstate>OTKAZ)
                {   
                    /* compare sensor state and minstate */
                    if(sensors[j].State>=sens_minstate) 
                    {   
                        sens_count++;  
                        warning=sensors[j].State-OTKAZ;
                        
                        /* sensors warnings for the relay (for kvit) */
                        sens_warn1|=((ULONG)(warning&0x01))<<j;
                        sens_warn2|=((ULONG)(warning>>1))<<j;       
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
                       

        /* Compare current sensors warnings and kvitation warnings */
        if(RelaisKvitWarn[i].Warn2>0 || RelaisKvitWarn[i].Warn1>0)
        {   
            equals=TRUE;     
            for(k=0;k<MAX_SENSORS;k++)
            {
                warning=(((sens_warn2>>k) & 0x01)<< 1 | ((sens_warn1>>k) & 0x01));    
                kvit_warning=(((RelaisKvitWarn[i].Warn2>>k) & 0x01)<< 1 | ((RelaisKvitWarn[i].Warn1>>k) & 0x01));  
                    
                /* disable kvit */
                if(warning>kvit_warning)
                {
                    RelaisKvitWarn[i].Warn1=0;
                    RelaisKvitWarn[i].Warn2=0;
                    break;    
                } 
                if(warning<kvit_warning) equals=FALSE;
            }   
        }
            
        /* if not kvitation */
        if(RelaisKvitWarn[i].Warn2==0 && RelaisKvitWarn[i].Warn1==0)
        {       
            if (sens_count!=0 && sens_count>=relais_conf[i].Union) 
            {
                /* if delay passed */
                if(RelaisDelay[i]>=relais_conf[i].Delay)
                {   
                    /* Enable kvitation if button is pressed  */ 
                    if(relais_conf[i].Type==SOUND && off_button)
                    { 
                        RelaisKvitWarn[i].Warn1=sens_warn1;
                        RelaisKvitWarn[i].Warn2=sens_warn2;  
                        RelaisKvitTime[i]=0;  
                        RelaisDelay[i]=0;
                    }
                    else
                    {             
                        relais[i/4]|= 1<<num; 
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
            if(equals) 
            {   
                RelaisKvitTime[i]=0;                 
            }  
            else
            {  
                if(++RelaisKvitTime[i]>KVIT_OFF_TIME)
                {    
                    if(sens_count>=relais_conf[i].Union)
                    {
                        RelaisKvitWarn[i].Warn1=sens_warn1;
                        RelaisKvitWarn[i].Warn2=sens_warn2; 
                    }     
                    else
                    {
                        RelaisKvitWarn[i].Warn1=0;
                        RelaisKvitWarn[i].Warn2=0;   
                    }
                    RelaisKvitTime[i]=0;
                } 
            }
        }
                       
        /* if check button is pressed  */  
        if((relais_conf[i].Type==LIGHT || relais_conf[i].Type==SOUND)  && check_button)
        {
            relais[i/4]|= 1<<num;
        }  
    }   
}



void vSensorToString(UCHAR* str,SensorData* sensor,eeprom UCHAR* text,eeprom UCHAR* dim,UCHAR length)
{
    UCHAR x;  
    divmod10_t value;
    UCHAR valbuf[12]; 
    UCHAR* start;
    UCHAR* end=valbuf+10;         

    memset(str,0x20,length);   
    if(sensor->State==NONE) return;
    
    str[0]=0x30+sensor->Zone;
    for(x=0;x<10;x++) str[2+x]= *(text+x); 
                   

    if(sensor->State==OTKAZ)
    {       
        str[12]='X';   
    }   
    else
    {   
        //state
        if(sensor->State==POROG0_NH3 || sensor->State==POROG1 || sensor->State==POROG2)
        {
            str[12]=0x30+sensor->State-2;   
        }  
        
        if(length==20)
        {
            //value
            value=divmodu10(sensor->Value_x10);
            x=17;   
            if(value.quot<10 && value.rem!=0)
            {
                if(value.rem!=0)
                {
                    str[x--]=0x30+value.rem; 
                    str[x--]=','; 
                }  
            } 
            start=utoa_fast_div(value.quot,valbuf);
            while(end>=start) str[x--] = *(end--); 

            //dim      
            if(*dim==0xB4)
            {
                str[18]= 0xBC;  
                str[19]= 0xB4;
            } 
            else   
            if(*dim=='o')
            {
                str[18]= 0x25;  
                str[19]= 'o';
            }  
            else   
            if(*dim=='H')
            {
                str[18]= 0x25;  
                str[19]= 'H';
            }
         
        }
    }
}


UCHAR ucGetSensorsCount()
{
    return ucSensorsCount; 
}


                            
                            
                            
                            
                            
                             
                            