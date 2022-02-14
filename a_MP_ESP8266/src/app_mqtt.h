#ifndef _app_mqtt_h_
#define _app_mqtt_h_

#include <WString.h>
#include "app_enum.h"

#define MQTT_PUBSUB_COUNT   4
#define MQTT_TOPIC_LEN      30
#define MQTT_CREDENTIAL_LEN 15

typedef struct 
{
  char topic[MQTT_TOPIC_LEN];
  bool publish;
  bool subscribe;
  int ioPinNumber;
} MQTT_DATA_struct;

extern APP_STATUS_enum app_mqtt_status;

extern MQTT_DATA_struct app_mqtt_topics[];
extern String app_mqtt_server;
extern String app_mqtt_username;
extern String app_mqtt_password;
//extern String app_mqtt_outTopic;
//extern String app_mqtt_inTopic;
extern bool app_mqtt_refreshFlag;


void app_mqtt_run(void);
void app_mqtt_publish(String topic, String payload);

#endif //_app_mqtt_h_