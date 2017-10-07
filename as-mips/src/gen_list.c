/**
 * @file gen_list.c
 * @author TAURAND Sébastien
 * @brief Définitions des fonctions permettant de travailler avec des listes génériques simplement chaïnées.
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <gen_list.h>
#include <notify.h>

/**
 * @param liste_p Pointeur sur une liste générique simplement chaînée
 * @param tailleElement Taille mémoire nécessaire pour contenir un élément de la liste
 * @param freeFn Pointeur sur la fonction de destruction des données dynamiques liées à l'élement de liste
 * @return Rien
 * @brief
 */
void initialiseListe(Liste_t *liste_p, int tailleElement,
	fonctionDestructeur *freeFn) {
	assert(tailleElement > 0);
	assert(liste_p);
	liste_p->nbElements = 0;
	liste_p->tailleElements = tailleElement;
	liste_p->debut_liste_p = liste_p->fin_liste_p = NULL;
	liste_p->fnDestructeur_p = freeFn;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return Rien
 * @brief Détruit l'ensemble des éléments de la liste en libérant la mémoire dynamique direct et indirecte associée
 */
void detruitListe(Liste_t *liste_p) {
	DEBUG_MSG("Destruction de la Liste: %p : %d éléments",liste_p,liste_p->nbElements);
	ElementListe_t *elementCourant_p;
	while (liste_p->debut_liste_p != NULL) {
		elementCourant_p = liste_p->debut_liste_p;
		liste_p->debut_liste_p = elementCourant_p->suivant_p;

		if (liste_p->fnDestructeur_p) {
			liste_p->fnDestructeur_p(elementCourant_p->donnees_p);
		}

		free(elementCourant_p->donnees_p);
		free(elementCourant_p);
	}
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param nouvelElement_p Pointeur sur le contenu du nouvel élément à insérer
 * @return Rien
 * @brief Insère un nouvel élément en début de liste
 */
void ajouteElementDebutListe(Liste_t *liste_p, void *nouvelElement_p) {
	ElementListe_t *element_p = malloc(sizeof(*element_p));
	element_p->donnees_p = malloc(liste_p->tailleElements);
	memcpy(element_p->donnees_p, nouvelElement_p, liste_p->tailleElements);

	element_p->suivant_p = liste_p->debut_liste_p;
	liste_p->debut_liste_p = element_p;

	/* Cas où la liste était vide et que ce sera le premier élément */
	if (!liste_p->fin_liste_p) {
		liste_p->fin_liste_p = liste_p->debut_liste_p;
	}

	liste_p->nbElements++;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param nouvelElement_p Pointeur sur le contenu du nouvel élément à insérer
 * @return Rien
 * @brief Insère un nouvel élément en fin de liste
 */
void ajouteElementFinListe(Liste_t *liste_p, void *nouvelElement_p) {
	ElementListe_t *element_p = malloc(sizeof(*element_p));
	element_p->donnees_p = malloc(liste_p->tailleElements);
	element_p->suivant_p = NULL;

	memcpy(element_p->donnees_p, nouvelElement_p, liste_p->tailleElements);

	if (liste_p->nbElements == 0) {
		liste_p->debut_liste_p = liste_p->fin_liste_p = element_p;
	} else {
		liste_p->fin_liste_p->suivant_p = element_p;
		liste_p->fin_liste_p = element_p;
	}

	liste_p->nbElements++;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return Le nombre d'éléments de cette liste
 * @brief Donne le nombre d'éléments de liste_p
 */
int tailleListe(Liste_t *liste_p) {
	return liste_p->nbElements;
}
