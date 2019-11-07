////////////////////////////////////////////////////////////////////////////
///////////////////      ! ERROR CORRECTED CODE     ////////////////////////
////////////////////////////////////////////////////////////////////////////

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
typedef int ll;

// T START OF THINGSPEAK CODE ////////////////////////////////////////////////////////////////////////

float p10, p25;
int error;

SDS011 my_sds;
MiCS6814 sensor;
bool sensorConnected;

#define SECRET_SSID "iPhone"                // BJS
#define SECRET_PASS "hello1234abc"
#define SECRET_SSID2 "LAPTOP-A7EFOJ34 9462" // replace MySSID with your WiFi network name // JAIBA
#define SECRET_PASS2 "Aq57]679"              // replace MyPassword with your WiFi password
#define SECRET_SSID3 "hotspot"
#define SECRET_PASS3 "plsconnect"
#define SECRET_SSID4 "VIKRANT_LAPTOP"
#define SECRET_PASS4 "62y978{V"
#define SECRET_SSID5 "bat"
#define SECRET_PASS5 "nanananabatman"
#define SECRET_SSID6 "kvaditya-Inspiron-7520"
#define SECRET_PASS6 "Us555zCI"
#define SECRET_SSID7 "moto g"
#define SECRET_PASS7 "justmonika"
#define SECRET_SSID8 "yoogottam"
#define SECRET_PASS8 "plis_/\\_plis"

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////////   MAIN WIFI DETAILS OF DEPLOYMENT SITE /////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

#define MAIN_SSID "MISHUKU"
#define MAIN_PASS "setagaya"

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

char CURRENT_SSID[] = MAIN_SSID;//SECRET_SSID7;
char CURRENT_PASS[] = MAIN_PASS;

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//                                    ERROR VARIABLES                                     //

double DHT_TEMP_MAX, DHT_HUMIDITY_MAX, DHT_HUMIDITY_DEFAULT, DHT_TEMP_DEFAULT, DHT_TEMP_CURR, DHT_HUMIDITY_CURR;
double SDS_PM25_MAX, SDS_PM10_MAX, SDS_PM25_DEFAULT, SDS_PM10_DEFAULT, SDS_PM25_CURR, SDS_PM10_CURR;
double SGP30_ECO2_MAX, SGP30_TVOC_MAX, SGP30_ECO2_DEFAULT, SGP30_TVOC_DEFAULT, SGP30_ECO2_CURR, SGP30_TVOC_CURR;
double MICS_CO_MAX, MICS_NH3_MAX, MICS_NO2_MAX, MICS_CO_DEFAULT, MICS_NH3_DEFAULT, MICS_NO2_DEFAULT, MICS_CO_CURR, MICS_NH3_CURR, MICS_NO2_CURR;

//                                    END OF ERROR VARIABLES                              //
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


//----------------  Fill in your credentails   ---------------------
char ssid[] = MAIN_SSID;                      // your network SSID (name)
char pass[] = MAIN_PASS;                      // your network password
char WIFI_SSID[] = MAIN_SSID;                 // WiFi for ThingSpeak part
char WIFI_PSWD[] = MAIN_PASS;                 // WiFi for OneM2M part
unsigned long myChannelNumber1 = 900845;         // DHT SENSOR
unsigned long myChannelNumber2 = 900846;         // NOVA PM SENSOR
unsigned long myChannelNumber3 = 900848;         // VOC AND CO2 GAS SENSOR
unsigned long myChannelNumber4 = 900849;         // MULTICHANNEL GAS SENSOR
unsigned long myChannelNumber1_error = 889198;   // ERROR FOR NODE 1
unsigned long myChannelNumber2_error = 889198;   // ERROR FOR NODE 1
unsigned long myChannelNumber3_error = 889198;   // ERROR FOR NODE 1
unsigned long myChannelNumber4_error = 889198;   // ERROR FOR NODE 1
const char *myWriteAPIKey1 = "MJ7O2FZ34Q8138F5"; // DHT22
const char *myWriteAPIKey2 = "DGSJGDKEZBMKBSCW"; // SDS011
const char *myWriteAPIKey3 = "HF5G68OFZMZC81XB"; // SGP30
const char *myWriteAPIKey4 = "2O2P90MY64BMVXYI"; // MiCS6814
const char *myWriteAPIKey_error1 = "7CW3LB57A1AT55UC"; //node_1
const char *myWriteAPIKey_error2 = "EWF0EVL573HCI4JS"; //node_2
const char *myWriteAPIKey_error3 = "MLCI9LXWTRBOJUMI"; //node_3
const char *myWriteAPIKey_error4 = "O5DNMDYI3QT1AL2Z"; //node_4

