#include "linked_list.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

//insert link at the first location
void list_insert(struct l_list* l_ptr, void *value) {
   //create a link
   struct node *link = (struct node*) malloc(sizeof(struct node));
	
   link->value = value;
   //point it to old first node
   link->next = l_ptr->head;
	
   //point first to new first node
   l_ptr->head = link;
}

void list_free(struct l_list* l_ptr, void (*value_free_fn)(void*))
{
   void* value;
   struct node* current;
   // while there are elements
   while (l_ptr->head != NULL)
   {
      // take the head object
      current = l_ptr->head;
      // make next object the new head object
      l_ptr->head = l_ptr->head->next;

      // get the value of the old head object
      value = current->value;

      // free the value and the head object
      if (value) {
         value_free_fn(value);
      }
      free(current);
   }
}