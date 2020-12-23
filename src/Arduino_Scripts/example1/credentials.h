#ifndef __CREDENTIALS_H
#define __CREDENTIALS_H

typedef int ll;
#include <SDS011.h>
#include <MiCS6814-I2C.h>
#include "DHT.h"

// WiFi Credentials

//Define the ssid and passwords


//--------Make changes here ----------------------------------------------------- 

#define MAIN_SSID "theorganization"
#define MAIN_PASS "elpsykongroo"

//-------------------------------------------------------------------------------

char CURRENT_SSID[] = MAIN_SSID;
char CURRENT_PASS[] = MAIN_PASS;


char ssid[] = MAIN_SSID;                      // your network SSID (name)
char pass[] = MAIN_PASS;                      // your network password
char WIFI_SSID[] = MAIN_SSID;                 // WiFi for ThingSpeak part
char WIFI_PSWD[] = MAIN_PASS;                 // WiFi for OneM2M part




//Thingspeak credentials
unsigned long myChannelNumber1 = 864235;         // DHT SENSOR
unsigned long myChannelNumber2 = 864604;         // MULTICHANNEL GAS SENSOR
unsigned long myChannelNumber3 = 864606;         // VOC AND CO2 GAS SENSOR
unsigned long myChannelNumber4 = 864607;         // NOVA PM SENSOR
unsigned long myChannelNumber1_error = 889198;   // ERROR FOR NODE 1
unsigned long myChannelNumber2_error = 889198;   // ERROR FOR NODE 1
unsigned long myChannelNumber3_error = 889198;   // ERROR FOR NODE 1
unsigned long myChannelNumber4_error = 889198;   // ERROR FOR NODE 1
const char *myWriteAPIKey1 = "1GG831U840UZ9GBW"; // DHT22
const char *myWriteAPIKey2 = "X9XZ4LAJY4PLQEAE"; // SGP30
const char *myWriteAPIKey3 = "L9GJIC50DXADAVN3"; // SDS011
const char *myWriteAPIKey4 = "RYIPO6HG7X8ZU3Y5"; // MiCS6814
const char *myWriteAPIKey_error1 = "7CW3LB57A1AT55UC"; //node_1
const char *myWriteAPIKey_error2 = "EWF0EVL573HCI4JS"; //node_2
const char *myWriteAPIKey_error3 = "MLCI9LXWTRBOJUMI"; //node_3
const char *myWriteAPIKey_error4 = "O5DNMDYI3QT1AL2Z"; //node_4


// ONE M2M CODE STARTS  /////////////////////////////////////////////
uint32_t delayMS;

// ##################### Update the Wifi SSID, Password and IP adress of the server ##########
// WIFI params
String CSE_IP = "http://onem2m.iiit.ac.in";
// #######################################################

int WIFI_DELAY = 100; //ms

// oneM2M : CSE params
int CSE_HTTP_PORT = 80;
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


//Global variables
float p10, p25;
int error;

SDS011 my_sds;
MiCS6814 sensor;
bool sensorConnected;

#define DHTPIN 2
int number = 0;

#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);


// DECLARING STRINGS
String string_humidity;
String string_temp;
String string_hif;
String string_tvoc;
String string_co2;
String string_CO = " ", string_NO2 = " ", string_NH3 = " ";
String string_p25;
String string_p10;





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


#endif