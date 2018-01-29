#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

//SENSORS
#define DHT_IN 22
#define LDR_IN A0
#define DS18_IN 24
#define CO2_IN 26
#define EC_IN A1
#define PH_IN A2
//ACUTATORS
#define PH_A_PUMP 0
#define PH_B_PUMP 1
#define WATER_PUMP 2
#define AIR_FAN 3
#define LED 4
#define AIR_PUMP 5
#define VENTIL_FAN 6
//LCD KEYPAD
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

// LCD KEYPAD
LiquidCrystal lcd(12,13,8,9,10,11);
int lcd_key = 3;
int adc_key_in = 0;
int flag_index = 0;
int flag_arr[8] = {"temp","hum","co2","ph","ec","dstemp","LDR","nerdfarmers"};
unsigned long last_millis = 0;
int lcd_interval = 30000;

// 8Channel Relay
int ch8_relay[8] = {39,41,43,45,47,49,51,53};


int dtime = 0.5; // Delay Time
boolean current_led_stat = true;
unsigned int min_cnt = 0;
unsigned int turn_on_min_cnt = 60 * 4;
unsigned int turn_off_min_cnt = 60 * 1;


// For DHT11 Sensors.
unsigned int ldr_val;
// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.



void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);

  while(!Serial)
  {
    return;
  }

  analogReference(DEFAULT);

  int i = 0;
  for (i = 0; i < 8; i++)
  {
    pinMode(ch8_relay[i], OUTPUT);
    digitalWrite(ch8_relay[i],HIGH);
  }

  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("PFC V2");
  lcd.setCursor(0,1);
  lcd.print("#NERDFARMERS");
  last_millis = millis(); 


}


