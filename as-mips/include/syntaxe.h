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

#include <lex.h>
#include <gen_list.h>
#include <global.h>
#include <notify.h>


/**
 * @struct Mot_Dictionnaire_t
 * @brief Elément définissant une instruction machine
 */
typedef struct Mot_Dictionnaire_t {
	char* instruction;					/**< nom de l'instruction */
	int nb_arg;							/**< nombre d'argument de l'instruction */
} Mot_Dictionnaire_t;

typedef Mot_Dictionnaire_t Dictionnaire_t[] ;


/**
 * @struct Instruction_t
 * @brief Elément définissant une instruction machine
 */
typedef struct Instruction_t {
	uint32_t pc;							/**< Adresse de l'instruction */
	uint32_t op_code;						/**< Code opération de l'instruction */
	char* acronyme;							/**< Nom de l'instruction */
	char* arg1;								/**< Acronyme de l'argument 1 */
	char* arg2;								/**< Acronyme de l'argument 2 */
	char* arg3;								/**< Acronyme de l'argument 3 */
} Instruction_t;

/* Définition d'une étiquette */



/* Définition d'une directive */

void bonneInstruction(Liste_t* ligne_lexemes_p);

#endif /* _SYNTAXE_H_ */
