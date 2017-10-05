/**
 * @file gen_list.h
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Travail avec les listes génériques simples
 *
 * Contient les définitions de types ainsi que les prototypes de fonctions associées
 */

#ifndef _GEN_LIST_
#define _GEN_LIST_

/* a common function used to free malloc'd objects */
typedef void (fonctionDestructeur)(void *);

typedef enum { FALSE, TRUE } bool;


typedef bool (*listIterator)(void *);

/**
 * @struct ElementListe_s
 * @brief Elément d'une liste simple générique
 */
typedef struct ElementListe_s {
  struct ElementListe_s *suivant_p;		/**< Pointeur sur le prochain élément de la liste générique simple */
  void *donnees_p;						/**< Pointeur non typé sur la donnée de l'élément de liste générique simple */
} ElementListe_t;

/**
 * @struct Liste_s
 * @brief Liste simple générique
 */
typedef struct Liste_s {
  int nbElements;						/**< Nombre d'éléments de la liste */
  ElementListe_t *debut_liste_p;		/**< Pointeur sur le premier élément de la liste générique simple. NULL si la liste est vide */
  ElementListe_t *fin_liste_p;			/**< Pointeur sur le dernier élément de la liste générique simple. NULL si la liste est vide */
  int tailleElements;					/**< Taille mémoire occupée par un élément de la liste générique simple */
  fonctionDestructeur *fnDestructeur_p;	/**< Pointeur sur la fonction de destruction d'un élément de liste. NULL s'il n'y a pas de destructeur associé */
} Liste_t;

/*
typedef struct ElementListeHeterogene_s {
  struct ElementListeHeterogene_s *next;
  int tailleElement;
  fonctionDestructeur fnDestructeur;
  void *data;
} ElementListeHeterogene_t;

typedef struct ListeHeterogene_s {
  int nbElements;
  ElementListeHeterogene_t *debut_liste_p;
  ElementListeHeterogene_t *fin_liste_p;
} ListeHeterogene_t;
*/

void list_new(Liste_t *list, int elementSize, fonctionDestructeur *freeFn);
void list_destroy(Liste_t *list);

void list_prepend(Liste_t *list, void *element);
void list_append(Liste_t *list, void *element);
int list_size(Liste_t *list);

void list_for_each(Liste_t *list, listIterator iterator);
void list_head(Liste_t *list, void *element, bool removeFromList);
void list_tail(Liste_t *list, void *element);

#endif
