#define APP_WIFI_CONNECT_TIME 30000 // allow 30ms to connect

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include "src/app_enum.h"
#include "src/app_wifi.h"


APP_STATUS_enum app_wifi_status = INITIALISE_REQ;
APP_WIFI_INIT_STATUS_enum app_wifi_init_status = APP_WIFI_INIT_TRY;

const char *apssid = "ESP8266_AP";
const int eepromStart = 0;

String app_wifi_deviceName = "";
String app_wifi_ssid = "";
String app_wifi_passPhrase = "";

uint32_t app_wifi_lastTime = 0;

bool app_wifi_apRunning = false;

void app_wifi_run()
{
   switch (app_wifi_status)
   {
   case INITIALISE_REQ:
      app_wifi_status = app_wifi_init();
      break;
   case INITIALISED:
      break;
   case OPERATE_REQ:
      break;
   case OPERATING:
      break;
   case SHUTDOWN_REQ:
      break;
   case SHUTTING_DOWN:
      break;
   default:
      break;
   }
}

APP_STATUS_enum app_wifi_init()
{

   if (app_wifi_init_status == APP_WIFI_INIT_FAILED)
   {
      app_wifi_setupAP();
      return INITIALISED;
   }
   else if (app_wifi_init_status == APP_WIFI_INIT_SUCCESS)
   {
      return INITIALISED;
   }
   else
   {
      app_wifi_init_status = app_wifi_connect();
      return INITIALISE_REQ;
   }
}

/*
 * Setup Wireless access point
 */
void app_wifi_setupAP()
{
   Serial.println("Configuring access point...");
   /* You can remove the password parameter if you want the AP to be open. */
   WiFi.softAP(apssid);
   //WiFi.softAP(ssid, password);
   app_wifi_apRunning = true;

#ifdef DEBUG
   IPAddress myIP = WiFi.softAPIP();
   Serial.print("AP Name: ");
   Serial.println(apssid);
   Serial.print("AP IP address: ");
   Serial.println(myIP);
#endif //DEBUG
}

/*****************************************************
** Use saved SSID and password to connect to WiFi 
*****************************************************/
APP_WIFI_INIT_STATUS_enum app_wifi_connect()
{
   if (app_wifi_init_status == APP_WIFI_INIT_TRY)
   {
#ifdef DEBUG
      Serial.println("Trying to connect to WiFi...");
      Serial.print("Saved SSID: ");
      Serial.println(app_wifi_ssid);

      Serial.print("Saved password: ");
      Serial.println(app_wifi_passPhrase);
#endif //DEBUG
      if (app_wifi_ssid.length() > 0)
      {
         WiFi.mode(WIFI_STA);

         if (app_wifi_passPhrase.length() > 0)
         {
            WiFi.begin(app_wifi_ssid.begin(), app_wifi_passPhrase.begin());
         }
         else
         {
            WiFi.begin(app_wifi_ssid.begin());
         }
         app_wifi_lastTime = millis();

#ifdef DEBUG
         Serial.println((app_wifi_lastTime + APP_WIFI_CONNECT_TIME));
         Serial.println(" - Wait for WiFi to connect...");
#endif //DEBUG
         return APP_WIFI_INIT_WAIT;
      }
      else
      {
#ifdef DEBUG
         Serial.println("No connection settings available!");
#endif //DEBUG
         return APP_WIFI_INIT_FAILED;
      }
   }
   else if (app_wifi_init_status == APP_WIFI_INIT_WAIT)
   {
      uint32_t ms = millis();
      if (ms > (app_wifi_lastTime + APP_WIFI_CONNECT_TIME))
      {
#ifdef DEBUG
         Serial.println("Connection failed! (Could not connect in 30 seconds)");
#endif //DEBUG
         return APP_WIFI_INIT_FAILED;
      }
      else if (WiFi.status() == WL_CONNECTED)
      {
#ifdef DEBUG
         Serial.println("\nSuccess!\n");
         Serial.print("IP address: ");
         Serial.println(WiFi.localIP());
#endif //DEBUG
         if (MDNS.begin(app_wifi_deviceName.begin()))
         {
#ifdef DEBUG
            Serial.println("MDNS responder started");
#endif //DEBUG
         }
         return APP_WIFI_INIT_SUCCESS;
      }
      delay(1); // This is to avoid the WDT resetting the firmware while waiting for the WiFi to connect
      return APP_WIFI_INIT_WAIT;
   }

   return APP_WIFI_INIT_FAILED; // Should never reach this statement
}
