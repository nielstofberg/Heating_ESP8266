#include <Arduino.h>

typedef struct
{
  int checkVal;
  char devname[10];
  char ssid[30];
  char password[50];
} configData_t;

