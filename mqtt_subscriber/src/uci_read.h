#ifndef _UCI_READ_H
#define _UCI_READ_H

#include "datatypes.h"
#include <uci.h>

int uci_load_package(struct uci_context *ctx, const char *config_name, struct uci_package **out_pkg);
int uci_set_topics(struct uci_package* package, ht* topics);
int uci_populate_events(struct uci_package *package_mqttsub, struct uci_package *package_usergroups, ht* topics);

#endif