/**
 * @file table.h
 * @author BERTRAND Antoine, TAURAND Sébastien
 * @brief Definition de type et de prototypes de fonctions pour les tables de hachage génériques
 */

#ifndef _TABLE_H_
#define _TABLE_H_

/* Definition commune de fonction permettant d'accéder à clef (str) en passant un pointeur sur une structure */
typedef char *(fonctionClef)(void *);

/* Definition commune d'une fonction de destruction du contenu des éléments */
typedef void (fonctionDestruction)(void *);

/**
 * @struct Table_s
 * @brief Table de hachage générique
 */
struct Table_s {
    size_t nb_elts_max;						/**< Taille de la table de hachage */
    size_t nb_elts;							/**< Nombre d'éléments qui y sont vraiment */
    void **table;							/**< Pointeur sur la table de pointeurs d'éléments */
    fonctionClef *fnClef_p;					/**< Pointeur sur la fonction Clef */
    fonctionDestruction *fnDestruction_p;	/**< Pointeur sur la fonction de destruction */
};

size_t nombre_premier_GET(size_t nombre);
size_t taille_table_hachage_recommandee(size_t nbElementsPrevus);

size_t hash_Bernstein(char *chaine);
size_t hash_KR2(char *chaine);

int creer_table(struct Table_s **table_pp, size_t nb_elts, fonctionClef *fn_clef_p, fonctionDestruction *fn_destruction_p);
int detruire_table(struct Table_s **table_pp);

size_t index_table(struct Table_s *table_p, char *clef);
void *donnee_table(struct Table_s *table_p, char *clef);
int ajouter_table(struct Table_s *table_p, void *donnee_p);
int supprimer_table(struct Table_s *table_p, char *clef);

#endif /* _TABLE_H_ */
