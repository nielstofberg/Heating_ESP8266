#include <ESP8266WiFi.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>

const char *apssid = "ESPap";
const int eepromStart = 0;
 
/*
 * Setup Wireless access point
 */
void setupAp(){
  Serial.print("Configuring access point...");  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(apssid);
  //WiFi.softAP(ssid, password);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);  
}

/*****************************************************
** Use saved SSID and password to connect to WiFi 
*****************************************************/
bool ConnectToWiFi() {
  Serial.println("Trying to connect to WiFi...");
  Serial.print("Saved SSID: ");
  Serial.println(configuration.ssid);

  Serial.print("Saved password: ");
  Serial.println(configuration.password);

  if (configuration.ssid[0] != '\0') {
    WiFi.mode(WIFI_STA);
    
    if (configuration.password[0] != '\0'){
      WiFi.begin(configuration.ssid, configuration.password);
    }
    else {
      WiFi.begin(configuration.ssid);
    }
    // Wait 30 seconds for connection
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      if (++counter > 60)
      {
        Serial.println("Connection failed! (Could not connect in 30 seconds)");
        return false;
      }
    }
  }
  else {
    Serial.println("No connection settings available!");  
    return false;
  }
  Serial.println("\nSuccess!\n");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(configuration.devname)) {
    Serial.println("MDNS responder started");
  }
  happyTime = 1000;
  return true;
}
