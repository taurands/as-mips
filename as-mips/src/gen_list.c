#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gen_list.h"

/* inspiré du code de http://pseudomuto.com/development/2013/05/02/implementing-a-generic-linked-list-in-c/ */


liste_t* add(liste_t* l_p, void* add1){

	liste_t* nouvel_element_p= (liste_t*)calloc(1,sizeof(*nouvel_element_p));
	nouvel_element_p->suiv=nouvel_element_p;
	nouvel_element_p->val=add1;
	if (!l_p) {
		return nouvel_element_p;
	}
	nouvel_element_p->suiv=l_p->suiv;
	l_p->suiv=nouvel_element_p;
	return nouvel_element_p;
}

void list_destroy(liste_t *list)
{
  liste_t* current_p;
  while(list) {
    current_p = list;
    list = current_p->suiv;

    if(list) {
    free(current_p->val);
    free(current_p);
    }
  }
}

/*
void list_new(list *list, int elementSize, freeFunction freeFn)
{
  assert(elementSize > 0);
  list->logicalLength = 0;
  list->elementSize = elementSize;
  list->head = list->tail = NULL;
  list->freeFn = freeFn;
}


void list_prepend(list *list, void *element)
{
  listNode *node = malloc(sizeof(listNode));
  node->data = malloc(list->elementSize);
  memcpy(node->data, element, list->elementSize);

  node->next = list->head;
  list->head = node;
*/
  /* first node? */
/*
  if(!list->tail) {
    list->tail = list->head;
  }

  list->logicalLength++;
}

void list_append(list *list, void *element)
{
  listNode *node = malloc(sizeof(listNode));
  node->data = malloc(list->elementSize);
  node->next = NULL;

  memcpy(node->data, element, list->elementSize);

  if(list->logicalLength == 0) {
    list->head = list->tail = node;
  } else {
    list->tail->next = node;
    list->tail = node;
  }

  list->logicalLength++;
}

void list_for_each(list *list, listIterator iterator)
{
  assert(iterator != NULL);

  listNode *node = list->head;
  bool result = TRUE;
  while(node != NULL && result) {
    result = iterator(node->data);
    node = node->next;
  }
}

void list_head(list *list, void *element, bool removeFromList)
{
  assert(list->head != NULL);

  listNode *node = list->head;
  memcpy(element, node->data, list->elementSize);

  if(removeFromList) {
    list->head = node->next;
    list->logicalLength--;

    free(node->data);
    free(node);
  }
}

void list_tail(list *list, void *element)
{
  assert(list->tail != NULL);
  listNode *node = list->tail;
  memcpy(element, node->data, list->elementSize);
}

int list_size(list *list)
{
  return list->logicalLength;
}
*/
