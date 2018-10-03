// IoT 플랫폼은 Blynk 이용.
// 아두이노,라즈베리파이 등 오픈소스 하드웨어를 IoT에 쉽게 결합할수 있도록 라이브러리와 스마트폰 API,앱 등을 제공
#define BLYNK_PRINT Serial // Blynk 앱에서 출력하는 내용들은 "Serial" 로 출력함을 지정
#define BLYNK_MAX_SENDBYTES 2048 //Blynk 내부에서 다루는 데이터의 처리 용량을 명시
#define BLYNK_MAX_READBYTES  4096 // Blynk 내부에서 다루는 데이터의 처리 용량을 명시
#define BLYNK_MSG_LIMIT 300 // 주어진 기간만큼 보낼 수 있는 메시지의 총량
#include <DHT.h> // DHT11 온습도 센서 라이브러리 추가
#include <OneWire.h> // DS18BS20 수온센서 "OneWire" 라이브러리 추가
#include <Wire.h> // I2C 통신용 라이브러리 추가
#include <DallasTemperature.h>  // DS18B20 수온센서 "DallasTemperature" 라이브러리 추가
#include <LiquidCrystal.h> // LCD 디스플레이용 라이브러리 추가
#include <ESP8266_Lib.h> // Blynk에서 배포한 ESP8266 기본 라이브러리 추가
#include <BlynkSimpleShieldEsp8266.h> // Blynk에서 배포한 BlynkSimpleShieldESP8266 라이브러리
#include <avr/wdt.h> //아두이노 WatchDog 라이브러리, 소프트웨어 리셋하기 위한 라이브러리
#include <EEPROM.h> // 아두이노 내부에 비휘발성 메모리를 입,출력하기 위한 라이브러리
#include <EEPROMAnything.h> // EEPROM 입,출력을 용이하게 하기위한 함수
#include <SPI.h> // Serial Peripheral Interface 를 위한 라이브러리
#include <SD.h> // SD카드를 위한 입,출력 라이브러리 추가

#define WRITESD_DEBUG 1 // SD 카드 출력을 지정하는 Flag. 1일 경우 SD카드에 로그 출력,0일 경우 SD카드에 로그 출력 X

//EEPROM Settings
#define EEPROM_RESET_ADDR 0x58 // Arduino RESET 횟수를 EEPROM에 저장하고, 저장하는 메모리주소가 "0x58"에 저장
struct config_reset_cnt_struct // EEPROM 에 데이터를 저장하기 위한 데이터 구조를
{
  unsigned int reset_cnt;
} eeprom_reset_struct;

//SENSORS
#define DHT_IN 7 // Air Temperature - 디지털 핀번호
#define LDR_IN A0 // Light Density - 아날로그 핀번호
#define DS18_IN 6 // Waterㄴ Temperature - 디지털 핀번호
#define CO2_IN A1 // Co2 - 아날로그 핀번호
#define EC_IN A2 // Elcectricity Conductivity - 아날로그 핀번호
#define PH_IN A3 // PH - 아날로그 핀번호

//Actuators
#define PH_MINUS_PUMP 0 // 릴레이 제어번호
#define PH_PLUS_PUMP 1 // 릴레이 제어번호
#define WATER_PUMP 2 // 릴레이 제어번호
#define AIR_FAN 3 // 릴레이 제어번호 - 내부팬
#define LED 4 // 릴레이 제어번호
#define AIR_PUMP 5 // 릴레이 제어번호
#define VENTIL_FAN 6 // 릴레이 제어번호 - 환기팬(외부와 기체교환)
#define SOL_A_PUMP 7 // 릴레이 제어번호
#define SOL_B_PUMP 8 // 릴레이 제어번호
#define HUMIDIFIER_1 9 // 릴레이 제어번호
#define HUMIDIFERR_2 10 // 릴레이 제어번호


int ch16_relay[16] = {26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41};// 릴레이 1번이 아두이노 디지털핀 26 ~ 릴레이 16번이 아두이노 디지털핀1
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


#define ESP_SERIAL Serial2 // 아두이노 메가의 경우 하드웨어 시리얼이 3개가 공급 가능하며, ESP8266과 통신을 위해 "Serial2"를 사용
#define ESP_BAUD 115200 // Baud Rate - ESP8266 통신을 위해 권장되는  "115200"를 사용 


//You should get Auth Token in the Blynk App.
//Go to the Project Settings (nut icon).
char auth[] = "__exitggfafjasdfieb0eifjie16e09eifie"; // PFC_TEST_SERIAL, 중복되거나 외부에 노출을 지양해야 하는 시리얼 코드, PFC 한 대당 1개의 시리얼코드가 부여됨

//Your WiFi credentials.
//Set password to "" for open networks.
char ssid[] = "FabLab_2.4G"; // Wi-Fi 정보
char pass[] = "innovationpark"; // Wi-Fi 비밀번호

ESP8266 wifi(&ESP_SERIAL); // ESP8266 와이파이 설정

BlynkTimer bl_timer; // Blynk 타이머 선언
int count_blynk_fail = 0;
float discon_msec = 0.0;
int last_connect_start = 0;

