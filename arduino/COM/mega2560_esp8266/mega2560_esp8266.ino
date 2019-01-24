#define BLYNK_PRINT Serial // Blynk Print Serial. This "define" should place on top of sketch
//#define BLYNK_DEBUG_ALL Serial
#define BLYNK_MAX_SENDBYTES 2048 // set Limit Blynk Symbol Number(include Subject + body)
#define BLYNK_MAX_READBYTES  4096
#define BLYNK_MSG_LIMIT 300
#include <DHT.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include <EEPROMAnything.h>
#include <SPI.h>
#include <SD.h>

#define WRITESD_DEBUG 0

//EEPROM Settings
#define EEPROM_RESET_ADDR 0x58
struct config_reset_cnt_struct
{
  unsigned int reset_cnt;
} eeprom_reset_struct;

//SENSORS
#define DHT_IN 25 // Air Temperature
#define LDR_IN A1 // Light Denity 
#define DS18_IN 26 // Waterㄴ Temperature
#define CO2_IN A0 // Co2
#define EC_IN A3 // Elcectricity Conductivity
#define PH_IN A2 // PH 

//Actuators
#define PH_MINUS_PUMP 4
#define PH_PLUS_PUMP 3
#define WATER_PUMP 2
#define SOL_A_PUMP 0
#define SOL_B_PUMP 1
#define LED 8
#define AIR_FAN 6
#define VENTIL_FAN 7
#define AIR_PUMP 5

#define HUMIDIFIER_1 14
#define HUMIDIFIER_2 15

int ch16_relay[16] = {10,11,12,13,5,2,3,6,7,8,9,46,38,39,40,41};
// 1,3,5,7,9,11,13,15 | 26,28,30,32,34,36,38,40
// 2,4,6,8,10,12,14,16 | 27,29,31,33,35,37,39,41

//LCD KEYPAD
#define btnRIGHT
#define btnUP
#define btnDOWN
#define btnLEFT
#define btnSELECT
#define btnNONE 
LiquidCrystal lcd(12,13,8,9,10,11); 


#define ESP_SERIAL Serial1
#define ESP_BAUD 115200


//You should get Auth Token in the Blynk App.
//Go to the Project Settings (nut icon).
char auth[] = "853983fce3974b528046908125161c50"; 

//Your WiFi credentials.
//Set password to "" for open networks.
char ssid[] = "FabLab_2.4G";
//char ssid[] = "inno-park";
char pass[] = "innovationpark";

//char ssid[] = "lcp";
//char pass[] = "travelshot";
ESP8266 wifi(&ESP_SERIAL);

BlynkTimer bl_timer;
int count_blynk_fail = 0;
float discon_msec = 0.0;
int last_connect_start = 0;

//Software Reset 
unsigned long RESET_TIMEOUT; // 주기적으로 아두이노를 리셋하는 주
unsigned long arduino_smsec;
WidgetLCD blynk_lcd(V21);

//SD card

int sd_card_pin = 53;
boolean is_sd_card_init = false;
#define SD_LOG_FILE  "blynklog.log"
File sd_file;


