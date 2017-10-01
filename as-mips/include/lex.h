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

/**
 * \enum Nature_lexeme_t
 * \brief Constantes de nature de lexèmes
 * 
 * La nature des lexème einsi que leur éventuelle valeur seront ensuite utilisées dans l'analyse syntaxique
 */
typedef enum Nature_lexeme_t {
 	L_SYMBOLE,				/**< Chaine de caractères alphanumériques ne commençant pas par un nombre.
 							     Peut désigner une étiquette ou une instruction suivant sa place dans la ligne */
 	L_DIRECTIVE,			/**< commence par un '.'. */
 	L_ETIQUETTE,			/**< termine par un ':' */
 	
 	L_REGISTRE,				/**< Nom d'un registre, commence toujours par '$'. */
 	L_VIRGULE,				/**< ',' permettant de séparer les opérandes. */
 	L_PARANTHESE_OUVRANTE,	/**< '(' utilisé pour adressage indirect. */
 	L_PARANTHESE_FERMANTE,	/**< ')' utilisé pour finir adressage indirect. */

  	L_MOINS,
	L_PLUS,
	L_NOMBRE_DECIMAL,		/**< Nombre décimal. */
 	L_NOMBRE_OCTAL,			/**< Nombre octal. */
 	L_NOMBRE_HEXADECIMAL,	/**< Nombre hexanumérique. */
 	
 	
 	L_COMMENTAIRE,			/**< Le commentaire commence par '#' et fini à la fin de la ligne. */
 	L_ERREUR				/**< Mauvaise configuration de caractères. */
} Nature_lexeme_t;

/**
 * \enum Etat_lex_t
 * \brief Constantes de définition des états de la machine à états finis d'analyse lexicale
 * 
 */
typedef	enum Etat_lex_t {
	SYMBOLE=L_SYMBOLE,
	DIRECTIVE=L_DIRECTIVE,
	ETIQUETTE=L_ETIQUETTE,

	REGISTRE=L_REGISTRE,
	VIRGULE=L_VIRGULE,
	PARANTHESE_OUVRANTE=L_PARANTHESE_OUVRANTE,
	PARANTHESE_FERMANTE=L_PARANTHESE_FERMANTE,

	MOINS=L_MOINS,
	PLUS=L_PLUS,

	DECIMAL=L_NOMBRE_DECIMAL,
	OCTAL=L_NOMBRE_OCTAL,
	HEXADECIMAL=L_NOMBRE_HEXADECIMAL,

	COMMENTAIRE=L_COMMENTAIRE,

	ERREUR=L_ERREUR,

	INIT,					/**< Etat initial */
	DEBUT_HEXADECIMAL,		/**< On a lu "0x" et on attend le premier caractère hexadécimal */
	DECIMAL_ZERO,			/**< On a lu un "0" */
	POINT					/**< On a lu un point */
} Etat_lex_t;


/**
 * \struct Lexeme_t
 * \brief Type de l'objet Lexeme
 * 
 * La nature des lexème ainsi que leur éventuelle valeur seront ensuite utilisées dans l'analyse syntaxique
 */
typedef struct Lexeme_t {
	enum Nature_lexeme_t nature;	/**< Identifie la nature du lexème. */
	char * data;					/**< Donnée éventuelle stockée sous forme de chaine de caractères. */
} Lexeme_t;

/**
 * \struct Liste_lexeme_t
 * \brief Type de l'objet Liste_lexeme : c'est une liste chainée de lexeme
 * 
 * Cette liste permet de stocker les lexemes dans l'ordre du fichier source
 */
typedef struct Liste_lexeme_t {
	Lexeme_t lexeme;				/**< Champ lexeme */
	struct Liste_lexeme_t * suiv;	/**< Pointeur sur le prochain lexeme de la liste */ 
} Liste_lexeme_t;

/**
 * \struct Liste_liste_lexeme_t
 * \brief Type de l'objet Liste__lite_lexeme : c'est une liste chainée de liste de lexeme
 * 
 * Cette liste permet de stocker les liste de lexemes correspondant aux lignes du fichier source
 */
typedef struct Liste_liste_lexeme_t {
	Liste_lexeme_t * liste_lexeme;			/**< Champ lexeme */
	int ligne;								/**< Numéro de la ligne source */
	struct Liste_liste_lexeme_t * suiv;		/**< Pointeur sur la prochaine liste de lexeme */ 
} Liste_liste_lexeme_t;

Liste_lexeme_t * lex_read_line( char *, int);
Liste_liste_lexeme_t * lex_load_file( char *, unsigned int * );
void lex_standardise( char*, char*  );

Liste_liste_lexeme_t * add_lex_line(Liste_liste_lexeme_t * fin_liste_p, Liste_lexeme_t * debut_liste_p, int ligne);
void free_lex_lines(Liste_liste_lexeme_t * debut_liste_p);
void lex_lines_visualisation(Liste_liste_lexeme_t * debut_liste_p);

Liste_lexeme_t * add_lex_list_item(Liste_lexeme_t * fin_liste_p, enum Nature_lexeme_t nature, char* data);
void free_lex_list(Liste_lexeme_t * debut_liste_p);
void lex_visualisation(Lexeme_t * lexeme_p);
void lex_list_visualisation(Liste_lexeme_t * debut_liste_p);

char * etat_lex_to_str(Etat_lex_t etat);

#endif /* _LEX_H_ */