void loop() {


  

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
    else if ( !strcmp(pfc_order_arr, "get_ec"))
    {
      int ds18temp = getDS18temp(DS18_IN);
      float ec_cms = getEC(EC_IN, ds18temp);
      Serial.println(ec_cms);
      return ec_cms;
    }
    else if ( !strcmp(pfc_order_arr, "get_ph"))
    {
      double ph_val = getPH(PH_IN);
      Serial.println(ph_val);
      return ph_val;
    }
    else if( !strcmp(pfc_order_arr, "get_all_sensors"))
    {
      // RESULT for CSV format( Comma Seperator)
      String csv_res = "";
      
      // DHT11
      float *dht_data = getDHT(DHT_IN);
      float air_temp = dht_data[1];
      float air_hum = dht_data[0];
      // LDR
      unsigned int ldr_val = getLDR(LDR_IN);
      // CO2
      int co2_ppm = getCo2ppm(CO2_IN);
      // DS18B20
       int ds18_temp = getDS18temp(DS18_IN);
      // EC
      float ec_cms = getEC(EC_IN, ds18_temp);
      // PH
      double ph_val = getPH(PH_IN);



      csv_res +=String(air_temp) + ",";
      csv_res +=String(air_hum) + ",";
      csv_res +=String(ldr_val) + ",";
      csv_res +=String(co2_ppm) + ",";
      csv_res +=String(ds18_temp) + ",";
      csv_res +=String(ec_cms) + ",";
      csv_res +=String(ph_val);
      //csv_res += "\n";
      Serial.println(csv_res);

    }


    //Actuators
    else if ( !strcmp(pfc_order_arr, "on_ventil_fan"))
    {
      digitalWrite(ch8_relay[VENTIL_FAN],LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_ventil_fan"))
    {
      digitalWrite(ch8_relay[VENTIL_FAN],HIGH);
      Serial.println("off");
    }
    else if ( !strcmp(pfc_order_arr, "on_air_fan"))
    {
      digitalWrite(ch8_relay[AIR_FAN],LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_air_fan"))
    {
      digitalWrite(ch8_relay[AIR_FAN],HIGH);
      Serial.println("off");
    }
    else if ( !strcmp(pfc_order_arr, "on_led"))
    {
      digitalWrite(ch8_relay[LED],LOW);
      Serial.println("on");
    } 
    else if ( !strcmp(pfc_order_arr, "off_led"))
    {
      digitalWrite(ch8_relay[LED],HIGH);
      Serial.println("off");
    } 
    else if ( !strcmp(pfc_order_arr, "on_air_pump"))
    {
      digitalWrite(ch8_relay[AIR_PUMP],LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_air_pump"))
    {
      digitalWrite(ch8_relay[AIR_PUMP],HIGH);
      Serial.println("off");
    }
    else if ( !strcmp(pfc_order_arr, "on_ph_a_pump"))
    {
      digitalWrite(ch8_relay[PH_A_PUMP],LOW);
      Serial.println("on");

    }
    else if ( !strcmp(pfc_order_arr, "off_ph_a_pump"))
    {
      digitalWrite(ch8_relay[PH_A_PUMP],HIGH);
      Serial.println("off");

    }
    else if ( !strcmp(pfc_order_arr, "on_ph_b_pump"))
    {
      digitalWrite(ch8_relay[PH_B_PUMP],LOW);
      Serial.println("on");

    }
    else if ( !strcmp(pfc_order_arr, "off_ph_b_pump"))
    {
      digitalWrite(ch8_relay[PH_B_PUMP],HIGH);
      Serial.println("off");

    }
    else if ( !strcmp(pfc_order_arr, "on_water_pump"))
    {
      digitalWrite(ch8_relay[WATER_PUMP],LOW);
      Serial.println("on");

    }
    else if ( !strcmp(pfc_order_arr, "off_water_pump"))
    {
      digitalWrite(ch8_relay[WATER_PUMP],HIGH);
      Serial.println("off");

    }
    // LCD PANEL
    else if ( !strcmp(pfc_order_arr, "display_status"))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write("PFC STATUS : " );
      lcd.setCursor(8,1);
      lcd.write("Healthy");
      Serial.println("display_status");
    }
    else if ( !strcmp(pfc_order_arr, "display_internet_disconnet"))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write("Internet");
      lcd.setCursor(0,1);
      lcd.write("Disconneted");
      Serial.println("display_internet_disconnect");      
    }
    else if ( !strcmp(pfc_order_arr, "display_ip_address"))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write("display_ip_address : " );
      Serial.println("display_ip_address"); 
    }
    else if ( !strcmp(pfc_order_arr, "display_pfc_ver"))
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write("PFC v2.0" );
      lcd.setCursor(0,1);
      lcd.write("#NerdFarmers");
       Serial.println("display_pfc_ver");
    }

    else
    {
      Serial.println("non-order");
    }



  }//
  

  if((millis() - last_millis) > lcd_interval)
  {

    
    if(flag_index == 8)
    {
      flag_index =0;
    }

    lcd.clear();
    
    switch(flag_index){
      case 0:{
          float *dht_data = getDHT(DHT_IN);
          float air_temp = dht_data[1];
          lcd.setCursor(0,0);
          lcd.print("AIR TEMP:");
          lcd.setCursor(7,1);
          lcd.print(String(air_temp));
          lcd.print(" `c");
          break;
      }
      case 1:{
          float *dht_data = getDHT(DHT_IN);
          float air_hum = dht_data[0];
          lcd.setCursor(0,0);
          lcd.print("AIR HUMIDITY:");
          lcd.setCursor(7,1);
          lcd.print(String(air_hum));
          lcd.print(" %");
          break;
      }
      case 2:{
          int co2_ppm = getCo2ppm(CO2_IN);
          lcd.setCursor(0,0);
          lcd.print("CO2 PPM:");
          lcd.setCursor(5,1);
          lcd.print(String(co2_ppm));
          lcd.print(" ppm");
          break;
      }
      case 3:{
          double ph_val = getPH(PH_IN);
          lcd.setCursor(0,0);
          lcd.print("PH:");
          lcd.setCursor(5,1);
          lcd.print(String(ph_val));
          lcd.print(" PH");
          break;
      }        
      case 4:{
          // DS18B20
          int ds18_temp = getDS18temp(DS18_IN);
          // EC
          float ec_cms = getEC(EC_IN, ds18_temp);
         
          lcd.setCursor(0,0);
          lcd.print("EC:");
          lcd.setCursor(5,1);
          lcd.print(String(ec_cms));
          lcd.print(" S/cm");
          break;
      }     
      case 5:{
          // DS18B20
          int ds18_temp = getDS18temp(DS18_IN);    
          lcd.setCursor(0,0);
          lcd.print("WATER TEMP:");
          lcd.setCursor(10,1);
          lcd.print(String(ds18_temp));
          lcd.print(" `c");
          break;
      }
      case 6:{
          unsigned int ldr_val = getLDR(LDR_IN);
          lcd.setCursor(0,0);
          lcd.print("PHOTOCELL :");
          lcd.setCursor(10,1);
          lcd.print(String(ldr_val));
          lcd.print("");
          break;
      }
      case 7:{
        lcd.setCursor(0,0);
        lcd.print("PFC V2");
        lcd.setCursor(0,1);
        lcd.print("#NERDFARMERS");
        break;
      }
            
    }


    flag_index++;
    last_millis = millis(); 
    
  }

}


char *floatToChar(float val)
{  
  char *buf = malloc(10*sizeof(char));
  return buf;
  
}

char *intToChar(int val)
{
  char *buf = malloc(10*sizeof(char));
  sprintf(buf, "%d", val);
  return buf;
}

char *doubleToChar(double val)
{
   char *buf = malloc(10*sizeof(char));
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
  int temp_arr[5]={0};
  int min_v = 0;
  int max_v = 0;
  int amount = 0;
  int max_iter = 5;
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  min_v = temp;
  max_v = temp;
  
    
  for(int in_i=0; in_i<max_iter;in_i++)
  {
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    
    if(temp < min_v)
    {
      min_v = temp;
    }
    else if(temp > max_v)
    {
      max_v = temp;
    }
    amount+= temp;
    
    delay(50);
  }

  int avg = (amount - min_v - max_v ) / ( max_iter -2 );
  
  return avg;
}

int getCo2ppm(int co2_in)
{
  int sensorValue = analogRead(co2_in);

  float voltage = sensorValue * (4600 / 1024.0);
  if (voltage == 0)
  {
//    Serial.println("Fault");
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


//    Serial.print("voltage : ");
//    Serial.print(voltage);
//    Serial.print("mv/");
//    Serial.print("concentration : ");
//    Serial.print(concentration);
//    Serial.println("ppm");
    
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

  while(true)
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

int read_LCD_buttons(){
  adc_key_in = analogRead(lcd_key);

  if(adc_key_in > 1000) return btnNONE;

  if(adc_key_in < 50 ) return btnRIGHT;
  if(adc_key_in < 250 ) return btnUP;
  if(adc_key_in < 450 ) return btnDOWN;
  if(adc_key_in < 650 ) return btnLEFT;
  if(adc_key_in < 950 ) return btnSELECT;

  return btnNONE;
}












