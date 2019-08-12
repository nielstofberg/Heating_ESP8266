#ifndef _app_wifi_h_
#define _app_wifi_h_

#include <wstring.h>
#include "app_enum.h"

typedef enum{
    APP_WIFI_INIT_TRY = 't',
    APP_WIFI_INIT_WAIT = 'w',
    APP_WIFI_INIT_FAILED = 'f',
    APP_WIFI_INIT_SUCCESS = 's'
} APP_WIFI_INIT_STATUS_enum;

extern APP_STATUS_enum app_wifi_status;
extern String app_wifi_deviceName;
extern String app_wifi_ssid;
extern String app_wifi_passPhrase;

#endif // _app_wifi_h_