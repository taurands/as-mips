/**
 * @file dico.c
 * @author BERTRAND Antoine, TAURAND Sébastien
 * @brief Definition des fonctions liées aux dictionnaires d'instructions et de registres
 */

#include <stdlib.h>

#include <global.h>
#include <notify.h>
#include <str_utils.h>
#include <table.h>
#include <dico.h>

const char TYPE_OPS[]= {'R', 'N', 'B'}; /* lettres associés à enum Operandes_e à la définition des type syntaxiques des instructions */

/**
 * @param donnee_p pointeur sur une structure de définition d'instruction
 * @return chaine de caractère représentant le clef d'identification
 * @brief Fourni le nom de l'instruction comme clef d'identification (fonction clef pour la table générique)
 *
 */
char *clef_def_pseudo(void *donnee_p)
{
	/* XXX à modifier */
	return (donnee_p && (struct DefinitionInstruction_s *)donnee_p) ? ((struct DefinitionInstruction_s *)donnee_p)->nom : NULL;
}

/**
 * @param donnee_p pointeur sur une structure de définition d'instruction
 * @return rien
 * @brief Détruit et libère la structure pointée (fonction de destruction pour la table générique)
 *
 */
void destruction_def_pseudo(void *donnee_p)
{
	/* XXX à modifier */
	if (donnee_p) {
		free(((struct DefinitionInstruction_s *)donnee_p)->nom);
		free(donnee_p);
	}
}

/**
 * @param donnee_p pointeur sur une structure de définition d'instruction
 * @return chaine de caractère représentant le clef d'identification
 * @brief Fourni le nom de l'instruction comme clef d'identification (fonction clef pour la table générique)
 *
 */
char *clef_def_instruction(void *donnee_p)
{
	return (donnee_p && (struct DefinitionInstruction_s *)donnee_p) ? ((struct DefinitionInstruction_s *)donnee_p)->nom : NULL;
}

/**
 * @param donnee_p pointeur sur une structure de définition d'instruction
 * @return rien
 * @brief Détruit et libère la structure pointée (fonction de destruction pour la table générique)
 *
 */
void destruction_def_instruction(void *donnee_p)
{
	if (donnee_p) {
		free(((struct DefinitionInstruction_s *)donnee_p)->nom);
		free(donnee_p);
	}
}

/**
 * @param donnee_p pointeur sur une structure de définition de pseudo instruction
 * @return chaine de caractère représentant le clef d'identification
 * @brief Fourni le nom de de la pseudo instruction comme clef d'identification (fonction clef pour la table générique)
 *
 */
char *clef_def_pseudo_instruction(void *donnee_p)
{
	return (donnee_p && (struct DefinitionPseudoInstruction_s *)donnee_p) ? ((struct DefinitionPseudoInstruction_s *)donnee_p)->nom : NULL;
}



/**
 * @param donnee_p pointeur sur une structure de définition de pseudo instruction
 * @return rien
 * @brief Détruit et libère la structure pointée (fonction de destruction pour la table générique)
 *
 */
void destruction_def_pseudo_instruction(void *donnee_p)
{
	int i;
	struct DefinitionPseudoInstruction_s *dpi_p = donnee_p;
	if (donnee_p) {
		for (i=0;i<dpi_p->nb_instruction;i++) {
			free(dpi_p->rempl[i].instruction);
			free(dpi_p->rempl[i].arg[0]);
			free(dpi_p->rempl[i].arg[1]);
			free(dpi_p->rempl[i].arg[2]);
		}
		free(dpi_p->nom);
		free(dpi_p);
	}
}

/**
 * @param donnee_p pointeur sur une structure de définition de registre
 * @return chaine de caractère représentant le clef d'identification
 * @brief Fourni le nom du registre comme clef d'identification (fonction clef pour la table générique)
 *
 */
char *clef_def_registre(void *donnee_p)
{
	return (donnee_p && (struct DefinitionRegistre_s *)donnee_p) ? ((struct DefinitionRegistre_s *)donnee_p)->nom : NULL;
}

/**
 * @param donnee_p pointeur sur une structure de définition de registre
 * @return rien
 * @brief Détruit et libère la structure pointée (fonction de destruction pour la table générique)
 *
 */
