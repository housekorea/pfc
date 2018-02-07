#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>




//SENSORS
#define DHT_IN 50
#define LDR_IN A0
#define DS18_IN 52
#define CO2_IN 46

//#define EC_IN A1
//#define PH_IN A2
//ACUTATORS
//#define PH_A_PUMP 0
//#define PH_B_PUMP 1
//#define WATER_PUMP 2
//#define AIR_FAN 3
//#define AIR_PUMP 5
#define AIR_FAN_IN 6
#define AIR_FAN_OUT 5
#define LED 7

// This value have to be a change on the realy wiring
SoftwareSerial mhz14_co2_SerialCom (A0,A1);

// 8Channel Relay
int ch8_relay[8] = {53, 51, 49, 47, 45, 43, 41, 39};


int dtime = 0.5;// Delay Time
boolean current_led_stat = true;
unsigned int min_cnt = 0;
unsigned int turn_on_min_cnt = 60 * 4;
unsigned int turn_off_min_cnt = 60 * 1;

unsigned int ldr_val;

//Co2 Sensor array(MH-Z14a)
byte mhz14_co2_addArray[] = {
0xFF, 0x01, 0x86,
0x00, 0x00, 0x00,
0x00, 0x00, 0x79
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);

  while (!Serial)
  {
    return;
  }

  analogReference(DEFAULT);

  int i = 0;
  for (i = 0; i < 8; i++)
  {
    pinMode(ch8_relay[i], OUTPUT);
    digitalWrite(ch8_relay[i], HIGH);
  }

  pinMode(CO2_IN, INPUT);

}


