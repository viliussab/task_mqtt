#ifndef _UCI_READ_H
#define _UCI_READ_H

#include "base.h"

#define OPTION_NOT_FOUND -301
#define OPTION_UNEXPECTED_TYPE -302

void uci_alloc();
void uci_free();

int uci_set_topics(struct topic *topics, int *topic_count);
int uci_get_option(const char* option_path, char **out_str);

// int do_uci_stuff(const char* name);

#endif