BLYNK_CONNECTED() {
  Serial.println("[Function Call]Called BLYNK_CONNECTED");
  count_blynk_fail = (unsigned long)0;

  // 아두이노메가 - 아날로그 핀(A*) / 디지털 핀(*)
  // Blynk - 아날로그 핀 / 디지털 핀 / 버츄얼 핀(V*)
  // Blynk 앱을 통해 아두이노의 아날로그,디지털 핀을 제어할 수 있지만, 

  int virtual_relay[] = {V26,V27,V28,V29,V30,V31,V32,V33,V34,V35,V36,V37,V38,V39,V40,V41};
  for(int i=0; i< (sizeof(virtual_relay) / sizeof(int)); i++)
  {
    Blynk.syncVirtual(virtual_relay[i]); // 명시한 Blynk Virtual Pin 의 값을 동기화(IoT Blynk 서버로부터 저장
    delay(10);
  }

  String log_data = String("[BLYNK Connected]") + String(millis());
  writeSD(log_data); // Blynk 접속사실을 SD 카드에 로그로 기록
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
  

  
//  lcd.begin(16,2);
//  lcd.setCursor(0,0);
//  lcd.print("[PFC]");
//  lcd.setCursor(0,1);
//  lcd.print("SetUp Now.....");


  arduino_smsec = millis();

  Serial.begin(9600);
  delay(10);
  ESP_SERIAL.begin(ESP_BAUD);
  delay(10);
  RESET_TIMEOUT = (unsigned long)4320 *  (unsigned long)60 * (unsigned long)1000; //3days
  
  Serial.println(">>>>>>>>>>>>");
  Serial.println("[Arduino Mega] Start");
  Serial.println(">>>>>>>>>>>>");
  EEPROM_readAnything(EEPROM_RESET_ADDR,eeprom_reset_struct);
  if (eeprom_reset_struct.reset_cnt > 60,000)
  {
    eeprom_reset_struct.reset_cnt = 0;
  }
  eeprom_reset_struct.reset_cnt += 1;
  EEPROM_writeAnything(EEPROM_RESET_ADDR, eeprom_reset_struct);
  Serial.print("[EEPROM RESET CNT] on EEPROM MEMORY : ");
  Serial.println(eeprom_reset_struct.reset_cnt);

  if (!SD.begin(sd_card_pin))
  {
    Serial.println("[SD Card]Initialization Failed"); 
  }
  else
  {
//    sd_file = SD.open(SD_LOG_FILE);
//
//    while (sd_file.available()) {
//      Serial.write(sd_file.read());
//    }
    String log_data = ">>>>>>>>>>>>>>>> Starting No." + String(eeprom_reset_struct.reset_cnt);
//    writeSD(log_data);
    Serial.println("[SD Card]Initialization Success");
 
  }



//  for(int i=0; i<16; i++)
//  {
//    pinMode(ch16_relay[i],OUTPUT);
//  }


  // Blynk Start(Automatic)
//  Blynk.begin(auth, wifi, ssid, pass);
  // Blynk Start(Manually)
  wifi.setDHCP(1,1,1);
  Blynk.config(wifi, auth,BLYNK_DEFAULT_DOMAIN,BLYNK_DEFAULT_PORT);  // Attempt general connection to network
  if (Blynk.connectWiFi(ssid, pass)) {  // If connected to WiFi...
    Blynk.connect();  // ...connect to Server
  }
  else
  {
    Serial.println("[Blynk.connecteWiFi(ssid,pass)] Failed");
    delay(5000);
    wdt_enable(WDTO_1S);
  }


  // Blynk Interval Event Attach
  bl_timer.setInterval(5000L, checkBlynk);
  bl_timer.setInterval(3000L,sendMillis);
  bl_timer.setInterval(30000L, sendDhtSensor);
//  bl_timer.setInterval(60*1000L,sendEmailReport);

  Serial.println("[Setup] Blynk Timer setted");
  
}

unsigned long last_msec = millis();
unsigned int ReCnctFlag = 0;
unsigned int ReCnctCount = 0;

