#define BLYNK_PRINT Serial // Blynk Print Serial. This "define" should place on top of sketch
//#define BLYNK_DEBUG Serial
#define BLYNK_MAX_SENDBYTES 500 // set Limit Blynk Symbol Number(include Subject + body)

#include <DHT.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>


//SENSORS
#define DHT_IN 7 // Air Temperature
#define LDR_IN A0 // Light Denity 
#define DS18_IN 6 // Waterㄴ Temperature
#define CO2_IN A1 // Co2
#define EC_IN A2 // Elcectricity Conductivity
#define PH_IN A3 // PH 

//Actuators
#define PH_MINUS_PUMP 0
#define PH_PLUS_PUMP 1
#define WATER_PUMP 2
#define AIR_FAN 3
#define LED 4
#define AIR_PUMP 5
#define VENTIL_FAN 6
#define SOL_A_PUMP 7
#define SOL_B_PUMP 8
#define HUMIDIFIER_1 9
#define HUMIDIFERR_2 10

int ch16_relay[16] = {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53};
// 1,3,5,7,9,11,13,15 | 38,40,42,44,46,48,50,52
// 2,4,6,8,10,12,14,16 | 39,41,43,45,47,49,51,53

//LCD KEYPAD
#define btnRIGHT
#define btnUP
#define btnDOWN
#define btnLEFT
#define btnSELECT
#define btnNONE 

LiquidCrystal lcd(12,13,8,9,10,11); 



#define ESP_SERIAL Serial2
#define ESP_BAUD 115200

//You should get Auth Token in the Blynk App.
//Go to the Project Settings (nut icon).
//char auth[] = "ffa88c6b016e4c30986a005d57bdc709"; // PFC_0001
char auth[] = "b1b31fe06fe445d18bbc01284dcaedbd"; // PFC_0002
//Your WiFi credentials.
//Set password to "" for open networks.
char ssid[] = "FabLab_2.4G";
char pass[] = "innovationpark";
ESP8266 wifi(&ESP_SERIAL);

BlynkTimer bl_timer;
unsigned long count_blynk_fail = 0;
float discon_msec = 0;
unsigned long last_connect_start = 0;


BLYNK_CONNECTED() {
  // Request Blynk server to re-send latest values for all pins
  Blynk.syncAll();

  // You can also update individual virtual pins like this:
  //Blynk.syncVirtual(V0, V2);

  // Let's write your hardware uptime to Virtual Pin 2
  //  int value = millis() / 1000;
  //  Blynk.virtualWrite(V2, value);
  Serial.println("[Function Call]Called BLYNK_CONNECTED");
  count_blynk_fail = 0;

  last_connect_start = millis(); 
//  bl_timer.setTimeout(1000L,LCD_display_connected);
}

BLYNK_APP_CONNECTED()
{
  Serial.println("[BLYNK] BLYNK APP CONNECTED");
}
BLYNK_APP_DISCONNECTED()
{
  Serial.println("[BLYNK] BLYNK APP DISCONNECTED");
}


void setup() {
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("[PFC]");
  lcd.setCursor(0,1);
  lcd.print("SetUp Now.....");

  Serial.begin(9600);
  delay(10);
  ESP_SERIAL.begin(ESP_BAUD);
  delay(10);


//
//  for(int i=0; i<16; i++)
//  {
//    pinMode(ch16_relay[i],OUTPUT);
//    digitalWrite(ch16_relay[i],LOW);
//  }

//  int ri = 0;
//  while(true)
//  {
//    digitalWrite(ch16_relay[ri],HIGH);
//    if(ri==0)
//    {
//      digitalWrite(ch16_relay[15],LOW);
//    }
//    else
//    {
//      digitalWrite(ch16_relay[ri-1],LOW);
//    }
//    if(ri++ == 16)
//    {
//      ri=0;
//    }
//    delay(200);
//  }

  for(int i=0; i<16;i++)
  {
    digitalWrite(ch16_relay[i],HIGH);
    if(i==0)
    {
      digitalWrite(ch16_relay[15],LOW); 
    }
    else
    {
      digitalWrite(ch16_relay[i-1],LOW);      
    }
    delay(50);
  }


 
  Blynk.begin(auth, wifi, ssid, pass);

//  bl_timer.setInterval(1000L, sendMillis);
  bl_timer.setInterval(30000L, sendDhtSensor);
//  bl_timer.setInterval(30*60*1000L,sendEmailReport);
  
  pinMode(3, OUTPUT);

  
}

