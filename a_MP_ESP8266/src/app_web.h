#ifndef _APP_WEB_H_
#define _APP_WEB_H_

#include <ESP8266WebServer.h>
#include "lib_eeprom_data.h"
#include "lib_eeprom_rw_anything.h"
#include "pins_arduino.h"
#include "app_mqtt.h"
#include "app_gpio.h"
#include "app_dht.h"

void app_web_setup(void);
void app_web_run(void);

#endif // _APP_WEB_H_
