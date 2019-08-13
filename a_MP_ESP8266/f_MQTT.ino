#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "src/app_mqtt.h"

String app_mqtt_server = "";
String app_mqtt_outTopic = "mqtt\\outTest";
String app_mqtt_inTopic = "mqtt\\inTest";
MQTT_DATA_struct app_mqtt_topics[MQTT_PUBSUB_COUNT];

APP_STATUS_enum app_mqtt_status = INITIALISE_REQ;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);
uint32_t app_mqtt_lastTime = 0;

void app_mqtt_run()
{
    switch (app_mqtt_status)
    {
    case INITIALISE_REQ:
        app_mqtt_status = app_mqtt_init();
        break;
    case INITIALISED:
        break;
    case OPERATE_REQ:
        app_mqtt_status = OPERATING;
        break;
    case OPERATING:
        app_mqtt_status = app_mqtt_operate();
        break;
    case SHUTDOWN_REQ:
        break;
    case SHUTTING_DOWN:
        break;
    }
}

APP_STATUS_enum app_mqtt_init()
{
    mqtt_client.setServer(app_mqtt_server.begin(), 1883);
    mqtt_client.setCallback(app_mqtt_msgCallback);
    return INITIALISED;
}

APP_STATUS_enum app_mqtt_operate()
{
    if (!mqtt_client.connected())
    {
        app_mqtt_reconnect();
    }
    mqtt_client.loop();

    return OPERATING;
}

void app_mqtt_reconnect()
{
    if (!mqtt_client.connected() &&
        app_mqtt_server.length() > 0 &&
        millis() > app_mqtt_lastTime + 5000)
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (mqtt_client.connect(clientId.c_str()))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            for (int n = 0; n < 1; n++)
            {
                if (String(app_mqtt_topics[n].topic).length() > 0)
                {
                    if (app_mqtt_topics[n].subscribe)
                    {
                        // ... and resubscribe
                        mqtt_client.subscribe(app_mqtt_topics[n].topic);
                        Serial.println("Subscribed to " + String(app_mqtt_topics[n].topic));
                    }
                    if (app_mqtt_topics[n].publish)
                    {
                        mqtt_client.publish(app_mqtt_topics[n].topic, "hello world");
                        Serial.println("Published to " + String(app_mqtt_topics[n].topic));
                    }
                }
            }
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            app_mqtt_lastTime = millis();
        }
    }
}

void app_mqtt_msgCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}