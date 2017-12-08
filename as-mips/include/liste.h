/**
 * @file liste.h
 * @author TAURAND Sébastien
 * @brief Définitions de types et structures ainsi que des prototypes des fonctions de travail avec les listes génériques simplement chaînées.
 */

#ifndef _LISTE_H_
#define _LISTE_H_

/* fonction de libération de la mémoire allouée aux éléments de la liste */
typedef void (fonctionDestructeur)(void *);

/**
 * @struct Noeud_Liste_s
 * @brief Elément d'une liste simple générique
 */
struct Noeud_Liste_s {
	struct Noeud_Liste_s *suivant_p;		/**< Pointeur sur le prochain élément de la liste générique simple */
	void *donnee_p;							/**< Pointeur non typé sur la donnée de l'élément de liste générique simple */
};

/**
 * @struct Liste_s
 * @brief Liste simplemement chaînée générique
 */
struct Liste_s {
	size_t nb_elts;							/**< Nombre d'éléments de la liste. 0 si la liste est vide */
	struct Noeud_Liste_s *courant_p;		/**< Pointeur sur l'élément courant de la liste générique simple. NULL si la liste est vide */
	struct Noeud_Liste_s *debut_p;			/**< Pointeur sur le premier élément de la liste générique simple. NULL si la liste est vide */
	struct Noeud_Liste_s *fin_p;			/**< Pointeur sur le dernier élément de la liste générique simple. NULL si la liste est vide */
	fonctionDestructeur *fnDestructeur_p;	/**< Pointeur sur la fonction de destruction d'un élément de liste. NULL s'il n'y a pas de destructeur associé */
};

int creer_liste (struct Liste_s **liste_pp, fonctionDestructeur *freeFn);
int detruire_liste (struct Liste_s **liste_pp);

int ajouter_debut_liste (struct Liste_s *liste_p, void *donnee_p);
int ajouter_courant_liste (struct Liste_s *liste_p, void *donnee_p);
int ajouter_fin_liste (struct Liste_s *liste_p, void *donnee_p);

struct Noeud_Liste_s *courant_liste (struct Liste_s *liste_p);
struct Noeud_Liste_s *debut_liste (struct Liste_s *liste_p);
struct Noeud_Liste_s *suivant_liste (struct Liste_s *liste_p);
struct Noeud_Liste_s *fin_liste (struct Liste_s *liste_p);

#endif /* _LISTE_H_ */
