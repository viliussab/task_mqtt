// Standard C libraries
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
// Standard GNU libraries
#include <argp.h>
// Third-party libraries
#include <mosquitto.h>
#include <curl/curl.h>
#include <sqlite3.h>
// Local headers
#include "datatypes.h"
#include "arg_parsing.h"
#include "network/mqtt.h"
#include "uci_read.h"
#include "sqlite3_logging.h"

int interrupt = 0;
void term_proc(int signo) 
{
    fprintf(stdout, "Received signal: %d\n", signo);
    interrupt = 1;
}

int main(int argc, char *argv[])
{
    fprintf(stdout, "MQTT subscriber is starting...\n");
    int rc = 0;
    struct arguments arguments;
    struct sigaction action;
    struct uci_context *ctx;
    struct uci_package *package;
    struct uci_package *usergroups;
    sqlite3* db;
    CURL* curl;
    struct mosquitto *mosq;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    rc = parse_subscriber_arguments(argc, argv, &arguments);
    if (rc) {
        return rc;
    }
    ht* topics = ht_create();
    if (topics == NULL) {
        return ALLOC_ERR;
    }
    db = sqlite3_create_db();
    if (db == NULL) {
        goto dealloc_ht;
    }
    curl = curl_easy_init();
    if (curl == NULL) {
        goto dealloc_sqlite;
    }
    ctx = uci_alloc_context();
    if (ctx == NULL) {
        goto dealloc_curl;
    }
    rc = uci_load_package(ctx, "mqtt_subscriber", &package);
    if (rc) {
        goto dealloc_uci;
    }
    rc = uci_load_package(ctx, "user_groups", &usergroups);
    if (rc) {
        goto dealloc_uci;
    }
    rc = uci_set_topics(package, topics);
    if (rc) {
        goto dealloc_uci;
    }
    rc = uci_populate_events(package, usergroups, topics);
    if (rc) {
        goto dealloc_uci;
    }
    rc = create_mqtt(arguments, &mosq);
    if (rc) {
        goto dealloc_mosq;
    }
    assign_topics(mosq, topics);
    assign_db(mosq, db);
    assign_smtp(mosq, curl);
    mqtt_loop(mosq, &interrupt);
dealloc_mosq:
    mqtt_cleanup(mosq);
dealloc_events:
    depopulate_events(topics);
dealloc_uci:
    uci_free_context(ctx);
dealloc_curl:
    curl_easy_cleanup(curl);
dealloc_sqlite:
    sqlite3_close(db);
dealloc_ht:
    ht_destroy(topics);
    return rc;
}
