/**
 * @file liste.h
 * @author TAURAND Sébastien
 * @brief Définitions de types et structures ainsi que des prototypes des fonctions de travail avec les listes génériques simplement chaînées.
 */

#ifndef _LISTE_H_
#define _LISTE_H_

/* a common function used to free malloc'd objects */
typedef void (fonctionDestructeur)(void *);

/**
 * @struct NoeudListe_s
 * @brief Elément d'une liste simple générique
 */
struct NoeudListe_s {
	struct NoeudListe_s *suivant_p;			/**< Pointeur sur le prochain élément de la liste générique simple */
	void *donnee_p;							/**< Pointeur non typé sur la donnée de l'élément de liste générique simple */
};

/**
 * @struct Liste_s
 * @brief Liste simplemement chaînée générique
 */
struct Liste_s {
	size_t nbElements;						/**< Nombre d'éléments de la liste. 0 si la liste est vide */
	struct NoeudListe_s *debut_liste_p;		/**< Pointeur sur le premier élément de la liste générique simple. NULL si la liste est vide */
	struct NoeudListe_s *fin_liste_p;		/**< Pointeur sur le dernier élément de la liste générique simple. NULL si la liste est vide */
	fonctionDestructeur *fnDestructeur_p;	/**< Pointeur sur la fonction de destruction d'un élément de liste. NULL s'il n'y a pas de destructeur associé */
};

struct Liste_s *creer_liste(fonctionDestructeur *freeFn);
struct Liste_s *detruire_liste(struct Liste_s *liste_p);

void ajouter_debut_liste(struct Liste_s *liste_p, void *donnee_p);
void ajouter_fin_liste(struct Liste_s *liste_p, void *donnee_p);

#endif /* _LISTE_H_ */
