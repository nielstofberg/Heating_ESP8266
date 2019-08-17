#include <Arduino.h>
#include <Esp.h>
#include <EEPROM.h>
#include "src/pins_arduino.h"
#include "src/lib_eeprom_data.h"
#include "src/lib_eeprom_rw_anything.h"
#include "src/app_enum.h"
#include "src/app_wifi.h"
#include "src/app_mqtt.h"
#include "src/app_dht.h"

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
    ((String) "ESP8266").toCharArray(configuration.devname, 8);
    ((String) "").toCharArray(configuration.ssid, 1);
    ((String) "").toCharArray(configuration.password, 1);
    ((String) "").toCharArray(configuration.mqttServer, 1);

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
        app_mqtt_server = configuration.mqttServer;
        for (int n = 0; n < MQTT_PUBSUB_COUNT; n++)
        {
            String(configuration.mqttData[n].topic).toCharArray(app_mqtt_topics[n].topic, String(configuration.mqttData[n].topic).length()+1);
            app_mqtt_topics[n].publish = configuration.mqttData[n].publish;
            app_mqtt_topics[n].subscribe = configuration.mqttData[n].subscribe;
            app_mqtt_topics[n].ioPinNumber = configuration.mqttData[n].ioPinNumber;
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
    analogWrite(HAPPY_LED, 900);
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
    setupWebServer();
    udpSetup();
    app_mqtt_run();
    app_dht_run();

    if (app_mqtt_status == INITIALISED)
    {
        Serial.println("MQTT Started");
        app_mqtt_status = OPERATE_REQ;
    }
    if (app_dht_status == INITIALISED)
    {
        Serial.println("MQTT Started");
        app_dht_status = OPERATE_REQ;
    }
}

void loop()
{
    app_mqtt_run();
    app_dht_run();
    handleWebRequest();
    handleUdpRequest();

    if (millis() - timer >= happyTime)
    {
        ledState = (ledState == HIGH) ? LOW : HIGH;
        timer = millis();
        //digitalWrite(HAPPY_LED, ledState);
    }

    if (app_dht_valuesUpdated)
    {
        app_mqtt_publish(app_wifi_deviceName + "\\Temperature", String(app_dht_temperature));
        app_mqtt_publish(app_wifi_deviceName + "\\Humidity", String(app_dht_humidity));
        app_dht_valuesUpdated = false;
        // send values to mqtt broker
    }
}