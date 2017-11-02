/**
 * @file str_utils.h
 * @author Sébastien TAURAND
 * @brief prototype de fonctions utilitaires sur les strings
 */

#ifndef _STR_UTILS_H_
#define _STR_UTILS_H_

#define ESCAPE_CHAR '\\'
#define SQ_CHAR '\''
#define DQ_CHAR '"'

char *strdup (char* sourceStr);
char *strlwr (char* sourceStr);
char *strupr (char* sourceStr);
char *strextract (char *debut, char *fin);

#endif /* _STR_UTILS_H_ */
