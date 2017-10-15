/**
 * @file table_hachage.h
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Definition de type et de prototypes de fonctions de dictionnaire par fonction de hachage
 */

#ifndef _TABLE_HACHAGE_H
#define _TABLE_HACHAGE_H

/* Definition commune de fonction permettant d'accéder à clef (str) en passant un pointeur sur une structure */
typedef char *(fonctionClef)(void *);

unsigned int hashBernstein(char *chaine);
unsigned int hashKR2(char *chaine);

int test_hachage();

#endif /* _TABLE_HACHAGE_H */
