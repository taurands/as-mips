/**
 * @file syntaxe.h
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Definition de type et de prototypes de fonctions liées au traitement syntaxique du fichier
 */

#ifndef _SYNTAXE_H_
#define _SYNTAXE_H_

#include <stdint.h>
#include <gen_list.h>
#include <table_hachage.h>

/**
 * @enum Section_e
 * @brief Constantes de nature des sections du programme assembleur
 */
enum Section_e {
 	S_INIT=0,				/**< état initial lorsque l'on n'a pas encore rencontré une direction de section */
 	S_TEXT=1,				/**< dès que l'on a rencontré une directive de section ".text", cela correspond aux instructions du programme */
 	S_DATA=2,				/**< dès que l'on a rencontré une directive de section ".data", cela correspond aux données du programme */
 	S_BSS=3					/**< dès que l'on a rencontré une directive de section ".bss", cela correspond aux données du tas du programme */
};

/**
 * @enum Instruction_e
 * @brief Constantes de nature des instructions du programme assembleur
 *
 * Cela servira à savoir la nature des opérandes ainsi que la façon d'intégrer ces opérandes dans le code opération
 */
enum Instruction_e {
 	I_PSEUDO=0,				/**< Pseudo instruction comme NOP */
 	I_REGISTRE=1,			/**<  */
 	I_DIRECT=2,				/**<  */
 	I_BASE_DEPLACEMENT=3,	/**<  */
	I_ABSOLU=4,				/**<  */
	I_RELATIF=5				/**<  */
};

/**
 * @enum Nature_Donnee_t
 * @brief Constantes de nature des données
 */
enum Donnee_e {
 	D_SPACE=0,				/**<  */
 	D_BYTE=1,				/**<  */
 	D_WORD=2,				/**<  */
 	D_ASCIIZ=3				/**<  */
};


/**
 * @struct Mot_Dictionnaire_s
 * @brief Structure permettant de stocker la définition d'une instruction
 */
typedef struct DefinitionInstruction_s {
	char* nom;						/**< nom de l'instruction */
	enum Instruction_e nature;		/**< nature de l'instruction */
	unsigned int nbOperandes;		/**< nombre d'arguments de l'instruction */
} DefinitionInstruction_t;

typedef DefinitionInstruction_t Mots_Dictionnaire_t[];

/**
 * @struct Dictionnaire_t
 * @brief Structure permettant de stocker l'ensemble des définitions d'instruction
 */
typedef struct Dictionnaire_t {
	int nbMots;							/**< Nombre de mots dans le dictionnaire */
	Mots_Dictionnaire_t *mots;			/**< Tableau contenant la définition de l'ensemble des instructions */
} Dictionnaire_t;

/**
 * @struct Instruction_s
 * @brief Elément définissant une instruction machine
 */
typedef struct Instruction_s {
	unsigned int ligneSource;				/**< Numéro de ligne source associé à la ligne de lexème traitée */
	uint32_t decalage;						/**< Décalage de l'instruction */
	uint32_t opCode;						/**< Code opération de l'instruction */
	DefinitionInstruction_t *definition_p;	/**< Nom de l'instruction */
	Lexeme_t *operande1_p;					/**< Lexème de l'opérande 1 */
	Lexeme_t *operande2_p;					/**< Lexème de l'opérande 2 */
	Lexeme_t *operande3_p;					/**< Lexème de l'opérande 3 */
} Instruction_t;


/**
 * @struct Etiquette_s
 * @brief Elément définissant une étiquette
 */
typedef struct Etiquette_s {
	Lexeme_t *nom_p;					/**< pointeur vers le Lexème contenant le nom de l'étiquette */
	unsigned int ligneSource;			/**< Numéro de ligne source associé à la ligne de lexème traitée */
	enum Section_e section;				/**< Section où se trouve l'étiquette */
	uint32_t decalage;					/**< décalage de l'adresse de l'étiquette par rapport à l'étiquette de la section */
} Etiquette_t;


/**
 * @struct Directive_s
 * @brief Elément définissant une directive
 */
typedef struct Directive_s {
	Lexeme_t *nom_directive; 					/**< nom de la directive */
	int ligneSource;						/**< Numéro de ligne source associé à la ligne de lexème traitée */
	int nb_operande;						/**< Nombre d'opérande de la directive */
	uint32_t decalage_operande;				/**< décalage de l'adresse de la directive par rapport à l'étiquette de la section */
	Lexeme_t *operande;						/**< Opérande suivant la directive */
} Directive_t;

/**
 * @struct Donnee_s
 * @brief Elément définissant un élément que l'on peut retrouver dans les sections .data ou .bss
 */
typedef struct Donnee_s {
	Lexeme_t *nom_p;				/**< permet de savoir si l'on travaille avec une étiquette ou une directive */
	unsigned int ligneSource;	/**< Numéro de ligne source associé à la ligne de lexème traitée */
	enum Section_e section;		/**< Section où se trouve l'étiquette */
	uint32_t decalage;			/**< décalage de l'adresse de la donnée par rapport à l'étiquette de la section */
	union {
		int8_t		octet;
		uint8_t		octetNS;
		char		car;
		char		*chaine;
		int32_t		mot;
		uint32_t	motNS;
		uint32_t	nbOctets;
	} valeur;
} Donnee_t;

/**
 * @struct Element_databss_t
 * @brief Elément définissant un élément que l'on peut retrouver dans les sections .sata ou .bss
 */
typedef struct Element_databss_s {
	Nature_lexeme_t identifiant;			/**< permet de savoir si l'on travaille avec une étiquette ou une directive */
	union {
		Etiquette_t etiquette;
		Directive_t directive;
	} union_databss;
} Element_databss_t;

Dictionnaire_t *chargeDictionnaire(char *nomFichierDictionnaire);
void effaceContenuDictionnaire(Dictionnaire_t *unDictionnaire_p);
int indexDictionnaire(Dictionnaire_t *unDictionnaire_p, char *unMot);

char *clefDefinitionInstruction(void *donnee_p);

Liste_t *analyseSyntaxe(Liste_t *lignesLexemes_p, Dictionnaire_t *monDictionnaire_p, TableHachage_t *tableEtiquettes_p);

void bonneInstruction(Liste_t* ligne_lexemes_p);

#endif /* _SYNTAXE_H_ */
