/**
 * @file table_hachage.h
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
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
typedef struct HashTable_t {
    int nbElementsMax;
    int nbElements;
    void **table;
    fonctionClef *fnClef_p;
    fonctionDestruction *fnDestruction_p;
} HashTable_t;

char *clefStr(void *uneStr);
void destructionStr(void *uneStr);

unsigned int nombrePremierGET(unsigned int nombre);
unsigned int tailleTableHachageRecommandee(unsigned int nbElementsPrevus);

unsigned int hashBernstein(char *chaine);
unsigned int hashKR2(char *chaine);

HashTable_t *creeTable(unsigned int nbElementsMax, fonctionClef *fnClef_p, fonctionDestruction *fnDestruction_p);
HashTable_t *detruitTable(HashTable_t *htable);

unsigned int trouve(HashTable_t *htable_p, char *key);
int insere(HashTable_t *htable_p, void *donnee_p);

/*
HashTable_t *redimensionneTable(HashTable_t *htable_p, int newSize);
*/
int test_hachage();

#endif /* _TABLE_HACHAGE_H */
