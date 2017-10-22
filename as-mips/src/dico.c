/**
 * @file syn.c
 * @author BERTRAND Antoine, TAURAND Sébastien
 * @brief Definition des fonctions liées aux dictionnaires d'instructions et de registres
 */

#include <stdlib.h>
#include <inttypes.h>

#include <global.h>
#include <notify.h>
#include <str_utils.h>
#include <table.h>
#include <dico.h>

const char TYPE_OPS[]= {'R', 'N', 'B'};

char *clef_def_instruction(void *donnee_p)
{
	return (donnee_p && (struct DefinitionInstruction_s *)donnee_p) ? ((struct DefinitionInstruction_s *)donnee_p)->nom : NULL;
}

void destruction_def_instruction(void *donnee_p)
{
	if (donnee_p) {
		free(((struct DefinitionInstruction_s *)donnee_p)->nom);
		free(donnee_p);
	}
}

char *clef_def_registre(void *donnee_p)
{
	return (donnee_p && (struct DefinitionRegistre_s *)donnee_p) ? ((struct DefinitionRegistre_s *)donnee_p)->nom : NULL;
}

void destruction_def_registre(void *donnee_p)
{
	if (donnee_p) {
		free(((struct DefinitionRegistre_s *)donnee_p)->nom);
		free(donnee_p);
	}
}

int charge_def_instruction(struct Table_s **table_definition_pp, char *nom_fichier)
{
	char *nom_instruction=calloc(STRLEN, sizeof(char));
	char car_nature;
	int nb_operandes=0;
	int i=0;
	int nb_mots;

	struct DefinitionInstruction_s *def_instruction_p=NULL;

	FILE* f_p=fopen(nom_fichier,"r"); /* Ouverture du dictionnaire d'instruction */
	if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");

	if (1!=fscanf(f_p,"%d",&nb_mots)) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */
	*table_definition_pp=creer_table(nb_mots, clef_def_instruction, destruction_def_instruction);

	while (f_p && (i < nb_mots)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

		if (1 != fscanf(f_p,"%s", nom_instruction)) ERROR_MSG("La ligne du dictionnaire ne comprenait pas le nom de l'instruction en cours");
		if (1 != fscanf(f_p,"%d", &nb_operandes)) ERROR_MSG("La ligne du dictionnaire ne comprenait pas le nombre d'arguments de l'instruction en cours");
		if (1 != fscanf(f_p,"%c", &car_nature)) ERROR_MSG("Pas de caractère de type sytaxique pour %s", nom_instruction);

		def_instruction_p=malloc(sizeof(*def_instruction_p));
		def_instruction_p->nom=strdup(nom_instruction);
		def_instruction_p->nb_ops=nb_operandes;

		if (car_nature==TYPE_OPS[I_OP_R])
			def_instruction_p->type_ops=I_OP_R;
		else if (car_nature==TYPE_OPS[I_OP_N])
			def_instruction_p->type_ops=I_OP_N;
		else if (car_nature==TYPE_OPS[I_OP_B])
			def_instruction_p->type_ops=I_OP_B;
		else
			ERROR_MSG("Type d'opérande inconnu pour l'instruction %s (ligne %d)", nom_instruction, i+1);

		ajouter_table(*table_definition_pp, def_instruction_p);
		i++;
	}
	fclose(f_p);

	free(nom_instruction);
	return SUCCESS;
}

int charge_def_registre(struct Table_s **table_definition_pp, char *nom_fichier)
{
	char *nom_reg=calloc(STRLEN, sizeof(char));
	int valeur=0;
	int i=0;
	int nb_mots;

	struct DefinitionRegistre_s *def_registre_p=NULL;

	FILE* f_p=fopen(nom_fichier,"r"); /* Ouverture du dictionnaire d'instruction */
	if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");

	if (1!=fscanf(f_p,"%d",&nb_mots)) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */
	*table_definition_pp=creer_table(nb_mots, clef_def_registre, destruction_def_registre);

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


