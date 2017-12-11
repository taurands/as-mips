/**
 * @file liste.c
 * @author TAURAND Sébastien
 * @brief Définitions des fonctions permettant de travailler avec des listes génériques simplement chaïnées.
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <global.h>
#include <notify.h>
#include <liste.h>

/**
 * @param freeFn Pointeur sur la fonction de destruction des données dynamiques liées à l'élement de liste
 * @return Pointeur sur la liste créée. NULL s'il y a eu des erreurs à la création.
 * @brief
 */
int creer_liste(struct Liste_s **liste_pp, fonctionDestructeur *freeFn)
{
	if (liste_pp && !*liste_pp) {
		(*liste_pp)=calloc(1, sizeof(**liste_pp));
		if (!*liste_pp)
			return FAIL_ALLOC;

		(*liste_pp)->fnDestructeur_p = freeFn;
		return SUCCESS;
	} else
		return FAILURE;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return SUCCESS si la liste à détruire existe, FAILURE sinon
 * @brief Détruit l'ensemble des éléments de la liste en libérant la mémoire dynamique direct et indirecte associée
 * pour ce faire, on utilise l'éventuelle fonction de destruction passée à la création de liste.
 */
int detruire_liste(struct Liste_s **liste_pp)
{
	struct Noeud_Liste_s *noeud_p;
	if (liste_pp && (*liste_pp)) {
		INFO_MSG("Destruction de la Liste: %p : %d éléments",liste_p,liste_p->nb_elts);
		while ((*liste_pp)->debut_p != NULL) {
			noeud_p = (*liste_pp)->debut_p;
			(*liste_pp)->debut_p = noeud_p->suivant_p;

			if ((*liste_pp)->fnDestructeur_p)
				(*liste_pp)->fnDestructeur_p(noeud_p->donnee_p);
			else
				free(noeud_p->donnee_p);

			free(noeud_p);
		}
		free(*liste_pp);
		*liste_pp = NULL;
		return SUCCESS;
	} else
		return FAILURE;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param donnee_p Pointeur sur la donnée à insérer
 * @return FAILURE si la liste n'existe pas, FAIL_ALLOC en cas d'impossibilité d'allocation mémoire ou SUCCESS si la donnée est bien insérée
 * @brief Insère un nouvel élément en début de liste
 */
int ajouter_debut_liste(struct Liste_s *liste_p, void *donnee_p)
{
	struct Noeud_Liste_s *noeud_p = NULL;

	if (liste_p) {
		noeud_p=calloc (1, sizeof(*noeud_p));
		if (!noeud_p)
			return FAIL_ALLOC;

		noeud_p->donnee_p = donnee_p;
		noeud_p->suivant_p = liste_p->debut_p;
		liste_p->debut_p = noeud_p;
		liste_p->courant_p = noeud_p;

		/* Cas où la liste était vide et que ce sera le premier élément */
		if (!liste_p->fin_p)
			liste_p->fin_p = liste_p->debut_p;
		liste_p->nb_elts++;
		return SUCCESS;
	} else
		return FAILURE;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param donnee_p Pointeur sur la donnée à insérer
 * @return FAILURE si la liste n'existe pas, FAIL_ALLOC en cas d'impossibilité d'allocation mémoire ou SUCCESS si la donnée est bien insérée
 * @brief Insère un nouvel élément après l'élément courant de la liste
 */
int ajouter_courant_liste (struct Liste_s *liste_p, void *donnee_p)
{
	struct Noeud_Liste_s *noeud_p = NULL;

	if (liste_p) {
		if (!liste_p->courant_p)
			return ajouter_fin_liste (liste_p, donnee_p);
		else {
			noeud_p=calloc (1, sizeof(*noeud_p));
			if (!noeud_p)
				return FAIL_ALLOC;

			noeud_p->donnee_p = donnee_p;
			noeud_p->suivant_p = liste_p->courant_p->suivant_p;
			liste_p->courant_p->suivant_p = noeud_p;
			liste_p->courant_p = noeud_p;


			/* Cas où l'on est le dernier élément de la liste */
			if (!liste_p->courant_p->suivant_p)
				liste_p->fin_p = liste_p->courant_p;
			liste_p->nb_elts++;
			return SUCCESS;
		}
	} else
		return FAILURE;

}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param donnee_p Pointeur sur la donnée à insérer
 * @return FAILURE si la liste n'existe pas, FAIL_ALLOC en cas d'impossibilité d'allocation mémoire ou SUCCESS si la donnée est bien insérée
 * @brief Insère un nouvel élément en fin de liste
 */
int ajouter_fin_liste(struct Liste_s *liste_p, void *donnee_p)
{
	struct Noeud_Liste_s *noeud_p = NULL;

	if (liste_p) {
		noeud_p=calloc (1, sizeof(*noeud_p));
		if (!noeud_p)
			return FAIL_ALLOC;

		noeud_p->donnee_p = donnee_p;
		noeud_p->suivant_p = NULL;
		if (liste_p->fin_p)
			liste_p->fin_p->suivant_p = noeud_p;
		liste_p->fin_p = noeud_p;
		liste_p->courant_p = noeud_p;

		if (!liste_p->debut_p)
			liste_p->debut_p = noeud_p;
		liste_p->nb_elts++;
		return SUCCESS;
	} else
		return FAILURE;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return Pointeur sur l'élement courant de la liste s'ils existent, NULL sinon
 * @brief Donne l'élément courant d'une liste générique simplement chaînée
 */
struct Noeud_Liste_s *courant_liste(struct Liste_s *liste_p)
{
	if (liste_p)
		return liste_p->courant_p;
	else
		return NULL;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return Pointeur sur le premier élément de la liste s'ils existent, NULL sinon
 * @brief Se met sur le premier élément d'une liste générique simplement chaînée et le renvoie
 */
struct Noeud_Liste_s *debut_liste(struct Liste_s *liste_p)
{
	if (liste_p) {
		liste_p->courant_p = liste_p->debut_p;
		return liste_p->courant_p;
	} else
		return NULL;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return Pointeur sur l'élément suivant de la liste s'ils existent, NULL sinon
 * @brief Se met sur l'élément suivant l'élément courant d'une liste générique simplement chaînée et le renvoie
 */
struct Noeud_Liste_s *suivant_liste(struct Liste_s *liste_p)
{
	if (liste_p && liste_p->courant_p) {
		liste_p->courant_p = liste_p->courant_p->suivant_p;
		return liste_p->courant_p;
	} else
		return NULL;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return Pointeur sur le dernier élément de la liste s'ils existent, NULL sinon
 * @brief Se met sur le dernier élément d'une liste générique simplement chaînée et le renvoie
 */
struct Noeud_Liste_s *fin_liste(struct Liste_s *liste_p)
{
	if (liste_p) {
		liste_p->courant_p = liste_p->fin_p;
		return liste_p->courant_p;
	} else
		return NULL;
}

