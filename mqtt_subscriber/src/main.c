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

    rc = parse_subscriber_arguments(argc, argv, &arguments);
    if (rc) {
        return rc;
    }

    run_mqtt_service(&arguments, &interrupt);
}
