#ifndef _MQTTSUB_BASE_H
#define _MQTTSUB_BASE_H

#define TOPIC_MAX_COUNT 512

// define a struct to hold the arguments.
struct arguments {
    char *username;
    char *password;
    char *host_ip;
    char *tls_certificate_path;
    int port;
};

struct topic {
    char *name;
    int security_level;
};
#endif