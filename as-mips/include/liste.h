/**
 * @file liste.h
 * @author TAURAND Sébastien
 * @brief Définitions de types ainsi que prototypes des fonctions de travail avec les listes génériques simplement chaînées.
 */

#ifndef _LISTE_H_
#define _LISTE_H_

/* a common function used to free malloc'd objects */
typedef void (fonctionDestructeur)(void *);

/**
 * @struct ElementListe_s
 * @brief Elément d'une liste simple générique
 *
 * Texte
 */
typedef struct ElementListe_s {
	struct ElementListe_s *suivant_p;		/**< Pointeur sur le prochain élément de la liste générique simple */
	void *donnee_p;						/**< Pointeur non typé sur la donnée de l'élément de liste générique simple */
} ElementListe_t;

/**
 * @struct Liste_s
 * @brief Liste simplemement chainée générique
 */
typedef struct Liste_s {
	size_t nbElements;							/**< Nombre d'éléments de la liste */
	ElementListe_t *debut_liste_p;			/**< Pointeur sur le premier élément de la liste générique simple. NULL si la liste est vide */
	ElementListe_t *fin_liste_p;			/**< Pointeur sur le dernier élément de la liste générique simple. NULL si la liste est vide */
	fonctionDestructeur *fnDestructeur_p;	/**< Pointeur sur la fonction de destruction d'un élément de liste. NULL s'il n'y a pas de destructeur associé */
} Liste_t;

Liste_t *creeListe(fonctionDestructeur *freeFn);
Liste_t *detruitListe(Liste_t *liste_p);

void listeAjouteDebut(Liste_t *liste_p, void *nouvelElement_p);
void listeAjouteFin(Liste_t *liste_p, void *nouvelElement_p);

#endif /* _LISTE_H_ */
