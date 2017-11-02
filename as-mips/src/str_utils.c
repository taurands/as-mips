/**
 * @file str_utils.c
 * @author Sébastien TAURAND
 * @brief définition de fonctions utilitaires sur les strings
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <global.h>
#include <str_utils.h>

char ESCAPED_CHAR[] =   {'\'', '"', 'n',  'r',  't',  'f',  'v',  'a',  'b',  '\\', '0', '\0' };
char UNESCAPED_CHAR[] = {'\'', '"', '\n', '\r', '\t', '\f', '\v', '\a', '\b', '\\', '\0'};

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

size_t str_unesc_len (char *str) {
	size_t len = 0;
	int escape = FALSE;
	char *car = str;
	while (car && *car) {
		if ((!escape) && (*car = ESCAPE_CHAR))
			escape = TRUE;
		else {
			escape = FALSE;
			len++;
		 }
	}
	return len;
}

char unesc_char (char esc) {
	char *esc_trouve = strchr(ESCAPED_CHAR, esc);
	if (esc_trouve)
		return *(UNESCAPED_CHAR + (esc_trouve - ESCAPED_CHAR));
	else
		return esc;
}

char str_squnesc_char (char *dqstr) {
	char car = '\0';
	if (dqstr && (*dqstr == SQ_CHAR)) {
		dqstr++;
		if (*dqstr != ESCAPE_CHAR)
			car = *dqstr;
		else {
			dqstr++;
			car = unesc_char (*dqstr);
		}
	}
	return car;
}

char str_dqunesc_str (char *dqstr) {
	char car = '\0';
	if (dqstr && (*dqstr == SQ_CHAR)) {
		dqstr++;
		if (*dqstr != ESCAPE_CHAR)
			car = *dqstr;
		else {
			dqstr++;
			car = unesc_char (*dqstr);
		}
	}
	return car;
}

