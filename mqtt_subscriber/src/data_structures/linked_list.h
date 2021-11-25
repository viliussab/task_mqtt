#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

struct node {
   void *value;
   struct node *next;
};

struct l_list {
   struct node *head;
};

//insert link at the first location
void list_insert(struct l_list* l_ptr, void *value);

void list_free(struct l_list* l_ptr, void (*value_free_fn)(void*));

#endif //_LINKED_LIST_H