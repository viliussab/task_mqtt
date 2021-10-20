#ifndef _MQTTSUB_BASE_H
#define _MQTTSUB_BASE_H

#define TOPIC_MAX_COUNT 512

// define a struct to hold the arguments.
struct arguments {
    // Flags
    int use_tls;
    // Parameters
    char *username;
    char *password;
    char *host_ip;
    int port;
    
    int topics_size;
    // Free arguments
    char *topics[TOPIC_MAX_COUNT];
};
#endif