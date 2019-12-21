#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H

#include "credentials.h"
#include <SoftwareSerial.h>
#include "ThingSpeak.h"

void init_error()
{
  DHT_TEMP_DEFAULT = 28.111;
  DHT_TEMP_CURR = DHT_TEMP_DEFAULT;
  DHT_HUMIDITY_DEFAULT = 78.222;
  DHT_HUMIDITY_CURR = DHT_HUMIDITY_DEFAULT;
  DHT_TEMP_MAX = 40.555;
  DHT_HUMIDITY_MAX = 100;

  SDS_PM25_DEFAULT = 25;
  SDS_PM25_CURR = SDS_PM25_DEFAULT;
  SDS_PM25_MAX = 1000;
  SDS_PM10_DEFAULT = 30;
  SDS_PM10_CURR = SDS_PM10_DEFAULT;
  SDS_PM10_MAX = 1000;

  SGP30_TVOC_DEFAULT = 10;
  SGP30_TVOC_CURR = SGP30_TVOC_DEFAULT;
  SGP30_TVOC_MAX = 1000;
  SGP30_ECO2_DEFAULT = 415;
  SGP30_ECO2_CURR = SGP30_ECO2_DEFAULT;
  SGP30_ECO2_MAX = 10000;

  MICS_CO_DEFAULT = 30.66;
  MICS_CO_CURR = MICS_CO_DEFAULT;
  MICS_CO_MAX = 40;
  MICS_NH3_DEFAULT = 0.06;
  MICS_NH3_CURR = MICS_NH3_DEFAULT;
  MICS_NH3_MAX = 110.9;
  MICS_NO2_DEFAULT = 16.66;
  MICS_NO2_CURR = MICS_NO2_DEFAULT;
  MICS_NO2_MAX = 100;
}




double error_handle(String a,double maximum,double default_value,int num)
{
    double value = a.toFloat();

      Serial.print("Got a value at \t");
      Serial.print(num);
      Serial.print(":\t:value:");
      Serial.println(value);
    if(value > maximum || value < 0)
    {
      Serial.print("Got faulty value at \t");
      Serial.print(num);
      Serial.print(":\t:value:");
      Serial.println(value);
      value = default_value;
      ThingSpeak.setField(num,String(value));
      for (ll i = 1; i <= 8; i++)
      {
        if(i != num)
        {
          ThingSpeak.setField(i,String(0));
        }
      }
      
      ThingSpeak.writeFields(myChannelNumber1_error,myWriteAPIKey_error1);
    }
    return value;
}



#endif