void loop() {
    bl_timer.run();

    if(Blynk.connected()){
      Blynk.run();
    }
    else if (ReCnctFlag == 0 ){
      ReCnctFlag = 1;
      Serial.println("Starting reconnection timer in 30 seconds...");
      bl_timer.setTimeout(30000L, [](){
        ReCnctFlag = 0;
        ReCnctCount++;
        Serial.print("Attempting reconne ction #");
        Serial.println(ReCnctCount);
        wifi.setDHCP(1,1,1);
        Blynk.config(wifi, auth, BLYNK_DEFAULT_DOMAIN, BLYNK_DEFAULT_PORT);
        Blynk.connect();
        if(Blynk.connectWiFi(ssid,pass)){
          Blynk.connect();
        }
      });
    }
 
  
  
  //  Serial.println(BUFFER_SIZE);



//  byte blynk_logs[8];
//  if(blynk_logs = BLYNK_DEBUG_ALL.availableForWrite())
//  {
//    Serial.println(blynk_logs);
//  }


  // Time Interval writing SD Card log
//  if(millis() - last_msec > 10000)
//  {
//    
//    String log_data = String("[millis]") + String(millis());
//    Serial.println(log_data);
//    writeSD(log_data);
//    last_msec = millis();
//  }


  if(millis() - arduino_smsec > RESET_TIMEOUT)
  {
    Serial.println(RESET_TIMEOUT);
    Serial.println(millis());
    Serial.println(arduino_smsec);
    String log_data = String("[RESET START]") + String(millis());
//    writeSD(log_data);
    delay(100);
    softwareReset(WDTO_60MS); 
  }

  
  if(millis() - last_msec > 60000)
  {
    Serial.println("[Elapsed Time in Loop()]" +  String(millis() - last_msec / 1000));
    last_msec =millis();
  }

//  if(!Blynk.connected())
//  {

//    if(count_blynk_fail == 0)
//    {
//      discon_msec = millis();
//    }
//    float cur_msec = (millis() - discon_msec)/1000;
//    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("WIFI Disconnect");
//    lcd.setCursor(0,1);
//    lcd.print('"'+String(cur_msec) + '"' +" Elapsed");
//    if (count_blynk_fail % 10 == 0)
//    {
//      Serial.print("[Disconnected]Not Connected Blynk :");
//      Serial.println(count_blynk_fail);
//      
//    }
//    delay(50);
//    count_blynk_fail+=1;
//    
//    if(count_blynk_fail > 100)
//    {
//      Blynk.begin(auth, wifi, ssid, pass);
//    }
//  }

  
  
  
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
  Serial.println("[SendEMailReport] Start");
  float *dht_data = getDHT();
  float ds18_val = getDS18B20();
  float co2_con = getCO2();
  unsigned int ldr_val = getLDR();

  String mail_message = "Dear NerdFarmer! \r\n PFC send to message to you!\r\n\r\n\r\n";
  mail_message +="\tHumidity : "+String(dht_data[0]) +"\r\n\tTemperature:"+ String(dht_data[1]) + "\r\n\tCo2:" +String(co2_con) +"\r\n\tLDR :" + String(ldr_val) +"\r\n\tDS18B20" + String(ds18_val);
  mail_message +="\r\n\r\n From PFC on EZFARM";
  
  Blynk.email("hkh9737@naver.com", "[{DEVICE_NAME}]Alarm _ From Blynk", mail_message);
  Serial.println("[SendEMailReport] End");
}




void sendMillis(){

  unsigned long cur_msec = millis();
  Blynk.virtualWrite(V20, cur_msec / 1000);
  Blynk.virtualWrite(V22, eeprom_reset_struct.reset_cnt);

  // Current Millis() , EEPROM RESET COUNT , BLYNK Connected status
//  Serial.print("Blynk Status : ");
//  Serial.println(Blynk.connected());
//  Serial.print("EEPROM COUNT :" );
//  Serial.println(eeprom_reset_struct.reset_cnt);

  String log_data = String("[SendMillis]") + String(millis()) + "," + String(eeprom_reset_struct.reset_cnt) + "," + String(Blynk.connected());
  writeSD(log_data);
}