void loop() {
  //  Serial.println(BUFFER_SIZE);


  if(!Blynk.connected())
  {

    if(count_blynk_fail == 0)
    {
      discon_msec = millis();
    }
    float cur_msec = (millis() - discon_msec)/1000;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WIFI Disconnect");
    lcd.setCursor(0,1);
    lcd.print('"'+String(cur_msec) + '"' +" Elapsed");
    Serial.println("[Disconnected]Not Connected Blynk");
    delay(500);
    count_blynk_fail+=1;
    
    if(count_blynk_fail > 7)
    {
      Blynk.begin(auth, wifi, ssid, pass);
    }
  }
  else
  {
    Blynk.run();
    bl_timer.run();
   
   }

  
  //  while(Serial.available()){
  //    Serial2.write(Serial.read());
  //  }
  //
  //  while(Serial2.available()){
  //    Serial.write(Serial2.read());
  //  }

  // put your main code here, to run repeatedly

  //  String bl_message;
  //
  //  while(Serial.available()){
  //    bl_message+=char(Serial.read());
  //  }
  //  if(!Serial.available()){
  //    if(bl_message!="")
  //    {
  //      Serial.println("reply:"+bl_message);
  //      bl_message="";
  //    }
  //  }

  //
  //
  //  delay(3000);
  //  float *dht_data = getDHT();
  //  unsigned int ldr_val = getLDR();
  //  float ds18_val = getDS18B20();
  //  float co2_con = getCO2();
  //  Serial.print("Humidity : ");
  //  Serial.print(dht_data[0]);
  //  Serial.print(", Temp : " );
  //  Serial.print(dht_data[1]);
  //  Serial.println();
  //  Serial.print("LDR : " );
  //  Serial.print(ldr_val);
  //  Serial.println();
  //  Serial.print("DS18B20 : ");
  //  Serial.print(ds18_val);
  //  Serial.println();
  //  Serial.print("CO2 : " );
  //  Serial.print(co2_con);
  //  Serial.println();
  //  Serial.println("----------------");
}
void sendMillis()
{

  unsigned long mil = millis() / 1000;
  Blynk.virtualWrite(V0,mil);
  Serial.println(mil);
}

void LCD_display_connected()
{
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("[PFC] Connected");
   lcd.setCursor(1,1);
   lcd.print("> ");
   lcd.print( (millis() - last_connect_start)/1000);
   lcd.print(" Sec.");
//   lcd.print("On Internet!");    
  
}
void sendDiscon(){
  Blynk.disconnect();
}

void sendEmailReport() {
  float *dht_data = getDHT();
  float ds18_val = getDS18B20();
  float co2_con = getCO2();
  unsigned int ldr_val = getLDR();

  String mail_message = "Dear NerdFarmer! \r\n PFC send to message to you!\r\n\r\n\r\n";
  mail_message +="\tHumidity : "+String(dht_data[0]) +"\r\n\tTemperature:"+ String(dht_data[1]) + "\r\n\tCo2:" +String(co2_con) +"\r\n\tLDR :" + String(ldr_val) +"\r\n\tDS18B20" + String(ds18_val);
  mail_message +="\r\n\r\n From PFC on EZFARM";
  
  Blynk.email("hkh9737@naver.com", "[{DEVICE_NAME}]Alarm _ From Blynk", mail_message);
}

void sendProbeSensor() {
  // PH , EC
  // V6 = PH
  // V7 = EC
  
  float ph_val = getPH();
  float ds18_val = getDS18B20();
  float ec_val = getEC(ds18_val);
  

  Serial.print("DS18B20: " );
  Serial.println(ds18_val);
  Serial.print("PH: ");
  Serial.println(ph_val);
  Serial.print("EC: ");
  Serial.println(ec_val);
  Serial.println("==============");

  Blynk.virtualWrite(V5, ds18_val);
  Blynk.virtualWrite(V6, ph_val);
  Blynk.virtualWrite(V7, ec_val);

  bl_timer.setTimeout(1000, LCD_display_connected);
}

