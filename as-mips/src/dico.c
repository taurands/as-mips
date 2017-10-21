/**
 * @file syn.c
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Definition des fonctions liées au traitement syntaxique du fichier
 */

#include <stdlib.h>
#include <inttypes.h>

#include <global.h>
#include <notify.h>
#include <str_utils.h>
#include <table.h>
#include <dico.h>

const char NATURE_INSTRUCTION[]= {'P', 'R', 'D', 'I', 'r', 'a'};

char *clef_def_instruction(void *donnee_p)
{
	return donnee_p ? ((struct DefinitionInstruction_s *)donnee_p)->nom : NULL;
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
	return donnee_p ? ((struct DefinitionRegistre_s *)donnee_p)->nom : NULL;
}

void destruction_def_registre(void *donnee_p)
{
	if (donnee_p) {
		free(((struct DefinitionRegistre_s *)donnee_p)->nom);
		free(donnee_p);
	}
}

int charge_def_instruction(struct Table_s **tableDefinition_pp, char *nomFichier)
{
	char *nomInstruction=calloc(128, sizeof(char));
	/* char carNature; */
	int nombreOperandes=0;
	int i=0;
	int nb_mots;

	struct DefinitionInstruction_s *def_instruction_p=NULL;

	FILE* f_p=fopen(nomFichier,"r"); /* Ouverture du dictionnaire d'instruction */
	if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");

	if (1!=fscanf(f_p,"%d",&nb_mots)) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */
	*tableDefinition_pp=creeTable(nb_mots, clef_def_instruction, destruction_def_instruction);

	while (f_p && (i < nb_mots)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

		if (1 != fscanf(f_p,"%s", nomInstruction)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");
		if (1 != fscanf(f_p,"%d", &nombreOperandes)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");
		/* if (1 != fscanf(f_p,"%c", &carNature)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours"); */

		def_instruction_p=malloc(sizeof(*def_instruction_p));
		def_instruction_p->nom=strdup(nomInstruction);
		def_instruction_p->nbOperandes=nombreOperandes;
		def_instruction_p->nature=I_PSEUDO;  /* XXX Penser à prendre en compte la nature de l'instruction */
		insereElementTable(*tableDefinition_pp, def_instruction_p);
		i++;
	}
	fclose(f_p);

	/* Il faut traiter les erreurs correspondant à : */
	/* Un nombre d'instructions incorrect */
	/*  - une absence d'instruction */
	/*  - une absence de nombre de paramètre */
	/*  - les données doivent être dans l'ordre alphabétique pour permettre une recherche rapide dichotomique */

	free(nomInstruction);
	return SUCCESS;
}

int charge_def_registre(struct Table_s **tableDefinition_pp, char *nomFichier)
{
	char *nom_reg=calloc(128, sizeof(char));
	int valeur=0;
	int i=0;
	int nb_mots;

	struct DefinitionRegistre_s *def_registre_p=NULL;

	FILE* f_p=fopen(nomFichier,"r"); /* Ouverture du dictionnaire d'instruction */
	if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");

	if (1!=fscanf(f_p,"%d",&nb_mots)) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */
	*tableDefinition_pp=creeTable(nb_mots, clef_def_registre, destruction_def_registre);

	while (f_p && (i < nb_mots)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

		if (1 != fscanf(f_p,"%s", nom_reg)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");
		if (1 != fscanf(f_p,"%d", &valeur)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");

		def_registre_p=malloc(sizeof(*def_registre_p));
		def_registre_p->nom=strdup(nom_reg);
		def_registre_p->valeur=valeur;
		insereElementTable(*tableDefinition_pp, def_registre_p);
		i++;
	}
	fclose(f_p);

	/* Il faut traiter les erreurs correspondant à : */
	/* Un nombre d'instructions incorrect */
	/*  - une absence d'instruction */
	/*  - une absence de nombre de paramètre */
	/*  - les données doivent être dans l'ordre alphabétique pour permettre une recherche rapide dichotomique */

	free(nom_reg);
	return SUCCESS;
}


