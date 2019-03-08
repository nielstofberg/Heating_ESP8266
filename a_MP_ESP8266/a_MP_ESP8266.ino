#include <EEPROM.h>
#include "Structures.h"
#include "EEPROMAnything.h"

#define HAPPY_LED D4
const int checkVal = 12345;
int ledState = HIGH;
unsigned long timer = 0;
unsigned long happyTime = 500;
configData_t configuration;

void configinit() {
  configuration.checkVal = checkVal;
  ((String)"ESP8266").toCharArray(configuration.devname,8);
  ((String)"").toCharArray(configuration.ssid,1);
  ((String)"").toCharArray(configuration.password,1);

  int count =  EEPROM_writeAnything(0, configuration);
  Serial.print("Configuration initialised: ");
  Serial.println(count);
  Serial.println(configuration.checkVal);
  Serial.println(configuration.devname);
  Serial.println(configuration.ssid);
  Serial.println(configuration.password);
}

void setup() {
  EEPROM.begin(512);
  Serial.begin(115200);
  Serial.print("BUILTIN_LED: ");
  Serial.println(BUILTIN_LED);

  EEPROM_readAnything(0, configuration);
  Serial.println("This is the current configuration");
  Serial.println(configuration.checkVal);
  Serial.println(configuration.devname);
  Serial.println(configuration.ssid);
  Serial.println(configuration.password);

  if (configuration.checkVal != checkVal) {
    configinit();
  }  
  pinMode(HAPPY_LED, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  digitalWrite(HAPPY_LED, HIGH);
  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
  digitalWrite(D7, HIGH);
  digitalWrite(D8, HIGH);
  if (!ConnectToWiFi()) {
    setupAp();
  }
  setupWebServer();
  udpSetup();
}

void loop() {
  handleWebRequest();
  handleUdpRequest();
  
  if (millis()-timer>=happyTime)
  {
    ledState = (ledState==HIGH)? LOW: HIGH;
    timer = millis();
    digitalWrite(HAPPY_LED, ledState);
  }
}

void setPin(String pin, int stat) {
  unsigned int p = 0;
  if (pin == "D0") p = D0;
  else if (pin == "D1") p = D1;
  else if (pin == "D2") p = D2;
  else if (pin == "D3") p = D3;
  else if (pin == "D4") p = D4;
  else if (pin == "D5") p = D5;
  else if (pin == "D6") p = D6;
  else if (pin == "D7") p = D7;
  else if (pin == "D8") p = D8;
  digitalWrite(p, stat);
}
