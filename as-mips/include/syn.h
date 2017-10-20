/**
 * @file syn.h
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition de type et de prototypes de fonctions liées au traitement syntaxique du fichier
 */

#ifndef _SYN_H_
#define _SYN_H_

#include <stdint.h>

#include <liste.h>
#include <table.h>

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
 * @enum Nature_struct Donnee_s
 * @brief Constantes de nature des données
 */
enum Donnee_e {
	D_UNDEF=-1,
 	D_SPACE=0,				/**<  */
 	D_BYTE=1,				/**<  */
 	D_WORD=2,				/**<  */
 	D_ASCIIZ=3				/**<  */
};

/**
 * @struct DefinitionInstruction_s
 * @brief Structure permettant de stocker la définition d'une instruction
 */
struct DefinitionInstruction_s {
	char* nom;						/**< nom de l'instruction */
	enum Instruction_e nature;		/**< nature de l'instruction */
	unsigned int nbOperandes;		/**< nombre d'arguments de l'instruction */
};

/**
 * @struct DefinitionRegistre_s
 * @brief Structure permettant de stocker la correspondance entre le nom du registre et sa valeur
 */
struct DefinitionRegistre_s {
	char* nom;						/**< nom du registre */
	uint8_t valeur;					/**< nombre d'arguments de l'instruction */
};

typedef struct DefinitionInstruction_s Mots_Dictionnaire_t[];

/**
 * @struct Dictionnaire_s
 * @brief Structure permettant de stocker l'ensemble des définitions d'instruction
 */
struct Dictionnaire_s {
	int nbMots;							/**< Nombre de mots dans le dictionnaire */
	Mots_Dictionnaire_t *mots;			/**< Tableau contenant la définition de l'ensemble des instructions */
};

/**
 * @struct Instruction_s
 * @brief Elément définissant une instruction machine
 */
typedef struct Instruction_s {
	struct DefinitionInstruction_s *definition_p;	/**< Définition de l'instruction */
	unsigned int ligneSource;						/**< Numéro de ligne source associé à la ligne de lexème traitée */
	uint32_t decalage;								/**< Décalage de l'instruction */
	Lexeme_t *operande1_p;							/**< Lexème de l'opérande 1 */
	Lexeme_t *operande2_p;							/**< Lexème de l'opérande 2 */
	Lexeme_t *operande3_p;							/**< Lexème de l'opérande 3 */
} Instruction_t;

/**
 * @struct Etiquette_s
 * @brief Elément définissant une étiquette
 */
struct Etiquette_s {
	Lexeme_t *nom_p;					/**< pointeur vers le Lexème contenant le nom de l'étiquette */
	unsigned int ligneSource;			/**< Numéro de ligne source associé à la ligne de lexème traitée */
	enum Section_e section;				/**< Section où se trouve l'étiquette */
	uint32_t decalage;					/**< décalage de l'adresse de l'étiquette par rapport à l'étiquette de la section */
};

/**
 * @struct Directive_s
 * @brief Elément définissant une directive
 */
struct Directive_s {
	Lexeme_t *nom_directive; 					/**< nom de la directive */
	int ligneSource;						/**< Numéro de ligne source associé à la ligne de lexème traitée */
	int nb_operande;						/**< Nombre d'opérande de la directive */
	uint32_t decalage_operande;				/**< décalage de l'adresse de la directive par rapport à l'étiquette de la section */
	Lexeme_t *operande;						/**< Opérande suivant la directive */
};

/**
 * @struct Donnee_s
 * @brief Elément définissant un élément que l'on peut retrouver dans les sections .data ou .bss
 */
struct Donnee_s {
	Lexeme_t *nom_p;				/**< permet de savoir si l'on travaille avec une étiquette ou une directive */
	unsigned int ligneSource;		/**< Numéro de ligne source associé à la ligne de lexème traitée */
	enum Donnee_e type;				/**< Type de la donnée à stocker */
	uint32_t decalage;				/**< décalage de l'adresse de la donnée par rapport à l'étiquette de la section */
	union {
		int8_t		octet;
		uint8_t		octetNS;
		char		car;
		char		*chaine;
		int32_t		mot;
		uint32_t	motNS;
		uint32_t	nbOctets;
	} valeur;
};

struct Dictionnaire_s *chargeDictionnaire(char *nomFichierDictionnaire);
void effaceContenuDictionnaire(struct Dictionnaire_s *unDictionnaire_p);
int indexDictionnaire(struct Dictionnaire_s *unDictionnaire_p, char *unMot);

char *clefDefinitionInstruction(void *donnee_p);
char *clefEtiquette(void *donnee_p);

void analyse_syntaxe(struct Liste_s *lignesLexemes_p, struct Dictionnaire_s *monDictionnaire_p, struct Table_s *tableEtiquettes_p,
					struct Liste_s *listeText_p, struct Liste_s *listeData_p, struct Liste_s *listeBss_p);

#endif /* _SYN_H_ */
