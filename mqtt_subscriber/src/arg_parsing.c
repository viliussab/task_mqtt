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
    {"tls",         't', 0,                         0, "Enable TLS to encrypt message- otherwise messages will be send in plain-text"},
    {"cafile",      'C', "ca_certfile",             0, "Path to the certificate to the certificate authority that issued certfile and keyfile"},
    {"certfile",    'c', "client_certfile",         0, "Certificate file for MQTT subscriber instance"},
    {"keyfile",     'k', "client_keyfile",          0, "Private key file for MQTT subscriber instance"},
    {"username",    'u', "username",                0, "Username to connect to the MQTT broker"},
    {"password",    'P', "password",                0, "Password to connect to the MQTT broker"},
    {"host_ip",     'h', "ip",                      0, "IP address of the MQTT broker"},
    {"port",        'p', "portnumber",              0, "Port number to use when connecting to host_ip"},
    {0}
};

// define a function which will parse the args.
static error_t parse_opt(int key, char *arg, struct argp_state *state){

    struct arguments *arguments = state->input;

    char *temp;
    switch (key) {

    case 'u':
        if (strcmp("", arg))
            arguments->username = arg;
        break;
    case 'P':
        if (strcmp("", arg))
            arguments->password = arg;
        break;

    case 'h':
        if (strcmp("", arg))            
            arguments->host_ip = arg;
        break;
    case 'p':
        arguments->port = atoi(arg);
        if (arguments->port == 0) {
            fprintf(stderr, "Error while parsing port number.\n");
        }
        break;

    case 't':
        arguments->tls_enable = 1;
        break;
    case 'C':
        if (strcmp("", arg))
            arguments->cafile_path = arg;
        break;
    case 'c':
        if (strcmp("", arg))
            arguments->certfile_path = arg;
        break;
    case 'k':
        if (strcmp("", arg))           
            arguments->keyfile_path = arg;
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
    int use_username_password = 1;

    if (args->host_ip == NULL) {
        fprintf(stderr, "No mandatory flag '-h' for host ip address provided\n");
        is_err = 1;
    }
    if (args->port == 0) {
        fprintf(stderr, "No mandatory flag '-p' for port number provided\n");
        is_err = 1;
    }

    if (args->tls_enable == 1) {
        int tls_err = 0;
        if (args->cafile_path == NULL) {
            fprintf(stderr, "No flag '-C' for certficicate authority .cert file path provided, even though TLS is enabled\n");
            tls_err = 1;
        }
        if (args->certfile_path == NULL) {
            fprintf(stderr, "No flag '-c' for mqtt subscriber .cert file path provided, even though TLS is enabled\n");
            tls_err = 1;
        }
        if (args->cafile_path == NULL) {
            fprintf(stderr, "No flag '-k' for mqtt subscriber .key file path provided, even though TLS is enabled\n");
            tls_err = 1;
        }
        if (tls_err) {
            args->tls_enable = 0;
        }
    }

    if (args->username == NULL) {
        fprintf(stdout, "No flag '-u' for username provided\n");
        use_username_password = 0;
    }
    if (args->password == NULL) {
        fprintf(stdout, "No flag '-P' for password provided\n");
        use_username_password = 0;
    }
    args->enable_username_pw = use_username_password;

    return is_err;
}

int parse_subscriber_arguments(int argc, char *args[], struct arguments *out_arg)
{
    // initialize the argp struct. Which will be used to parse and use the args.
    struct argp argp = {options, parse_opt, args_doc, doc};

    // set the default values for all of the args.
    out_arg->tls_enable = 0;
    out_arg->cafile_path = NULL;
    out_arg->certfile_path = NULL;
    out_arg->keyfile_path = NULL;

    out_arg->enable_username_pw = 0;
    out_arg->username = NULL;
    out_arg->password = NULL;

    out_arg->host_ip = NULL;
    out_arg->port = 0;

    // parse the cli arguments.
    argp_parse(&argp, argc, args, 0, 0, out_arg);

    int rc = check_mandatory_options(out_arg);
    if (rc) {
        return 1;
    }

    printf("Use TLS: %s\n", out_arg->tls_enable? "yes" : "no");
    printf("Use username and password: %s\n", out_arg->enable_username_pw? "yes" : "no");
    printf("Host_ip: %s\n", out_arg->host_ip);
    printf("Ports: %d\n", out_arg->port);

    return 0;
}


