#ifndef _MQTTSUB_BASE_H
#define _MQTTSUB_BASE_H

#include "data_structures/linked_list.h"
#include "data_structures/hash_table.h"

#define UCI_SECTION_NAME "mqtt_subscriber"

#define MSG_NUMBER_TYPE "number"
#define MSG_STRING_TYPE "string"

#define COMPARISON_EQ "=="
#define COMPARISON_NEQ "!="
#define COMPARISON_GT ">"
#define COMPARISON_LT "<"
#define COMPARISON_GTEQ ">="
#define COMPARISON_LTEQ "<="

// error message codes
enum {
    SUCCESS,
    ALLOC_ERR,
    INPUT_ERR,
    PARAM_ERR,
    CONNECTION_ERR,
    UNHANDLED_ERR,
    UNSUPPORTED,
    INSUFFICIENT,
    NOT_EQUAL,
};

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
    char *security_level;
    struct l_list events;
};

struct message {
    char* value;
    char* key;
    char* value_type;
};

enum cmp_op {
    EQ = 1,
    NEQ,
    GT,
    GTEQ,
    LT,
    LTEQ,
};

struct sender_email {
    char* smtp_server;
    int port;
    char* username;
    char* password;
    char* email;
};

struct observable_message {
    char *topic_name;
    struct message contents;
    enum cmp_op comparison_op;
    struct sender_email sender;
    char *receiver_mail;
};

void free_struct_message(void *struct_message);
void empty_free(void *unused);
void depopulate_events(ht *topics);
const char* operator_to_words(enum cmp_op var);

#endif