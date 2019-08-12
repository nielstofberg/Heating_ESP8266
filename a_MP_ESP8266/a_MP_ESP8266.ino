#include <Arduino.h>
#include <Esp.h>
#include <EEPROM.h>
#include <PubSubClient.h>  // This is the next thing to add!!!
#include "src/lib_eeprom_data.h"
#include "src/lib_eeprom_rw_anything.h"
#include "src/pins_arduino.h"
#include "src/app_enum.h"
#include "src/app_wifi.h"

#define DEBUG
#define HAPPY_LED D4

const int checkVal = 12345;
int ledState = HIGH;
unsigned long timer = 0;
unsigned long happyTime = 500;
EEPROM_DATA_struct configuration;

void configinit()
{
   configuration.checkVal = checkVal;
   ((String) "jimmy").toCharArray(configuration.devname, 8);
   ((String) "").toCharArray(configuration.ssid, 1);
   ((String) "").toCharArray(configuration.password, 1);

   int count = lib_eeprom_writeAnything(0, configuration);
}

void setup()
{
   EEPROM.begin(512);
   Serial.begin(115200);
   lib_eeprom_readAnything(0, configuration);

#ifdef DEBUG
   Serial.println("This is the current configuration");
   Serial.println(configuration.checkVal);
   Serial.println(configuration.devname);
   Serial.println(configuration.ssid);
   Serial.println(configuration.password);
#endif // DEBUG

   if (configuration.checkVal == checkVal)
   {
      app_wifi_deviceName = configuration.devname;
      app_wifi_ssid = configuration.ssid;
      app_wifi_passPhrase = configuration.password;
   }
   else
   {
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

   while (true)
   {
      app_wifi_run();

      if (app_wifi_status == INITIALISED)
      {
         app_wifi_status = OPERATE_REQ;
         break;
      }
   }

   setupWebServer();
   udpSetup();
}

void loop()
{
   handleWebRequest();
   handleUdpRequest();

   if (millis() - timer >= happyTime)
   {
      ledState = (ledState == HIGH) ? LOW : HIGH;
      timer = millis();
      digitalWrite(HAPPY_LED, ledState);
   }
}

void setPin(String pin, int stat)
{
   unsigned int p = 0;
   if (pin == "D0")
      p = D0;
   else if (pin == "D1")
      p = D1;
   else if (pin == "D2")
      p = D2;
   else if (pin == "D3")
      p = D3;
   else if (pin == "D4")
      p = D4;
   else if (pin == "D5")
      p = D5;
   else if (pin == "D6")
      p = D6;
   else if (pin == "D7")
      p = D7;
   else if (pin == "D8")
      p = D8;
   digitalWrite(p, stat);
}
