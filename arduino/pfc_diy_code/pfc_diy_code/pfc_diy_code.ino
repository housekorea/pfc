#include <DHT.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>


//SENSORS
#define DHT_IN 7 // Air Temperature
#define LDR_IN A0 // Light Denity 
#define DS18_IN 6 // Water Temperature
#define CO2_IN A1 // Co2
#define EC_IN A12 // Elcectricity Conductivity
#define PH_IN A13 // PH 



void setup() {
  Serial.begin(9600);



}

void loop() {
  // put your main code here, to run repeatedly:

  delay(3000);
  float *dht_data = getDHT();
  unsigned int ldr_val = getLDR(); 
  float ds18_val = getDS18B20();
  float co2_con = getCO2();
  Serial.print("Humidity : ");
  Serial.print(dht_data[0]);
  Serial.print(", Temp : " );
  Serial.print(dht_data[1]);
  Serial.println();
  Serial.print("LDR : " );
  Serial.print(ldr_val);
  Serial.println();
  Serial.print("DS18B20 : ");
  Serial.print(ds18_val);
  Serial.println();
  Serial.print("CO2 : " );
  Serial.print(co2_con);
  Serial.println();
  
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

unsigned int getLDR()
{
  unsigned int ldr_val = analogRead(LDR_IN);
  return ldr_val;
}

float getDS18B20()
{
  OneWire oneWire(DS18_IN);
  DallasTemperature sensors(&oneWire);
  float temp = 0 ;
  float temp_arr[5] = {0.0};
  float min_v = 0;
  float max_v = 0;
  float amount = 0;
  int max_iter = 5;
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  min_v = temp;
  max_v = temp;

  for(int in_i=0; in_i<max_iter;in_i++)
  {
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);

    if(temp < min_v )
    {
      min_v = temp;
    }
    else if(temp > max_v )
    {
      max_v = temp;
    }
    amount+= temp;
    delay(50);
  }
  float avg = (amount - min_v - max_v) / (max_iter - 2);
  return avg;
}

float getCO2()
{
  int co2_vol = analogRead(CO2_IN);
  float voltage = co2_vol * ( 5000 / 1024.0 );
  if(voltage == 0 )
  {
    Serial.println("Fault");
  }
  else if(voltage < 400)
  {
    Serial.println("preheating");
  }
  else
  {
    Serial.print("Co2 Voltage : " );
    Serial.println(voltage);
    int voltage_diference = voltage-400;
    float concentration = voltage_diference*50.0/16.0;
    return concentration;    
  }
}




