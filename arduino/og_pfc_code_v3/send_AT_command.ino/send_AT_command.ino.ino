#define BLYNK_PRINT Serial // Blynk Print Serial. This "define" should place on top of sketch
#define BLYNK_MAX_SENDBYTES 500 // set Limit Blynk Symbol Number(include Subject + body)

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

#define ESP_SERIAL Serial2
#define ESP_BAUD 115200
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ESP_SERIAL.begin(ESP_BAUD);


//Major Commands
//AT+RST
//AT+CWMODE
//AT+CWJAP
//AT+CIPSTART
//AT+CIPSEND
//AT+CIFSR
//
//
//
//
//

}

void loop() {
  // put your main code here, to run repeatedly:

//  Serial.println(ESP_SERIAL.available());
  
  while(Serial.available()){
    ESP_SERIAL.write(Serial.read());
  }
  while(ESP_SERIAL.available()){
    Serial.write(ESP_SERIAL.read());
  }
}
