#include "DHT.h"
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include "sensirion_common.h"
#include "sgp30.h"
#include <SDS011.h>
#include <MiCS6814-I2C.h>
#include <Wire.h>
//#include "MutichannelGasSensor.h"

float p10,p25;
int error;

SDS011 my_sds;
MiCS6814 sensor;
bool sensorConnected;

#define SECRET_SSID "iPhone"    // replace MySSID with your WiFi network name
#define SECRET_PASS "hello1234abc"  // replace MyPassword with your WiFi password


//----------------  Fill in your credentails   ---------------------
char ssid[] = SECRET_SSID;             // your network SSID (name)
char pass[] = SECRET_PASS;         // your network password
unsigned long myChannelNumber1 = 864235;  // Replace the 0 with your channel number
unsigned long myChannelNumber2 = 864604;  // Replace the 0 with your channel number
unsigned long myChannelNumber3 = 864606;  // Replace the 0 with your channel number
unsigned long myChannelNumber4 = 864607;  // Replace the 0 with your channel number
const char * myWriteAPIKey1 = "1GG831U840UZ9GBW";    // DHT22
const char * myWriteAPIKey2 = "X9XZ4LAJY4PLQEAE";    // SGP30
const char * myWriteAPIKey3 = "L9GJIC50DXADAVN3";    // SDS011
const char * myWriteAPIKey4 = "RYIPO6HG7X8ZU3Y5";    // MiCS6814

//------------------------------------------------------------------
#define DHTPIN 2

WiFiClient  client;
int number = 0;

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

void setup() {
//  Serial.begin(9600);

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
  while (sgp_probe() != STATUS_OK) {
    Serial.println("SGP failed");
    while (1);
  }
  err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                          &scaled_h2_signal);
  if (err == STATUS_OK) {
    Serial.println("get ram signal!");
  } else {
    Serial.println("error reading signals");
  }
  err = sgp_iaq_init();

  //     TESTING THE SENSOR DHT
  Serial.println(F("DHTxx test!"));
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  dht.begin();

  //     SDS011
  my_sds.begin(D5,D6);

  //    MICS6814

   sensorConnected = sensor.begin();

  if (sensorConnected == true) {
    // Print status message
    Serial.println("Connected to MiCS-6814 sensor");

    // Turn heater element on
    sensor.powerOn();
    
    // Print header for live values
    Serial.println("Current concentrations:");
    Serial.println("CO\tNO2\tNH3\tC3H8\tC4H10\tCH4\tH2\tC2H5OH");
  } else {
    // Print error message on failed connection
    Serial.println("Couldn't connect to MiCS-6814 sensor");
  }
}

void loop() {


  // DHT CODE

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));



  // UPLOAD CODE FOR DHT
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println("adarsh-iot");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  ThingSpeak.setField(1, String(t));
  ThingSpeak.setField(2, String(h));
  ThingSpeak.setField(3, String(hif));

  int x = ThingSpeak.writeFields(myChannelNumber1, myWriteAPIKey1);
  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }


  // SGP CODE
  s16 err = 0;
  u16 tvoc_ppb, co2_eq_ppm;
  err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
  if (err == STATUS_OK) {
    Serial.print("tVOC  Concentration:");
    Serial.print(tvoc_ppb);
    Serial.println("ppb");

    Serial.print("CO2eq Concentration:");
    Serial.print(co2_eq_ppm);
    Serial.println("ppm");
  } else {
    Serial.println("error reading IAQ values\n");
  }

  // UPLOAD CODE FOR SGP30
   if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println("adarsh-iot");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  ThingSpeak.setField(1, String(tvoc_ppb));
  ThingSpeak.setField(2, String(co2_eq_ppm));

  x = ThingSpeak.writeFields(myChannelNumber2, myWriteAPIKey2);
  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // SDS011
  error = my_sds.read(&p25,&p10);
  if (! error) {
    Serial.println("P2.5: "+String(p25));
    Serial.println("P10:  "+String(p10));
  }
  // UPLOAD SDS011
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println("adarsh-iot");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  ThingSpeak.setField(1, String(p25));
  ThingSpeak.setField(2, String(p10));

   x = ThingSpeak.writeFields(myChannelNumber3, myWriteAPIKey3);
  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // MiCS 6814
   if (sensorConnected) {
    // Print live values
    Serial.print(sensor.measureCO());
    Serial.print("\t");
    Serial.print(sensor.measureNO2());
    Serial.print("\t");
    Serial.print(sensor.measureNH3());
    Serial.print("\t");
    Serial.print(sensor.measureC3H8());
    Serial.print("\t");
    Serial.print(sensor.measureC4H10());
    Serial.print("\t");
    Serial.print(sensor.measureCH4());
    Serial.print("\t");
    Serial.print(sensor.measureH2());
    Serial.print("\t");
    Serial.println(sensor.measureC2H5OH());
  }

  // UPLOAD CODE FOR MICS 6814

 
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println("adarsh-iot");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  ThingSpeak.setField(1, String(sensor.measureCO()));
  ThingSpeak.setField(2, String(sensor.measureNO2()));
  ThingSpeak.setField(3, String(sensor.measureNH3() ));

  x = ThingSpeak.writeFields(myChannelNumber4, myWriteAPIKey4);
  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }



  delay(20000); // Wait 20 seconds before sending a new value

}
