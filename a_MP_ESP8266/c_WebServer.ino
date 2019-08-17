#include <ESP8266WebServer.h>
#include "src/lib_eeprom_data.h"
#include "src/lib_eeprom_rw_anything.h"
#include "src/pins_arduino.h"
#include "src/app_mqtt.h"
#include "src/app_gpio.h"
#include "src/app_dht.h"

ESP8266WebServer server(80);

/*
 * Setup webserver to handle http requests
 */
void setupWebServer()
{
    server.on("/", handleRoot);
    server.on("/connect", handleConnect);
    server.on("/connected", handleConnected);
    server.on("/setpin", handleSetPin);
    server.on("/temphum", handleTempHum);
    server.begin();
    Serial.println("HTTP server started");
}

/*
 * Handle any incoming web requests
 */
void handleWebRequest()
{
    server.handleClient();
}

/*
 * Return a request for the root page
 */
void handleRoot()
{
    String message = "Possible arguments are:<br/>";
    message += "./connect<br/>";
    message += "./setpin[/D1=0]<br/>";
    message += "./temphum<br/>";
    server.send(200, "text/html", message);
}

void handleConnect()
{
    String devName = configuration.devname;
    String netId = configuration.ssid;
    String pw = configuration.password;
    String mqttServer = configuration.mqttServer;
    String topic1 = configuration.mqttData[0].topic;
    bool t1Pub = configuration.mqttData[0].publish;
    bool t1Sub = configuration.mqttData[0].subscribe;
    int t1Pin = configuration.mqttData[0].ioPinNumber;
    String html = "<html><body><h1>Connect to WiFi Network</h1>";
    html += "<form action=\"/connected\" method=\"post\">";
    html += "Device name: <input type=\"text\" name=\"devName\" value=\"" + devName + "\"><br>";
    html += "SSID: <input type=\"text\" name=\"ssid\" value=\"" + netId + "\"><br>";
    html += "Password: <input type=\"text\" name=\"password\" value=\"" + pw + "\"><br>";
    html += "MQTT Server: <input type=\"text\" name=\"mqttServer\" value=\"" + mqttServer + "\"><br>";
    for (int i = 0; i < MQTT_PUBSUB_COUNT; i++)
    {
        html += "Topic " + String(i) + ": <input type=\"text\" name=\"topic" + String(i) + "\" value=\"" + configuration.mqttData[i].topic + "\"> ";
        html += "Publish<input type=\"checkbox\" value=\"pub\" name=\"topic" + String(i) + "Pub\" " + (String)((configuration.mqttData[i].publish) ? "checked" : "") + "> ";
        html += "Subscribe<input type=\"checkbox\" value=\"sub\" name=\"topic" + String(i) + "Sub\" " + (String)((configuration.mqttData[i].subscribe) ? "checked" : "") + "><br>";
        html += "Topic " + String(i) + " Pin: <input type=\"text\" name=\"topic" + String(i) + "Pin\" value=\"" + (String)(configuration.mqttData[i].ioPinNumber) + "\"><br>";
    }

    html += "<input type=\"submit\" value=\"Subscribe\"></form><p>Click the \"Submit\" button.</p>";
    html += "</body></html>";

    server.send(200, "text/html", html);
}

/*
 * Rerurn request for the Connect page.
 * This page gets the ssid and password for the 
 * WiFi the user is connect to 
 */
void handleConnected()
{
    //int memAddr = eepromStart;
    for (int n = 0; n < MQTT_PUBSUB_COUNT; n++)
    {
        configuration.mqttData[n].publish = false;
        configuration.mqttData[n].subscribe = false;
    }
    String message = "<h1>Connect</h1>";
    message += "URI: ";
    message += server.uri();
    message += "<br/>Method: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "<br/>Arguments: ";
    message += server.args();
    message += "<br/>";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + " LEN=";
        message += server.arg(i).length();
        message += "<br/>";
        uint8_t len = server.arg(i).length() + 1;
        if (server.argName(i) == "devName")
        {
            len = (len < DEVNAME_LEN) ? len : DEVNAME_LEN;
            server.arg(i).toCharArray(configuration.devname, len);
        }
        else if (server.argName(i) == "ssid")
        {
            len = (len < SSID_LEN) ? len : SSID_LEN;
            server.arg(i).toCharArray(configuration.ssid, len);
        }
        else if (server.argName(i) == "password")
        {
            len = (len < PASSWORD_LEN) ? len : PASSWORD_LEN;
            server.arg(i).toCharArray(configuration.password, len);
        }
        else if (server.argName(i) == "mqttServer")
        {
            len = (len < SERVER_LEN) ? len : SERVER_LEN;
            server.arg(i).toCharArray(configuration.mqttServer, len);
        }
        else
        {
            for (int n = 0; n < MQTT_PUBSUB_COUNT; n++)
            {
                if (server.argName(i) == ("topic" + String(n)))
                {
                    len = (len < MQTT_TOPIC_LEN) ? len : MQTT_TOPIC_LEN;
                    server.arg(i).toCharArray(configuration.mqttData[n].topic, len);
                }
                else if (server.argName(i) == "topic" + String(n) + "Pub")
                {
                    configuration.mqttData[n].publish = true;
                }
                else if (server.argName(i) == "topic" + String(n) + "Sub")
                {
                    configuration.mqttData[n].subscribe = true;
                }
                else if (server.argName(i) == "topic" + String(n) + "Pin")
                {
                    configuration.mqttData[n].ioPinNumber = server.arg(i).toInt();
                }
            }
        }
    }
    int count = lib_eeprom_writeAnything(0, configuration);
    server.send(200, "text/html", message);
}

/*
 * 
 */
void handleSetPin()
{
    String message = "";
    int dir = server.method();
    if (server.args() > 0)
    {
        for (uint8_t i = 0; i < server.args(); i++)
        {
            String pin = server.argName(i);
            int stat = (server.arg(i) == "0") ? LOW : HIGH;
            app_gpio_setPin(pin, stat);
            message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
        }
    }
    else
    {
        message += "{\"D0\":" + String(digitalRead(D0));
        //message += (digitalRead(D0) == HIGH) ? "1" : "0";
        message += ", \"D1\":" + String(digitalRead(D1));
        //message += (digitalRead(D1) == HIGH) ? "1" : "0";
        message += ", \"D2\":" + String(digitalRead(D2));
        //message += (digitalRead(D2) == HIGH) ? "1" : "0";
        message += ", \"D3\":" + String(digitalRead(D3));
        //message += (digitalRead(D3) == HIGH) ? "1" : "0";
        message += ", \"D4\":" + String(digitalRead(D4));
        //message += (digitalRead(D4) == HIGH) ? "1" : "0";
        message += ", \"D5\":" + String(digitalRead(D5));
        //message += (digitalRead(D5) == HIGH) ? "1" : "0";
        message += ", \"D6\":" + String(digitalRead(D6));
        //message += (digitalRead(D6) == HIGH) ? "1" : "0";
        message += ", \"D7\":" + String(digitalRead(D7));
        //message += (digitalRead(D7) == HIGH) ? "1" : "0";
        message += ", \"D8\":" + String(digitalRead(D8));
        //message += (digitalRead(D8) == HIGH) ? "1" : "0";
        message += "}";
    }
    server.send(200, "text/plain", message);
}

void handleTempHum()
{
    String message = "{\"temp\":";
    float temp = app_dht_temperature;
    float hum = app_dht_humidity;
    message += temp;
    message += ", \"hum\":";
    message += hum;
    message += "}";
    server.send(200, "text/plain", message);
}
