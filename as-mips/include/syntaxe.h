/**
 * @file syntaxe.h
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Definition de type et de prototypes de fonctions liées au traitement syntaxique du fichier
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
 * @enum Nature_Section_t
 * @brief Constantes de nature des sections du programme assembleur
 */
typedef enum Nature_Section_t {
 	S_INIT=0,				/**< état initial lorsque l'on n'a pas encore rencontré une direction de section */
 	S_TEXT=1,				/**< dès que l'on a rencontré une directive de section ".text", cela correspond aux instructions du programme */
 	S_DATA=2,				/**< dès que l'on a rencontré une directive de section ".data", cela correspond aux données statiques du programme */
 	S_BSS=3					/**< dès que l'on a rencontré une directive de section ".bss", cela correspond aux données du tas du programme */
} Nature_Section_t;

/**
 * @enum Nature_Instruction_t
 * @brief Constantes de nature des instructions du programme assembleur
 */
typedef enum Nature_Instruction_t {
 	I_PSEUDO=0,				/**<  */
 	I_REGISTRE=1,			/**<  */
 	I_DIRECT=2,				/**<  */
 	I_BASE_DEPLACEMENT=3,	/**<  */
	I_ABSOLU=4,				/**<  */
	I_RELATIF=5				/**<  */
} Nature_Instruction_t;

/**
 * @enum Nature_Donnee_t
 * @brief Constantes de nature des données
 */
typedef enum Nature_Donnee_t {
 	D_SPACE=0,				/**<  */
 	D_BYTE=1,				/**<  */
 	D_WORD=2,				/**<  */
 	D_ASCIIZ=3				/**<  */
} Nature_Donnee_t;


/**
 * @struct Mot_Dictionnaire_t
 * @brief Structure permettant de stocker la définition d'une instruction
 */
typedef struct Mot_Dictionnaire_t {
	char* instruction;					/**< nom de l'instruction */
	enum Nature_Instruction_t nature;	/**< nature de l'instruction */
	int nb_arg;							/**< nombre d'arguments de l'instruction */
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
	/*Lexeme_t *etiquette;	*/				/**< Etiquette de la ligne */
	Lexeme_t *nom_instruction;					/**< Nom de l'instruction */
	Lexeme_t *arg1;							/**< Lexème de l'argument 1 */
	Lexeme_t *arg2;							/**< Lexème de l'argument 2 */
	Lexeme_t *arg3;							/**< Lexème de l'argument 3 */
	/*Lexeme_t *arg4;	*/					/**< Lexème de l'argument 4 */
	/*Lexeme_t *commentaire;	*/			/**< Lexème commentaire */
} Instruction_t;


/**
 * @struct Etiquette_t
 * @brief Elément définissant une étiquette
 */
typedef struct Etiquette_t {
	Lexeme_t *nom_etiquette;				/**< nom de l'étiquette */
	int ligneSource;						/**< Numéro de ligne source associé à la ligne de lexème traitée */
	Nature_Section_t section;				/**< Section où se trouve l'étiquette */
	uint32_t decalage_etiquette;						/**< décalage de l'adresse de l'étiquette par rapport à l'étiquette de la section */
} Etiquette_t;


/**
 * @struct Directive_t
 * @brief Elément définissant une directive
 */
typedef struct Directive_t {
	Lexeme_t *nom_directive; 					/**< nom de la directive */
	int ligneSource;						/**< Numéro de ligne source associé à la ligne de lexème traitée */
	int nb_operande;						/**< Nombre d'opérande de la directive */
	uint32_t decalage_operande;				/**< décalage de l'adresse de la directive par rapport à l'étiquette de la section */
	Lexeme_t *operande;						/**< Opérande suivant la directive */
} Directive_t;


/**
 * @struct Element_databss_t
 * @brief Elément définissant un élément que l'on peut retrouver dans les sections .sata ou .bss
 */
typedef struct Element_databss_t {
	Nature_lexeme_t identifiant;			/**< permet de savoir si l'on travaille avec une étiquette ou une directive */
	union {
		Etiquette_t etiquette;
		Directive_t directive;
	}union_databss;
} Element_databss_t;

Dictionnaire_t *chargeDictionnaire(char *nomFichierDictionnaire);
void effaceContenuDictionnaire(Dictionnaire_t *unDictionnaire_p);
int indexDictionnaire(Dictionnaire_t *unDictionnaire_p, char *unMot);

Liste_t *analyseSyntaxe(Liste_t *lignesLexemes_p, Dictionnaire_t *monDictionnaire_p);

void bonneInstruction(Liste_t* ligne_lexemes_p);

#endif /* _SYNTAXE_H_ */