//Software Reset 
unsigned long RESET_TIMEOUT; // 주기적으로 아두이노를 리셋하는 주기(Interval)
unsigned long arduino_smsec;
WidgetLCD blynk_lcd(V21);

//SD card
int sd_card_pin = 53; // SD카드가 연결된 디지털 핀번호
boolean is_sd_card_init = false;
#define SD_LOG_FILE  "blynklog.log" // SD 카드내에 로그를 기록하는 파일명(File_path)
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

BLYNK_APP_CONNECTED() // Blynk 스마트폰 앱이 Blynk IoT 서버 -> Arduino Mega 에 서로 연결될 경우 실행하는 콜백함수
{
  Serial.println("[BLYNK] BLYNK APP CONNECTED");
}
BLYNK_APP_DISCONNECTED()// Blynk 스마트폰 앱이 Blynk IoT 서버 -> Arduino Mega 에 연결이 끊겼 경우 실행하는 콜백함수
{
  Serial.println("[BLYNK] BLYNK APP DISCONNECTED");
}


void setup() {
// Setup() 함수는 아두이노 부팅후 초기 1회 실행 되는 함수. 즉, 변수나 데이터구조를 초기화하는 데 사용됨.

  arduino_smsec = millis(); // 아두이노가 동작된 시간(밀리세컨드) 저장
  analogReference(DEFAULT); // 아날로그 Input 기준전압 설정
  
  Serial.begin(9600);
  ESP_SERIAL.begin(ESP_BAUD);
  RESET_TIMEOUT = (unsigned long)720 *  (unsigned long)60 * (unsigned long)1000; //아두이노가 동작한지 12시간이 초과될경우 Softreset.
  
  Serial.println(">>>>>>>>>>>>");
  Serial.println("[Arduino Mega] Start");
  Serial.println(">>>>>>>>>>>>");
  
  EEPROM_readAnything(EEPROM_RESET_ADDR,eeprom_reset_struct); // 아두이노 비휘발성 메모리 EEPROM에 아두이노 리셋횟수를 불러옴
  if (eeprom_reset_struct.reset_cnt > 100000) // EEPROM 에 명시적 입,출력 최대 가능횟수를 10만으로 설정.
  {
    eeprom_reset_struct.reset_cnt = 0;
  }
  eeprom_reset_struct.reset_cnt += 1;
  EEPROM_writeAnything(EEPROM_RESET_ADDR, eeprom_reset_struct); // EEPROM에 RESET 횟수 저장
  Serial.print("[EEPROM RESET CNT] on EEPROM MEMORY : ");
  Serial.println(eeprom_reset_struct.reset_cnt);

  if (!SD.begin(sd_card_pin)) // SD 카드 초기화 설정 - 아두이노 메가에 SD카드가 연결되어 있을경우
  {
    Serial.println("[SD Card]Initialization Failed"); 
  }
  else
  {
    sd_file = SD.open(SD_LOG_FILE);
//    while (sd_file.available()) {
//      Serial.write(sd_file.read());
//    }
    String log_data = ">>>>>>>>>>>>>>>> Starting No." + String(eeprom_reset_struct.reset_cnt);
//    writeSD(log_data);
    Serial.println("[SD Card]Initialization Success");
 
  }


  wifi.setDHCP(1,1,1);
  Blynk.config(wifi, auth,BLYNK_DEFAULT_DOMAIN,BLYNK_DEFAULT_PORT);  // Attempt general connection to network
  if (Blynk.connectWiFi(ssid, pass)) {  // Blynk 와이파이 접속
    Blynk.connect();  // Blynk 서버 접속
  }
  else
  {
    Serial.println("[Blynk.connecteWiFi(ssid,pass)] Failed");
    delay(5000);
    wdt_enable(WDTO_1S); // Blynk 와이파이 접속실패시 5초 뒤("delay(5000)")에 Software Reset() 시행
  }


  // Blynk Interval Event Attach
  // Blynk 는 내장 BlynkTimer 함수를 통해서 주어진 "주기(Interval)" 마다 특정 함수를 호출하는 방식으로 운용됨
  bl_timer.setInterval(5000L, checkBlynk); // Blynk 접속 여부 판별
  bl_timer.setInterval(5000L,sendMillis); // Blynk에 현재 아두이노 실행시간 전송
  bl_timer.setInterval(60000L, sendDhtSensor); // Blynk에 DHT 센서 정보 전달
  Serial.println("[Setup] Blynk Timer setted");

  
}

unsigned long last_msec = millis();
unsigned int ReCnctFlag = 0;
unsigned int ReCnctCount = 0;

