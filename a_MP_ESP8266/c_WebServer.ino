#include <ESP8266WebServer.h>
#include "src/lib_eeprom_data.h"

ESP8266WebServer server(80);

/*
 * Setup webserver to handle http requests
 */
void setupWebServer() {
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
void handleWebRequest() {
  server.handleClient();
}

/*
 * Return a request for the root page
 */
void handleRoot() {
  String message = "Possible arguments are:<br/>";
  message += "./connect<br/>";
  message += "./setpin[/D1=0]<br/>";
  message += "./temphum<br/>";
  server.send(200, "text/html", message);
}


void handleConnect() {
  String devName = configuration.devname;
  String netId = configuration.ssid;
  String pw = configuration.password;
  String html = "<html><body><h1>Connect to WiFi Network</h1>";
  html += "<form action=\"/connected\" method=\"post\">";
  html += "Device name: <input type=\"text\" name=\"devName\" value=\""+ devName + "\"><br>";
  html += "SSID: <input type=\"text\" name=\"ssid\" value=\""+ netId + "\"><br>";
  html += "Password: <input type=\"text\" name=\"password\" value=\""+ pw + "\"><br>";
  html += "<input type=\"submit\" value=\"Connect\"></form><p>Click the \"Submit\" button.</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

/*
 * Rerurn request for the Connect page.
 * This page gets the ssid and password for the 
 * WiFi the user is connect to 
 */
void handleConnected() {
  int memAddr = eepromStart;
  String message = "<h1>Connect</h1>";
  message += "URI: ";
  message += server.uri();
  message += "<br/>Method: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "<br/>Arguments: ";
  message += server.args();
  message += "<br/>";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + " LEN=" ;
    message += server.arg(i).length();
    message += "<br/>";
    uint8_t len = server.arg(i).length() + 1;
    if (server.argName(i) == "devName") {
      len = (len < DEVNAME_LEN) ? len : DEVNAME_LEN;
      server.arg(i).toCharArray(configuration.devname,len);
    }
    else if (server.argName(i) == "ssid") {
      len = (len < SSID_LEN) ? len : SSID_LEN;
      server.arg(i).toCharArray(configuration.ssid,len);
    }
    else if (server.argName(i) == "password") {
      len = (len < PASSWORD_LEN) ? len : PASSWORD_LEN;
      server.arg(i).toCharArray(configuration.password,len);
    }
  }

  int count =  lib_eeprom_writeAnything(0, configuration);
  server.send(200, "text/html", message);
}

/*
 * 
 */
void handleSetPin() {
  String message = "";
  int dir = server.method();
  if (server.args() > 0)
  {
    for (uint8_t i = 0; i < server.args(); i++) {
      String pin = server.argName(i);
      int stat = (server.arg(i) == "0") ? LOW: HIGH;
      setPin(pin,stat);
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
  }
  else
  {
    message += "{\"D0\":"; 
    message += (digitalRead(D0) == HIGH) ? "1" : "0";        
    message += ", \"D1\":"; 
    message += (digitalRead(D1) == HIGH) ? "1" : "0";        
    message += ", \"D2\":"; 
    message += (digitalRead(D2) == HIGH) ? "1" : "0";        
    message += ", \"D3\":"; 
    message += (digitalRead(D3) == HIGH) ? "1" : "0";        
    message += ", \"D4\":"; 
    message += (digitalRead(D4) == HIGH) ? "1" : "0";        
    message += ", \"D5\":"; 
    message += (digitalRead(D5) == HIGH) ? "1" : "0";        
    message += ", \"D6\":"; 
    message += (digitalRead(D6) == HIGH) ? "1" : "0";        
    message += ", \"D7\":"; 
    message += (digitalRead(D7) == HIGH) ? "1" : "0";        
    message += ", \"D8\":"; 
    message += (digitalRead(D8) == HIGH) ? "1" : "0";        
    message += "}"; 
  }
  server.send(200, "text/plain", message);
}

void handleTempHum() {
  String message = "{\"temp\":";
  float temp = GetTempratureC();
  float hum = GetHumidity();
  message += temp;
  message += ", \"hum\":";
  message += hum;
  message += "}";
  server.send(200, "text/plain", message);  
}
