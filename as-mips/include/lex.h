/**
 * @file lex.h
 * @author François Portet <francois.portet@imag.fr>
 * @brief Lexem-related stuff.
 *
 * Contains lexem types definitions, some low-level syntax error codes,
 * the lexem structure definition and the associated prototypes.
 */

#ifndef _LEX_H_
#define _LEX_H_


#include <stdio.h>


void	lex_read_line( char *, int);
void	lex_load_file( char *, unsigned int * );
void	lex_standardise( char*, char*  );

#endif /* _LEX_H_ */

