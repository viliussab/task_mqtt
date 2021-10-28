#include <mosquitto.h>
#include <stdio.h>

#include <unistd.h>

#include "mqtt.h"
#include "sqlite3_logging.h"

static void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    fprintf(stdout, "Connection with ID: %d\n", *(int *)obj);
    if (rc) {
        fprintf(stderr, "Error with result code: %d\n", rc);
    }
}

static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    fprintf(stdout, "New message with topic %s: %s\n", msg->topic, (char *)msg->payload);
    save_message_to_db((char *)msg->payload, msg->topic);
}

static int subscribe_topics(struct mosquitto *mosq, struct topic *topics, int topics_count)
{
    int rc = 0;
    fprintf(stdout, "Total number of topics: %d\n", topics_count);
    for (int i = 0; i < topics_count; i++) {
        fprintf(stdout, "Subscribing to topic %s\n", topics[i]);
        rc = mosquitto_subscribe(mosq, NULL, topics[i].name, topics[i].security_level);
        if (rc) {
            fprintf(stderr, "Could not subscribe to the topic \"%s\" with return code %d\n", topics[i], rc);
            break;
        }
    }
    return rc;
}

int run_mqtt_service(struct arguments args, struct topic *topics, int topic_count, int *interrupt)
{
    int rc = 0;
    struct mosquitto *mosq;

    mosquitto_lib_init();

    mosq = mosquitto_new("mqtt_subscriber", true, NULL);
    if (!mosq) {
		fprintf(stderr, "Error: Out of memory.\n");
        rc = 1;
		goto cleanup_lib;
	}
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (args.enable_username_pw) {
        rc = mosquitto_username_pw_set(mosq, args.username, args.password);
        if (rc) {
            fprintf(stderr, "Could not set username and password with return code %d\n", rc);
            goto cleanup_mosq_obj;
        }
    }

    // TLS. TLS implementation should be expanded later. For, it only approves .ca files
    if (args.tls_enable) {
        rc = mosquitto_tls_set(mosq, args.cafile_path, NULL, args.certfile_path, args.keyfile_path, NULL);
        if (rc) {
            fprintf(stderr, "Could not set tls parameters with return code %d\n", rc);
            goto cleanup_mosq_obj;
        }
    }

    rc = mosquitto_connect(mosq, args.host_ip, args.port, 30);
    if(rc) {
        fprintf(stderr, "Could not connect to the Broker with return code %d\n", rc);
        goto cleanup_mosq_obj;
    }

    rc = subscribe_topics(mosq, topics, topic_count);
    if (rc) {
        goto cleanup_connection;
    }

    mosquitto_loop_start(mosq);
    while (!(*interrupt)) {
        fflush(stdout);
        sleep(1);
    }
    mosquitto_loop_stop(mosq, true);

    fprintf(stdout, "MQTT message loop terminated successfully\n");

cleanup_connection:
    mosquitto_disconnect(mosq);
cleanup_mosq_obj:
    mosquitto_destroy(mosq);
cleanup_lib:
    mosquitto_lib_cleanup();

    fprintf(stdout, "Exiting MQTT service...\n");
    return rc;
}