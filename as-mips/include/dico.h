/**
 * @file dico.h
 * @author BERTRAND Antoine, TAURAND Sébastien
 * @brief Definition des types et des prototypes de fonctions liées aux dictionnaires d'instructions et de registres
 */

#ifndef _DICO_H_
#define _DICO_H_

#include <inttypes.h>
#include <global.h>
#include <table.h>
#include <syn.h>


/**
 * @enum Operandes_e
 * @brief Constantes de nature des instructions du programme assembleur
 *
 * Cela servira à savoir la nature des opérandes ainsi que la façon d'intégrer ces opérandes dans le code opération
 */
enum Operandes_e {
 	I_OP_R=0,		/**< Pour cette instruction, les opérandes sont des registres séparés par des virgules  */
 	I_OP_N=1,		/**< Pour cette instruction, les opérandes sont des registres séparés par des virgules
 							sauf le dernier qui est un nombre ou un symbole */
 	I_OP_B=2,		/**< Pour cette instruction, on attend "registre" "," "nombre" "(" "registre" ")" */
};

enum Reloc_e {
	R_MIPS_PSEUDO=-2,			/**< Pour identifier les pseudos-instructions */
	R_MIPS_AUCUN=-1,			/**< Pour les instructions ne nécessitant pas de relocation */
 	R_MIPS_32=0,				/**< Dans liste_data_p si .word et si opérande est un SYMBOLE */
 	R_MIPS_26=1,				/**< Dans liste_text_p si instruction = J ou JAL et argument = SYMBOLE */
 	R_MIPS_HI16=2,				/**< Dans liste_text_p si instruction = LWI ou SWI et argument = SYMBOLE */
 	R_MIPS_LO16=3,				/**< Dans liste_text_p si instruction = LWI ou SWI et argument = SYMBOLE */
};

/**
 * @struct RemplacementPseudo_s
 * @brief Structure permettant de stocker la définition du remplacement d'une pseudo instruction
 */
struct RemplacementPseudo_s {
	char *syntaxe_remplacement;	/**< chaine de remplacement de pseudo instruction */
	unsigned int num_param[2];	/**< indice du paramètre de la pseudo instruction à appeler (0 = chaine vide) */
};

/**
 * @struct DefinitionPseudo_s
 * @brief Structure permettant de stocker la définition d'une pseudo instruction
 */
struct DefinitionPseudo_s {
	char* nom;									/**< nom de la pseudo instruction */
	unsigned int nb_instr;						/**< nombre de lignes de remplacement pour la pseudo-instruction */
	struct RemplacementPseudo_s **remplacement;	/**< Table pointant sur les lignes de remplacement */
};

/**
 * @struct CodeOperande_s
 * @brief Structure permettant de stocker la méthode d'encodage d'un opérande
 */
struct CodeOperande_s {
	unsigned int nb_bits;	/**< Nombre de bit utilisé pour le codage de l'opérande. 0 si pas d'opérande */
	int	signe;				/**< 0 indique un opérande non signé. 1 indique un opérande signé */
	int shift;				/**< Nombre de bits à décaler (positif à droite, négatif à gauche) */
	unsigned int dest_bit;	/**< Bit de destination pour le bit de poids faible */
};

/**
 * @struct DefinitionInstruction_s
 * @brief Structure permettant de stocker la définition d'une instruction
 */
struct DefinitionInstruction_s {
	char* nom;						/**< nom de l'instruction */
	enum Operandes_e type_ops;		/**< nature de l'instruction */
	unsigned int nb_ops;			/**< nombre d'opérandes de l'instruction */
	enum Reloc_e reloc;				/**< Type de relocation */
	uint32_t opcode;				/**< Code opération vierge */
	struct CodeOperande_s codes[3];	/**< Information pour l'encodage des opérandes */
};


/**
 * @struct RemplacementInstruction_s
 * @brief Structure permettant de stocker les informations de remplacement d'une pseudo instruction
 */
struct RemplacementInstruction_s {
	char* instruction;						/**< instruction à remplacer */
	char* arg[3];							/**< indique le contenu différenciation double cote ou non */
};


/**
 * @struct DefinitionPseudoInstruction_s
 * @brief Structure permettant de stocker une pseudo instruction
 */
struct DefinitionPseudoInstruction_s {
	char* nom;						/**< nom de la pseudo instruction */
	unsigned int nb_instruction;	/**< nombre d'instruction de la pseudo instruction */
	struct RemplacementInstruction_s rempl[NB_INSTR_PSEUDO_MAX];	/**< Information pour le remplacement des instructions */
};

/**
 * @struct DefinitionRegistre_s
 * @brief Structure permettant de stocker la correspondance entre le nom du registre et sa valeur
 */
struct DefinitionRegistre_s {
	char* nom;						/**< nom du registre */
	uint8_t valeur;					/**< valeur du registren */
};

char *clef_def_pseudo(void *donnee_p);
void destruction_def_pseudo(void *donnee_p);

char *clef_def_instruction(void *donnee_p);
void destruction_def_instruction(void *donnee_p);

char *clef_def_pseudo_instruction(void *donnee_p);
void destruction_def_pseudo_instruction(void *donnee_p);

char *clef_def_registre(void *donnee_p);
void destruction_def_registre(void *donnee_p);

int charge_def_pseudo(struct Table_s **table_definition_pp, char *nom_fichier);
int charge_def_instruction(struct Table_s **table_definition_pp, char *nom_fichier);
int charge_def_registre(struct Table_s **table_definition_pp, char *nom_fichier);

#endif /* _DICO_H_ */
