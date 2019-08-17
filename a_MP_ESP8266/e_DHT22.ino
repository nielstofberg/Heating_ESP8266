#define ARDUINO 100

#include <DHT.h>
#include "src/pins_arduino.h"
#include "src/app_enum.h"

#define DHTPIN          D2    // what digital pin the DHT22 is conected to
#define DHTTYPE         DHT22 // there are multiple kinds of DHT sensors

#define MIN_INTERVAL    2000

DHT dht(DHTPIN, DHTTYPE);

APP_STATUS_enum app_dht_status = INITIALISE_REQ;
bool app_dht_enabled = false;
bool app_dht_valuesUpdated = false;
float app_dht_temperature = 0;
float app_dht_humidity = 0;
uint32_t interval = 5000;

uint32_t lastRead = 0;

void app_dht_run()
{
    switch (app_dht_status)
    {
    case INITIALISE_REQ:
        app_dht_status = app_dht_init();
        break;
    case INITIALISED:
        break;
    case OPERATE_REQ:
        app_dht_status = OPERATING;
        break;
    case OPERATING:
        app_dht_status = app_dht_operate();
        break;
    case SHUTDOWN_REQ:
        break;
    case SHUTTING_DOWN:
        break;
    }
}

APP_STATUS_enum app_dht_init()
{
    if (!app_dht_enabled) return INITIALISE_REQ;
    dht.begin();
    lastRead = millis();
    if (interval < MIN_INTERVAL)
    {
        /**
         * Make sure the sensor is not read less than 2 seconds after the dht is started
         */
        lastRead += MIN_INTERVAL - interval;
    }
    return INITIALISED;
}

APP_STATUS_enum app_dht_operate()
{
    float temp = 0;
    float hum = 0;
    if ((lastRead + interval) < millis())
    {
        hum = dht.readHumidity();
        temp = dht.readTemperature();

        if (isnan(temp) || isnan(hum))
        {
            /**
             * If the sensor read failed, set the time interval to the minimum to re-read.
             */
            lastRead += interval - MIN_INTERVAL;
            Serial.println("TH Read failed");
        }
        else
        {
            lastRead = millis();
            app_dht_valuesUpdated = true;
            app_dht_humidity = hum;
            app_dht_temperature = temp;
            Serial.println("Temp:\t" + String(temp));
            Serial.println("Hum:\t" + String(hum));
        }
    }
    return OPERATING;
}