void sendProbeSensor() {
  // PH , EC
  // V6 = PH
  // V7 = EC
  String log_data = String("[SendProbeSensor]") + String(millis()) +String("__START");
  writeSD(log_data);
    
  float ph_val = getPH();
  writeSD("getPH() complete.");
  float ds18_val = getDS18B20();
  writeSD("getDS18B20() complete.");
  float ec_val = getEC(ds18_val);
  writeSD("getEC() complete.");

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

  // WebHook with Blynk => It will throw to the Data Lake server
  Blynk.virtualWrite(V50, 
    "http://210.92.91.225:5000/v1/" + String(auth) +"/insert/" + "?wt=" + String(ds18_val)
    + "&ph=" + String(ph_val)
    + "&ec=" + String(ec_val)
  );
  Serial.println("[WebHook]http://210.92.91.225:5000/v1/" 
    + String(auth) +"/insert/"
    + "?wt=" + String(ds18_val)
    + "&ph=" + String(ph_val)
    + "&ec=" + String(ec_val));

    
  
  log_data = String("[SendProbeSensor]") + String(millis()) +String("__END");
  writeSD(log_data);


  //bl_timer.setTimeout(1000, LCD_display_connected);
}

void sendAirSensor() {
  String log_data = String("[SendAirSensor]") + String(millis()) +String("__START");
  writeSD(log_data);

  //Co2, LDR
  float co2_con = getCO2();
  writeSD("getCo2() complete.");
  unsigned int ldr_val = getLDR();
  writeSD("geLDR() complete.");
  Serial.print("Ligth Density Resistor : ");
  Serial.println(ldr_val);
  Serial.print("Co2 : ");
  Serial.println(co2_con);
  Blynk.virtualWrite(V3, co2_con);
  Blynk.virtualWrite(V4, ldr_val);

  // WebHook with Blynk => It will throw to the Data Lake server
  Blynk.virtualWrite(V51, 
    "http://210.92.91.225:5000/v1/" 
    + String(auth) +"/insert/"
    + "?co2=" + String(co2_con)
    + "&ldr=" + String(ldr_val)
  );
  Serial.println("[WebHook]http://210.92.91.225:5000/v1/" 
    + String(auth) +"/insert/" 
    + "?co2=" + String(co2_con)
    + "&ldr=" + String(ldr_val));

  log_data = String("[SendAirSensor]") + String(millis()) +String("__END");
  writeSD(log_data);

  bl_timer.setTimeout(3000L, sendProbeSensor);
}

