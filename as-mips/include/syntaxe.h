/**
 * @file lex.h
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief syntax-related stuff.
 *
 * Contains syntax types definitions, some low-level syntax error codes,
 */

#ifndef _SYNTAXE_H_
#define _SYNTAXE_H_

#include <stdint.h>

/*
#include <lex.h>
#include <gen_list.h>
#include <global.h>
#include <notify.h>
*/

/**
 * @struct Mot_Dictionnaire_t
 * @brief Structure permettant de stocker la définition d'une instruction
 */
typedef struct Mot_Dictionnaire_t {
	char* instruction;					/**< nom de l'instruction */
	int nb_arg;							/**< nombre d'argument de l'instruction */
} Mot_Dictionnaire_t;

typedef Mot_Dictionnaire_t Mots_Dictionnaire_t[];
/**
 * @struct Dictionnaire_t
 * @brief Structure permettant de stocker l'ensemble des définitions d'instruction
 */
typedef struct Dictionnaire_t {
	int nbMots;							/**< Nombre de mots dans le dictionnaire */
	Mots_Dictionnaire_t *mots;			/**< Tableau contenant la définition de l'ensemble des instructions */
} Dictionnaire_t;


/**
 * @struct Instruction_t
 * @brief Elément définissant une instruction machine
 */
typedef struct Instruction_t {
	int ligneSource;						/**< Numéro de ligne source associé à la ligne de lexème traitée */
	uint32_t pc;							/**< Adresse de l'instruction */
	uint32_t op_code;						/**< Code opération de l'instruction */
	Lexeme_t *etiquette;					/**< Etiquette de la ligne */
	Lexeme_t *instruction;					/**< Nom de l'instruction */
	Lexeme_t *arg1;							/**< Lexème de l'argument 1 */
	Lexeme_t *arg2;							/**< Lexème de l'argument 2 */
	Lexeme_t *arg3;							/**< Lexème de l'argument 3 */
	Lexeme_t *arg4;							/**< Lexème de l'argument 4 */
	Lexeme_t *commentaire;					/**< Lexème commentaire */
} Instruction_t;

Dictionnaire_t *chargeDictionnaire(char *nomFichierDictionnaire);
void effaceContenuDictionnaire(Dictionnaire_t *unDictionnaire_p);
int indexDictionnaire(Dictionnaire_t *unDictionnaire_p, char *unMot);

void bonneInstruction(Liste_t* ligne_lexemes_p);

#endif /* _SYNTAXE_H_ */
