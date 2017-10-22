/**
 * @file dico.h
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition des dictionnaires et des fonctions liées aux dictionnaires
 */

#ifndef _DICO_H_
#define _DICO_H_

#include <table.h>

/**
 * @enum Instruction_e
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
 * @struct DefinitionInstruction_s
 * @brief Structure permettant de stocker la définition d'une instruction
 */
struct DefinitionInstruction_s {
	char* nom;						/**< nom de l'instruction */
	enum Operandes_e type_ops;		/**< nature de l'instruction */
	unsigned int nb_ops;		/**< nombre d'arguments de l'instruction */
};

/**
 * @struct DefinitionRegistre_s
 * @brief Structure permettant de stocker la correspondance entre le nom du registre et sa valeur
 */
struct DefinitionRegistre_s {
	char* nom;						/**< nom du registre */
	uint8_t valeur;					/**< nombre d'arguments de l'instruction */
};

char *clef_def_instruction(void *donnee_p);
void destruction_def_instruction(void *donnee_p);

char *clef_def_registre(void *donnee_p);
void destruction_def_registre(void *donnee_p);

int charge_def_instruction(struct Table_s **tableDefinition_pp, char *nomFichier);
int charge_def_registre(struct Table_s **tableDefinition_pp, char *nomFichier);

#endif /* _DICO_H_ */