//------------------------------------------------------------------
#define DHTPIN 2

int number = 0;

#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
// T OUR  ENDS    //////////////////////////////////////////////////////////////////////////////////

// ONE M2M CODE STARTS  /////////////////////////////////////////////
uint32_t delayMS;

// ##################### Update the Wifi SSID, Password and IP adress of the server ##########
// WIFI params
String CSE_IP = "139.59.42.21";
// #######################################################

int WIFI_DELAY = 100; //ms

// oneM2M : CSE params
int CSE_HTTP_PORT = 8080;
String CSE_NAME = "in-name";
String CSE_M2M_ORIGIN = "admin:admin";

// oneM2M : resources' params
String DESC_CNT_NAME = "DESCRIPTOR";
String DATA_CNT_NAME = "DATA";
String CMND_CNT_NAME = "COMMAND";
int TY_AE = 2;
int TY_CNT = 3;
int TY_CI = 4;
int TY_SUB = 23;

// HTTP constants
int LOCAL_PORT = 9999;
char *HTTP_CREATED = "HTTP/1.1 201 Created";
char *HTTP_OK = "HTTP/1.1 200 OK\r\n";
int REQUEST_TIME_OUT = 5000; //ms

//MISC
int LED_PIN = D1;
int SERIAL_SPEED = 9600;

#define DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////////
// ERROR HANDLING FUNCTIONS ///////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////

// Global variables
WiFiServer server(LOCAL_PORT); // HTTP Server (over WiFi). Binded to listen on LOCAL_PORT contant
WiFiClient client;
String context = "";
String command = ""; // The received command

// Method for creating an HTTP POST with preconfigured oneM2M headers
// param : url  --> the url path of the targted oneM2M resource on the remote CSE
// param : ty --> content-type being sent over this POST request (2 for ae, 3 for cnt, etc.)
// param : rep  --> the representaton of the resource in JSON format
String doPOST(String url, int ty, String rep)
{

  String postRequest = String() + "POST " + url + " HTTP/1.1\r\n" +
                       "Host: " + CSE_IP + ":" + CSE_HTTP_PORT + "\r\n" +
                       "X-M2M-Origin: " + CSE_M2M_ORIGIN + "\r\n" +
                       "Content-Type: application/json;ty=" + ty + "\r\n" +
                       "Content-Length: " + rep.length() + "\r\n"
                                                           "Connection: close\r\n\n" +
                       rep;

  // Connect to the CSE address

  Serial.println("connecting to " + CSE_IP + ":" + CSE_HTTP_PORT + " ...");

  // Get a client
  WiFiClient client2;
  if (!client2.connect("139.59.42.21", CSE_HTTP_PORT))
  {
    Serial.println("Connection failed !");
    return "error";
  }

  // if connection succeeds, we show the request to be send
#ifdef DEBUG
  Serial.println(postRequest);
#endif

  // Send the HTTP POST request
  client2.print(postRequest);

  // Manage a timeout
  unsigned long startTime = millis();
  while (client2.available() == 0)
  {
    if (millis() - startTime > REQUEST_TIME_OUT)
    {
      Serial.println("Client Timeout");
      client2.stop();
      return "error";
    }
  }

  // If success, Read the HTTP response
  String result = "";
  if (client2.available())
  {
    result = client2.readStringUntil('\r');
    //    Serial.println(result);
  }
  while (client2.available())
  {
    String line = client2.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing connection...");
  return result;
}

// Method for creating an ContentInstance(CI) resource on the remote CSE under a specific CNT (this is done by sending a POST request)
// param : ae --> the targted AE name (should be unique under the remote CSE)
// param : cnt  --> the targeted CNT name (should be unique under this AE)
// param : ciContent --> the CI content (not the name, we don't give a name for ContentInstances)
String createCI(String ae, String cnt, String ciContent)
{
  String ciRepresentation =
      "{\"m2m:cin\": {"
      "\"con\":\"" +
      ciContent + "\""
                  "}}";
  return doPOST("/" + CSE_NAME + "/" + ae + "/" + cnt, TY_CI, ciRepresentation);
}

void setup()
{
  // intialize the serial liaison
  init_error();

  // Connect to WiFi network
  //    init_WiFi();

  // Start HTTP server
//  init_HTTPServer();

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
  unsigned long int start = millis();
  while (sgp_probe() != STATUS_OK && (millis() - start < REQUEST_TIME_OUT))
  {
    Serial.println("SGP failed");
    while (1 && (millis() - start < REQUEST_TIME_OUT))
      ;
  }
  err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                          &scaled_h2_signal);
  if (err == STATUS_OK)
  {
    Serial.println("get ram signal!");
  }
  else
  {
    Serial.println("error reading signals");
  }
  err = sgp_iaq_init();

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

  // ###################################################### //
}

