/**
 * @file gen_list.c
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Travail avec les listes génériques simples
 *
 * Contient les définitions des fonctions
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gen_list.h"

/* inspiré du code de http://pseudomuto.com/development/2013/05/02/implementing-a-generic-linked-list-in-c/ */

void list_new(Liste_t *list, int elementSize, fonctionDestructeur *freeFn)
{
  assert(elementSize > 0);
  list->nbElements = 0;
  list->tailleElements = elementSize;
  list->debut_liste_p = list->fin_liste_p = NULL;
  list->fnDestructeur_p = freeFn;
}

void list_destroy(Liste_t *list)
{
  ElementListe_t *current;
  while(list->debut_liste_p != NULL) {
    current = list->debut_liste_p;
    list->debut_liste_p = current->suivant_p;

    if(list->fnDestructeur_p) {
      list->fnDestructeur_p(current->donnees_p);
    }

    free(current->donnees_p);
    free(current);
  }
}

void list_prepend(Liste_t *list, void *element)
{
  ElementListe_t *node = malloc(sizeof(ElementListe_t));
  node->donnees_p = malloc(list->tailleElements);
  memcpy(node->donnees_p, element, list->tailleElements);

  node->suivant_p = list->debut_liste_p;
  list->debut_liste_p = node;

  /* first node? */
  if(!list->fin_liste_p) {
    list->fin_liste_p = list->debut_liste_p;
  }

  list->nbElements++;
}

void list_append(Liste_t *list, void *element)
{
  ElementListe_t *node = malloc(sizeof(ElementListe_t));
  node->donnees_p = malloc(list->tailleElements);
  node->suivant_p = NULL;

  memcpy(node->donnees_p, element, list->tailleElements);

  if(list->nbElements == 0) {
    list->debut_liste_p = list->fin_liste_p = node;
  } else {
    list->fin_liste_p->suivant_p = node;
    list->fin_liste_p = node;
  }

  list->nbElements++;
}

void list_for_each(Liste_t *list, listIterator iterator)
{
  assert(iterator != NULL);

  ElementListe_t *node = list->debut_liste_p;
  bool result = TRUE;
  while(node != NULL && result) {
    result = iterator(node->donnees_p);
    node = node->suivant_p;
  }
}

void list_head(Liste_t *list, void *element, bool removeFromList)
{
  assert(list->debut_liste_p != NULL);

  ElementListe_t *node = list->debut_liste_p;
  memcpy(element, node->donnees_p, list->tailleElements);

  if(removeFromList) {
    list->debut_liste_p = node->suivant_p;
    list->nbElements--;

    free(node->donnees_p);
    free(node);
  }
}

void list_tail(Liste_t *list, void *element)
{
  assert(list->fin_liste_p != NULL);
  ElementListe_t *node = list->fin_liste_p;
  memcpy(element, node->donnees_p, list->tailleElements);
}

int list_size(Liste_t *list)
{
  return list->nbElements;
}