void destruction_def_registre(void *donnee_p)
{
	if (donnee_p) {
		free(((struct DefinitionRegistre_s *)donnee_p)->nom);
		free(donnee_p);
	}
}


/**
 * @param table_definition_pp Pointeur sur un pointeur de table de hachage générique
 * @param nom_fichier chaine contenant le nom du fichier à charger
 * @return SUCCESS si la table de definition des pseudo-instructions a pu être chargée correctement
 * @brief Fonction de chargement de la définition des pseudo-instructions
 *
 */
int charge_def_pseudo(struct Table_s **table_definition_pp, char *nom_fichier)
{
	/* XXX à compléter en prenant exemple sur gestion des erreurs de charge_def_instruction */
	int resultat = SUCCESS;

	FILE *f_p = NULL;
	struct DefinitionPseudoInstruction_s *definition_pseudo_instruction_p = NULL;

	char *nom_pseudo_instruction = NULL;
	int nb_instructions = 0;
	char *instruction_spl = NULL;
	char *arg1 = NULL;
	char *arg2 = NULL;
	char *arg3 = NULL;
	int nb_mots = 0;
	int i=0;
	int j=0;


	do {
		if (!(nom_pseudo_instruction = calloc(STRLEN+1, sizeof(char)))) {
			resultat = FAIL_ALLOC;
			WARNING_MSG ("Plus assez de mémoire pour créer un nom de pseudo instruction");
			break;
		}

		if (!(instruction_spl = calloc(STRLEN+1, sizeof(char)))) {
			resultat = FAIL_ALLOC;
			WARNING_MSG ("Plus assez de mémoire pour créer un nom d'instruction supplémentaire");
			break;
		}

		if (!(arg1 = calloc(STRLEN+1, sizeof(char)))) {
			resultat = FAIL_ALLOC;
			WARNING_MSG ("Plus assez de mémoire pour créer un nom d'argument 1 d'instruction");
			break;
		}

		if (!(arg2 = calloc(STRLEN+1, sizeof(char)))) {
			resultat = FAIL_ALLOC;
			WARNING_MSG ("Plus assez de mémoire pour créer un nom d'argument 2 d'instruction");
			break;
		}

		if (!(arg3 = calloc(STRLEN+1, sizeof(char)))) {
			resultat = FAIL_ALLOC;
			WARNING_MSG ("Plus assez de mémoire pour créer un nom d'argument 3 d'instruction");
			break;
		}

		f_p = fopen(nom_fichier,"r"); /* Ouverture du dictionnaire de pseudo instruction */
		if (!f_p) {
			resultat = FAILURE;
			WARNING_MSG ("Impossible d'ouvrir le fichier %s de définition de pseudo instruction", nom_fichier);
			break;
		}

		if (1 != fscanf(f_p,"%u",&nb_mots)) { /* Lecture de la première ligne du dictionnaire */
			resultat = FAILURE;
			WARNING_MSG ("Nombre de pseudo instructions du dictionnaire introuvable dans %s", nom_fichier);
			break;
		} else if (nb_mots < 1) {
			resultat = FAILURE;
			WARNING_MSG ("Le dictionnaire %s doit contenir au moins une pseudo instruction", nom_fichier);
			break;
		}

		if ((resultat = creer_table(table_definition_pp, nb_mots, clef_def_pseudo_instruction, destruction_def_pseudo_instruction))) {
			WARNING_MSG ("Plus assez de mémoire pour créer la table de définitions de pseudo instructions");
			break;
		}

		while (f_p && (i < nb_mots)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

			if (1 != fscanf(f_p,"%s", nom_pseudo_instruction)) {
				resultat = FAILURE;
				WARNING_MSG ("La ligne du dictionnaire ne comprenait pas le nom de la pseudo instruction en cours");
				break;
			}
			if (1 != fscanf(f_p,"%d", &nb_instructions)) {
				resultat = FAILURE;
				WARNING_MSG ("La ligne du dictionnaire ne comprenait pas le nombre d'instructions de la pseudo instruction en cours");
				break;
			}
			if ((nb_instructions<1) || (nb_instructions>NB_INSTR_PSEUDO_MAX)) {
				resultat = FAILURE;
				WARNING_MSG ("Le nombre d'instruction de remplacement de la pseudo instruction est au delà des limites acceptables");
				break;
			}
			if (!(definition_pseudo_instruction_p = calloc (1, sizeof(*definition_pseudo_instruction_p)))) {
				resultat = FAIL_ALLOC;
				WARNING_MSG ("Plus assez de mémoire pour créer une nouvelle définition de pseudo instruction");
				break;
			}
			if (!(definition_pseudo_instruction_p->nom = strdup(nom_pseudo_instruction)) && nom_pseudo_instruction) {
				resultat = FAIL_ALLOC;
				WARNING_MSG ("Plus assez de mémoire pour dupliquer le nom de la pseudo instruction");
				break;
			}

			definition_pseudo_instruction_p->nb_instruction=nb_instructions;

			for (j=0; ((resultat == SUCCESS) && (j<nb_instructions));j++) {
				if (1 != fscanf(f_p,"%s", instruction_spl)) {
					resultat = FAILURE;
					WARNING_MSG ("La ligne du dictionnaire ne comprenait pas le nom de l'instruction en cours");
					break;
				}
				if (1 != fscanf(f_p,"%s", arg1)) {
					resultat = FAILURE;
					WARNING_MSG ("La ligne du dictionnaire ne comprenait pas le nom de l'argument 1 en cours");
					break;
				}
				if (1 != fscanf(f_p,"%s", arg2)) {
					resultat = FAILURE;
					WARNING_MSG ("La ligne du dictionnaire ne comprenait pas le nom de l'argument 2 en cours");
					break;
				}
				if (1 != fscanf(f_p,"%s", arg3)) {
					resultat = FAILURE;
					WARNING_MSG ("La ligne du dictionnaire ne comprenait pas le nom de l'argument 3 en cours");
					break;
				}

				if (!(definition_pseudo_instruction_p->rempl[j].instruction = strdup(instruction_spl)) && instruction_spl) {
					resultat = FAIL_ALLOC;
					WARNING_MSG ("Plus assez de mémoire pour dupliquer le nom de l'instruction de remplacement");
					break;
				}
				if (!(definition_pseudo_instruction_p->rempl[j].arg[0] = strdup(arg1)) && arg1) {
					resultat = FAIL_ALLOC;
					WARNING_MSG ("Plus assez de mémoire pour dupliquer le nom de l'argument 1");
					break;
				}
				if (!(definition_pseudo_instruction_p->rempl[j].arg[1] = strdup(arg2)) && arg2) {
					resultat = FAIL_ALLOC;
					WARNING_MSG ("Plus assez de mémoire pour dupliquer le nom de l'argument 2");
					break;
				}
				if (!(definition_pseudo_instruction_p->rempl[j].arg[2] = strdup(arg3)) && arg3) {
					resultat = FAIL_ALLOC;
					WARNING_MSG ("Plus assez de mémoire pour dupliquer le nom de l'argument 3");
					break;
				}
			}

			if ((resultat = ajouter_table(*table_definition_pp, definition_pseudo_instruction_p))) {
				WARNING_MSG ("Le dictionnaire de pseudo instructions contient l'instruction %s en double", definition_pseudo_instruction_p->nom);
				destruction_def_pseudo_instruction(definition_pseudo_instruction_p);
				definition_pseudo_instruction_p = NULL;
				break;
			}
			else
				definition_pseudo_instruction_p = NULL;
			i++;
		}
	} while (FALSE);

	if (f_p)
		fclose(f_p);

	free (arg3);
	free (arg2);
	free (arg1);
	free (instruction_spl);
	free (nom_pseudo_instruction);
	free (definition_pseudo_instruction_p);

	return resultat;
}

