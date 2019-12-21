#ifndef __POST_H
#define __POST_H

#include "credentials.h"
#include "error_handling.h"
#include "onem2m.h"
#include <JSONVar.h>
#include <JSON.h>
#include <Arduino_JSON.h>                                                         
#include "DHT.h"
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "sensirion_common.h"
#include "sgp30.h"
#include <SDS011.h>
#include <MiCS6814-I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <time.h>

void post_data()
{// ############################################################ //
  // SENDING DATA TO THINGSPEAK SERVER //
  // ############################################################## //
  // UPLOAD CODE FOR DHT
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  double ok;
  ok = error_handle(string_temp,DHT_TEMP_MAX,DHT_TEMP_DEFAULT,1); DHT_TEMP_DEFAULT = ok;string_temp = String(ok);
  ok = error_handle(string_humidity,DHT_HUMIDITY_MAX,DHT_HUMIDITY_DEFAULT,1); DHT_HUMIDITY_DEFAULT = ok; string_humidity = String(ok);
//  error_handle(hif,2,2);    // to check if pushed correctly
  ThingSpeak.setField(1, string_temp);
  ThingSpeak.setField(2, string_humidity);

//  // UPLOAD CODE FOR SGP30
//  if (WiFi.status() != WL_CONNECTED)
//  {
//    Serial.print("Attempting to connect to SSID: ");
//    Serial.println("adarsh-iot");
//    while (WiFi.status() != WL_CONNECTED)
//    {
//      WiFi.begin(ssid, pass);
//      Serial.print(".");
//      delay(5000);
//    }
//    Serial.println("\nConnected.");
//  }

  
  ok = error_handle(string_tvoc,SGP30_TVOC_MAX,SGP30_TVOC_DEFAULT,7); SGP30_TVOC_DEFAULT = ok;string_tvoc = String(ok);
  ok = error_handle(string_co2,SGP30_ECO2_MAX,SGP30_ECO2_DEFAULT,8); SGP30_ECO2_DEFAULT = ok; string_co2 = String(ok);
  ThingSpeak.setField(8, (string_tvoc));
  
  // UPLOAD SDS011
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println("adarsh-iot");
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  ok = error_handle(string_p25,SDS_PM25_MAX,SDS_PM25_DEFAULT,2); SDS_PM25_DEFAULT = (ok); string_p25 = String(ok);
  ok = error_handle(string_p10,SDS_PM10_MAX,SDS_PM10_DEFAULT,3); SDS_PM10_DEFAULT = (ok); string_p10 = String(ok);
  ThingSpeak.setField(3, string_p25);
  ThingSpeak.setField(4, string_p10);

  // UPLOAD CODE FOR MICS 6814
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  ok = error_handle(string_CO,MICS_CO_MAX,MICS_CO_DEFAULT,4); string_CO = String(ok);MICS_CO_DEFAULT = ok;
  ok = error_handle(string_NO2,MICS_NO2_MAX,MICS_NO2_DEFAULT,5); string_NO2 = String(ok); MICS_NO2_DEFAULT = ok;
  ok = error_handle(string_NH3,MICS_NH3_MAX,MICS_NH3_DEFAULT,6); string_NH3 = String(ok); MICS_NH3_DEFAULT = ok;
  ThingSpeak.setField(5, string_CO);
  ThingSpeak.setField(6, string_NO2);
  ThingSpeak.setField(7, string_NH3);

  int x = ThingSpeak.writeFields(myChannelNumber1, myWriteAPIKey1);
  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // ######################################################### //

  // ################################################################### //
  // ######### USE THIS SPACE FOR YOUR SENDING DATA TO ONEM2M SERVER  ######### //
  // ################################################################### //

  /*
      CreateCI(AE_NAME,CONTAINER_NAME,SENSOR_VALUE)

      CreateCI is what posts your sensor data into the container.

      In the below example:
            AE_NAME: Team8_Automated_Driving (As stated in the resource tree)
            CONTAINER_NAME : node_1 ( or as stated in the resource tree)
            SENSOR_VALUE : string of comma separated all sensor values (Eg: 27,25 is temp,hum)
  */

  // Storing as a string in a single containers
  String sensor_value_string;
  sensor_value_string = String(string_temp) + String(",") + String(string_humidity) + String(",");
  sensor_value_string += String(string_CO) + String(",") + String(string_NO2) + String(string_NH3) + String(",");
  sensor_value_string += String(string_p10) + String(",") + String(string_p25) + String(",");
  sensor_value_string += String(string_tvoc) + String(",") + String(string_co2);
  createCI("Team33_Indoor-air_pollution-3_Houses", "node_1", sensor_value_string);

}


#endif
