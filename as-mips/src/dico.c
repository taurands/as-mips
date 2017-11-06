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
	char car_nature;
	char car_reloc;
	int nb_operandes = 0;
	int i = 0;
	int nb_mots;

	do {
		if (!(nom_instruction = calloc(STRLEN, sizeof(char)))) {
			resultat = FAIL_ALLOC;
			WARNING_MSG ("Plus assez de mémoire pour créer un nom d'instruction");
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
			else {
				resultat = FAILURE;
				WARNING_MSG("Type de relocation inconnu pour l'instruction %s (ligne %d)", nom_instruction, i+1);
				break;
			}

			if ((resultat = ajouter_table(*table_definition_pp, def_instruction_p))) {
				WARNING_MSG ("Le dictionnaire d'instructions contient l'instruction %s en double", def_instruction_p->nom);
				free(def_instruction_p->nom);
				break;
			}
			else
				def_instruction_p = NULL;
			i++;
		}
	} while (FALSE);

	if (f_p)
		fclose(f_p);

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

	char *nom_reg=calloc(STRLEN, sizeof(char));
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


