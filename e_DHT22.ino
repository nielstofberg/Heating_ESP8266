#include <DHT.h>

#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);

void SetupDht22(){
  
}

float GetTempratureC() {
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  return t;
}

float GetTempratureF() {
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  return f;
}

float GetHumidity() {
  float h = dht.readHumidity();
  return h;
}
