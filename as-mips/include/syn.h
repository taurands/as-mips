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
 	S_BSS=3,				/**< dès que l'on a rencontré une directive de section ".bss", cela correspond aux données du tas du programme */
	S_UNDEF=4				/**< Etat lors du traitement des relocations où les étiquettes ne sont pas définies */
};

/**
 * @enum Donnee_e
 * @brief Constantes de nature des données
 */
enum Donnee_e {
	D_UNDEF=-1,				/**< Nature indéfinie */
 	D_SPACE=0,				/**< Réservace d'espace */
 	D_BYTE=1,				/**< Octet */
	D_HALF=2,				/**< demi mot (16 bits) */
 	D_WORD=3,				/**< Mot de 32 bits */
	D_FLOAT=4,				/**< nombre flottant simple 32 bits */
 	D_ASCIIZ=5				/**< Chaine de caractère avec '\0` à la fin */
};

/**
 * @struct Instruction_s
 * @brief Elément définissant une instruction machine
 */
struct Instruction_s {
	struct DefinitionInstruction_s *definition_p;	/**< Définition de l'instruction */
	unsigned int ligne;								/**< Numéro de ligne source associé à la ligne de lexème traitée */
	uint32_t decalage;								/**< Décalage de l'instruction */
	uint32_t op_code;								/**< Code opération de l'instruction */
	struct Lexeme_s *operandes[3];					/**< Lexème de l'opérande 1 */
	char *source;									/**< Pointeur sur le texte source de cette instruction */
};

/**
 * @struct Etiquette_s
 * @brief Elément définissant une étiquette
 */
struct Etiquette_s {
	struct Lexeme_s *lexeme_p;			/**< pointeur vers le Lexème contenant le nom de l'étiquette */
	unsigned int ligne;					/**< Numéro de ligne source associé à la ligne de lexème traitée */
	enum Section_e section;				/**< Section où se trouve l'étiquette */
	uint32_t decalage;					/**< décalage de l'adresse de l'étiquette par rapport à l'étiquette de la section */
};

/**
 * @struct Donnee_s
 * @brief Elément définissant un élément que l'on peut retrouver dans les sections .data ou .bss
 */
struct Donnee_s {
	struct Lexeme_s *lexeme_p;		/**< permet de récupérer la définition du symbole le cas échéant */
	unsigned int ligne;				/**< Numéro de ligne source associé à la ligne de lexème traitée */
	enum Donnee_e type;				/**< Type de la donnée stockée */
	uint32_t decalage;				/**< décalage de l'adresse de la donnée par rapport à l'étiquette de la section */
	union {
		int8_t		octet;
		uint8_t		octetNS;
		char		car;
		int16_t		demi;
		uint16_t	demiNS;
		int32_t		mot;
		uint32_t	motNS;
		uint32_t	nbOctets;
		float		simple;
		char		*chaine;
	} valeur;
};

char *clefEtiquette (void *donnee_p);
void detruit_donnee (void *donnee_p);
void detruit_etiquette(void *donnee_p);

char *type_donnee_to_str(enum Donnee_e donnee);

int analyser_syntaxe (struct Liste_s *lignes_lexemes_p, struct Liste_s *lexemes_supl_p,
		struct Table_s *table_def_instructions_p, struct Table_s *table_def_pseudo_p, struct Table_s *table_def_registres_p,
		struct Liste_s *liste_etiquette_p, struct Table_s *table_etiquettes_p,
		struct Liste_s *liste_text_p, struct Liste_s *liste_data_p, struct Liste_s *liste_bss_p,
		uint32_t *decalage_text_p, uint32_t *decalage_data_p, uint32_t *decalage_bss_p);

void encodage_liste_instruction(struct Liste_s *liste_text_p,struct Table_s *table_etiquettes_p, struct Table_s *table_def_registres_p, struct Table_s *table_def_instructions_p);

#endif /* _SYN_H_ */
