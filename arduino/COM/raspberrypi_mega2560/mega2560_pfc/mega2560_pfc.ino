//Gihtub(https://github.com/housekorea/pfc) include below codes. 
//NerdFarmers KwangHee Han.
#include <PJON.h>
#include <DHT.h> // DHT11 온습도 센서 라이브러리 추가
#include <OneWire.h> // DS18BS20 수온센서 "OneWire" 라이브러리 추가
#include <Wire.h> // I2C 통신용 라이브러리 추가
#include <DallasTemperature.h>  // DS18B20 수온센서 "DallasTemperature" 라이브러리 추가
#include <avr/wdt.h>
#include <SPI.h> // Serial Peripheral Interface 를 위한 라이브러리
#include <SD.h> // SD카드를 위한 입,출력 라이브러리 추가

//SENSORS
#define DHT_IN 11 // Air Temperature - 디지털 핀번호
#define LDR_IN A2 // Light Density - 아날로그 핀번호
#define DS18_IN 6 // Waterㄴ Temperature - 디지털 핀번호
#define CO2_IN A3 // Co2 - 아날로그 핀번호
#define EC_IN A0 // Elcectricity Conductivity - 아날로그 핀번호
#define PH_IN A1 // PH - 아날로그 핀번호

//Actuator
#define PH_MINUS_PUMP 1 // 릴레이 제어번호
#define PH_PLUS_PUMP 2 // 릴레이 제어번호
#define WATER_PUMP 3 // 릴레이 제어번호
#define SOL_A_PUMP 4 // 릴레이 제어번호
#define SOL_B_PUMP 5 // 릴레이 제어번호
#define LED 10 // 릴레이 제어번호
#define AIR_FAN 11 // 릴레이 제어번호 - 내부팬
#define VENTIL_FAN 12 // 릴레이 제어번호 - 환기팬(외부와 기체교환)
#define AIR_PUMP 13 // 릴레이 제어번호
#define HUMIDIFIER_1 14 // 릴레이 제어번호
#define HUMIDIFERR_2 15 // 릴레이 제어번호




//Delay(Milli Seconds)
#define PUMP_PH_PLUS_DELAY 3000
#define PUMP_PH_MINUS_DELAY 3000
#define PUMP_WATER_DELAY 30000
#define PUMP_SOLUTION_A_DELAY 3000
#define PUMP_SOLUTION_B_DELAY 3000



int ch16_relay[16] = {26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41};// 릴레이 1번이 아두이노 디지털핀 26 ~ 릴레이 16번이 아두이노 디지털핀1
// 1,3,5,7,9,11,13,15 | 26,28,30,32,34,36,38,40
// 2,4,6,8,10,12,14,16 | 27,29,31,33,35,37,39,41


//Software Reset 
unsigned long RESET_TIMEOUT; // 주기적으로 아두이노를 리셋하는 주기(Interval)
unsigned long arduino_smsec;


// Setup for time interval code for Actuators
boolean led_status = true;
boolean air_fan_status = true;
boolean ventil_fan_status = true;
boolean air_pump_status = true;


unsigned long led_interval=3600000;
unsigned long air_fan_interval=600000;
unsigned long ventil_fan_interval=600000;
unsigned long air_pump_interval=300000;

unsigned long led_next_time = led_interval;
unsigned long air_fan_next_time = air_fan_interval;
unsigned long ventil_fan_next_time = ventil_fan_interval;
unsigned long air_pump_next_time = air_pump_interval;

//unsigned long interval_time = millis();

//PJON Settings
PJON<ThroughSerial> bus(44);
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  Serial.println("we'v got a message");
  Serial.println(length);
  Serial.println(payload[0]);

  for(int i=0;i<length;i++)
  {
    Serial.print(char(payload[i]));
  }
  Serial.println();
  
  if(payload[0] == 'B') {
//    digitalWrite(LED_BUILTIN, HIGH);
//    delay(30);
//    digitalWrite(LED_BUILTIN, LOW);
//    bus.reply("A", 1);
//    bus.reply("B", 1);
//    bus.reply("C", 1);
    delay(1000);
    bus.reply("2239abcz", 8);
    
  }
  
};


void setup() {


  arduino_smsec = millis(); // 아두이노가 동작된 시간(밀리세컨드) 저장
  analogReference(DEFAULT); // 아날로그 Input 기준전압 설정
  Serial.begin(9600);
  Serial2.begin(9600);


  bus.strategy.set_serial(&Serial2);
  bus.set_receiver(receiver_function);
//  bus.send_repeatedly(45, "B", 1, 100000);

  bus.begin();

}

