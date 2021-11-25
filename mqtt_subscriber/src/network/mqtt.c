#include <mosquitto.h>
#include <stdio.h>
#include <sqlite3.h>
#include <unistd.h>


#include "mqtt.h"
#include "../datatypes.h"
#include "../sqlite3_logging.h"
#include "json_message_parsing.h"
#include "../msg_compare.h"
#include "smtp.h"

struct mqtt_subscriber {
    bool on_msg_error;
    bool on_connect_error;
    ht *topics;
    sqlite3 *db;
    CURL *curl;
};

static const struct mqtt_subscriber empty_ctx;
static struct mqtt_subscriber g_ctx;

static void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    fprintf(stdout, "Connection with ID: %d\n", *(int *)obj);
    if (rc) {
        g_ctx.on_connect_error = true;
        if (rc == MOSQ_ERR_CONN_REFUSED) {
            fprintf(stderr, "Broker does not acknowledge the connect (perhaps certificate files or username/password combination is false), aborting...\n");
        } else {
            fprintf(stderr, "Error with result code: %d\n", rc);
        }
    }
}

static void send_if_event_match(CURL* curl, struct l_list *incoming_msgs, struct l_list *events)
{
    struct node *i, *j;

    for (i = incoming_msgs->head; i != NULL; i = i->next) {
        struct message *msg = (struct message *)i->value;
        j = events->head;
        for (j = events->head; j != NULL; j = j->next) {
            struct observable_message *e = (struct observable_message *)j->value;
            if (strcmp(msg->key, e->contents.key) == 0) {
                if (compare_messages(&e->contents, msg, e->comparison_op) == SUCCESS) {
                    fprintf(stdout, "Event on key %s found! Sending an email to %s\n (topic %s)", e->contents.key, e->receiver_mail, e->topic_name);
                    prepare_email_template(curl, *e, msg->value);
                    send_prepared_mail(curl, e->sender.smtp_server, e->sender.port, e->sender.email, e->sender.password, e->receiver_mail);
                }
            }
        }
    }
}

static void event_sending(CURL* curl, ht* topics, const struct mosquitto_message *msg)
{
    int rc = 0;
    struct topic *t;
    struct l_list incoming_msgs;
    incoming_msgs.head = NULL;
    if (curl == NULL) {
        return;
    }
    if (topics == NULL) {
        return;
    }

    t = (struct topic *)ht_get(topics, msg->topic);
    if (t == NULL) {
        return;
    }
    rc = parse_message((char *)msg->payload, &incoming_msgs);
    if (rc == SUCCESS) {
        send_if_event_match(curl, &incoming_msgs, &t->events);
    }
    list_free(&incoming_msgs, free_struct_message);
}


static void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    fprintf(stdout, "New message with topic %s: %s\n", msg->topic, (char *)msg->payload);

    if (g_ctx.db != NULL) {
        save_message_to_db(g_ctx.db, (char *)msg->payload, msg->topic);
    }

    event_sending(g_ctx.curl, g_ctx.topics, msg);
}

static int subscribe_topics(struct mosquitto *mosq, ht* topics)
{
    int rc = SUCCESS;
    hti it = ht_iterator(topics);
    while (ht_next(&it)) {
        struct topic t = *(struct topic*)it.value;
        int security_level = atoi(t.security_level);

        fprintf(stdout, "Subscribing to topic %s\n", t.name);
        rc = mosquitto_subscribe(mosq, NULL, t.name, security_level);
        if (rc) {
            fprintf(stderr, "Could not subscribe to the topic \"%s\" with return code %d\n", t.name, rc);
            break;
        }
    }
    return rc;
}

int create_mqtt(struct arguments args, struct mosquitto **out)
{
    int rc = 0;
    struct mosquitto *mosq;
    g_ctx = empty_ctx;

    mosquitto_lib_init();

    mosq = mosquitto_new("mqtt_subscriber", true, NULL);
    if (mosq == NULL) {
		fprintf(stderr, "Error: Out of memory.\n");
        return ALLOC_ERR;
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

    if (args.tls_enable) {
        rc = mosquitto_tls_set(mosq, args.cafile_path, NULL, args.certfile_path, args.keyfile_path, NULL);
        if (rc) {
            fprintf(stderr, "Could not set tls parameters with return code %d\n", rc);
            goto cleanup_mosq_obj;
        }
    }

    rc = mosquitto_connect(mosq, args.host_ip, args.port, 30);
    if (rc) {
        fprintf(stderr, "Could not connect to the Broker with return code %d\n", rc);
        goto cleanup_mosq_obj;
    }

    *out = mosq;
    return SUCCESS;

cleanup_mosq_obj:
    mosquitto_destroy(mosq);
    return rc;
}

int assign_topics(struct mosquitto *mosq, ht *topics)
{
    if (mosq == NULL) {
        fprintf(stderr, "Invalid mosquitto object in assign_topics");
        return PARAM_ERR;
    }
    if (topics == NULL) {
        fprintf(stderr, "Invalid topics object in assign_topics");
        return PARAM_ERR;
    }
    int rc = subscribe_topics(mosq, topics);
    if (rc) {
        return rc;
    }
    g_ctx.topics = topics;
    return SUCCESS;
}

int assign_db(struct mosquitto *mosq, sqlite3* db)
{
    if (mosq == NULL) {
        fprintf(stderr, "Invalid mosquitto object in_assign db");
        return PARAM_ERR;
    }
    if (db == NULL) {
        fprintf(stderr, "Invalid db object in assign_db");
        return PARAM_ERR;
    }
    g_ctx.db = db;
    return SUCCESS;
}

int assign_smtp(struct mosquitto *mosq, CURL *curl)
{
    if (mosq == NULL) {
        fprintf(stderr, "Invalid mosquitto object in_assign db");
        return PARAM_ERR;
    }
    if (curl == NULL) {
        fprintf(stderr, "Invalid db object in assign_db");
        return PARAM_ERR;
    }
    g_ctx.curl = curl;
    return SUCCESS;
}

int mqtt_loop(struct mosquitto *mosq, int *terminate_sig)
{
    if (mosq == NULL) {
        return PARAM_ERR;
    }
    mosquitto_loop_start(mosq);
    while ((*terminate_sig) == 0
    && g_ctx.on_msg_error == false
    && g_ctx.on_connect_error == false) {
        fflush(stdout);
        sleep(1);
    }
    mosquitto_loop_stop(mosq, true);

    if (g_ctx.on_msg_error) {
        fprintf(stdout, "MQTT terminated with an error.\n");
        return UNHANDLED_ERR;
    }
    if (g_ctx.on_connect_error) {
        fprintf(stdout, "MQTT terminated with an error.\n");
        return CONNECTION_ERR;
    }

    fprintf(stdout, "MQTT message loop terminated succesfully\n");
    return SUCCESS;
}

void mqtt_cleanup(struct mosquitto *mosq)
{
    g_ctx = empty_ctx;
    if (mosq != NULL) {
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
    }
    mosquitto_lib_cleanup();
}