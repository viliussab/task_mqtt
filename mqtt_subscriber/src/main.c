// Standard C libraries
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
// Standard GNU libraries
#include <argp.h>
// Third-party libraries
#include <mosquitto.h>
// Local headers
#include "base.h"
#include "arg_parsing.h"
#include "mqtt.h"
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
    int rc = 0;
    struct arguments arguments;
    struct sigaction action;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    struct topic topics[TOPIC_MAX_COUNT];
    int size = -1;

    parse_subscriber_arguments(argc, argv, &arguments);

    uci_alloc();
    sqlite3_messaging_ctx_create();

    uci_set_topics(topics, &size);

    run_mqtt_service(arguments, topics, size, &interrupt);

    uci_free();
    sqlite3_messaging_ctx_close();
    
}
