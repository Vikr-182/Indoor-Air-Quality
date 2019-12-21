
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
#include "credentials.h"
#include "onem2m.h"
#include "error_handling.h"
#include "poll_sensors.h"
#include "post.h"


void setup()
{
  // intialize the serial liaison
  init_error();


  // ######### USE THIS SPACE FOR YOUR SETUP CODE ######### //

  //    SGP30
  s16 err;
  u16 scaled_ethanol_signal, scaled_h2_signal;
  Serial.begin(115200);
  Serial.println("serial start!!");

#if defined(ESP8266)
  pinMode(15, OUTPUT);
  digitalWrite(15, 1);
  Serial.println("Set wio link power!");
  delay(500);
#endif
//  unsigned long int start = millis();
//  while (sgp_probe() != STATUS_OK && (millis() - start < REQUEST_TIME_OUT))
//  {
//    Serial.println("SGP failed");
//    while (1 && (millis() - start < REQUEST_TIME_OUT))
//      ;
//  }
//  err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
//                                          &scaled_h2_signal);
//  if (err == STATUS_OK)
//  {
//    Serial.println("get ram signal!");
//  }
//  else
//  {
//    Serial.println("error reading signals");
//  }
//  err = sgp_iaq_init();
//
  //     TESTING THE SENSOR DHT
  Serial.println(F("DHTxx test!"));
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  dht.begin();

  //     SDS011
  my_sds.begin(D5, D6);

  //    MICS6814

  sensorConnected = sensor.begin();

  if (sensorConnected == true)
  {
    // Print status message
    Serial.println("Connected to MiCS-6814 sensor");

    // Turn heater element on
    sensor.powerOn();

    // Print header for live values
    Serial.println("Current concentrations:");
    Serial.println("CO\tNO2\tNH3\tC3H8\tC4H10\tCH4\tH2\tC2H5OH");
  }
  else
  {
    // Print error message on failed connection
    Serial.println("Couldn't connect to MiCS-6814 sensor");
  }
}

void loop()
{
  IPAddress ip(192, 168, 1, 8);
  IPAddress gw(192, 168, 1, 1);
  IPAddress sn(255, 255, 255, 0);
  WiFi.config(ip, gw, sn, gw);
    poll_sensors();
    post_data();
    delay(15000);
}
