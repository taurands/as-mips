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

	L_NOMBRE,				/**< Nombre */

	L_CAR,					/**< Caractère */
	L_CHAINE,				/**< chaine de caractère avec un zero final , de la forme "xxx\0" */
 	
 	L_COMMENTAIRE,			/**< Le commentaire commence par '#' et fini à la fin de la ligne. */
	L_FIN_LIGNE
};

/**
 * @struct Lexeme_s
 * @brief Type de l'objet Lexeme
 * 
 * La nature des lexème ainsi que leur éventuelle valeur seront ensuite utilisées dans l'analyse syntaxique
 */
struct Lexeme_s {
	char *data;						/**< Donnée éventuelle stockée sous forme de chaine de caractères. */
	enum Nature_lexeme_e nature;	/**< Identifie la nature du lexème. */
	int ligne;						/**< Ligne source du lexème */
};

int creer_lexeme(struct Lexeme_s **lexeme_pp, char *data, enum Nature_lexeme_e nature, int ligne);

void lex_read_line(char *ligne, struct Liste_s *liste_lexemes_p, unsigned int num_ligne, unsigned int *nb_etiquettes_p, unsigned int *nb_instructions_p);
void lex_load_file(char *nom_fichier, struct Liste_s *liste_lexemes_p, struct Liste_s *liste_lignes_source_p, unsigned int *nb_lignes_p, unsigned int *nb_etiquettes_p, unsigned int *nb_instructions_p);
void lex_standardise(char* in, char* out);

void visualisation_lexeme(struct Lexeme_s * lexeme_p);
void visualisation_liste_lexemes(struct Liste_s * liste_p);

void detruit_lexeme(void *lexeme_p);
char *nature_lex_to_str(enum Nature_lexeme_e nature);

#endif /* _LEX_H_ */

