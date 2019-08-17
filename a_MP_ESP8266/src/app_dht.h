#ifndef _app_dht_h_
#define _app_dht_h_

#include <stdint.h>
#include "app_enum.h"

extern APP_STATUS_enum app_dht_status;

extern bool app_dht_enabled;
extern bool app_dht_valuesUpdated;
extern float app_dht_temperature;
extern float app_dht_humidity;
extern uint32_t interval;

void app_dht_run(void);

#endif // _app_dht_h_