void sendAirSensor() {
  //Co2, LDR
  float co2_con = getCO2();
  unsigned int ldr_val = getLDR();
  Serial.print("Ligth Density Resistor : ");
  Serial.println(ldr_val);
  Serial.print("Co2 : ");
  Serial.println(co2_con);
  Blynk.virtualWrite(V3, co2_con);
  Blynk.virtualWrite(V4, ldr_val);

  bl_timer.setTimeout(1200, sendProbeSensor);
}

void sendDhtSensor() {
  //Humidity,Temperature,DS18B20

  float *dht_data = getDHT();
  Serial.print("Humidity: ");
  Serial.println(dht_data[0]);
  Serial.print("Temperature: ");
  Serial.println(dht_data[1]);


  Blynk.virtualWrite(V1, dht_data[0]);
  Blynk.virtualWrite(V2, dht_data[1]);

  bl_timer.setTimeout(1200, sendAirSensor);

  //  Blynk.virtualWrite(V5,millis());
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

  for (int in_i = 0; in_i < max_iter; in_i++)
  {
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);

    if (temp < min_v )
    {
      min_v = temp;
    }
    else if (temp > max_v )
    {
      max_v = temp;
    }
    amount += temp;
    delay(50);
  }
  float avg = (amount - min_v - max_v) / (max_iter - 2);
  return avg;
}

float getCO2()
{
  int co2_vol = analogRead(CO2_IN);
  float voltage = co2_vol * ( 5000 / 1024.0 );
  if (voltage == 0 )
  {
    Serial.println("Fault");
  }
  else if (voltage < 400)
  {
    Serial.println("preheating");
  }
  else
  {
    //    Serial.print("Co2 Voltage : " );
    //    Serial.println(voltage);
    int voltage_diference = voltage - 400;
    float concentration = voltage_diference * 50.0 / 16.0;
    return concentration;
  }
}

float getEC(float temperature)
{
  const byte numReadings = 20;//the number of sample times
  unsigned int AnalogSampleInterval = 25;
  unsigned int printInterval = 700;
  unsigned int tempSampleInterval = 850;
  unsigned int readings[numReadings];
  byte index = 0;
  unsigned long AnalogValueTotal = 0;
  unsigned int AnalogAverage = 0;
  unsigned int averageVoltage = 0;
  unsigned long AnalogSampleTime;
  unsigned long printTime;
  unsigned long tempSampleTime;
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
      readings[index] = analogRead(EC_IN);
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
        ECcurrent = 0;
      }
      else if (CoefficientVoltage <= 1457)
      {
        ECcurrent = 6.98 * CoefficientVoltage - 127;
      }
      else
      {
        ECcurrent = 5.3 * CoefficientVoltage + 2278;
      }

      ECcurrent = ECcurrent / 1000;
      return ECcurrent;
    }
    delay(10);
  }
}

float getPH()
{
  float offset = 0.00;
  int samplingInterval = 20;
  int printInterval = 900;
  int numReadings = 40;
  int pHArray[numReadings];
  int pHArrayIndex = 0;

  unsigned long samplingTime = millis();
  unsigned long printTime = millis();
  float pHValue;
  float voltage;

  while(true)
  {
    if(millis() - samplingTime > samplingInterval)
    {
      pHArray[pHArrayIndex++] = analogRead(PH_IN);
      if(pHArrayIndex == numReadings)
      {
        pHArrayIndex = 0;
      }
      voltage = averageArray(pHArray, numReadings) * 5.0 / 1024;
      pHValue = 3.5 * voltage + offset;
      
    }

    if (millis() - printTime > printInterval)
    {
      return pHValue;
    } 
  }
}


float averageArray(int* arr, int number)
{
  int i;
  int max_v;
  int min_v;
  float avg;
  float amount = 0;

  if(number <= 0 )
  {
    return 0;
  }
  if (number < 5 )
  {
    for(i=0; i < number; i++)
    {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  }
  else
  {
    if(arr[0] < arr[1])
    {
      min_v = arr[0];
      max_v = arr[1];
    }
    else
    {
      min_v = arr[1];
      max_v = arr[0];
    }
    amount = arr[0] + arr[1];

    for(i=2; i<number; i++)
    {
      if(arr[i] < min_v)
      {
        min_v = arr[i]; 
      }
      else if(arr[i] > max_v)
      {
        max_v = arr[i];
      }

      amount += arr[i];
    }
    amount = amount - max_v - min_v;
    avg = (float)amount / (number - 2);
    return avg;
  }


   
}


void setATCommand(char *command)
{

}









