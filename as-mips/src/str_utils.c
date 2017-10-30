/**
 * @file str_utils.c
 * @author Sébastien TAURAND
 * @brief définition de fonctions utilitaires sur les strings
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <str_utils.h>

/**
 * @param sourceStr pointeur sur la chaine à dupliquer
 * @return pointeur sur la chaine dupliquée
 * @brief Cette fonction permet de faire ne copie de chaine de caractères
 */
char *strdup (char* sourceStr) {
	char* destStr=NULL;
	if (sourceStr) {
		destStr = malloc (sizeof(*destStr)*(1+strlen (sourceStr)));
		if (destStr) strcpy (destStr,sourceStr);
	}
	return destStr;
}

/**
 * @param sourceStr pointeur sur la chaine à passer en minuscules
 * @return pointeur sur la chaine passée en minuscules
 * @brief Cette fonction permet passer une chaine de caractères en minuscules
 *
 * Attention, le contenu de la chaine source est modifié
 */
char *strlwr (char* sourceStr)
{
    char* tmp = sourceStr;
    for (;*tmp;++tmp) {
        *tmp = tolower ((unsigned char) *tmp);
    }
    return sourceStr;
}

/**
 * @param sourceStr pointeur sur la chaine à passer en majuscules
 * @return pointeur sur la chaine passée en majuscules
 * @brief Cette fonction permet passer une chaine de caractères en majuscules
 *
 * Attention, le contenu de la chaine source est modifié
 */
char *strupr (char* sourceStr)
{
    char* tmp = sourceStr;
    for (;*tmp;++tmp) {
        *tmp = toupper ((unsigned char) *tmp);
    }
    return sourceStr;
}

char *strextract (char *debut, char *fin) {
	char *extract = NULL;

	if (!debut || !fin || (debut == fin))
		return NULL;
	else {
		extract = calloc (1+fin-debut, sizeof(*extract));
		if (extract)
			memcpy (extract, debut, fin-debut);
		return extract;
	}
}