/**
 * @param table_definition_pp Pointeur sur un pointeur de table de hachage générique
 * @param nom_fichier chaine contenant le nom du fichier à charger
 * @return SUCCESS si la table de definition des instructions a pu être chargée correctement
 * @brief Fonction de chargement de la définition des instructions
 *
 */
int charge_def_instruction(struct Table_s **table_definition_pp, char *nom_fichier)
{
	int resultat = SUCCESS;

	FILE *f_p = NULL;
	struct DefinitionInstruction_s *def_instruction_p = NULL;

	char *nom_instruction = NULL;
	char *str_hexa = NULL;
	char car_nature;
	char car_reloc;
	int nb_operandes = 0;
	int i = 0;
	int j = 0;
	int nb_mots;
	int opcode_init;
	int nb_bits;
	int signe;
	int shift;
	int dest_bit;
	int LEN_HEXA = 10;

	do {
		if (!(nom_instruction = calloc(STRLEN+1, sizeof(char)))) {
			resultat = FAIL_ALLOC;
			WARNING_MSG ("Plus assez de mémoire pour créer un nom d'instruction");
			break;
		}

		if (!(str_hexa = calloc(LEN_HEXA+1,sizeof(char)))) {
			resultat = FAIL_ALLOC;
			WARNING_MSG ("Plus assez de mémoire pour créer la string contenant opcode_init");
			break;
		}

		f_p = fopen(nom_fichier,"r"); /* Ouverture du dictionnaire d'instruction */
		if (!f_p) {
			resultat = FAILURE;
			WARNING_MSG ("Impossible d'ouvrir le fichier %s de définition d'instruction", nom_fichier);
			break;
		}

		if (1 != fscanf(f_p,"%u",&nb_mots)) { /* Lecture de la première ligne du dictionnaire */
			resultat = FAILURE;
			WARNING_MSG ("Nombre d'instructions du dictionnaire introuvable dans %s", nom_fichier);
			break;
		} else if (nb_mots < 1) {
			resultat = FAILURE;
			WARNING_MSG ("Le dictionnaire %s doit contenir au moins une instruction", nom_fichier);
			break;
		}

		if ((resultat = creer_table(table_definition_pp, nb_mots, clef_def_instruction, destruction_def_instruction))) {
			WARNING_MSG ("Plus assez de mémoire pour créer la table de définitions d'instructions");
			break;
		}
		while (f_p && (i < nb_mots)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

			if (1 != fscanf(f_p,"%s", nom_instruction)) {
				resultat = FAILURE;
				WARNING_MSG ("La ligne du dictionnaire ne comprenait pas le nom de l'instruction en cours");
				break;
			}
			if (1 != fscanf(f_p,"%d", &nb_operandes)) {
				resultat = FAILURE;
				WARNING_MSG ("La ligne du dictionnaire ne comprenait pas le nombre d'arguments de l'instruction en cours");
				break;
			}
			if (1 != fscanf(f_p,"%c", &car_nature)) {
				resultat = FAILURE;
				WARNING_MSG ("Pas de caractère de type syntaxique pour %s", nom_instruction);
				break;
			}

			if (1 != fscanf(f_p,"%c", &car_reloc)) {
				resultat = FAILURE;
				WARNING_MSG ("Pas de caractère de type syntaxique pour %s", nom_instruction);
				break;
			}

			if (!(def_instruction_p = calloc (1, sizeof(*def_instruction_p)))) {
				resultat = FAIL_ALLOC;
				WARNING_MSG ("Plus assez de mémoire pour créer une nouvelle définition d'instruction");
				break;
			}
			if (!(def_instruction_p->nom = strdup(nom_instruction)) && nom_instruction) {
				resultat = FAIL_ALLOC;
				WARNING_MSG ("Plus assez de mémoire pour dupliquer le nom de l'instruction");
				break;
			}
			def_instruction_p->nb_ops=nb_operandes;

			if (car_nature == TYPE_OPS[I_OP_R])
				def_instruction_p->type_ops=I_OP_R;
			else if (car_nature == TYPE_OPS[I_OP_N])
				def_instruction_p->type_ops=I_OP_N;
			else if (car_nature == TYPE_OPS[I_OP_B])
				def_instruction_p->type_ops=I_OP_B;
			else {
				resultat = FAILURE;
				WARNING_MSG("Type d'opérande inconnu pour l'instruction %s (ligne %d)", nom_instruction, i+1);
				break;
			}

			if (car_reloc == 'A')
				def_instruction_p->reloc=R_MIPS_AUCUN;
			else if (car_reloc == 'H')
				def_instruction_p->reloc=R_MIPS_HI16;
			else if (car_reloc == 'L')
				def_instruction_p->reloc=R_MIPS_LO16;
			else if (car_reloc == '2')
				def_instruction_p->reloc=R_MIPS_26;
			else if (car_reloc == 'P')
				def_instruction_p->reloc=R_MIPS_PSEUDO;
			else {
				resultat = FAILURE;
				WARNING_MSG("Type de relocation inconnu pour l'instruction %s (ligne %d)", nom_instruction, i+1);
				break;
			}

			def_instruction_p->nb_ops=nb_operandes;

			if (def_instruction_p->reloc != R_MIPS_PSEUDO) {
				if (1 != fscanf(f_p, "%s", str_hexa)) {
						resultat = FAILURE;
						WARNING_MSG ("Pas de caractère de type syntaxique pour %s", nom_instruction);
						break;
				}
				opcode_init = strtol(str_hexa, NULL, 16); /* Conversion de la chaine en int hexa */
				def_instruction_p->opcode =opcode_init;

				for (j=0;j<nb_operandes;j++){


					if (1 != fscanf(f_p, "%d", &nb_bits)) {
						resultat = FAILURE;
						WARNING_MSG ("Pas de caractère de type int pour %s", nom_instruction);
						break;
					}


					if (1 != fscanf(f_p, "%d", &signe)) {
						resultat = FAILURE;
						WARNING_MSG ("Pas de caractère de type int pour %s", nom_instruction);
						break;
					}

					if (1 != fscanf(f_p, "%d", &shift)) {
						resultat = FAILURE;
						WARNING_MSG ("Pas de caractère de type int pour %s", nom_instruction);
						break;
					}

					if (1 != fscanf(f_p, "%d", &dest_bit)) {
						resultat = FAILURE;
						WARNING_MSG ("Pas de caractère de type int pour %s", nom_instruction);
						break;
					}

					/* Remplissage des structures operandes */

					def_instruction_p->codes[j].nb_bits=nb_bits;
					def_instruction_p->codes[j].signe=signe;
					def_instruction_p->codes[j].shift=shift;
					def_instruction_p->codes[j].dest_bit=dest_bit;
				}
			}

			if ((resultat = ajouter_table(*table_definition_pp, def_instruction_p))) {
				WARNING_MSG ("Le dictionnaire d'instructions contient l'instruction %s en double", def_instruction_p->nom);
				free(def_instruction_p->nom);
				break;
			}
			else {
				def_instruction_p = NULL;
			}
			i++;
		}
	} while (FALSE);

	if (f_p)
		fclose(f_p);

	free (str_hexa);
	free (def_instruction_p);
	free (nom_instruction);
	return resultat;
}

