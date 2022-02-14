/**
 * Version 1.1 
 * 
 */
#include <Arduino.h>
#include <Esp.h>
#include <EEPROM.h>
#include "src/pins_arduino.h"
#include "src/lib_eeprom_data.h"
#include "src/lib_eeprom_rw_anything.h"
#include "src/app_enum.h"
#include "src/app_gpio.h"
#include "src/app_wifi.h"
#include "src/app_mqtt.h"
#include "src/app_dht.h"
#include "src/app_web.h"
#include "src/app_udp.h"

#define DEBUG
#define HAPPY_LED D4
#define HAPPY_ON    900
#define HAPPY_OFF   1023

const int checkVal = 12345;
int ledState = HAPPY_ON;
unsigned long timer = 0;
unsigned long reset_timer = 0;
unsigned long happyTime = 500;
EEPROM_DATA_struct lib_eeprom_config;

void configinit()
{
    lib_eeprom_config.checkVal = checkVal;
    ((String) "ESP8266").toCharArray(lib_eeprom_config.devname, 8);
    ((String) "").toCharArray(lib_eeprom_config.ssid, 1);
    ((String) "").toCharArray(lib_eeprom_config.password, 1);
    ((String) "").toCharArray(lib_eeprom_config.mqttServer, 1);

    int count = lib_eeprom_writeAnything(0, lib_eeprom_config);
}

void setup()
{
    EEPROM.begin(512);
    Serial.begin(115200);
    lib_eeprom_readAnything(0, lib_eeprom_config);

#ifdef DEBUG
    Serial.println("This is the current configuration");
    Serial.println(lib_eeprom_config.checkVal);
    Serial.println(lib_eeprom_config.devname);
    Serial.println(lib_eeprom_config.ssid);
    Serial.println(lib_eeprom_config.password);
#endif // DEBUG

    if (lib_eeprom_config.checkVal == checkVal)
    {
        app_wifi_deviceName = lib_eeprom_config.devname;
        app_wifi_ssid = lib_eeprom_config.ssid;
        app_wifi_passPhrase = lib_eeprom_config.password;
        app_mqtt_server = lib_eeprom_config.mqttServer;
        app_mqtt_username = lib_eeprom_config.mqttUsername;
        app_mqtt_password = lib_eeprom_config.mqttPassword;
        for (int n = 0; n < MQTT_PUBSUB_COUNT; n++)
        {
            sprintf(app_mqtt_topics[n].topic, "%s\\%s\0",lib_eeprom_config.devname, lib_eeprom_config.mqttData[n].topic);
            //String(lib_eeprom_config.mqttData[n].topic).toCharArray(app_mqtt_topics[n].topic, String(lib_eeprom_config.mqttData[n].topic).length()+1);
            app_mqtt_topics[n].publish = lib_eeprom_config.mqttData[n].publish;
            app_mqtt_topics[n].subscribe = lib_eeprom_config.mqttData[n].subscribe;
            app_mqtt_topics[n].ioPinNumber = lib_eeprom_config.mqttData[n].ioPinNumber;
        }
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
    analogWrite(HAPPY_LED, HAPPY_ON);
    //digitalWrite(HAPPY_LED, HIGH);
    digitalWrite(D5, HIGH);
    digitalWrite(D6, HIGH);
    digitalWrite(D7, HIGH);
    digitalWrite(D8, HIGH);

    app_dht_enabled = true;

    while (true)
    {
        app_wifi_run();
        if (app_wifi_status == INITIALISED)
        {
            app_wifi_status = OPERATE_REQ;
            break;
        }
    }
    app_web_setup();
    app_udp_setup();
    app_mqtt_run();
    app_dht_run();

    if (app_mqtt_status == INITIALISED)
    {
        Serial.println("MQTT Started");
        app_mqtt_status = OPERATE_REQ;
    }
    if (app_dht_status == INITIALISED)
    {
        Serial.println("DHT Started");
        app_dht_status = OPERATE_REQ;
    }
}

void loop()
{
    app_mqtt_run();
    app_dht_run();
    app_web_run();
    app_udp_run();

    if (millis() - timer >= happyTime)
    {
        ledState = (ledState == HAPPY_ON) ? HAPPY_OFF : HAPPY_ON;
        timer = millis();
        analogWrite(HAPPY_LED, ledState);
    }

    if (app_dht_valuesUpdated)
    {
        app_mqtt_publish(app_wifi_deviceName + "\\Temperature", String(app_dht_temperature));
        app_mqtt_publish(app_wifi_deviceName + "\\Humidity", String(app_dht_humidity));
        app_dht_valuesUpdated = false;
        // send values to mqtt broker
    }

    if (app_mqtt_refreshFlag)
    {
        // Publish current statusses
        for(int i = 0; i < MQTT_PUBSUB_COUNT; i++)
        {
            app_mqtt_publish(String(app_mqtt_topics[i].topic), String(app_gpio_getPin("D" + String(app_mqtt_topics[i].ioPinNumber))));
        }
        app_mqtt_refreshFlag = false;
    }
    if (app_wifi_reset_flag)
    {
        if (reset_timer == 0)
        {
            reset_timer = millis();
        }
        else if ((millis()-reset_timer) > 5000)
        {
            reset_timer == 0;
            Serial.println("Resetting ESP");
            ESP.restart(); //ESP.reset();
        }
    }
}