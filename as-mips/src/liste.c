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
 * @param freeFn Pointeur sur la fonction de destruction des données dynamiques liées à l'élement de liste
 * @return Pointeur sur la liste créée. NULL s'il y a eu des erreurs à la création.
 * @brief
 */
struct Liste_s *creer_liste(fonctionDestructeur *freeFn)
{
	struct Liste_s *liste_p=calloc(1, sizeof(*liste_p));
	if (!liste_p) {
		ERROR_MSG("Impossible de créer une nouvelle liste");
		return NULL;
	}
	liste_p->fnDestructeur_p = freeFn;
	return liste_p;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @return Pointeur NULL
 * @brief Détruit l'ensemble des éléments de la liste en libérant la mémoire dynamique direct et indirecte associée
 * pour ce faire, on utilise l'éventuelle fonction de destruction passée à la création de liste.
 */
struct Liste_s *detruire_liste(struct Liste_s *liste_p)
{
	struct Noeud_Liste_s *noeud_p;
	if (liste_p) {
		INFO_MSG("Destruction de la Liste: %p : %d éléments",liste_p,liste_p->nb_elts);
		while (liste_p->debut_p != NULL) {
			noeud_p = liste_p->debut_p;
			liste_p->debut_p = noeud_p->suivant_p;

			if (liste_p->fnDestructeur_p)
				liste_p->fnDestructeur_p(noeud_p->donnee_p);
			else
				free(noeud_p->donnee_p);

			free(noeud_p);
		}
		free(liste_p);
	}
	return NULL;
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param donnee_p Pointeur sur la donnée à insérer
 * @return Rien
 * @brief Insère un nouvel élément en début de liste
 */
void ajouter_debut_liste(struct Liste_s *liste_p, void *donnee_p)
{
	struct Noeud_Liste_s *noeud_p = NULL;

	if (liste_p) {
		noeud_p=malloc(sizeof(*noeud_p));
		if (!noeud_p)
			ERROR_MSG("Impossible de créer un nouvel élément de liste");

		noeud_p->donnee_p = donnee_p;
		noeud_p->suivant_p = liste_p->debut_p;
		liste_p->debut_p = noeud_p;

		/* Cas où la liste était vide et que ce sera le premier élément */
		if (!liste_p->fin_p)
			liste_p->fin_p = liste_p->debut_p;
		liste_p->nb_elts++;
		liste_p->courant_p = noeud_p;
	}
}

/**
 * @param liste_p pointeur sur une liste générique simplement chaînée
 * @param donnee_p Pointeur sur la donnée à insérer
 * @return Rien
 * @brief Insère un nouvel élément en fin de liste
 */
void ajouter_fin_liste(struct Liste_s *liste_p, void *donnee_p)
{
	struct Noeud_Liste_s *noeud_p = NULL;

	if (liste_p) {
		noeud_p=malloc(sizeof(*noeud_p));
		if (!noeud_p)
			ERROR_MSG("Impossible de créer un nouvel élément de liste");

		noeud_p->donnee_p = donnee_p;
		noeud_p->suivant_p = NULL;
		if (liste_p->fin_p)
			liste_p->fin_p->suivant_p = noeud_p;
		liste_p->fin_p = noeud_p;

		if (!liste_p->debut_p)
			liste_p->debut_p = noeud_p;
		liste_p->nb_elts++;
		liste_p->courant_p = noeud_p;
	}
}

struct Noeud_Liste_s *courant_liste(struct Liste_s *liste_p)
{
	if (liste_p)
		return liste_p->courant_p;
	else
		return NULL;
}

struct Noeud_Liste_s *debut_liste(struct Liste_s *liste_p)
{
	if (liste_p) {
		liste_p->courant_p = liste_p->debut_p;
		return liste_p->courant_p;
	} else
		return NULL;
}

struct Noeud_Liste_s *suivant_liste(struct Liste_s *liste_p)
{
	if (liste_p && liste_p->courant_p) {
		liste_p->courant_p = liste_p->courant_p->suivant_p;
		return liste_p->courant_p;
	} else
		return NULL;
}

struct Noeud_Liste_s *fin_liste(struct Liste_s *liste_p)
{
	if (liste_p) {
		liste_p->courant_p = liste_p->fin_p;
		return liste_p->courant_p;
	} else
		return NULL;
}

