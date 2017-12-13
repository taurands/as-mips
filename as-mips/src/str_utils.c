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
		destStr = calloc (1+strlen (sourceStr), sizeof(*destStr));
		if (!destStr)
			return NULL;
		else
			strcpy (destStr,sourceStr);
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
		if (!extract)
			return NULL;
		else
			memcpy (extract, debut, fin-debut);
		return extract;
	}
}

size_t str_unesc_len (char *str) {
	size_t len = 0;
	int escape = FALSE;
	char *car = str;
	while (car && *car) {
		if ((!escape) && (*car == ESCAPE_CHAR))
			escape = TRUE;
		else {
			escape = FALSE;
			len++;
		 }
		car++;
	}
	return len;
}

char unesc_char (char esc)
{
	char *esc_trouve = strchr(ESCAPED_CHAR, esc);
	if (esc_trouve)
		return *(UNESCAPED_CHAR + (esc_trouve - ESCAPED_CHAR));
	else
		return esc;
}

char sqstr_unesc_char (char *sqstr)
{
	char car = '\0';
	if (sqstr && (*sqstr == SQ_CHAR) && (*(sqstr+strlen(sqstr)-1) == SQ_CHAR)) {
		sqstr++;
		if (*sqstr != ESCAPE_CHAR)
			car = *sqstr;
		else {
			sqstr++;
			car = unesc_char (*sqstr);
		}
	}
	return car;
}

char *dqstr_unesc_str (char *dqstr)
{
	char *unesc_str = NULL;
	size_t i;
	size_t j=0;

	if (dqstr && (*dqstr == DQ_CHAR) && (*(dqstr + strlen (dqstr) - 1) == DQ_CHAR)) {
		unesc_str = calloc (str_unesc_len (dqstr) - 1, sizeof(*unesc_str));
		if (!unesc_str)
			return NULL;
		for (i = 1 ; i < strlen (dqstr) - 1 ; i++) {
			if (dqstr[i] == ESCAPE_CHAR)
				unesc_str[j++] = unesc_char (dqstr[++i]);
			else
				unesc_str[j++] = dqstr[i];;
		}
	}
	return unesc_str;
}

void replace_or_add_extension(char *nom_fichier, char *nouvelle_extension)
{
	char *ext = strrchr (nom_fichier, '.');
	if (!(ext) || (strchr(ext, '/'))) { /* pas de point trouvé dans le nom ou caractère '/' apparait après le dernier point du nom : il n'y a pas d'extension*/
		strcpy (nom_fichier + strlen (nom_fichier), nouvelle_extension);
	} else {
		strcpy (ext, nouvelle_extension);
	}

}
