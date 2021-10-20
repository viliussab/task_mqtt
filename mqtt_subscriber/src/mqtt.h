#ifndef _MQTTSUB_MQTT_H
#define _MQTTSUB_MQTT_H

#include "base.h"
typedef int error_t;

int run_mqtt_service(struct arguments *args, int *interrupt);

#endif