/**
 * @file table.h
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition de type et de prototypes de fonctions de dictionnaire par fonction de hachage
 */

#ifndef _TABLE_H_
#define _TABLE_H_

/* Definition commune de fonction permettant d'accéder à clef (str) en passant un pointeur sur une structure */
typedef char *(fonctionClef)(void *);

/* Definition commune d'une fonction de destruction du contenu des éléments */
typedef void (fonctionDestruction)(void *);

/*
 * Table Declaration
 */
struct Table_s {
    size_t nbEltsMax;
    size_t nbElts;
    void **table;
    fonctionClef *fnClef_p;
    fonctionDestruction *fnDestruction_p;
};

size_t nombrePremierGET(size_t nombre);
size_t tailleTableHachageRecommandee(size_t nbElementsPrevus);

size_t hashBernstein(char *chaine);
size_t hashKR2(char *chaine);

struct Table_s *creeTable(size_t nbElts, fonctionClef *fnClef_p, fonctionDestruction *fnDestruction_p);
struct Table_s *detruitTable(struct Table_s *table_p);

size_t indexTable(struct Table_s *table_p, char *clef);
void *donneeTable(struct Table_s *table_p, char *clef);
int insereElementTable(struct Table_s *table_p, void *donnee_p);
int supprimeElementTable(struct Table_s *table_p, char *clef);

void afficher_table(struct Table_s *table_p);

/*
HashTable_t *redimensionneTable(HashTable_t *table_p, size_t newSize);
*/

#endif /* _TABLE_H_ */
