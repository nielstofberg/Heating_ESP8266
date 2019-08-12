#ifndef _app_enum_h_
#define _app_enum_h_

typedef enum{
    INITIALISE_REQ = 'i',
    INITIALISED = 'I',
    OPERATE_REQ = 'o',
    OPERATING = 'O',
    SHUTDOWN_REQ = 's',
    SHUTTING_DOWN = 'S'
} APP_STATUS_enum;

/*
void app_applicationName_run(){
  switch (app_wifi_status)
  {
    case INITIALISE_REQ:
    case INITIALISED:
    case OPERATE_REQ:
    case OPERATING:
    case SHUTDOWN_REQ:
    case SHUTTING_DOWN:
  }
}
*/

#endif // _app_enum_h_