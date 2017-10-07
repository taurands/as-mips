#ifndef _GEN_LIST_
#define _GEN_LIST_

/* a common function used to free malloc'd objects */
typedef void (*callback)(void *);


typedef struct liste_t {
	void* val;
	struct liste_t *suiv;
}liste_t;

liste_t* add(liste_t* l_p, void* add1);
void list_destroy(liste_t *list);

/*
typedef struct {
  int logicalLength;
  int elementSize;
  listNode *head;
  listNode *tail;
  freeFunction freeFn;
} list;

void list_new(list *list, int elementSize, freeFunction freeFn);
void list_destroy(list *list);

void list_prepend(list *list, void *element);
void list_append(list *list, void *element);
int list_size(list *list);

void list_for_each(list *list, listIterator iterator);
void list_head(list *list, void *element, bool removeFromList);
void list_tail(list *list, void *element);
*/
#endif
