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

#include <gen_list.h>
#include <notify.h>

/* inspiré du code de http://pseudomuto.com/development/2013/05/02/implementing-a-generic-linked-list-in-c/ */

void initialiseListe(Liste_t *liste_p, int tailleElement, fonctionDestructeur *freeFn)
{
  assert(tailleElement > 0);
  liste_p->nbElements = 0;
  liste_p->tailleElements = tailleElement;
  liste_p->debut_liste_p = liste_p->fin_liste_p = NULL;
  liste_p->fnDestructeur_p = freeFn;
}

void detruitListe(void *liste_p)
{
	DEBUG_MSG("Liste: %p ... %d",liste_p,((Liste_t *)liste_p)->nbElements);
  ElementListe_t *elementCourant_p;
  while(((Liste_t *)liste_p)->debut_liste_p != NULL) {
    elementCourant_p = ((Liste_t *)liste_p)->debut_liste_p;
    ((Liste_t *)liste_p)->debut_liste_p = elementCourant_p->suivant_p;

    if(((Liste_t *)liste_p)->fnDestructeur_p) {
    	((Liste_t *)liste_p)->fnDestructeur_p(elementCourant_p->donnees_p);
    }

    free(elementCourant_p->donnees_p);
    free(elementCourant_p);
  }
}

void ajouteElementDebutListe(Liste_t *liste_p, void *nouvelElement_p)
{
  ElementListe_t *element_p = malloc(sizeof(ElementListe_t));
  element_p->donnees_p = malloc(liste_p->tailleElements);
  memcpy(element_p->donnees_p, nouvelElement_p, liste_p->tailleElements);

  element_p->suivant_p = liste_p->debut_liste_p;
  liste_p->debut_liste_p = element_p;

  /* Cas où la liste était vide et que ce sera le premier élément */
  if(!liste_p->fin_liste_p) {
    liste_p->fin_liste_p = liste_p->debut_liste_p;
  }

  liste_p->nbElements++;
}

void ajouteElementFinListe(Liste_t *liste_p, void *nouvelElement_p)
{
  ElementListe_t *element_p = malloc(sizeof(ElementListe_t));
  element_p->donnees_p = malloc(liste_p->tailleElements);
  element_p->suivant_p = NULL;

  memcpy(element_p->donnees_p, nouvelElement_p, liste_p->tailleElements);

  if(liste_p->nbElements == 0) {
    liste_p->debut_liste_p = liste_p->fin_liste_p = element_p;
  } else {
    liste_p->fin_liste_p->suivant_p = element_p;
    liste_p->fin_liste_p = element_p;
  }

  liste_p->nbElements++;
}

int tailleListe(Liste_t *liste_p)
{
  return liste_p->nbElements;
}
