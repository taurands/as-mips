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
 * @struct Instruction_s
 * @brief Elément définissant une instruction machine
 */
struct Instruction_s {
	struct DefinitionInstruction_s *definition_p;	/**< Définition de l'instruction */
	unsigned int ligne;						/**< Numéro de ligne source associé à la ligne de lexème traitée */
	uint32_t decalage;								/**< Décalage de l'instruction */
	struct Lexeme_s *operandes[3];							/**< Lexème de l'opérande 1 */
};

/**
 * @struct Etiquette_s
 * @brief Elément définissant une étiquette
 */
struct Etiquette_s {
	struct Lexeme_s *nom_p;				/**< pointeur vers le Lexème contenant le nom de l'étiquette */
	unsigned int ligneSource;			/**< Numéro de ligne source associé à la ligne de lexème traitée */
	enum Section_e section;				/**< Section où se trouve l'étiquette */
	uint32_t decalage;					/**< décalage de l'adresse de l'étiquette par rapport à l'étiquette de la section */
};

/* XXX A-t-on vraiment besoin de Directive_s ? */
/**
 * @struct Directive_s
 * @brief Elément définissant une directive
 */
struct Directive_s {
	struct Lexeme_s *nom_directive; 					/**< nom de la directive */
	int ligneSource;						/**< Numéro de ligne source associé à la ligne de lexème traitée */
	int nb_operande;						/**< Nombre d'opérande de la directive */
	uint32_t decalage_operande;				/**< décalage de l'adresse de la directive par rapport à l'étiquette de la section */
	struct Lexeme_s *operande;						/**< Opérande suivant la directive */
};

/**
 * @struct Donnee_s
 * @brief Elément définissant un élément que l'on peut retrouver dans les sections .data ou .bss
 */
struct Donnee_s {
	struct Lexeme_s *nom_p;			/**< permet de savoir si l'on travaille avec une étiquette ou une directive */
	unsigned int ligneSource;		/**< Numéro de ligne source associé à la ligne de lexème traitée */
	enum Donnee_e type;				/**< Type de la donnée à stocker */
	uint32_t decalage;				/**< décalage de l'adresse de la donnée par rapport à l'étiquette de la section */
	union {
		int8_t		octet;
		uint8_t		octetNS;
		int32_t		mot;
		uint32_t	motNS;
		uint32_t	nbOctets;
		char		*chaine;
	} valeur;
};

char *clefEtiquette(void *donnee_p);

void analyse_syntaxe(struct Liste_s *lignesLexemes_p,
		struct Table_s *tableDefinitionInstructions_p, struct Table_s *tableDefinitionRegistres_p, struct Table_s *tableEtiquettes_p,
		struct Liste_s *listeText_p, struct Liste_s *listeData_p, struct Liste_s *listeBss_p);

#endif /* _SYN_H_ */