unsigned long last_msec = millis();
unsigned int ReCnctFlag = 0;
unsigned int ReCnctCount = 0;
String s_reads;



void loop() {

//  if(Serial2.available())
//  {
//    Serial.println("Serial2 avail");
//    s_reads = Serial2.readString();
//    s_reads.trim();
//
//    Serial.println(s_reads);
////    execute_command(s_reads);
//  }

  bus.update();
  uint16_t receive_num = bus.receive();
//  Serial.println(receive_num);
//Sensor Read List(Serial Command List)
// ph
// ec
// co2
// air_temp
// air_hum
// water_temp
// ldr

//Actuator Control List(Serial Command List)
// pump_ph_plus
// pump_ph_minus
// pump_water
// pump_solution_a
// pump_solution_b
// fan_air
// fan_ventil
// pump_air


  
//  if(millis() - last_msec > 30000)
//  {
//     매 30초 마다 아두이노의 경과 시간을 출력.(Serial 및 SD카드에 로그 기록)
//    Serial.println("[Elapsed Time in Loop()] " +  String(millis() - last_msec / 1000));
//    String log_data = String("[Elapsed_time]") + String(millis() - last_msec / 1000);
//    writeSD(log_data);
//    last_msec =millis();
//  }

//  execute_time_interval();
}

void execute_time_interval()
{
//  Serial.println("Execute_time_interval : " + String(millis()));

  if(millis() >= led_next_time)
  {
   
    led_next_time = millis() + led_interval;
    Serial.println("LED touch");
//    Serial.println("[LED INTERVAL]NEXT INTERVAL : " + String(led_next_time));
    if(led_status == true)
    {
      digitalWrite(ch16_relay[LED],HIGH);
      led_status = false;
    }
    else
    {
      digitalWrite(ch16_relay[LED],LOW);      
      led_status = true;
    }
  }
  
  if(millis() >= air_fan_next_time)
  {
    air_fan_next_time = millis() + air_fan_interval;
    Serial.println("air_fan touch");
//    Serial.println("[AIR_FAN INTERVAL]NEXT INTERVAL : " + String(air_fan_next_time));    
    if(air_fan_status == true)
    {
      digitalWrite(ch16_relay[AIR_FAN],HIGH);
      air_fan_status = false;
    }
    else
    {
      digitalWrite(ch16_relay[AIR_FAN],LOW);      
      air_fan_status = true;
    }    
  }
  if(millis() >= ventil_fan_next_time)
  {
    ventil_fan_next_time = millis() + ventil_fan_interval;
    Serial.println("ventil fan touch");
//    Serial.println("[VENTIL_FAN INTERVAL]NEXT INTERVAL : " + String(ventil_fan_next_time));    
    if(ventil_fan_status == true)
    {
      digitalWrite(ch16_relay[VENTIL_FAN],HIGH);
      ventil_fan_status = false;
    }
    else
    {
      digitalWrite(ch16_relay[VENTIL_FAN],LOW);      
      ventil_fan_status = true;
    }    
  }
  if(millis() >= air_pump_next_time)
  {
    air_pump_next_time = millis() + air_pump_interval;
    Serial.println("air_pump touch");
//    Serial.println("[AIR_PUMP INTERVAL]NEXT INTERVAL : " + String(air_pump_next_time));    
    if(air_pump_status == true)
    {
      digitalWrite(ch16_relay[AIR_PUMP],HIGH);
      air_pump_status = false;
    }
    else
    {
      digitalWrite(ch16_relay[AIR_PUMP],LOW);      
      air_pump_status = true;
    }    
  }

  
}



void execute_relay_order(int ord)
{
  Serial.println("Execute relay order");
  digitalWrite(ch16_relay[ord], LOW);
  delay(3000);
  digitalWrite(ch16_relay[ord],HIGH);
  Serial.println("[ch16_relay test] order is " + String(ord));
}


