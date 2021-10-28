#ifndef _MQTTSUB_BASE_H
#define _MQTTSUB_BASE_H

#define TOPIC_MAX_COUNT 512

// define a struct to hold the arguments.
struct arguments {
    int enable_username_pw;
    char *username;
    char *password;

    char *host_ip;
    int port;

    int tls_enable;
    char *cafile_path;
    char *certfile_path;
    char *keyfile_path;
};

struct topic {
    char *name;
    int security_level;
};
#endif