
#define UDP_PORT 5454
const String GET_ID = "GET_ESP8266_ID";

WiFiUDP Udp;

char packetBuffer[255]; //buffer to hold incoming packet

void udpSetup() {
  Udp.begin(UDP_PORT);
  Serial.print("UDP Server started listening on port: ");
  Serial.println(UDP_PORT);
}

void handleUdpRequest() {
    // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    String devname = configuration.devname;
    devname = "ID_" + devname;
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    String rec = packetBuffer;
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    if (rec == GET_ID) {
      // send a reply, to the IP address and port that sent us the packet we received
      Udp.beginPacket(Udp.remoteIP(), UDP_PORT);
      Udp.print(devname);
      Udp.endPacket();
      Serial.print("Reply sent to ");
      Serial.print(remoteIp);
      Serial.print(" on port ");
      Serial.println(UDP_PORT);
      Serial.print("Reply message: ");
      Serial.println(devname);            
    }
  }
}

