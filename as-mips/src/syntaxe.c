#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <list_gen.h>

void bonneInstruction(Liste_t* liste_lexemes_p) {

	Liste_t* p_1=liste_lexemes_p;
	Liste_t* p_2=liste_lexemes_p;
	while (liste_lexemes_p->val>nature!=SYMBOLE) { /* On cherche le premier symbole (seul lexeme sucsecptible d'être une instruction) */
		p_1=p_1->suiv;
		p_2=p_2->suiv;
	}
}
