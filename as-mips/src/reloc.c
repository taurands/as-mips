/**
 * @file reloc.c
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Definition des fonctions liées au traitement des relocations
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <liste.h>
#include <dico.h>
#include <lex.h>
#include <syn.h>
#include <reloc.h>

/**
 * @param liste_data_p Pointeur sur la liste des données de la section .data
 * @param liste_reloc_data_p Pointeur sur la liste des différentes relocations dans la section rel.data
 * @param table_etiquettes_p Pointeur sur la table des etiquettes
 * @return SUCCESS si la relocation des data s'est bien passée, FAIL_ALLOC s'il n'y a pas assez de mémoire, FAILURE s'il manque la liste des données, des reloc_data ou la table d'étiquettes
 * @brief Remplissage de la liste rel.data
 */
int relocater_data(
		struct Liste_s *liste_data_p,
		struct Liste_s *liste_reloc_data_p,
		struct Table_s *table_etiquettes_p)
{
	int code_retour = SUCCESS;
	struct Noeud_Liste_s *noeud_courant_p = NULL;
	struct Donnee_s *donnee_p = NULL;
	struct Relocateur_s *relocateur_p=NULL;

	if (!liste_data_p || !liste_reloc_data_p || !table_etiquettes_p)
			return FAILURE;
	else if ((noeud_courant_p = debut_liste(liste_data_p)) && (donnee_p = noeud_courant_p->donnee_p)) {
		do {
			if ((donnee_p->type == D_WORD) && (donnee_p->lexeme_p->nature == L_SYMBOLE)) {
				relocateur_p = calloc(1,sizeof(*relocateur_p));
				if (!relocateur_p) {
					WARNING_MSG ("Impossible de créer un nouveau relocateur");
					return FAIL_ALLOC;
				}
				relocateur_p->decalage = donnee_p->decalage;
				relocateur_p->type_reloc = R_MIPS_32;
				relocateur_p->etiquette_p = donnee_table(table_etiquettes_p, donnee_p->lexeme_p->data);
				if ((code_retour = ajouter_fin_liste(liste_reloc_data_p, relocateur_p)) != SUCCESS) {
					free (relocateur_p);
					return code_retour;
				}
			}
		} while ((noeud_courant_p = suivant_liste(liste_data_p)) && (donnee_p = noeud_courant_p->donnee_p));
	}
	return SUCCESS;
}

/**
 * @param liste_text_p Pointeur sur la liste des données de la section .text
 * @param liste_reloc_text_p Pointeur sur la liste des différentes relocations dans la section rel.text
 * @param table_def_instructions_p Pointeur sur la table des définitions
 * @return SUCCESS si la relocation texte s'est bien passée, FAIL_ALLOC s'il n'y a pas assez de mémoire, FAILURE s'il manque la liste des text, des reloc_text ou la table d'étiquettes
 * @brief Remplissage de la liste rel.text
 */
int relocater_texte(
		struct Liste_s *liste_text_p,
		struct Liste_s *liste_reloc_text_p,
		struct Table_s *table_etiquettes_p)
{
	int code_retour = SUCCESS;
	struct Noeud_Liste_s *noeud_courant_p = NULL;
	struct Instruction_s *instruction_p = NULL;
	struct Relocateur_s *relocateur_p=NULL;

	if (!liste_text_p || !liste_reloc_text_p || !table_etiquettes_p)
			return FAILURE;
	else if ((noeud_courant_p = debut_liste(liste_text_p)) && (instruction_p = noeud_courant_p->donnee_p)) {
		do {
			if ((((instruction_p->definition_p->reloc == R_MIPS_26) || (instruction_p->definition_p->reloc == R_MIPS_HI16))) &&
				(((instruction_p->operandes[instruction_p->definition_p->nb_ops-1]) && (instruction_p->operandes[instruction_p->definition_p->nb_ops-1]->nature == L_SYMBOLE)))) {
				relocateur_p = calloc(1,sizeof(*relocateur_p));
				if (!relocateur_p) {
					WARNING_MSG ("Impossible de créer un nouveau relocateur");
					return FAIL_ALLOC;
				}
				relocateur_p->decalage = instruction_p->decalage;
				relocateur_p->type_reloc = instruction_p->definition_p->reloc;
				relocateur_p->etiquette_p = donnee_table(table_etiquettes_p, instruction_p->operandes[instruction_p->definition_p->nb_ops-1]->data);
				if ((code_retour = ajouter_fin_liste(liste_reloc_text_p, relocateur_p)) != SUCCESS) {
					free (relocateur_p);
					return code_retour;
				}
			}

			if (instruction_p->definition_p->reloc == R_MIPS_LO16) {
				if ((instruction_p->operandes[instruction_p->definition_p->nb_ops-2]) && (instruction_p->operandes[instruction_p->definition_p->nb_ops-2]->nature == L_SYMBOLE)){
					relocateur_p = calloc(1,sizeof(*relocateur_p));
					if (!relocateur_p) {
						WARNING_MSG ("Impossible de créer un nouveau relocateur");
						return FAIL_ALLOC;
					}
					relocateur_p->decalage = instruction_p->decalage;
					relocateur_p->type_reloc = instruction_p->definition_p->reloc;
					relocateur_p->etiquette_p = donnee_table(table_etiquettes_p, instruction_p->operandes[instruction_p->definition_p->nb_ops-2]->data);
					if ((code_retour = ajouter_fin_liste(liste_reloc_text_p, relocateur_p)) != SUCCESS) {
						free (relocateur_p);
						return code_retour;
					}
				}
				if ((instruction_p->operandes[instruction_p->definition_p->nb_ops-1]) && (instruction_p->operandes[instruction_p->definition_p->nb_ops-1]->nature == L_SYMBOLE)){
					relocateur_p = calloc(1,sizeof(*relocateur_p));
					if (!relocateur_p) {
						WARNING_MSG ("Impossible de créer un nouveau relocateur");
						return FAIL_ALLOC;
					}
					relocateur_p->decalage = instruction_p->decalage;
					relocateur_p->type_reloc = instruction_p->definition_p->reloc;
					relocateur_p->etiquette_p = donnee_table(table_etiquettes_p, instruction_p->operandes[instruction_p->definition_p->nb_ops-1]->data);
					if ((code_retour = ajouter_fin_liste(liste_reloc_text_p, relocateur_p)) != SUCCESS) {
						free (relocateur_p);
						return code_retour;
					}
				}
			}
		} while ((noeud_courant_p = suivant_liste(liste_text_p)) && (instruction_p = noeud_courant_p->donnee_p));
	}
	return SUCCESS;
}

