#include <DHT.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <avr/wdt.h>

//Communication with Raspberry Pi
#define SLAVE_ADDRESS 0x04


//SENSORS
#define DHT_IN 7
#define LDR_IN A15
#define DS18_IN 6
#define CO2_IN A14
#define EC_IN A12
#define PH_IN A13
//ACUTATORS
#define PH_MINUS_PUMP 11
#define PH_PLUS_PUMP 12
#define WATER_PUMP 15
#define AIR_FAN 1
#define LED 0
#define AIR_PUMP 2
#define VENTIL_FAN false
#define SOL_A_PUMP 14
#define SOL_B_PUMP 13
#define HUMIDIFIER false

//LCD KEYPAD
#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

//CO2 - SKU : SEN0159 Modle 
/************************Hardware Related Macros************************************/
#define         MG_PIN                       (A14)     //define which analog input channel you are going to use
#define         BOOL_PIN                     (2)
#define         DC_GAIN                      (8.5)   //define the DC gain of amplifier

/***********************Software Related Macros************************************/
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interval(in milisecond) between each samples in 
                                                     //normal operation

/**********************Application Related Macros**********************************/
//These two values differ from sensor to sensor. user should derermine this value.
#define         ZERO_POINT_VOLTAGE           (0.220) //define the output of the sensor in volts when the concentration of CO2 is 400PPM
#define         REACTION_VOLTGAE             (0.030) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2

/*****************************Globals***********************************************/
float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};   
                                                     //two points are taken from the curve. 
                                                     //with these two points, a line is formed which is
                                                     //"approximately equivalent" to the original curve.
                                                     //data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280) 
                                                     //slope = ( reaction voltage ) / (log400 –log1000) 



// LCD KEYPAD
LiquidCrystal lcd(12,13,8,9,10,11);
int lcd_key = 3;
int adc_key_in = 0;
int flag_index = 0;
int flag_arr[8] = {"temp","hum","co2","ph","ec","dstemp","LDR","nerdfarmers"};
unsigned long last_millis = 0;
int lcd_interval = 30000;
char* act_maps[23] = {"all","led","solution_a_pump","solution_b_pump","ph_minus_pump","ph_plus_pump",
"water_pump","air_pump","air_fan","ventil_fan","humidifier","heater","col_color","dummy1","dummy2","dummy3",
"co2","dht11_hum","dht11_temp","ds18bs20","ec","ldr","ph"};

// 8Channel Relay
int ch8_relay[8] = {39,41,43,45,47,49,51,53};
int ch16_relay[16] = {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};

int dtime = 0.5; // Delay Time
boolean current_led_stat = true;
unsigned int min_cnt = 0;
unsigned int turn_on_min_cnt = 60 * 4;
unsigned int turn_off_min_cnt = 60 * 1;
int act_result = 2;
int sensor_result = 0;

// For DHT11 Sensors.
unsigned int ldr_val;
// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
unsigned long reset_st_time;


void softwareReset( uint8_t prescaller) {
  // start watchdog with the provided prescaller
  wdt_enable( prescaller);
  // wait for the prescaller time to expire
  // without sending the reset signal by using
  // the wdt_reset() method
  while(1) {}
}



void setup() {

  Serial.begin(19200);

  //Initialize i2c communication as a slave
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(i2c_receiveData);
  Wire.onRequest(i2c_sendData);


//  Serial.println(Wire.available());

//  analogReference(DEFAULT);

  int i = 0;
  for (i = 0; i < 16; i++)
  {
    pinMode(ch16_relay[i], OUTPUT);
    digitalWrite(ch16_relay[i],HIGH);
  }

//  lcd.begin(16,2);
//  lcd.setCursor(0,0);
//  lcd.print("PFC V2");
//  lcd.setCursor(0,1);
//  lcd.print("#NERDFARMERS");


  // Variable to reset automatically
  last_millis = millis(); 
  reset_st_time = millis();
  


}


