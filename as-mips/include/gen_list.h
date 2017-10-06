/**
 * @file gen_list.h
 * @author TAURAND Sébastien
 * @brief Définitions de types ainsi que prototypes des fonctions de travail avec les listes génériques simplement chaînées.
 */

#ifndef _GEN_LIST_
#define _GEN_LIST_

/* a common function used to free malloc'd objects */
typedef void (fonctionDestructeur)(void *);

/**
 * @struct ElementListe_t
 * @brief Elément d'une liste simple générique
 *
 * Texte
 */
typedef struct ElementListe_t {
	struct ElementListe_t *suivant_p;		/**< Pointeur sur le prochain élément de la liste générique simple */
	void *donnees_p;						/**< Pointeur non typé sur la donnée de l'élément de liste générique simple */
} ElementListe_t;

/**
 * @struct Liste_t
 * @brief Liste simple générique
 */
typedef struct Liste_t {
	int nbElements;							/**< Nombre d'éléments de la liste */
	ElementListe_t *debut_liste_p;			/**< Pointeur sur le premier élément de la liste générique simple. NULL si la liste est vide */
	ElementListe_t *fin_liste_p;			/**< Pointeur sur le dernier élément de la liste générique simple. NULL si la liste est vide */
	int tailleElements;						/**< Taille mémoire occupée par un élément de la liste générique simple */
	fonctionDestructeur *fnDestructeur_p;	/**< Pointeur sur la fonction de destruction d'un élément de liste. NULL s'il n'y a pas de destructeur associé */
} Liste_t;

void initialiseListe(Liste_t *liste_p, int elementSize, fonctionDestructeur *freeFn);
void detruitListe(Liste_t *liste_p);

void ajouteElementDebutListe(Liste_t *liste_p, void *nouvelElement_p);
void ajouteElementFinListe(Liste_t *liste_p, void *nouvelElement_p);
int tailleListe(Liste_t *liste_p);

#endif
