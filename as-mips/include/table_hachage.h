/**
 * @file table_hachage.h
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition de type et de prototypes de fonctions de dictionnaire par fonction de hachage
 */

#ifndef _TABLE_HACHAGE_H
#define _TABLE_HACHAGE_H

/* Definition commune de fonction permettant d'accéder à clef (str) en passant un pointeur sur une structure */
typedef char *(fonctionClef)(void *);

/* Definition commune d'une fonction de destruction du contenu des éléments */
typedef void (fonctionDestruction)(void *);

/*
 * Table Declaration
 */
typedef struct TableHachage_s {
    size_t nbElementsMax;
    size_t nbElements;
    void **table;
    fonctionClef *fnClef_p;
    fonctionDestruction *fnDestruction_p;
} TableHachage_t;

void *copiePointeur(void *original_p, size_t taille);
char *clefStr(void *uneStr);
void destructionStr(void *uneStr);

size_t nombrePremierGET(size_t nombre);
size_t tailleTableHachageRecommandee(size_t nbElementsPrevus);

size_t hashBernstein(char *chaine);
size_t hashKR2(char *chaine);

TableHachage_t *creeTable(size_t nbElementsMax, fonctionClef *fnClef_p, fonctionDestruction *fnDestruction_p);
TableHachage_t *detruitTable(TableHachage_t *htable);

size_t indexTable(TableHachage_t *htable_p, char *clef);
void *donneeTable(TableHachage_t *htable_p, char *clef);
int insereElementTable(TableHachage_t *htable_p, void *donnee_p);
int supprimeElementTable(TableHachage_t *htable_p, char *clef);

/*
HashTable_t *redimensionneTable(HashTable_t *htable_p, int newSize);
*/
int test_hachage();

#endif /* _TABLE_HACHAGE_H */
