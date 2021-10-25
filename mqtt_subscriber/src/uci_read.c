#include <stdlib.h>
#include <string.h>

#include <uci.h>
#include "uci_read.h"
#include "base.h"

static struct uci_context *g_ctx;

#define TOPIC_TYPEDSECTION_PATH "mqtt_subscriber.@mqttsub_topic"
#define TOPIC_NAME_OPTION "name"
#define TOPIC_SECURITYLVL_OPTION "security_level"

void uci_alloc()
{
    g_ctx = uci_alloc_context();
}
void uci_free()
{
    uci_free_context(g_ctx);
}

int uci_set_topics(struct topic *topics, int *topic_count) {
    int rc = 0;

    int count = 0;

    #define STRBUFFER_LEN 256
    char buffer[STRBUFFER_LEN];

    for (int i = 0; i < TOPIC_MAX_COUNT; i++) {

        char* topic_name;
        char* security_level;

        // Get the topic name
        snprintf(buffer, STRBUFFER_LEN - 1, "%s[%d].%s", TOPIC_TYPEDSECTION_PATH, i, TOPIC_NAME_OPTION);
        rc = uci_get_option(buffer, &topic_name);
        if (rc == OPTION_NOT_FOUND) { // no more topics found, everything's good
            rc = 0;
            break;
        }
        else if (rc) { // this is bad though
            fprintf(stderr, "Error while setting uci topics\n");
            rc = 1;
            break;
        }

        // Get the security level for the specified topic
        snprintf(buffer, STRBUFFER_LEN - 1, "%s[%d].%s", TOPIC_TYPEDSECTION_PATH, i, TOPIC_SECURITYLVL_OPTION);
        rc = uci_get_option(buffer, &security_level);
        if (rc) {
            break;
        }

        // Assign values
        topics[count].name = topic_name;
        topics[count].security_level = atoi(security_level);
        free(security_level);

        count++;
    }

    *topic_count = count;
    return rc;
}

int uci_get_option(const char* option_path, char **out_str)
{
    int rc = 0;

    struct uci_ptr ptr;
    char* name = strdup(option_path);

    if ((rc = uci_lookup_ptr(g_ctx, &ptr, name, true)) != UCI_OK)
    {
        fprintf(stderr, "Can not find anything in uci_lookup_ptr, section name = \"%s\"\n", name);
        goto free_ptr;
    }

    if (ptr.o == NULL) {
        rc = OPTION_NOT_FOUND;
        goto free_ptr;
    }

    switch(ptr.o->type) {

    case UCI_TYPE_STRING:
        *out_str = strdup(ptr.o->v.string);
        break;

    case UCI_TYPE_LIST:
        rc = OPTION_UNEXPECTED_TYPE;
        goto free_ptr;

    default:
        rc = OPTION_UNEXPECTED_TYPE;
        goto free_ptr;
    }

free_ptr:
    free(name);

    return rc;
}

