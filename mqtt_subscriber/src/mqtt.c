#include <mosquitto.h>
#include <stdio.h>

#include <unistd.h>

#include "mqtt.h"

static void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    fprintf(stdout, "Connection with ID: %d\n", *(int *)obj);
    if(rc) {
        fprintf(stderr, "Error with result code: %d\n", rc);
    }
}

static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    fprintf(stdout, "New message with topic %s: %s\n", msg->topic, (char *)msg->payload);
}

static int subscribe_topics(struct mosquitto *mosq, char *topics[], int topics_size, int security_level)
{
    int rc = 0;
    printf("Total number of topics: %d\n", topics_size);
    for (int i = 0; i < topics_size; i++) {
        fprintf(stdout, "Subscribing to topic %s\n", topics[i]);
        rc = mosquitto_subscribe(mosq, NULL, topics[i], security_level);
        if(rc) {
            fprintf(stderr, "Could not subscribe to the topic \"%s\" with return code %d\n", topics[i], rc);
            break;
        }
    }
    return rc;
}

int run_mqtt_service(struct arguments* args, int *interrupt)
{
    int rc = 0;
    struct mosquitto *mosq;

    mosquitto_lib_init();

    mosq = mosquitto_new("mqtt_subscriber", true, NULL);
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    rc = mosquitto_connect(mosq, args->host_ip, args->port, 30);
    if(rc) {
        fprintf(stderr, "Could not connect to the Broker with return code %d\n", rc);
        goto cleanup_mosq_obj;
    }

    rc = subscribe_topics(mosq, args->topics, args->topics_size, 0);
    if (rc) {
        goto cleanup_connection;
    }

    mosquitto_loop_start(mosq);
    while (!(*interrupt)) {
        fflush(stdout);
        sleep(1);
    }
    mosquitto_loop_stop(mosq, true);

    fprintf(stdout, "MQTT service has not received any errors during termination\n");

cleanup_connection:
    mosquitto_disconnect(mosq);
cleanup_mosq_obj:
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    fprintf(stdout, "Exiting MQTT service...\n");
    return rc;
}