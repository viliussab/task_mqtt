#include "json_message_parsing.h"

#include <json-c/json.h>
#include <stdio.h>
#include <string.h>

void json_traverse(struct json_object *jobj, struct l_list *msgs_ptr);
void json_traverse_jobj(struct json_object *jobj, struct l_list *msgs_ptr);
void json_traverse_jarray(struct json_object *jarray, struct l_list *msgs_ptr);

int parse_message(const char *message, struct l_list *msgs_ptr)
{
	struct json_object *jobj;
	jobj = json_tokener_parse(message);
	if (jobj == NULL) {
        return PARAM_ERR;
	}
    json_traverse(jobj, msgs_ptr);
	json_object_put(jobj);
    return SUCCESS;
}

enum {
	VAL_INT,
	VAL_DOUBLE,
	VAL_STRING
};

void set_message(struct message *ptr, char* key, json_object* value, int data_type)
{
    char *str;
	switch (data_type) {
	case VAL_INT:
		str = malloc(sizeof(char) * 64);
		snprintf(str, 64, "%d", json_object_get_int(value));

		ptr->value_type = strdup(MSG_NUMBER_TYPE);
		ptr->value = str;
		ptr->key = strdup(key);
		break;
	case VAL_DOUBLE:
		str = malloc(sizeof(char) * 64);
		snprintf(str, 64, "%f", json_object_get_double(value));

		ptr->value_type = strdup(MSG_NUMBER_TYPE);
		ptr->value = str;
		ptr->key = strdup(key);
		break;
	case VAL_STRING:
		ptr->value_type = strdup(MSG_STRING_TYPE);
		ptr->value = strdup(json_object_get_string(value));
		ptr->key = strdup(key);
		break;
	default:
		printf("Undefined value type found\n");
	}
}



void json_traverse_jobj(struct json_object *jobj, struct l_list *msgs_ptr)
{
	int val_type;
    void* value_ptr;
    struct message *ptr;

	json_object_object_foreach(jobj, key, val)
    {
		val_type = json_object_get_type(val);

		switch (val_type) {
			
		case json_type_boolean:
			fprintf(stderr, "%s has an unsupported type boolean\n", key);
			break;

		case json_type_double:
			ptr = malloc(sizeof(struct message));
			set_message(ptr, key, val, VAL_DOUBLE);
			list_insert(msgs_ptr, ptr);
			break;

		case json_type_int:
			ptr = malloc(sizeof(struct message));
			set_message(ptr, key, val, VAL_INT);
			list_insert(msgs_ptr, ptr);

			break;

		case json_type_string:
			ptr = malloc(sizeof(struct message));
			set_message(ptr, key, val, VAL_STRING);
			list_insert(msgs_ptr, ptr);

			break;

		case json_type_object:
			json_traverse_jobj(val, msgs_ptr);
			break;

		case json_type_array:
			json_traverse_jarray(val, msgs_ptr);
			break;

		default:
			break;
		}
	}
}

void json_traverse_jarray(struct json_object *jarray, struct l_list *msgs_ptr)
{
	struct json_object* array_obj;
	int arraylen;
	int i;
	arraylen = json_object_array_length(jarray);

	for (i = 0; i < arraylen; i++) {
		array_obj = json_object_array_get_idx(jarray, i);
		json_traverse(array_obj, msgs_ptr);
	}
}

void json_traverse(struct json_object *jobj, struct l_list *msgs_ptr)
{
	int val_type;
	val_type = json_object_get_type(jobj);

	switch (val_type) {
		case json_type_object:
			json_traverse_jobj(jobj, msgs_ptr);
			break;
		case json_type_array:
			json_traverse_jarray(jobj, msgs_ptr);
			break;
		default:
			return;
	}
}