void loop() {


//  while (1)
//  {
//    Serial.println(getPWM());
//    delay(1000);
//  }


  if (Serial.available() > 0 ) {
    String pfc_order;
    pfc_order = Serial.readString();
    const char *pfc_order_arr = pfc_order.c_str();



    Serial.print("order=");
    Serial.print(pfc_order);
    Serial.print("/");

    Serial.print("result=");
    //Sensors
    if ( !strcmp(pfc_order_arr, "get_temp"))
    {
      float *dht_data = getDHT(DHT_IN);
      Serial.println(dht_data[1]);

      return dht_data[1];

    }
    if ( !strcmp(pfc_order_arr, "get_hum"))
    {
      float *dht_data = getDHT(DHT_IN);
      Serial.println(dht_data[0]);
      return dht_data[0];

    }
    else if ( !strcmp(pfc_order_arr, "get_ds18temp"))
    {
      int ds18_temp = getDS18temp(DS18_IN);
      Serial.println(ds18_temp);
      return ds18_temp;
    }
    else if ( !strcmp(pfc_order_arr, "get_ldr"))
    {
      unsigned int ldr_val = getLDR(LDR_IN);
      Serial.println(ldr_val);
      return ldr_val;
    }
    else if ( !strcmp(pfc_order_arr, "get_co2"))
    {
      int co2_ppm = getCo2ppm(CO2_IN);
      Serial.println(co2_ppm);
      return co2_ppm;
    }
    else if ( !strcmp(pfc_order_arr, "get_all_sensors"))
    {
      // RESULT for CSV format( Comma Seperator)
      String csv_res = "";

      // DHT11
      float *dht_data = getDHT(DHT_IN);
      float air_temp = dht_data[1];
      float air_hum = dht_data[0];
      unsigned int ldr_val = getLDR(LDR_IN);
      // DS18B20
      int ds18_temp = getDS18temp(DS18_IN);

      csv_res += String(air_temp) + ",";
      csv_res += String(air_hum) + ",";
      csv_res += String(ldr_val) + ",";
      csv_res += String(ds18_temp);
      Serial.println(csv_res);

    }


    //Actuators
    else if ( !strcmp(pfc_order_arr, "on_air_fan_in"))
    {
      digitalWrite(ch8_relay[AIR_FAN_IN], LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_air_fan_in"))
    {
      digitalWrite(ch8_relay[AIR_FAN_IN], HIGH);
      Serial.println("off");
    }
    else if ( !strcmp(pfc_order_arr, "on_air_fan_out"))
    {
      digitalWrite(ch8_relay[AIR_FAN_OUT], LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_air_fan_out"))
    {
      digitalWrite(ch8_relay[AIR_FAN_OUT], HIGH);
      Serial.println("off");

    }
    else if ( !strcmp(pfc_order_arr, "on_led"))
    {
      digitalWrite(ch8_relay[LED], LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_led"))
    {
      digitalWrite(ch8_relay[LED], HIGH);
      Serial.println("off");
    }
    else
    {
      Serial.println("non-order");
    }
  }
}


char *floatToChar(float val)
{
  char *buf = malloc(10 * sizeof(char));
  return buf;

}

char *intToChar(int val)
{
  char *buf = malloc(10 * sizeof(char));
  sprintf(buf, "%d", val);
  return buf;
}

char *doubleToChar(double val)
{
  char *buf = malloc(10 * sizeof(char));
  sprintf(buf, "%d", val);
  return buf;
}


float *getDHT(int dht_in)
{
  DHT dht(dht_in, DHT11);


  float *dht_data = malloc(2 * sizeof(float));

  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  dht_data[0] = hum;
  dht_data[1] = temp;
  return dht_data;


}

unsigned int getLDR(int ldr_in)
{
  unsigned int ldr_val = analogRead(ldr_in);

  return ldr_val;
}


int getDS18temp(int ds_in)
{
  OneWire oneWire(ds_in);
  DallasTemperature sensors(&oneWire);
  int temp = 0;
  int temp_arr[5] = {0};
  int min_v = 0;
  int max_v = 0;
  int amount = 0;
  int max_iter = 5;
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  min_v = temp;
  max_v = temp;


  for (int in_i = 0; in_i < max_iter; in_i++)
  {
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);

    if (temp < min_v)
    {
      min_v = temp;
    }
    else if (temp > max_v)
    {
      max_v = temp;
    }
    amount += temp;

    delay(50);
  }

  int avg = (amount - min_v - max_v ) / ( max_iter - 2 );

  return avg;
}

int getCo2ppm(int co2_in)
{
  int sensorValue = analogRead(co2_in);

  float voltage = sensorValue * (4600 / 1024.0);
  if (voltage == 0)
  {
    return false;
  }
  else if (voltage < 400)
  {
    //    Serial.println("Preheating");
    return false;
  }
  else
  {
    int voltage_diference = voltage - 400;
    float concentration = voltage_diference * 50.0 / 16.0;
    return concentration;
  }
}

float getEC(int ec_in, float temperature)
{
  const byte numReadings = 20; // the number of sample times
  unsigned int AnalogSampleInterval = 25, printInterval = 700, tempSampleInterval = 850;
  unsigned int readings[numReadings];
  byte index = 0;
  unsigned long AnalogValueTotal = 0;
  unsigned int AnalogAverage = 0, averageVoltage = 0;
  unsigned long AnalogSampleTime, printTime, tempSampleTime;
  float ECcurrent;

  for (byte thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;
  }
  AnalogSampleTime = millis();
  printTime = millis();
  tempSampleTime = millis();


  for (int in_i = 0; in_i < 50000; in_i++)
  {
    if (millis() - AnalogSampleTime >= AnalogSampleInterval)
    {
      AnalogSampleTime = millis();
      AnalogValueTotal = AnalogValueTotal - readings[index];
      readings[index] = analogRead(ec_in);
      AnalogValueTotal = AnalogValueTotal + readings[index];
      index = index + 1;
      if (index >= numReadings)
      {
        index = 0;
      }

      AnalogAverage = AnalogValueTotal / numReadings;
    }

    if (millis() - printTime >= printInterval)
    {
      printTime = millis();
      averageVoltage = AnalogAverage * (float)5000 / 1024;

      float TempCoefficient = 1.0 + 0.0185 * (temperature - 25.0);
      float CoefficientVoltage = (float)averageVoltage / TempCoefficient;
      if (CoefficientVoltage < 150)
      {
        //          Serial.println("[EC] No Solution!");
        ECcurrent = 0;
      }
      else if (CoefficientVoltage <= 1457)
      {
        //          Serial.println("EC is mid");
        ECcurrent = 6.98 * CoefficientVoltage - 127;
      }
      else
      {
        //          Serial.println("EC is high");
        ECcurrent = 5.3 * CoefficientVoltage + 2278;
      }
      ECcurrent /= 1000;
      return ECcurrent;
    }
    delay(10);
  }

}

double getPH(int ph_in)
{
  double offset = 0.00;
  int samplingInterval = 20;
  int printInterval = 900;
  int ArrayLength = 40;
  int pHArray[ArrayLength];
  int pHArrayIndex = 0;


  unsigned long samplingTime = millis();
  unsigned long printTime = millis();
  float pHValue, voltage;

  while (true)
  {

    if (millis() - samplingTime > samplingInterval)
    {
      pHArray[pHArrayIndex++] = analogRead(ph_in);
      if (pHArrayIndex == ArrayLength) pHArrayIndex = 0;
      samplingTime = millis();
      voltage = averagearray(pHArray, ArrayLength) * 5.0 / 1024;
      pHValue = 3.5 * voltage + offset;

    }

    if (millis() - printTime > printInterval)
    {
      return pHValue;
    }

  }


}

double averagearray(int* arr, int number) {
  int i;
  int max_v, min_v;
  double avg;
  double amount = 0;
  if (number <= 0)
  {
    return 0;
  }
  if (number < 5)
  {
    for (i = 0; i < number; i++)
    {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  }
  else
  {
    if (arr[0] < arr[1])
    {
      min_v = arr[0];
      max_v = arr[1];
    }
    else
    {
      min_v = arr[1];
      max_v = arr[0];
    }

    for (i = 2; i < number; i++)
    {
      if (arr[i] < min_v)
      {
        amount += min_v;
        min_v = arr[i];
      }
      else if (arr[i] > max_v)
      {
        amount += max_v;
        max_v = arr[i];
      }
      else
      {
        amount += arr[i];
      }
    }
    avg = (double)amount / (number - 2);

    return avg;
  }
}
int getPWM()
{
  while (digitalRead(CO2_IN) == LOW);  
  long startTime = micros();
  while (digitalRead(CO2_IN) == HIGH);
  long duration = micros() - startTime;
  long co2ppm = 5 * ((duration / 1000) - 2);
  return co2ppm;
}


int mhz14_co2()
{
  char co2_dataValue[9];
  mhz14_co2_SerialCom.write(mhz14_co2_addArray,9);
  mhz14_co2_SerialCom.readBytes(co2_dataValue,9);
  int resHigh = (int)co2_dataValue[2];
  int resLow = (int)co2_dataValue[3];
  int pulse = (256*resHigh) + resLow;
  return pulse;

}









