#include <DHT.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>


//SENSORS
#define DHT_IN 7 // Air Temperature
#define LDR_IN A15 // Light Denity 
#define DS18_IN 6 // Water Temperature
#define CO2_IN A14 // Co2
#define EC_IN A12 // Elcectricity Conductivity
#define PH_IN A13 // PH 



void setup() {
  Serial.begin(19200);

  

}

void loop() {
  // put your main code here, to run repeatedly:

}

float *getDHT()
{
  DHT dht(DHT_IN, DHT11);

  float *dht_data = malloc(2 * sizeof(float));

  float air_hum = dht.readHumidity();
  float air_temp = dht.readTemperature();
  dht_data[0] = air_hum;
  dht_data[1] = air_temp;

  return dht_data;
}










