#include "datatypes.h"

void free_struct_message(void *struct_message)
{
    if (struct_message == NULL) {
        return;
    }
    struct message* p = (struct message *)struct_message;
    if (p->value_type) {
        free(p->value_type);
    }
    if (p->value) {
        free(p->value);
    }
	if (p->key) {
		free(p->key);
	}
    free(p);
}

void empty_free(void *unused) { }

void depopulate_events(ht *topics)
{
    hti it = ht_iterator(topics);
    int i = 1;
    while (ht_next(&it)) {
        struct topic *t = (struct topic *)it.value;
        if (t != NULL) {
            list_free(&t->events, free);
        }
    }
}

const char* operator_to_words(enum cmp_op operator)
{
	switch(operator) {
	case EQ:
		return "equals";
	case NEQ:
		return "is not equal to";
	case LT:
		return "is less than";
	case LTEQ:
		return "equals or is less than";
	case GT:
		return "is greater than";
	case GTEQ:
		return "equals or is greater than";
	default:
		return "invalid";
	}
}