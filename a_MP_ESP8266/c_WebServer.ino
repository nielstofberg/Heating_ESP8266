#include "src/app_web.h"

ESP8266WebServer server(80);
bool app_wifi_reset_flag = false;
//EEPROM_DATA_struct lib_eeprom_config;

/*
 * Setup webserver to handle http requests
 */
void app_web_setup()
{
    server.on("/", handleRoot);
    server.on("/connect", handleConnect);
    server.on("/connected", handleConnected);
    server.on("/setpin", handleSetPin);
    server.on("/temphum", handleTempHum);
    server.on("/reset", handleReset);
    server.begin();
    Serial.println("HTTP server started");
}

/*
 * Handle any incoming web requests
 */
void app_web_run()
{
    server.handleClient();
}

/*
 * Return a request for the root page
 */
void handleRoot()
{
    String message = "Possible arguments are:<br/>";
    message += "<a href='./connected'><b>Show Current Setup</b></a><br/>";
    message += "<a href='./connect'><b>Device Setup</b></a><br/>";
    message += "<a href='./setpin'><b>Show Pin Status</b></a>Add [/D1=0] to url to set a pin<br/>";
    message += "<a href='./temphum'><b>Show Temperature/Humidity</b></a><br/>";
    message += "<a href='./reset'><b>Restart Module</b></a><br/>";
    server.send(200, "text/html", message);
}

void handleConnect()
{
    String devName = lib_eeprom_config.devname;
    String netId = lib_eeprom_config.ssid;
    String pw = lib_eeprom_config.password;
    String mqttServer = lib_eeprom_config.mqttServer;
    String topic1 = lib_eeprom_config.mqttData[0].topic;
    bool t1Pub = lib_eeprom_config.mqttData[0].publish;
    bool t1Sub = lib_eeprom_config.mqttData[0].subscribe;
    int t1Pin = lib_eeprom_config.mqttData[0].ioPinNumber;
    String html = "<html><body><h1>Connect to WiFi Network</h1>";
    html += "<form action=\"/connected\" method=\"post\">";
    html += "Device name: <input type=\"text\" name=\"devName\" value=\"" + devName + "\"><br>";
    html += "SSID: <input type=\"text\" name=\"ssid\" value=\"" + netId + "\"><br>";
    html += "Password: <input type=\"text\" name=\"password\" value=\"" + pw + "\"><br>";
    html += "MQTT Server: <input type=\"text\" name=\"mqttServer\" value=\"" + mqttServer + "\"><br>";
    for (int i = 0; i < MQTT_PUBSUB_COUNT; i++)
    {
        html += "Topic " + String(i) + ": <input type=\"text\" name=\"topic" + String(i) + "\" value=\"" + lib_eeprom_config.mqttData[i].topic + "\"> ";
        html += "Publish<input type=\"checkbox\" value=\"pub\" name=\"topic" + String(i) + "Pub\" " + (String)((lib_eeprom_config.mqttData[i].publish) ? "checked" : "") + "> ";
        html += "Subscribe<input type=\"checkbox\" value=\"sub\" name=\"topic" + String(i) + "Sub\" " + (String)((lib_eeprom_config.mqttData[i].subscribe) ? "checked" : "") + "><br>";
        html += "Topic " + String(i) + " Pin: <input type=\"text\" name=\"topic" + String(i) + "Pin\" value=\"" + (String)(lib_eeprom_config.mqttData[i].ioPinNumber) + "\"><br>";
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
    if (server.args() > 0)
    {
        for (int n = 0; n < MQTT_PUBSUB_COUNT; n++)
        {
            lib_eeprom_config.mqttData[n].publish = false;
            lib_eeprom_config.mqttData[n].subscribe = false;
        }
        for (uint8_t i = 0; i < server.args(); i++)
        {
            uint8_t len = server.arg(i).length() + 1;
            if (server.argName(i) == "devName")
            {
                len = (len < DEVNAME_LEN) ? len : DEVNAME_LEN;
                server.arg(i).toCharArray(lib_eeprom_config.devname, len);
            }
            else if (server.argName(i) == "ssid")
            {
                len = (len < SSID_LEN) ? len : SSID_LEN;
                server.arg(i).toCharArray(lib_eeprom_config.ssid, len);
            }
            else if (server.argName(i) == "password")
            {
                len = (len < PASSWORD_LEN) ? len : PASSWORD_LEN;
                server.arg(i).toCharArray(lib_eeprom_config.password, len);
            }
            else if (server.argName(i) == "mqttServer")
            {
                len = (len < SERVER_LEN) ? len : SERVER_LEN;
                server.arg(i).toCharArray(lib_eeprom_config.mqttServer, len);
            }
            else
            {
                for (int n = 0; n < MQTT_PUBSUB_COUNT; n++)
                {
                    if (server.argName(i) == ("topic" + String(n)))
                    {
                        len = (len < MQTT_TOPIC_LEN) ? len : MQTT_TOPIC_LEN;
                        server.arg(i).toCharArray(lib_eeprom_config.mqttData[n].topic, len);
                    }
                    else if (server.argName(i) == "topic" + String(n) + "Pub")
                    {
                        lib_eeprom_config.mqttData[n].publish = true;
                    }
                    else if (server.argName(i) == "topic" + String(n) + "Sub")
                    {
                        lib_eeprom_config.mqttData[n].subscribe = true;
                    }
                    else if (server.argName(i) == "topic" + String(n) + "Pin")
                    {
                        lib_eeprom_config.mqttData[n].ioPinNumber = server.arg(i).toInt();
                    }
                }
            }
        }
        int count = lib_eeprom_writeAnything(0, lib_eeprom_config);
    }

    String message = "<h1>Current Setup</h1>";
    message += "<b>Arguments:</b> ";
    message += server.args();
    message += "<br/>";
    message += "<table>";
    message += "<tr><td><b>Device Name:</b></td><td>" + String(lib_eeprom_config.devname) + "</td></tr>";
    message += "<tr><td><b>FiFi SSID:</b></td><td>" + String(lib_eeprom_config.ssid) + "</td></tr>";
    message += "<tr><td><b>WiFi Password</b></td><td>" + String(lib_eeprom_config.password) + "</td></tr>";
    message += "<tr><td><b>MQTT Server</b></td><td>" + String(lib_eeprom_config.mqttServer) + "</td></tr>";
    message += "</table>";
    message += "<table>";
    message += "<tr><th>ID</th><th>Topic</th><th>Pin Nr</th><th>Publish</th><th>Subscribe</th></tr>";

    for (uint8_t m = 0; m < MQTT_PUBSUB_COUNT; m++ ) 
    {
        message += "<tr><td>" + String(m+1) + "</td>";
        message += "<td>" + String(lib_eeprom_config.mqttData[m].topic) + "</td>";
        message += "<td>" + String((lib_eeprom_config.mqttData[m].publish || lib_eeprom_config.mqttData[m].subscribe) ? 
            String(lib_eeprom_config.mqttData[m].ioPinNumber): "-") + "</td>";
        message += "<td>" + String((lib_eeprom_config.mqttData[m].publish) ? "Yes": "No") + "</td>";
        message += "<td>" + String((lib_eeprom_config.mqttData[m].subscribe) ? "Yes": "No") + "</td></tr>";
    }
    message += "</table>";

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

void handleReset()
{
    String message = "<h1>Rebooting Device</h1>";
    message += "<h2><a href='../'>Home</a></h2>";
    server.send(200, "text/html", message);
    app_wifi_reset_flag = true;
}