/**
 * @param type_reloc défini le type de relocation
 * @return chaine de caractères contenant le nom du type de relocation
 * @brief Cette fonction permet de donner le nom correspondant à un type de relocation
 *
 */
char *type_reloc_to_str(enum Reloc_e type_reloc)
{
	switch(type_reloc) {
		case R_MIPS_32:					return "R_MIPS_32";
		case R_MIPS_26:					return "R_MIPS_26";
		case R_MIPS_HI16:				return "R_MIPS_HI16";
		case R_MIPS_LO16:				return "R_MIPS_LO16";
		case R_MIPS_PSEUDO:				return "R_MIPS_PSEUDO";
		default :
			ERROR_MSG("Erreur de résolution du nom du type de relocation... Il manque donc au moins un nom à rajouter pour %d", type_reloc);
	}
	return NULL;
}

/**
 * @param section défini la section où se trouve l'étiquette
 * @return chaine de caractères contenant le nom de la section
 * @brief Cette fonction permet de donner le nom correspondant à la section
 *
 */
char *type_section_to_str(enum Section_e section)
{
	switch(section) {
		case S_TEXT:					return ".text";
		case S_DATA:					return ".data";
		case S_BSS:						return ".bss";
		case S_UNDEF:					return "undef";
		default :
			ERROR_MSG("Erreur de résolution du nom du type de relocation... Il manque donc au moins un nom à rajouter pour %d", section);
	}
	return NULL;
}


/**
 * @param fichier stream d'écriture pour le relocateur
 * @param relocateur_p Pointeur sur un relocateur
 * @return Rien
 * @brief Affichage d'un relocateur
 */
void affiche_relocateur(
		FILE *fichier,
		struct Relocateur_s * relocateur_p)
{
	if (relocateur_p->etiquette_p->section != S_UNDEF) {
		fprintf(fichier, "%08x\t%s\t%-5s:%08x\t%s\n",relocateur_p->decalage,type_reloc_to_str(relocateur_p->type_reloc),type_section_to_str(relocateur_p->etiquette_p->section),relocateur_p->etiquette_p->decalage,relocateur_p->etiquette_p->lexeme_p->data);
	} else {
		fprintf(fichier, "%08x\t%s\t[UNDEFINED]\t%s\n",relocateur_p->decalage,type_reloc_to_str(relocateur_p->type_reloc),relocateur_p->etiquette_p->lexeme_p->data);
	}

}

/**
 * @param fichier stream d'écriture pour le relocateur
 * @param liste_reloc_data_p Pointeur sur la liste des données de la section rel.data
 * @return Rien
 * @brief Affichage de la liste des relocateurs de rel.data
 */
void affiche_liste_relocation(
		FILE *fichier,
		struct Liste_s *liste_reloc_p)
{
	struct Noeud_Liste_s *noeud_liste_p = NULL;
	struct Relocateur_s *relocateur_p = NULL;

	if (!liste_reloc_p) {
		fprintf(stderr, "La liste rel.data n'existe pas !\n");
	} else {
		if (liste_reloc_p->nb_elts) {
			for (noeud_liste_p = debut_liste (liste_reloc_p) ; (noeud_liste_p) ; noeud_liste_p = suivant_liste (liste_reloc_p)) {
				relocateur_p = noeud_liste_p->donnee_p;
				affiche_relocateur(fichier, relocateur_p);
			}
		}
	}
}

