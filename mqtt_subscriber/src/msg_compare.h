#ifndef MESSAGE_COMPARE_H
#define MESSAGE_COMPARE_H

#include "datatypes.h"

int compare_messages(struct message *event, struct message *received, enum cmp_op comparison);

#endif