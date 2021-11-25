#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <uci.h>
#include "uci_read.h"
#include "data_structures/hash_table.h"
#include "data_structures/linked_list.h"
#include "datatypes.h"

int uci_load_package(struct uci_context *ctx, const char *config_name, struct uci_package **out_pkg)
{
    int rc = uci_load(ctx, config_name, out_pkg);
	if (rc != UCI_OK) {
		uci_free_context(ctx);
	}
    return rc;
}

static bool uci_get_element(struct uci_option *opt, const char* var_name, void **var)
{
    if (opt == NULL) return false;
	if (strcmp(opt->e.name, var_name) != 0) return false;
	if (opt->v.string == NULL) return false;
	*var = (void*)opt->v.string;
	return true;
}


static bool uci_get_element_int(struct uci_option *opt, const char* var_name, int *var)
{
	if (opt == NULL) return false;
	if (strcmp(opt->e.name, var_name) != 0) return false;
	if (opt->v.string == NULL) return false;

	int val = atoi(opt->v.string);
	if (val == 0) { return false; }
	*var = val;
	return true;
}

static bool validate_topic(struct topic *t)
{
	return (t->name != NULL
		&& t->security_level != NULL
		&& (
			strcmp(t->security_level, "0") == 0
			|| strcmp(t->security_level, "1") == 0
			|| strcmp(t->security_level, "2") == 0));
}

static bool set_topic(struct uci_section *section, struct topic *t)
{
	struct uci_element *j;
	uci_foreach_element(&section->options, j) {
		struct uci_option *option = uci_to_option(j);
		uci_get_element(option, "name", (void**)&t->name);
		uci_get_element(option, "security_level", (void**)&t->security_level);
	}
	return validate_topic(t);
}

int uci_set_topics(struct uci_package* package, ht* topics)
{
	struct uci_element *i;
	uci_foreach_element(&package->sections, i)
	{
		struct uci_section *section = uci_to_section(i);
		char *section_type = section->type;
		if (strcmp(section_type, "mqttsub_topic") == 0) {
			struct topic *t = malloc(sizeof(struct topic));
			if (t == NULL) {
				return ALLOC_ERR;
			}
			t->events.head = NULL;
			if (set_topic(section, t)) {
				ht_set(topics, t->name, t);
			}
		}
	}
	return SUCCESS;
}

static bool validate_event(struct observable_message *m)
{
	if (
		m->topic_name == NULL
		|| m->contents.key == NULL
		|| m->contents.value_type == NULL
		|| m->receiver_mail == NULL
		|| m->contents.value == NULL
		|| m->comparison_op == 0
		|| m->sender.email == NULL
		|| m->sender.password == NULL
		|| m->sender.username == NULL
		|| m->sender.port == 0
		|| m->sender.smtp_server == NULL
	) {
		return false;
	}
	if (strcmp(m->contents.value_type, "number") == 0) {
		if (strtod(m->contents.value, NULL) == 0) {
			return false;
		}
	}
	return true;
}

static int uci_set_sender(struct uci_package *package, struct sender_email *out)
{
	if (out->email == NULL) {
		return PARAM_ERR;
	}
	char* email;
	struct uci_element *i, *j;
	
	uci_foreach_element(&package->sections, i)
	{
		struct uci_section *section = uci_to_section(i);
		char *section_type = section->type;
		if (strcmp(section_type, "email") == 0) {
			email = NULL;
			uci_foreach_element(&section->options, j) {
				struct uci_option *option = uci_to_option(j);
				uci_get_element(option, "senderemail", (void**)&email);
				uci_get_element(option, "username", (void**)&out->username);
				uci_get_element(option, "smtp_ip", (void**)&out->smtp_server);
				uci_get_element_int(option, "smtp_port", &out->port);
				uci_get_element(option, "password", (void**)&out->password);
			}
			if (email == NULL) continue;
			if (strcmp(email, out->email) == 0) return SUCCESS;
		}
	}
	return PARAM_ERR;
}


static bool uci_set_event(struct uci_section *section, struct uci_package *package_usergroups, struct observable_message *m)
{
	struct uci_element *j;
	m->sender.email = NULL;
	m->sender.username = NULL;
	m->sender.password = NULL;
	m->sender.smtp_server = NULL;
	uci_foreach_element(&section->options, j) {
		struct uci_option *option = uci_to_option(j);
		uci_get_element(option, "topic", (void**)&m->topic_name);
		uci_get_element(option, "key", (void**)&m->contents.key);
		uci_get_element(option, "data_type", (void**)&m->contents.value_type);
		uci_get_element(option, "sender_conf", (void**)&m->sender.email);
		uci_get_element(option, "receiver_mail", (void**)&m->receiver_mail);
		uci_get_element(option, "value", (void**)&m->contents.value);
		uci_get_element_int(option, "operator", (int*)(&m->comparison_op));
	}
	int rc = uci_set_sender(package_usergroups, &m->sender);
	return rc == SUCCESS && validate_event(m);
}

int uci_populate_events(struct uci_package *package_mqttsub, struct uci_package *package_usergroups, ht* topics)
{
	struct uci_element *i;
	uci_foreach_element(&package_mqttsub->sections, i)
	{
		struct uci_section *section = uci_to_section(i);
		char *section_type = section->type;
		if (strcmp(section_type, "mqttsub_event") == 0) {
			struct observable_message *m = malloc(sizeof(struct observable_message));
			if (m == NULL) {
				return ALLOC_ERR;
			}
			if (uci_set_event(section, package_usergroups, m)) {
				void *value = ht_get(topics, m->topic_name);
				if (value != NULL) {
					struct topic *t = (struct topic*) value;
					list_insert(&t->events, m);
				}
			}
			else { free(m); }
		}
	}
	return SUCCESS;
}
