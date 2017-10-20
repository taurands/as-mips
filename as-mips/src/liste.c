/**
 * @file liste.c
 * @author TAURAND Sébastien
 * @brief Définitions des fonctions permettant de travailler avec des listes génériques simplement chaïnées.
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <notify.h>
#include <liste.h>

/**
 * @param liste_p Pointeur sur une liste générique simplement chaînée
 * @param tailleElement Taille mémoire nécessaire pour contenir un élément de la liste
 * @param freeFn Pointeur sur la fonction de destruction des données dynamiques liées à l'élement de liste
 * @return Rien
 * @brief
 */
Liste_t *creeListe(fonctionDestructeur *freeFn) {
	Liste_t *liste_p=calloc(1, sizeof(*liste_p));
	if (!liste_p) ERROR_MSG("Impossible de créer une nouvelle liste");

	liste_p->fnDestructeur_p = freeFn;

	return liste_p;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return Rien
 * @brief Détruit l'ensemble des éléments de la liste en libérant la mémoire dynamique direct et indirecte associée
 */
Liste_t *detruitListe(Liste_t *liste_p) {
	ElementListe_t *elementCourant_p;
	if (liste_p) {
		INFO_MSG("Destruction de la Liste: %p : %d éléments",liste_p,liste_p->nbElements);
		while (liste_p->debut_liste_p != NULL) {
			elementCourant_p = liste_p->debut_liste_p;
			liste_p->debut_liste_p = elementCourant_p->suivant_p;

			if (liste_p->fnDestructeur_p) {
				liste_p->fnDestructeur_p(elementCourant_p->donnee_p);
			}
			else {
				free(elementCourant_p->donnee_p);
			}

			free(elementCourant_p);
		}
		free(liste_p);
	}
	return NULL;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param nouvelElement_p Pointeur sur le contenu du nouvel élément à insérer
 * @return Rien
 * @brief Insère un nouvel élément en début de liste
 */
void listeAjouteDebut(Liste_t *liste_p, void *donnee_p) {
	ElementListe_t *element_p = NULL;

	element_p=malloc(sizeof(*element_p));
	if (!element_p)
		ERROR_MSG("Impossible de créer un nouvel élément de liste");

	element_p->donnee_p = donnee_p;
	element_p->suivant_p = liste_p->debut_liste_p;
	liste_p->debut_liste_p = element_p;

	/* Cas où la liste était vide et que ce sera le premier élément */
	if (!liste_p->fin_liste_p)
		liste_p->fin_liste_p = liste_p->debut_liste_p;
	liste_p->nbElements++;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param nouvelElement_p Pointeur sur le contenu du nouvel élément à insérer
 * @return Rien
 * @brief Insère un nouvel élément en fin de liste
 */
void listeAjouteFin(Liste_t *liste_p, void *donnee_p) {
	ElementListe_t *element_p = NULL;

	element_p=malloc(sizeof(*element_p));
	if (!element_p)
		ERROR_MSG("Impossible de créer un nouvel élément de liste");

	element_p->donnee_p = donnee_p;
	element_p->suivant_p = NULL;

	if (liste_p->nbElements == 0) {
		liste_p->debut_liste_p = liste_p->fin_liste_p = element_p;
	} else {
		liste_p->fin_liste_p->suivant_p = element_p;
		liste_p->fin_liste_p = element_p;
	}
	liste_p->nbElements++;
}
