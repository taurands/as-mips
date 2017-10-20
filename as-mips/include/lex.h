/**
 * @file lex.h
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Lexem-related stuff.
 *
 * Contains lexem types definitions, some low-level syntax error codes,
 * the lexem structure definition and the associated prototypes.
 */

#ifndef _LEX_H_
#define _LEX_H_

#include <liste.h>

/**
 * @enum Nature_lexeme_e
 * @brief Constantes de nature de lexèmes
 * 
 * La nature des lexème einsi que leur éventuelle valeur seront ensuite utilisées dans l'analyse syntaxique
 */
enum Nature_lexeme_e {
 	L_ERREUR,				/**< Mauvaise configuration de caractères. */

 	L_SYMBOLE,				/**< Chaine de caractères alphanumériques ne commençant pas par un nombre.
 							     Peut désigner une étiquette ou une instruction suivant sa place dans la ligne */
	L_INSTRUCTION,			/**< Le premier symbole d'une ligne sera une instruction */
 	L_DIRECTIVE,			/**< commence par un '.'. */
 	L_ETIQUETTE,			/**< termine par un ':' */
 	
 	L_REGISTRE,				/**< Nom d'un registre, commence toujours par '$'. */
 	L_VIRGULE,				/**< ',' permettant de séparer les opérandes. */
 	L_PARENTHESE_OUVRANTE,	/**< '(' utilisé pour adressage indirect. */
 	L_PARENTHESE_FERMANTE,	/**< ')' utilisé pour finir adressage indirect. */

	L_NOMBRE_DECIMAL,		/**< Nombre décimal. */
 	L_NOMBRE_OCTAL,			/**< Nombre octal. */
 	L_NOMBRE_HEXADECIMAL,	/**< Nombre hexanumérique. */

	L_CHAINE,				/**< chaine de caractère avec un zero final */
 	
 	L_COMMENTAIRE,			/**< Le commentaire commence par '#' et fini à la fin de la ligne. */
	L_FIN_LIGNE
};

/**
 * @enum Etat_lex_e
 * @brief Constantes de définition des états de la machine à états finis d'analyse lexicale
 * 
 */
enum Etat_lex_e {
	ERREUR=L_ERREUR,

	SYMBOLE=L_SYMBOLE,
	INSTRUCTION=L_INSTRUCTION,
	DIRECTIVE=L_DIRECTIVE,
	ETIQUETTE=L_ETIQUETTE,

	REGISTRE=L_REGISTRE,
	VIRGULE=L_VIRGULE,
	PARENTHESE_OUVRANTE=L_PARENTHESE_OUVRANTE,
	PARENTHESE_FERMANTE=L_PARENTHESE_FERMANTE,

	DECIMAL=L_NOMBRE_DECIMAL,
	OCTAL=L_NOMBRE_OCTAL,
	HEXADECIMAL=L_NOMBRE_HEXADECIMAL,

	CHAINE=L_CHAINE,

	COMMENTAIRE=L_COMMENTAIRE,
	FIN_LIGNE=L_FIN_LIGNE,

	INIT,					/**< Etat initial */
	MOINS,
	PLUS,
	DEBUT_HEXADECIMAL,		/**< On a lu "0x" et on attend le premier caractère hexadécimal */
	DECIMAL_ZERO,			/**< On a lu un "0" */
	POINT					/**< On a lu un point */
};


/**
 * @struct Lexeme_e
 * @brief Type de l'objet Lexeme
 * 
 * La nature des lexème ainsi que leur éventuelle valeur seront ensuite utilisées dans l'analyse syntaxique
 */
struct Lexeme_s {
	char *data;						/**< Donnée éventuelle stockée sous forme de chaine de caractères. */
	enum Nature_lexeme_e nature;	/**< Identifie la nature du lexème. */
	int ligne;						/**< Ligne source du lexème */
};

void lex_read_line( char *, struct Liste_s *listeLexemes_p, unsigned int, unsigned int *nbEtiquettes, unsigned int *nbInstructions_p);
void lex_load_file( char *, struct Liste_s *listeLexemes_p, unsigned int * ,unsigned int *nbEtiquettes, unsigned int *nbInstructions_p);
void lex_standardise( char*, char*  );

void visualisationLexeme(struct Lexeme_s * lexeme_p);
void visualisationListeLexemes(struct Liste_s * liste_p);

void detruitLexeme(void *Lexeme_p);
char * etat_lex_to_str(enum Etat_lex_e etat);

#endif /* _LEX_H_ */

