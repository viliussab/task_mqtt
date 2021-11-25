#ifndef _JSON_MESSAGE_PARSING_H
#define _JSON_MESSAGE_PARSING_H

#include "../datatypes.h"

int parse_message(const char *message, struct l_list *msgs_ptr);

#endif 