void loop() {

//   int i = 0;
   
//   for(i=0; i<5; i++)
//   {
//     digitalWrite(ch16_relay[15-(i-1)],HIGH);
//     digitalWrite(ch16_relay[15-i],LOW);
//     delay(3000);
//   }
//    digitalWrite(ch16_relay[15],LOW);



//  int i =0;
//  for(i=0;i<16; i++)
//  {
//    if(i > 0 )
//    {
//      digitalWrite(ch16_relay[i-1],HIGH);     
//    }
//    else if(i==0)
//    {
//      digitalWrite(ch16_relay[15],HIGH);
//
//    }
//    digitalWrite(ch16_relay[i],LOW);
//    delay(1500);
//    
//  }
      // always on LED
//      digitalWrite(ch16_relay[LED],LOW);
//      digitalWrite(ch16_relay[AIR_FAN],LOW);

//
//      int ds18temp = getDS18temp(DS18_IN);
//      float ec_cms = getEC(EC_IN, ds18temp);
//      Serial.print(ds18temp);
//      Serial.print('/');
//      Serial.println(ec_cms);
//      delay(1000);

 

//  //Reset codes inteval every 12 hours.//  //Reset codes inteval every 12 hours.
//  if(millis() - reset_st_time >= 43200000)
//  if(millis() - reset_st_time >= 1000 * 5)
  // Reset codes interval every 60 minutes.
  if(millis() - reset_st_time >= 1200000)
  {
     Serial.println("RESET----");
     Serial.println(millis());
     delay(100);
     softwareReset(WDTO_60MS);
  }

  
  if (Serial.available() > 0 ) {
    String pfc_order;
    pfc_order = Serial.readString();
    const char *pfc_order_arr = pfc_order.c_str();
//    
//       size_t Size = strlen(str);
//    return Size;
//    
//    Serial.println(strlen(pfc_order_arr));
    

    
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
      int percentage;
      float volts;
//      int co2_ppm = getCo2ppm(CO2_IN);
      volts = MGRead(MG_PIN);
//      Serial.print( "SEN0159:" );
//      Serial.print(volts); 
//      Serial.print( "V");
//      Serial.println("\n");
      percentage = MGGetPercentage(volts,CO2Curve);
//      Serial.print("CO2:");
      if (percentage == -1) {
        Serial.print( "<400" );
      } else {
        Serial.print(percentage);
      }
      return percentage;
//      Serial.print( "ppm" );  
//      Serial.print("\n");
//      Serial.println(co2_ppm);
//      return co2_ppm;
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
      digitalWrite(ch16_relay[VENTIL_FAN],LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_ventil_fan"))
    {
      digitalWrite(ch16_relay[VENTIL_FAN],HIGH);
      Serial.println("off");
    }
    else if ( !strcmp(pfc_order_arr, "on_air_fan"))
    {
      digitalWrite(ch16_relay[AIR_FAN],LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_air_fan"))
    {
      digitalWrite(ch16_relay[AIR_FAN],HIGH);
      Serial.println("off");
    }
    else if ( !strcmp(pfc_order_arr, "on_led"))
    {
      digitalWrite(ch16_relay[LED],LOW);
      Serial.println("on");
    } 
    else if ( !strcmp(pfc_order_arr, "off_led"))
    {
      digitalWrite(ch16_relay[LED],HIGH);
      Serial.println("off");
    } 
    else if ( !strcmp(pfc_order_arr, "on_air_pump"))
    {
      digitalWrite(ch16_relay[AIR_PUMP],LOW);
      Serial.println("on");
    }
    else if ( !strcmp(pfc_order_arr, "off_air_pump"))
    {
      digitalWrite(ch16_relay[AIR_PUMP],HIGH);
      Serial.println("off");
    }
    else if ( !strcmp(pfc_order_arr, "on_ph_minus_pump"))
    {
      digitalWrite(ch16_relay[PH_MINUS_PUMP],LOW);
      Serial.println("on");

      delay(3300); // every 5 second supply 7.5ml , 3.3second for 5ml.
      digitalWrite(ch16_relay[PH_MINUS_PUMP],HIGH);
      Serial.println("off on automatically");
    }
    else if ( !strcmp(pfc_order_arr, "off_ph_minus_pump"))
    {
      digitalWrite(ch16_relay[PH_MINUS_PUMP],HIGH);
      Serial.println("off");

    }
    else if ( !strcmp(pfc_order_arr, "on_ph_plus_pump"))
    {
      digitalWrite(ch16_relay[PH_PLUS_PUMP],LOW);
      Serial.println("on");
      delay(3300); // every 5 second supply 7.5ml , 3.3second for 5ml.
      digitalWrite(ch16_relay[PH_PLUS_PUMP],HIGH);
      Serial.println("off on automatically");
    }
    else if ( !strcmp(pfc_order_arr, "off_ph_plus_pump"))
    {
      digitalWrite(ch16_relay[PH_PLUS_PUMP],HIGH);
      Serial.println("off");

    }
    else if ( !strcmp(pfc_order_arr, "on_sol_a_pump"))
    {
       digitalWrite(ch16_relay[SOL_A_PUMP],LOW);
      Serial.println("on");
      
      delay(3300); // every 5 second supply 7.5ml , 3.3second for 5ml.
      digitalWrite(ch16_relay[SOL_A_PUMP],HIGH);
      Serial.println("off on automatically");
    }
    else if ( !strcmp(pfc_order_arr, "off_sol_a_pump"))
    {
       digitalWrite(ch16_relay[SOL_A_PUMP],HIGH);
       Serial.println("off");
       
    }
    else if ( !strcmp(pfc_order_arr, "on_sol_b_pump"))
    {
       digitalWrite(ch16_relay[SOL_B_PUMP],LOW);
       Serial.println("on");
             
      delay(3300); // every 5 second supply 7.5ml , 3.3second for 5ml.
      digitalWrite(ch16_relay[SOL_B_PUMP],HIGH);
      Serial.println("off on automatically");

    }
    else if ( !strcmp(pfc_order_arr, "off_sol_b_pump"))
    {
       digitalWrite(ch16_relay[SOL_B_PUMP],HIGH);
       Serial.println("off");
    }
    else if ( !strcmp(pfc_order_arr, "on_water_pump"))
    {
      digitalWrite(ch16_relay[WATER_PUMP],LOW);
      

    }
    else if ( !strcmp(pfc_order_arr, "off_water_pump"))
    {
      digitalWrite(ch16_relay[WATER_PUMP],HIGH);
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
    else if ( !strcmp(pfc_order_arr, "reset_arduino"))
    {
      softwareReset( WDTO_60MS);  
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

/*****************************  MGRead *********************************************
Input:   mg_pin - analog channel
Output:  output of SEN-000007
Remarks: This function reads the output of SEN-000007
************************************************************************************/ 
float MGRead(int mg_pin)
{
    int i;
    float v=0;

    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;  
}

/*****************************  MQGetPercentage **********************************
Input:   volts   - SEN-000007 output measured in volts
         pcurve  - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(MG-811 output) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/ 
int  MGGetPercentage(float volts, float *pcurve)
{
   
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
      return -1;
   } else { 
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
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
//      Serial.println(CoefficientVoltage);
//      Serial.println(TempCoefficient);
//      Serial.println(averageVoltage);
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


int act_led(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[LED],LOW);
//    Serial.println("on");
    return 1;
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[LED],HIGH);
//    Serial.println("off");
    return 0;
  }
}


int act_solution_a_pump(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[SOL_A_PUMP],LOW);
//    Serial.println("on");
//    delay(3300);
//    digitalWrite(ch16_relay[SOL_A_PUMP],HIGH);
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[SOL_A_PUMP],HIGH);
//    Serial.println("off");
    return 0;
  }
}
int act_solution_b_pump(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[SOL_B_PUMP],LOW);
//    Serial.println("on");
//    delay(3300);
//    digitalWrite(ch16_relay[SOL_B_PUMP],HIGH);
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[SOL_B_PUMP],HIGH);
//    Serial.println("off");
    return 0;
  }
}
int act_ph_minus_pump(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[PH_MINUS_PUMP],LOW);
//    Serial.println("on");
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[PH_MINUS_PUMP],HIGH);
//    Serial.println("off");
    return 0;
  }
}
int act_ph_plus_pump(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[PH_PLUS_PUMP],LOW);
//    Serial.println("on");
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[PH_PLUS_PUMP],HIGH);
//    Serial.println("off");
    return 0;
  }
}
int act_water_pump(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[WATER_PUMP],LOW);
//    Serial.println("on");
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[WATER_PUMP],HIGH);
//    Serial.println("off");
    return 0;
  }
}
int act_air_pump(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[AIR_PUMP],LOW);
//    Serial.println("on");
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[AIR_PUMP],HIGH);
//    Serial.println("off");
    return 0;
  }
  
}
int act_air_fan(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[AIR_FAN],LOW);
//    Serial.println("on");
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[AIR_FAN],HIGH);
//    Serial.println("off");
    return 0;
  }
}
int act_ventil_fan(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[VENTIL_FAN],LOW);
//    Serial.println("on");
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[VENTIL_FAN],HIGH);
//    Serial.println("off");
    return 0;
  }
}
int act_humidifier(int act_status){
  if(act_status == 1)
  {
    digitalWrite(ch16_relay[HUMIDIFIER],LOW);
//    Serial.println("on");
    return 1;
    
  }
  else if(act_status == 0)
  {
    digitalWrite(ch16_relay[HUMIDIFIER],HIGH);
//    Serial.println("off");
    return 0;
  }
}




String rasp_order;
const char *rasp_order_arr;
int cnt = 0;
int order_indicator;
void i2c_receiveData(int byteCount){
 
  while(Wire.available()){
    rasp_order = Wire.read();
    rasp_order_arr = rasp_order.c_str();
    
    if(cnt == 0 && order_indicator==0)
    {
      order_indicator = atoi(rasp_order_arr);      
    }
    if( (cnt-1) == order_indicator)
    {
      if (act_maps[order_indicator] == "all")
      {
              
      }
      else if(act_maps[order_indicator] == "led")
      {


        if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_led(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_led(0);
        }
      }
      else if(act_maps[order_indicator] == "solution_a_pump")
      {
        if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_solution_a_pump(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_solution_a_pump(0);
        }
      }
      else if(act_maps[order_indicator] == "solution_b_pump")
      {
        if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_solution_b_pump(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_solution_b_pump(0);
        }
      }
      else if(act_maps[order_indicator] == "ph_minus_pump")
      {
       if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_ph_minus_pump(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_ph_minus_pump(0);
        } 
      }
      else if(act_maps[order_indicator] == "ph_plus_pump")
      {
       if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_ph_plus_pump(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_ph_plus_pump(0);
        } 
      }
      else if(act_maps[order_indicator] == "water_pump")
      {
        if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_water_pump(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_water_pump(0);
        }
      }
      else if(act_maps[order_indicator] == "air_pump")
      {
        if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_air_pump(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_air_pump(0);
        }

        
      }
      else if(act_maps[order_indicator] == "air_fan")
      {
        if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_air_fan(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_air_fan(0);
        }
      }
      else if(act_maps[order_indicator] == "ventil_fan")
      {
        if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_ventil_fan(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_ventil_fan(0);
        }
      }
      else if(act_maps[order_indicator] == "humidifier")
      {
        if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_humidifier(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_humidifier(0);
        }
      }
      else if(act_maps[order_indicator] == "col_color")
      {
         if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_humidifier(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_humidifier(0);
        }
      }
      else if(act_maps[order_indicator] == "dummy1")
      {
         if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_humidifier(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_humidifier(0);
        }
      }
      else if(act_maps[order_indicator] == "dummy2")
      {
         if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_humidifier(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_humidifier(0);
        }
      }
      else if(act_maps[order_indicator] == "dummy3")
      {
         if(atoi(rasp_order_arr) == 1)
        {
          act_result=act_humidifier(1);
        }
        else if(atoi(rasp_order_arr) == 0)
        {
          act_result=act_humidifier(0);
        }
      }
      else if(act_maps[order_indicator] == "co2")
      {
         if(atoi(rasp_order_arr) == 1)
        {
          sensor_result = getCo2ppm(CO2_IN);
        }
      }
      else if(act_maps[order_indicator] == "dht11_hum")
      {
         if(atoi(rasp_order_arr) == 1)
        {
            float *dht_data = getDHT(DHT_IN);
            float air_hum = dht_data[0];
            sensor_result= dht_data[0];
        }
      }
      else if(act_maps[order_indicator] == "dht11_temp")
      {
         if(atoi(rasp_order_arr) == 1)
        {
            float *dht_data = getDHT(DHT_IN);  
            sensor_result= dht_data[1];
        }
      }
      else if(act_maps[order_indicator] == "ds18bs20")
      {
         if(atoi(rasp_order_arr) == 1)
        {
          sensor_result = getDS18temp(DS18_IN);

        }
      }
      else if(act_maps[order_indicator] == "ec")
      {
         if(atoi(rasp_order_arr) == 1)
        {
          float ds18temp = getDS18temp(DS18_IN);

          float ec_cms = getEC(EC_IN, ds18temp);

          sensor_result= ec_cms; 
        }
      }
      else if(act_maps[order_indicator] == "ldr")
      {

        if(atoi(rasp_order_arr) == 1)
        {
          sensor_result = getLDR(LDR_IN);
        }

      }
      else if(act_maps[order_indicator] == "ph")
      {
         if(atoi(rasp_order_arr) == 1)
        {
//          act_result=act_humidifier(1);
        }
       }             
    }

    cnt =cnt+1;
  }
  cnt=0;

}
void i2c_sendData(){
//  Serial.print("[i2c_sendData]Order_indicator : "); 
//  Serial.println(order_indicator);
  

  if(order_indicator <= 15)
  {
    //Request Actuator Data by Raspberry Pi 
//    Serial.print("[i2c_sendData]order indicator : ");
//    Serial.println(act_maps[order_indicator]);
    Wire.write(act_result); 
  }
  else
  {
    char sval[10];
    itoa(sensor_result,sval,10);
//    Serial.print("Sensor Value : ");
//    Serial.print(sensor_result);
//    Serial.print("|");
//    Serial.println(sval);
////    Serial.print("[i2c_sendData]order indicator : ");
////    Serial.print(act_maps[order_indicator]);
////    Serial.println(sval);
//    Wire.write(sval); 
    
  }
//  delay(500);
//  Wire.endTransmission(false);
  act_result=2;
  sensor_result=0;
  order_indicator=0;
}







