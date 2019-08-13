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
    break;
    case INITIALISED:
    break;
    case OPERATE_REQ:
    break;
    case OPERATING:
    break;
    case SHUTDOWN_REQ:
    break;
    case SHUTTING_DOWN:
    break;
  }
}
*/

#endif // _app_enum_h_