/**
 * @param table_definition_pp Pointeur sur un pointeur de table de hachage générique
 * @param nom_fichier chaine contenant le nom du fichier à charger
 * @return SUCCESS si la table de definition des registres a pu être chargée correctement
 * @brief Fonction de chargement de la définition des registres
 *
 */
int charge_def_registre(struct Table_s **table_definition_pp, char *nom_fichier)
{
	int erreur = SUCCESS;

	char *nom_reg=calloc(STRLEN+1, sizeof(char));
	int valeur=0;
	int i=0;
	int nb_mots;

	struct DefinitionRegistre_s *def_registre_p=NULL;

	FILE* f_p=fopen(nom_fichier,"r"); /* Ouverture du dictionnaire d'instruction */
	if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");

	if (1!=fscanf(f_p,"%d",&nb_mots)) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */
	if ((erreur = creer_table(table_definition_pp, nb_mots, clef_def_registre, destruction_def_registre)))
			return FAIL_ALLOC;

	while (f_p && (i < nb_mots)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

		if (1 != fscanf(f_p,"%s", nom_reg)) ERROR_MSG("La ligne du dictionnaire ne comprenait pas le nom du registre en cours");
		if (1 != fscanf(f_p,"%d", &valeur)) ERROR_MSG("La ligne du dictionnaire ne comprenait pas la valeur du registre en cours");

		def_registre_p=malloc(sizeof(*def_registre_p));
		def_registre_p->nom=strdup(nom_reg);
		def_registre_p->valeur=valeur;
		ajouter_table(*table_definition_pp, def_registre_p);
		i++;
	}
	fclose(f_p);

	free(nom_reg);
	return SUCCESS;
}


