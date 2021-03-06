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
 * @enum Donnee_e
 * @brief Constantes de nature des données
 */
enum Donnee_e {
	D_UNDEF=-1,				/**< Nature indéfinie */
 	D_SPACE=0,				/**< Réservace d'espace */
 	D_BYTE=1,				/**< Octet */
 	D_WORD=2,				/**< Mot de 32 bits */
 	D_ASCIIZ=3				/**< Chaine de caractère avec '\0` à la fin */
};

/**
 * @struct Instruction_s
 * @brief Elément définissant une instruction machine
 */
struct Instruction_s {
	struct DefinitionInstruction_s *definition_p;	/**< Définition de l'instruction */
	unsigned int ligne;								/**< Numéro de ligne source associé à la ligne de lexème traitée */
	uint32_t decalage;								/**< Décalage de l'instruction */
	struct Lexeme_s *operandes[3];					/**< Lexème de l'opérande 1 */
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
		int32_t		mot;
		uint32_t	motNS;
		uint32_t	nbOctets;
		char		*chaine;
	} valeur;
};

char *clefEtiquette(void *donnee_p);

void affiche_table_etiquette(struct Table_s *table_p, char *titre);
void affiche_liste_donnee(struct Liste_s *liste_p, struct Table_s *table_p, char *titre_liste);
void affiche_liste_instructions(struct Liste_s *liste_p, struct Table_s *table_p, char *titre_liste);

int analyser_syntaxe(struct Liste_s *lignes_lexemes_p,
		struct Table_s *table_def_instructions_p, struct Table_s *table_def_registres_p, struct Table_s *table_etiquettes_p,
		struct Liste_s *liste_text_p, struct Liste_s *liste_data_p, struct Liste_s *liste_bss_p);

#endif /* _SYN_H_ */