void execute_command(String cmd)
{
//Sensor Read List(Serial Command List)
// ph
// ec
// co2
// air_temp
// air_hum
// water_temp
// ldr
//Actuator Control List(Serial Command List)
// pump_ph_plus
// pump_ph_minus
// pump_water
// pump_solution_a
// pump_solution_b
// fan_air
// fan_ventil
// pump_air

//  Serial.println(cmd.toInt());
  if(cmd.toInt() < 16)
  {
    if(cmd.toInt() == 0 && cmd.length() > 1)
    {
      false;   
    }
    else
    {
      execute_relay_order(cmd.toInt());

      return true;
      
    }
  }
 
  if(cmd == "" || cmd.length() <= 1)
  {
    Serial.println("Wrong Command");
    return false;
  }

  if(cmd == "ph")
  {
    float ph_data = getPH();
    Serial.println("[Sensor]pH = " + String(ph_data));
    
  }
  else if(cmd == "ec")
  {
    float w_temp = getDS18B20();
    float ec_data = getEC(w_temp); 
    Serial.println("[Sensor]EC = " + String(ec_data));
  }
  else if(cmd == "co2")
  {
    float co2_data = getCO2();
    Serial.println("[Sensor]Co2 = " + String(co2_data)); 
  }
  else if(cmd == "air_temp")
  {
    float *dht_data =  getDHT();
    float air_temp_data = dht_data[1];
    Serial.println("[Sensor]Air_temp = " + String(air_temp_data));
  }
  else if(cmd == "air_hum")
  {
    float *dht_data =  getDHT();
    float air_hum_data = dht_data[0];
    Serial.println("[Sensor]Air_humidity = " + String(air_hum_data));        
  
  }
  else if(cmd == "water_temp")
  {
    float w_temp = getDS18B20();
    Serial.println("[Sensor]Water_temp = " + String(w_temp)); 
   
  }
  else if(cmd == "ldr")
  {
    unsigned int ldr_data = getLDR();
    Serial.println("[Sensor]LDR = " + String(ldr_data));
  }
// pump_ph_plus
// pump_ph_minus
// pump_water
// pump_solution_a
// pump_solution_b
// fan_air
// fan_ventil
// pump_air
// LED
  else if(cmd == "pump_ph_plus")
  {
    digitalWrite(ch16_relay[PH_PLUS_PUMP],LOW);
    delay(PUMP_PH_PLUS_DELAY);
    digitalWrite(ch16_relay[PH_PLUS_PUMP],HIGH);
    Serial.println("[Actuator]" + cmd);
  }
  else if(cmd == "pump_ph_minus")
  {
    digitalWrite(ch16_relay[PH_MINUS_PUMP],LOW);
    delay(PUMP_PH_MINUS_DELAY);    
    digitalWrite(ch16_relay[PH_MINUS_PUMP],HIGH);
    Serial.println("[Actuator]" + cmd);

  }
  else if(cmd == "pump_water")
  {
    digitalWrite(ch16_relay[WATER_PUMP], LOW);
    delay(PUMP_WATER_DELAY);    
    digitalWrite(ch16_relay[WATER_PUMP], HIGH);
    Serial.println("[Actuator]" + cmd);
  }
  else if(cmd == "pump_solution_a")
  {
    digitalWrite(ch16_relay[SOL_A_PUMP],LOW);
    delay(PUMP_SOLUTION_A_DELAY);
    digitalWrite(ch16_relay[SOL_A_PUMP],HIGH);
    Serial.println("[Actuator]" + cmd);
    
  }
  else if(cmd == "pump_solution_b")
  {
    digitalWrite(ch16_relay[SOL_B_PUMP],LOW);
    delay(PUMP_SOLUTION_B_DELAY);
    digitalWrite(ch16_relay[SOL_B_PUMP],HIGH);
    Serial.println("[Actuator]" + cmd);

  }
  else if(cmd == "on_fan_air")
  {
    digitalWrite(ch16_relay[AIR_FAN],LOW);
    Serial.println("[Actuator]" + cmd);
    
  }
  else if(cmd == "off_fan_air")
  {
    digitalWrite(ch16_relay[AIR_FAN],HIGH);
    Serial.println("[Actuator]" + cmd);
    
  }
  else if(cmd == "on_fan_ventil")
  {
    digitalWrite(ch16_relay[VENTIL_FAN],LOW);
    Serial.println("[Actuator]" + cmd);
    
  }
  else if(cmd == "off_fan_ventil")
  {
    digitalWrite(ch16_relay[VENTIL_FAN],HIGH);
    Serial.println("[Actuator]" + cmd);
       
  }
  else if(cmd == "on_pump_air")
  {
    digitalWrite(ch16_relay[AIR_PUMP], LOW);
    Serial.println("[Actuator]" + cmd);

  }
  else if(cmd == "off_pump_air")
  {
    digitalWrite(ch16_relay[AIR_PUMP], HIGH);
    Serial.println("[Actuator]" + cmd);
     
  }
  else if(cmd == "on_led")
  {
    digitalWrite(ch16_relay[LED],LOW);
    Serial.println("[Actuator]" + cmd);
     
  }
  else if(cmd == "off_led")
  {
    digitalWrite(ch16_relay[LED],HIGH);
    Serial.println("[Actuator]" + cmd);  
  }
  else
  {
    Serial.println("[Error] Command Not Exists");
  }
  
  
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



