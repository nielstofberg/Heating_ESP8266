#ifndef _lib_eeprom_data_h_
#define _lib_eeprom_data_h_

#include "app_mqtt.h"

#define DEVNAME_LEN   15
#define SSID_LEN      30
#define PASSWORD_LEN  50

#define SERVER_LEN    50

typedef struct
{
  int checkVal;
  char devname[DEVNAME_LEN];
  char ssid[SSID_LEN];
  char password[PASSWORD_LEN];
  char mqttServer[SERVER_LEN];
  char mqttUsername[MQTT_CREDENTIAL_LEN];
  char mqttPassword[MQTT_CREDENTIAL_LEN];
  MQTT_DATA_struct mqttData[MQTT_PUBSUB_COUNT];
} EEPROM_DATA_struct;

extern EEPROM_DATA_struct lib_eeprom_config;
extern bool app_wifi_reset_flag;

#endif //_lib_eeprom_data_h_
