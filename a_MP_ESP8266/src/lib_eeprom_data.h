#include <Arduino.h>

#ifndef _lib_eeprom_data_h_
#define _lib_eeprom_data_h_


#define DEVNAME_LEN   15
#define SSID_LEN      30
#define PASSWORD_LEN  50

typedef struct
{
  int checkVal;
  char devname[DEVNAME_LEN];
  char ssid[SSID_LEN];
  char password[PASSWORD_LEN];
} EEPROM_DATA_struct;

#endif //_lib_eeprom_data_h_