// Main loop of the µController
void loop()
{
  // ############################################################### //
  // ######### USE THIS SPACE FOR YOUR SENSOR POLING CODE  ######### //
  // ###############################################################  //

  // DECLARING STRINGS
  String string_humidity;
  String string_temp;
  String string_tvoc;
  String string_co2;
  String string_CO = " ", string_NO2 = " ", string_NH3 = " ";
  String string_p25;
  String string_p10;

  //DHT polling CODE
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  string_humidity = String(h);
  string_temp = String(t);

  if (isnan(h) || isnan(t) || isnan(f))
  {
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

  // SGP CODE
  s16 err = 0;
  u16 tvoc_ppb, co2_eq_ppm;
  err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
  if (err == STATUS_OK)
  {
    Serial.print("tVOC  Concentration:");
    Serial.print(tvoc_ppb);
    string_tvoc = String(tvoc_ppb);
    Serial.println("ppb");

    Serial.print("CO2eq Concentration:");
    string_co2 = String(co2_eq_ppm);
    Serial.print(co2_eq_ppm);
    Serial.println("ppm");
  }
  else
  {
    Serial.println("error reading IAQ values\n");
  }

  // SDS011
  error = my_sds.read(&p25, &p10);
  if (!error)
  {
    string_p25 = String(p25);
    Serial.println("P2.5: " + String(p25));
    string_p10 = String(p10);
    Serial.println("P10:  " + String(p10));
  }

  // MiCS 6814
  if (sensorConnected)
  {
    string_CO = sensor.measureCO();
    string_NO2 = sensor.measureNO2();
    string_NH3 = sensor.measureNH3();

    // Print live values
    Serial.print(string_CO);
    Serial.print("\t");
    Serial.print(string_NO2);
    Serial.print("\t");
    Serial.print(string_NH3);
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

  // ############################################################ //
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
  ThingSpeak.setField(3, String(hif));

  int x = ThingSpeak.writeFields(myChannelNumber1, myWriteAPIKey1);
  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // UPLOAD CODE FOR SGP30
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

  
  ok = error_handle(string_tvoc,SGP30_TVOC_MAX,SGP30_TVOC_DEFAULT,7); SGP30_TVOC_DEFAULT = ok;string_tvoc = String(ok);
  ok = error_handle(string_co2,SGP30_ECO2_MAX,SGP30_ECO2_DEFAULT,8); SGP30_ECO2_DEFAULT = ok; string_co2 = String(ok);
  ThingSpeak.setField(1, (string_tvoc));
  ThingSpeak.setField(2, (string_co2));

  x = ThingSpeak.writeFields(myChannelNumber3, myWriteAPIKey2);
  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

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
  ThingSpeak.setField(1, string_p25);
  ThingSpeak.setField(2, string_p10);

  x = ThingSpeak.writeFields(myChannelNumber2, myWriteAPIKey3);
  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

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
  ThingSpeak.setField(1, string_CO);
  ThingSpeak.setField(2, string_NO2);
  ThingSpeak.setField(3, string_NH3);

  x = ThingSpeak.writeFields(myChannelNumber4, myWriteAPIKey4);
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

  // Check if the data instance was created.
  delay(15000); // DO NOT CHANGE THIS VALUE

  // ################################################################### //
}
