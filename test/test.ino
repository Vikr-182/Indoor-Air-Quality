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

// T START OF THINGSPEAK CODE ////////////////////////////////////////////////////////////////////////

float p10, p25;
int error;

SDS011 my_sds;
MiCS6814 sensor;
bool sensorConnected;

#define SECRET_SSID "iPhone"                    // BJS
#define SECRET_PASS "hello1234abc"
#define SECRET_SSID2 "LAPTOP-A7EFOJ34 9462"     // replace MySSID with your WiFi network name // JAIBA
#define SECRET_PASS "Aq57]679" // replace MyPassword with your WiFi password
#define SECRET_SSID3 "hotspot"
#define SECRET_PASS3 "plsconnect"

//----------------  Fill in your credentails   ---------------------
char ssid[] = SECRET_SSID3;                       // your network SSID (name)
char pass[] = SECRET_PASS3;                       // your network password
unsigned long myChannelNumber1 = 864235;         // DHT SENSOR
unsigned long myChannelNumber2 = 864604;         // MULTICHANNEL GAS SENSOR
unsigned long myChannelNumber3 = 864606;         // VOC AND CO2 GAS SENSOR
unsigned long myChannelNumber4 = 864607;         // 
const char *myWriteAPIKey1 = "1GG831U840UZ9GBW"; // DHT22
const char *myWriteAPIKey2 = "X9XZ4LAJY4PLQEAE"; // SGP30
const char *myWriteAPIKey3 = "L9GJIC50DXADAVN3"; // SDS011
const char *myWriteAPIKey4 = "RYIPO6HG7X8ZU3Y5"; // MiCS6814

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
char *WIFI_SSID = "hotspot";
char *WIFI_PSWD = "plsconnect";

String CSE_IP = "127.0.0.1";
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
String doPOST(String url, int ty, String rep) {

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
  WiFiClient client;
  if (!client.connect(CSE_IP, CSE_HTTP_PORT)) {
    Serial.println("Connection failed !");
    return "error";
  }

  // if connection succeeds, we show the request to be send
#ifdef DEBUG
  Serial.println(postRequest);
#endif

  // Send the HTTP POST request
  client.print(postRequest);

  // Manage a timeout
  unsigned long startTime = millis();
  while (client.available() == 0) {
    if (millis() - startTime > REQUEST_TIME_OUT) {
      Serial.println("Client Timeout");
      client.stop();
      return "error";
    }
  }

  // If success, Read the HTTP response
  String result = "";
  if (client.available()) {
    result = client.readStringUntil('\r');
    //    Serial.println(result);
  }
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  Serial.println();
  Serial.println("closing connection...");
  return result;
}

// Method for creating an ApplicationEntity(AE) resource on the remote CSE (this is done by sending a POST request)
// param : ae --> the AE name (should be unique under the remote CSE)
String createAE(String ae) {
  String aeRepresentation =
    "{\"m2m:ae\": {"
    "\"rn\":\"" + ae + "\","
    "\"api\":\"org.demo." + ae + "\","
    "\"rr\":\"true\","
    "\"poa\":[\"http://" + WiFi.localIP().toString() + ":" + LOCAL_PORT + "/" + ae + "\"]"
    "}}";
#ifdef DEBUG
  Serial.println(aeRepresentation);
#endif
  return doPOST("/" + CSE_NAME, TY_AE, aeRepresentation);
}

// Method for creating an Container(CNT) resource on the remote CSE under a specific AE (this is done by sending a POST request)
// param : ae --> the targeted AE name (should be unique under the remote CSE)
// param : cnt  --> the CNT name to be created under this AE (should be unique under this AE)
String createCNT(String ae, String cnt) {
  String cntRepresentation =
    "{\"m2m:cnt\": {"
    "\"rn\":\"" + cnt + "\","
    "\"min\":\"" + -1 + "\""
    "}}";
  return doPOST("/" + CSE_NAME + "/" + ae, TY_CNT, cntRepresentation);
}

// Method for creating an ContentInstance(CI) resource on the remote CSE under a specific CNT (this is done by sending a POST request)
// param : ae --> the targted AE name (should be unique under the remote CSE)
// param : cnt  --> the targeted CNT name (should be unique under this AE)
// param : ciContent --> the CI content (not the name, we don't give a name for ContentInstances)
String createCI(String ae, String cnt, String ciContent) {
  String ciRepresentation =
    "{\"m2m:cin\": {"
    "\"con\":\"" + ciContent + "\""
    "}}";
  return doPOST("/" + CSE_NAME + "/" + ae + "/" + cnt, TY_CI, ciRepresentation);
}


// Method for creating an Subscription (SUB) resource on the remote CSE (this is done by sending a POST request)
// param : ae --> The AE name under which the SUB will be created .(should be unique under the remote CSE)
//          The SUB resource will be created under the COMMAND container more precisely.
String createSUB(String ae) {
  String subRepresentation =
    "{\"m2m:sub\": {"
    "\"rn\":\"SUB_" + ae + "\","
    "\"nu\":[\"" + CSE_NAME + "/" + ae  + "\"], "
    "\"nct\":1"
    "}}";
  return doPOST("/" + CSE_NAME + "/" + ae + "/" + CMND_CNT_NAME, TY_SUB, subRepresentation);
}