void sendDhtSensor() {
  //Humidity,Temperature,DS18B20
  String log_data = String("[SendDhtSensor]") + String(millis()) +String("__START");
  writeSD(log_data);


  float *dht_data = getDHT();
  writeSD("getDHT complete.");
  Serial.print("Humidity: ");
  Serial.println(dht_data[0]);
  Serial.print("Temperature: ");
  Serial.println(dht_data[1]);


  Blynk.virtualWrite(V1, dht_data[0]);
  Blynk.virtualWrite(V2, dht_data[1]);



  // WebHook with Blynk => It will throw to the Data Lake server
  Blynk.virtualWrite(V52, 
    "http://210.92.91.225:5000/v1/" 
    + String(auth) +"/insert/"
    + "?at=" + String(dht_data[1])
    + "&ah=" + String(dht_data[0])
  );
  Serial.println("[WebHook]http://210.92.91.225:5000/v1/" 
    + String(auth) +"/insert/"
    + "?at=" + String(dht_data[1])
    + "&ah=" + String(dht_data[0]));

  log_data = String("[SendDhtSensor]") + String(millis()) + String("__END");
  writeSD(log_data);

  bl_timer.setTimeout(3000L, sendAirSensor);

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
  // 수온 센서 데이터 읽기 함수
  OneWire oneWire(DS18_IN); // ds18bs20 함수의 OneWire 프로토콜 정의
  DallasTemperature sensors(&oneWire); // 정의된 OneWire 프로토콜 센서를 온도함수로 읽어오기
  float temp = 0;
  float temp_arr[10] = {0.0};
  float min_v = 0;
  float max_v = 0;
  float amount = 0;
  int max_iter = 10;
  int acc_iter = 0;

  for(int in_i = 0; in_i < 100; in_i++)
  {
    // 최대 20번의 반복을 수행하여 센서의 정상값으로 최소,최대 센서데이터값을 초기화
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    if(temp >= 60 || temp <= -50)
    {
      continue;
    }
    else
    {
      min_v = temp;
      max_v = temp;
      break;
    }
  }

  for (int in_i = 0; in_i < max_iter; in_i++)
  {
    //반복문을 통해서 매 50ms 주기로 수온센서의 데이터를 읽어서 평균값을 반환.

    acc_iter++;
    if(acc_iter > 500)
    {
      //최대 500번 이상 수온센서 데이터 읽기를 시도했음에도, 올바른 정상범위의 온도데이터를 읽지 못할경우 "0"을 반환하고 함수실행종료
      return 0;
    }
    sensors.requestTemperatures();
    temp = sensors.getTempCByIndex(0);
    
    if(temp >= 60 || temp <= -50)
    {
      in_i--;
//      Serial.println("Abnomaly temp Skipped : " + String(temp));
      continue;
    }
  
//    Serial.println("[" + String(in_i) + "]" + temp);


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
  float avg = (amount - min_v - max_v) / (max_iter - 2); //수온의 최소값,최대값을 배제한 정상 범위내의 관측데이터의 평균값을 반환
  return avg;
}


float getCO2()
{
  int samplingInterval = 50; // 50ms를 주기로 하여 데이터를 측정
  int numReadings = 10;
  float co2Array[numReadings] = {0.0};
  int co2ArrayIndex=0;
  unsigned long funcTime = millis();
  //Co2 센서 데이터 읽기
  int co2_vol=0;
  float voltage=0; 
  // 아두이노의 analogRead() 2^10 으로 표현되며 (0~1023 의 범위를 가진다.) 즉 5볼트의 아날로그 최대 전압을 기준하였을 떄 5(volt)/1024(2^10)= 0.0049 mv 라는 값을 가진다. 
  // 현재 구입한 Co2의 모델은 5000 ppm 까지 최대 측정가능한 모듈로서, 디지털과 아날로그 데이터간의 "비율"을 구한다.

  while(millis() - funcTime < 1500)
  {
    co2_vol = analogRead(CO2_IN);
    voltage = co2_vol * (5000 / 1024.0);
    if (voltage == 0 )
    {
      // 아날로그로 측정한 전압값이 "0"일 경우는 오류
     Serial.println("Voltage Value is Zero.");
     continue;
    }
    else if (voltage < 400)
   {
      // 아날로그로 측정한 전압값이 400 미만인 경우는 Co2 센서가 아직 활성화되지 않은 상태(pre-heating)
      Serial.println("Co2 Sensor preheating");
      continue;
    }
    else
    {
      //    Serial.print("Co2 Voltage : " );
      //    Serial.println(voltage);
      int voltage_diference = voltage - 400;
      float concentration = voltage_diference * 50.0 / 16.0; // Co2 센서의 레퍼런스 데이터를 반영하여 연산
      co2Array[co2ArrayIndex] = concentration;
      co2ArrayIndex++;
      if(co2ArrayIndex == numReadings)
      {
        co2ArrayIndex=0;
      }
    }
    delay(samplingInterval);
  }
  return averageArray(co2Array, numReadings);
 
}



float getEC(float temperature)
{
 // EC(Electric Conductivity) 전기전도도 읽기 
  const byte numReadings = 20;// 측정할 전기전도도 센서의 횟수
  unsigned int AnalogSampleInterval = 25; // 센서 측정 주기(Time Interval)
  unsigned int printInterval = 700; // Debug 를 위한 Print 
  unsigned int tempSampleInterval = 850; //
  unsigned int readings[numReadings]; // "numReadings" 횟수만큼 배열공간 할당
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
  // pH 농도 측정함수
  float offset = 0.00;
  int samplingInterval = 20; //20ms를 주기로 하여 데이터를 측정
  int printInterval = 900; 
  int numReadings = 40; // 40번을 측정하여 평균값을 반환
  float pHArray[numReadings] = {0.0}; // "numReadings" 만큼 측정하는 배열 선언
  int pHArrayIndex = 0;

  unsigned long funcTime = millis(); // 함수 초기 실행 시각
  unsigned long samplingTime = millis(); // 함수 초기 실행 시각
  unsigned long printTime = millis(); // 함수 초기 실행 시각
  float pHValue; // pH 저장 변수
  float voltage; // 측정 전압 저장 변수


  

  while(millis() - funcTime < 2000)
  {
    // 측정시간이 최대 2초이내 범위에서만 "while 루프문을 실행"
    if(millis() - samplingTime > samplingInterval)
    {
      // samplingIntervla 주기를 가지고 ph센서 데이터를 측정
      pHArray[pHArrayIndex++] = analogRead(PH_IN);
      if(pHArrayIndex == numReadings)
      {
        // Queue 구조를 차용하여 최근 pH 센서 측정값이 "numReadings" 
        pHArrayIndex = 0;
      }
      voltage = averageArray(pHArray, numReadings) * 5.0 / 1024; // pHArray 에서 데이터값이 "0"으로 초기화되어 있을떄 평균값 체크가 필요함
      pHValue = 3.5 * voltage + offset;
      samplingTime = millis();
    }

    if (millis() - printTime > printInterval)
    {
      return pHValue;
    } 
  }
}

float averageArray(float* arr, int number)
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





void softwareReset( uint8_t prescaller) {
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>");
  Serial.println(">>>>[Software Reset] From Arduino Mega 2560");
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>");

//  blynk_lcd.clear();
//  blynk_lcd.print(0,0, "Software Reset");
//  blynk_lcd.print(0,1, millis());

  delay(100);
  // start watchdog with the provided prescaller
  wdt_enable(prescaller);

  // wait for the prescaller time to expire
  // without sending the reset signal by using
//  the wdt_reset() method
//  wdt_reset();
  while(1) {};
}


//V26,V27,V28,V29,V30,V31,V32,V33,V34,V35,V36,V37,V38,V39,V40,V41
BLYNK_WRITE(V26)
{
  int pin_num = 10;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}

BLYNK_WRITE(V27)
{
  int pin_num = 11;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V28)
{
  int pin_num = 12;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V29)
{
  int pin_num = 13;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V30)
{
  int pin_num = 5;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V31)
{
  int pin_num = 2;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}

BLYNK_WRITE(V32)
{
  int pin_num = 3;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V33)
{
  int pin_num = 6;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V34)
{
  int pin_num = 7;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V35)
{
  int pin_num = 8;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}BLYNK_WRITE(V36)
{
  int pin_num = 9;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V37)
{
  int pin_num = 46;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}BLYNK_WRITE(V38)
{
  int pin_num = 38;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V39)
{
  int pin_num = 39;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}BLYNK_WRITE(V40)
{
  int pin_num = 40;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}
BLYNK_WRITE(V41)
{
  int pin_num = 41;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,LOW);
  }
  else
  {
    digitalWrite(pin_num,HIGH);    
  }
}


void writeSD(String log_data)
{
  if( ! WRITESD_DEBUG ) 
  {
    return; 
  }

  
  sd_file = SD.open(SD_LOG_FILE,FILE_WRITE);


  if(sd_file)
  {
    sd_file.println(log_data);  
    delay(10);
    sd_file.close();
  }
  else
  {
    Serial.println("[SD CARD] Couldn't open SD Card");  
  }
  

}

void checkBlynk(){  
// Serial.println("[CheckBlynk()]" + String(millis()));
 if(!Blynk.connected()){
    Serial.println("[CheckBlynk()] Not connected to Blynk server"); 
    wdt_enable(WDTO_1S);
  }
} 
