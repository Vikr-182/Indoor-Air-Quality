#ifndef __POLL_SENSORS_H
#define __POLL_SENSORS_H

#include "credentials.h"
#include "DHT.h"
#include "sgp30.h"
#include <SDS011.h>
#include <MiCS6814-I2C.h>
#include <SoftwareSerial.h>


void poll_sensors()
{

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

  string_hif = String(hif);

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

//// SGP CODE
// s16 err = 0;
// u16 tvoc_ppb, co2_eq_ppm;
// err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
// if (err == STATUS_OK)
// {
//   Serial.print("tVOC  Concentration:");
//   Serial.print(tvoc_ppb);
//   string_tvoc = String(tvoc_ppb);
//   Serial.println("ppb");
//
//   Serial.print("CO2eq Concentration:");
//   string_co2 = String(co2_eq_ppm);
//   Serial.print(co2_eq_ppm);
//   Serial.println("ppm");
// }
// else
// {
//   Serial.println("error reading IAQ values\n");
// }

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

}

#endif