void loop() {
  bl_timer.run(); // Setup() 함수에서 등록한 BlynkTimer 함수를 실행.

  if(Blynk.connected()){ 
    // Blynk의 Wi-Fi 접속이 된 경우에는 Blynk를 실행
    Blynk.run();
  }

  if(millis() - arduino_smsec > RESET_TIMEOUT)
  {
    // 아두이노 구동시간이 특정 시간(Reset_timeout Interval)이 경과된 경우 software reset 실행.
    Serial.println(RESET_TIMEOUT);
    Serial.println(millis());
    Serial.println(arduino_smsec);
    String log_data = String("[RESET START]") + String(millis());
    writeSD(log_data);
    delay(100);
    softwareReset(WDTO_60MS); // 아두이노 리셋 실행.
  }

  
  if(millis() - last_msec > 600000)
  {
    // 매 10분마다 아두이노의 경과 시간을 출력.(Serial 및 SD카드에 로그 기록)
    Serial.println("[Elapsed Time in Loop()]" +  String(millis() - last_msec / 1000));
    String log_data = String("[Elapsed_time]") + String(millis() - last_msec / 1000);
    writeSD(log_data);
    last_msec =millis();
  }

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
}

void sendMillis(){
  // Blynk Virtual Pin : V20 - 아두이노 구동 경과 시간
  // Blynk Virtual Pin : V22 - 아두이노 EEPROM 에 저장되어 있는 리셋 횟수를 전송
  unsigned long cur_msec = millis();
  Blynk.virtualWrite(V20, cur_msec / 1000);
  Blynk.virtualWrite(V22, eeprom_reset_struct.reset_cnt);
}

void sendProbeSensor() {
  // Blynk Virtual Pin : V5 - DS18B20 수온 센서의 값 전송
  // Blynk Virtual Pin : V6 - pH 센서 값 전송 
  // Blynk Virtual Pin : V7 - EC 센서 값 전송
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

  // Blynk Virtual Pin : V50 - WebHook => pH,EC,DS18B20 센서 데이터를 원격 On-Premise 서버에 전송
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
  // Blynk Virtual Pin : V3 - 이산화탄소(Co2) 센서 값 전송
  // Blynk Virtual Pin : V4 - 조도(Light Dependent Resistor) 센서 값 전송
  String log_data = String("[SendAirSensor]") + String(millis()) +String("__START");
  writeSD(log_data);


  float co2_con = getCO2();
  writeSD("getCo2() complete.");
  unsigned int ldr_val = getLDR();
  writeSD("geLDR() complete.");
  Serial.print("Ligth Dependency Resistor : ");
  Serial.println(ldr_val);
  Serial.print("Co2 : ");
  Serial.println(co2_con);
  Blynk.virtualWrite(V3, co2_con);
  Blynk.virtualWrite(V4, ldr_val);

  // Blynk Virtual Pin : V51 - WebHook => Co2,LDR 센서 데이터를 원격 On-Premise 서버에 전송
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

  bl_timer.setTimeout(3000L, sendProbeSensor); // Air Sensor 에 대한 전송이 완료된 후 3초 뒤에 ProbeSensor 전송 함수 호출
}

void sendDhtSensor() {
  // Blynk Virtual Pin : V1 - 대기 습도(Air Humidity) 전송 
  // Blynk Virtual Pin : V2 - 기온(Air Temperature) 전송 

  //AIR(DHT) Humidity,Temperature
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


  // Blynk Virtual Pin : V52 - WebHook => DHT센서 데이터를 원격 On-Premise 서버에 전송

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
}

float *getDHT()
{
  //DHT11 센서 값 반환
  DHT dht(DHT_IN, DHT11); // DHT 센서의 아날로그 디지털 핀 번호 및 모델명 기입

  float *dht_data = malloc(2 * sizeof(float)); // 온,습도 데이터를 저장하기 위한 동적할당(malloc)

  float air_hum = dht.readHumidity(); // 연결된 센서의 dht 습도 데이터 저장
  float air_temp = dht.readTemperature(); // 연결된 센서의 dht 온도 데이터 저장

  dht_data[0] = air_hum; // 동적할당된 메모리( dht_data ) 에 "0" 인덱스에 습도 센서데이터 저장
  dht_data[1] = air_temp; // 동적할당된 메모리( dht_data ) 에 "1" 인덱스에 기온 센서데이터 저장

  return dht_data; // 해당 배열 데이터를 함수에서 반환
}

unsigned int getLDR()
{
  // Light Dependence Resistor (조도) 센서 데이터 읽기
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
  int pin_num = 26;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}

BLYNK_WRITE(V27)
{
  int pin_num = 27;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V28)
{
  int pin_num = 28;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V29)
{
  int pin_num = 29;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V30)
{
  int pin_num = 30;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V31)
{
  int pin_num = 31;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V44)
{
  int pin_num = 44;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V32)
{
  int pin_num = 32;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V33)
{
  int pin_num = 33;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V34)
{
  int pin_num = 34;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V35)
{
  int pin_num = 35;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V36)
{
  int pin_num = 36;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V37)
{
  int pin_num = 37;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V38)
{
  int pin_num = 38;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V39)
{
  int pin_num = 39;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V40)
{
  int pin_num = 40;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
  }
}
BLYNK_WRITE(V41)
{
  int pin_num = 41;
  pinMode(pin_num,OUTPUT);
  int pin_val = param.asInt();
  if(pin_val == 1)
  {
    digitalWrite(pin_num,HIGH);
  }
  else
  {
    digitalWrite(pin_num,LOW);    
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
  else
  {
//    Serial.println("[CheckBlynk()] Blynk connected"); 
  }
} 