// Method to register a module (i.e. sensor or actuator) on a remote oneM2M CSE
void registerModule(String module, bool isActuator, String intialDescription, String initialData) {
  if (WiFi.status() == WL_CONNECTED) {
    String result;
    // 1. Create the ApplicationEntity (AE) for this sensor
    result = createAE(module);
    if (result == HTTP_CREATED) {
#ifdef DEBUG
      Serial.println("AE " + module + " created  !");
#endif

      // 2. Create a first container (CNT) to store the description(s) of the sensor
      result = createCNT(module, DESC_CNT_NAME);
      if (result == HTTP_CREATED) {
#ifdef DEBUG
        Serial.println("CNT " + module + "/" + DESC_CNT_NAME + " created  !");
#endif


        // Create a first description under this container in the form of a ContentInstance (CI)
        result = createCI(module, DESC_CNT_NAME, intialDescription);
        if (result == HTTP_CREATED) {
#ifdef DEBUG
          Serial.println("CI " + module + "/" + DESC_CNT_NAME + "/{initial_description} created !");
#endif
        }
      }

      // 3. Create a second container (CNT) to store the data  of the sensor
      result = createCNT(module, DATA_CNT_NAME);
      if (result == HTTP_CREATED) {
#ifdef DEBUG
        Serial.println("CNT " + module + "/" + DATA_CNT_NAME + " created !");
#endif

        // Create a first data value under this container in the form of a ContentInstance (CI)
        result = createCI(module, DATA_CNT_NAME, initialData);
        if (result == HTTP_CREATED) {
#ifdef DEBUG
          Serial.println("CI " + module + "/" + DATA_CNT_NAME + "/{initial_aata} created !");
#endif
        }
      }

      // 3. if the module is an actuator, create a third container (CNT) to store the received commands
      if (isActuator) {
        result = createCNT(module, CMND_CNT_NAME);
        if (result == HTTP_CREATED) {
#ifdef DEBUG
          Serial.println("CNT " + module + "/" + CMND_CNT_NAME + " created !");
#endif

          // subscribe to any ne command put in this container
          result = createSUB(module);
          if (result == HTTP_CREATED) {
#ifdef DEBUG
            Serial.println("SUB " + module + "/" + CMND_CNT_NAME + "/SUB_" + module + " created !");
#endif
          }
        }
      }
    }
  }
}


void init_WiFi() {
  Serial.println("Connecting to  " + String(WIFI_SSID) + " ...");
  WiFi.persistent(false);
  WiFi.begin(WIFI_SSID, WIFI_PSWD);

  // wait until the device is connected to the wifi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(WIFI_DELAY);
    Serial.print(".");
  }

  // Connected, show the obtained ip address
  Serial.println("WiFi Connected ==> IP Address = " + WiFi.localIP().toString());
}

void init_HTTPServer() {
  server.begin();
  Serial.println("Local HTTP Server started !");
}

void task_HTTPServer() {
  // Check if a client is connected
  client = server.available();
  if (!client)
    return;

  // Wait until the client sends some data
  Serial.println("New client connected. Receiving request... ");
  while (!client.available()) {
#ifdef DEBUG_MODE
    Serial.print(".");
#endif
    delay(5);
  }

  // Read the request
  String request = client.readString();
  Serial.println(request);
  client.flush();



  int start, end;
  // identify the right module (sensor or actuator) that received the notification
  // the URL used is ip:port/ae
  start = request.indexOf("/");
  end = request.indexOf("HTTP") - 1;
  context = request.substring(start + 1, end);
#ifdef DEBUG
  Serial.println(String() + start + " , " + end + " -> " + context + ".");
#endif


  // ingore verification messages
  if (request.indexOf("vrq") > 0) {
    client.flush();
    return;
  }


  //Parse the request and identify the requested command from the device
  //Request should be like "[operation_name]"
  start = request.indexOf("[");
  end = request.indexOf("]"); // first occurence of
  command = request.substring(start + 1, end);
#ifdef DEBUG
  Serial.println(String() + start + " , " + end + " -> " + command + ".");
#endif

  client.flush();
}

/////////////////////////////////////////////////////////////////////////////////

// ######################################################## //
// ######### USE THIS SPACE TO DECLARE VARIABLES  ######### //
// ########################################################  //

// float temp, hum;

// ########################################################  //

void setup()
{
    // intialize the serial liaison

    // Connect to WiFi network
//    init_WiFi();

    // Start HTTP server
    init_HTTPServer();

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
    while (sgp_probe() != STATUS_OK)
    {
        Serial.println("SGP failed");
        while (1)
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

    ThingSpeak.setField(1, String(t));
    ThingSpeak.setField(2, String(h));
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

    ThingSpeak.setField(1, String(sensor.measureCO()));
    ThingSpeak.setField(2, String(sensor.measureNO2()));
    ThingSpeak.setField(3, String(sensor.measureNH3()));

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
    sensor_value_string = String(string_temp) + String(",") + String(string_humidity)+String(",");
    sensor_value_string += String(string_CO)+String(",")+String(string_NO2)+String(string_NH3)+String(",");
    sensor_value_string += String(string_p10)+String(",")+String(string_p25)+String(",");
    sensor_value_string += String(string_tvoc)+String(",")+String(string_co2);
    createCI("Team33_Indoor-air_pollution-3_Houses", "node_1", sensor_value_string);

    // Check if the data instance was created.
    delay(15000); // DO NOT CHANGE THIS VALUE

    // ################################################################### //
}
