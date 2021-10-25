// Standard C libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Standard GNU libraries
#include <argp.h>
// Local headers
#include "arg_parsing.h"

// need to mention a version string.
const char *argp_program_version = "mqtt-subscriber 1.0.0";

// documentation string that will be displayed in the help section.
static char doc[] = "MQTT subscriber prototype that is integrated with RutOS webGUI";

// email address for bug reporting.
const char *argp_program_bug_address = "<email address not provided>";

// argument list for doc. This will be displayed on --help
static char args_doc[] = "[ARG...] -h, -n are mandatory";

// cli argument availble options.
static struct argp_option options[] = {
    {"tls",         't', "tls_certificate_path",    0, "Use TLS to encrypt message- otherwise messages will be send in plain-text"},
    {"username",    'u', "username",                0, "Username to connect to the MQTT broker"},
    {"password",    'p', "password",                0, "Password to connect to the MQTT broker"},
    {"host_ip",     'h', "ip",                      0, "IP address of the MQTT broker"},
    {"port",        'n', "portnumber",              0, "Port number to use when connecting to host_ip"},
    {0}
};

// define a function which will parse the args.
static error_t parse_opt(int key, char *arg, struct argp_state *state){

    struct arguments *arguments = state->input;

    char *temp;
    switch(key){

        case 't':
            arguments->tls_certificate_path = arg;
            break;
        case 'u':
            arguments->username = arg;
            break;
        case 'p':
            arguments->password = arg;
            break;
        case 'h':                
            arguments->host_ip = arg;
            break;
        case 'n':
            arguments->port = atoi(arg);
            if (arguments->port == 0) {
                fprintf(stderr, "Error while parsing port number.\n");
            }
            break;

        case ARGP_KEY_ARG:
            // Too many arguments.
            if(state->arg_num > 0)
                argp_usage(state);

            break;

        case ARGP_KEY_END:

            // Not enough arguments.
            if (state->arg_num < 0)
                argp_usage(state);
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static int check_mandatory_options(struct arguments *args)
{
    int is_err = 0;
    if (args->host_ip == NULL) {
        fprintf(stderr, "No mandatory flag '-h' for host ip address provided\n");
        is_err = 1;
    }
    if (args->port == 0) {
        fprintf(stderr, "No mandatory flag '-n' for port number provided\n");
        is_err = 1;
    }
    if (args->tls_certificate_path == -1) {
        fprintf(stderr, "No flag '-t' for TLS certiciate provided\n");
    }
    if (args->username == -1) {
        fprintf(stderr, "No flag '-u' for username provided\n");
    }
    if (args->password == -1) {
        fprintf(stderr, "No flag '-p' for password provided\n");
    }
    return is_err;
}

int parse_subscriber_arguments(int argc, char *args[], struct arguments *out_arg)
{
    // initialize the argp struct. Which will be used to parse and use the args.
    struct argp argp = {options, parse_opt, args_doc, doc};

    // set the default values for all of the args.
    out_arg->tls_certificate_path = NULL;
    out_arg->username = NULL;
    out_arg->password = NULL;
    out_arg->host_ip = NULL;
    out_arg->port = 0;

    // parse the cli arguments.
    argp_parse(&argp, argc, args, 0, 0, out_arg);

    int rc = check_mandatory_options(out_arg);
    if (rc) {
        return -1;
    }

    printf("Use TLS: %s\n", out_arg->tls_certificate_path? "yes" : "no");
    printf("Username: %s\n", out_arg->username? out_arg->username : "no username provided");
    printf("Password: %s\n", out_arg->password? out_arg->password : "no password provided");
    printf("Host_ip: %s\n", out_arg->host_ip);
    printf("Ports: %d\n", out_arg->port);

    return 0;
}


