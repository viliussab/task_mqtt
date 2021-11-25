#ifndef _MQTTSUB_MQTT_H
#define _MQTTSUB_MQTT_H

#include "../datatypes.h"
#include <sqlite3.h>
#include <curl/curl.h>

int create_mqtt(struct arguments args, struct mosquitto **out);
int assign_topics(struct mosquitto *mosq, ht *topics);
int assign_db(struct mosquitto *mosq, sqlite3* db);
int assign_smtp(struct mosquitto *mosq, CURL *curl);
int mqtt_loop(struct mosquitto *mosq, int *terminate_sig);
void mqtt_cleanup(struct mosquitto *mosq);

#endif