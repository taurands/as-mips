/**
 * @file listage.c
 * @author TAURAND Sébastien
 * @brief Définitions des fonctions permettant de fabriquer le listage
 */

#include <stdlib.h>
#include <stdio.h>
#include <notify.h>
#include <listage.h>
#include <str_utils.h>
#include <lex.h>
#include <syn.h>
#include <dico.h>
#include <reloc.h>

/**
 * @param donnee_p pointeur sur un listage à détruire
 * @return Rien
 * @brief Cette fonction permet de détuire et libérer le contenu d'un listage
 *
 * Cela inclut en particulier la chaine représentation le contenu d'un ASCIIZ.
 * Ceci est nécessaire pour le mécanisque de gestion propre des liste génériques.
 */
void detruit_listage (void *donnee_p)
{
	if (donnee_p) {
		if (((struct Ligne_Chaine_s *)donnee_p)->chaine)
			free(((struct Ligne_Chaine_s *)donnee_p)->chaine);
		free(donnee_p);
	}
}

/**
 * @param instruction_p pointeur sur une instruction à lire
 * @param table_p pointeur sur une table d'instructions
 * @return Rien
 * @brief Cette fonction permet d'afficher une instruction
 *
 */
void str_instruction(FILE *fichier, struct Instruction_s * instruction_p, struct Table_s *table_p)
{
	int i;

	fprintf(fichier, "%3d %08x ",
			instruction_p->ligne,
			instruction_p->decalage);
	if (instruction_p->definition_p) {
		fprintf(fichier, "%08x %-40s",
			instruction_p->op_code,
			instruction_p->source ? instruction_p->source : "");

		fprintf(fichier, "%s %s %s %s",
		instruction_p->definition_p->nom,
		instruction_p->operandes[0] ? instruction_p->operandes[0]->data : "",
		instruction_p->operandes[1] ? instruction_p->operandes[1]->data : "",
		instruction_p->operandes[2] ? instruction_p->operandes[2]->data : "");

		for (i=0 ; i<3 ; i++)
			if (instruction_p->operandes[i] && (instruction_p->operandes[i]->nature==L_SYMBOLE)){
				if (!donnee_table(table_p, instruction_p->operandes[i]->data) || ((struct Etiquette_s *)(donnee_table(table_p, instruction_p->operandes[i]->data)))->section == S_UNDEF){
					fprintf(fichier, "    0xXXXXXXXX : symbole %c[%d;%dm%s%c[%d;%dm inconnu dans la table des étiquettes",
							0x1B, STYLE_BOLD, COLOR_RED,
							instruction_p->operandes[i]->data,
							0x1B, STYLE_BOLD, 0);
				} else {
					fprintf(fichier, "    0x%08x : symbole %c[%d;%dm%s%c[%d;%dm",
							((struct Etiquette_s *)donnee_table(table_p, instruction_p->operandes[i]->data))->decalage,
							0x1B, STYLE_BOLD, COLOR_GREEN,
							instruction_p->operandes[i]->data,
							0x1B, STYLE_BOLD, 0);
				}
			}
	} else
		fprintf(fichier, "%*s %s", 8, "", "");

	fprintf(fichier, "\n");
}

/*
affiche_element_databss (fichier, donnee_data_p, table_etiquettes_p, listage_p->chaine ? listage_p->chaine : "");
while ((noeud_data_p = suivant_liste (liste_data_p)) && (donnee_data_p = noeud_data_p->donnee_p) && (donnee_data_p->ligne == listage_p->ligne))
	affiche_element_databss (fichier, donnee_data_p, table_etiquettes_p, "");
*/

/**
 * @param donnee_p pointeur sur un element de .data ou .bss à lire
 * @param table_p pointeur sur une table d'etiuette
 * @return Rien
 * @brief Cette fonction permet d'afficher un element des sections .data ou .bss
 *
 */
void affiche_element_databss(FILE *fichier, struct Liste_s *liste_p, struct Table_s *table_p, char *source, unsigned int ligne)
{
	unsigned int i, j, k;
	struct Etiquette_s *etiquette_p = NULL;

	struct Noeud_Liste_s *noeud_p = NULL;
	struct Donnee_s *donnee_p = NULL;

	noeud_p = courant_liste (liste_p);
	donnee_p = (noeud_p) ? noeud_p->donnee_p : NULL;

	for (k=0; (noeud_p) && (donnee_p = noeud_p->donnee_p) && (donnee_p->ligne == ligne); k++) {
		/* printf("nom directive %s  numero de ligne %d  decalage %d",donnee_p->lexeme_p->data, donnee_p->ligne, donnee_p->decalage); */
		if (!(donnee_p->decalage & 3) || (donnee_p->type == D_ASCIIZ)|| (donnee_p->type == D_SPACE)) {
			fprintf(fichier, "%3d %08X ", donnee_p->ligne, donnee_p->decalage);
		} else
			fprintf(fichier, "%3d %08X ", donnee_p->ligne, donnee_p->decalage);
		switch(donnee_p->type) {
		case D_BYTE:
			for (i=0; (noeud_p) && (donnee_p) && (donnee_p->ligne == ligne);) {
				fprintf(fichier, "%02X", donnee_p->valeur.octetNS);
				i++;
				noeud_p = suivant_liste (liste_p);
				donnee_p = (noeud_p) ? noeud_p->donnee_p : NULL;

				if (!(i & 3) && (donnee_p) && (donnee_p->ligne == ligne)) {
					if (i == 4)
						fprintf(fichier, " %s", (source) ? source : "");
					else
						fprintf(fichier, " ");
					fprintf(fichier, "\n%3d %08X ", donnee_p->ligne, donnee_p->decalage);
				}
			}
			if (i<=4) {
				for ( ; (i & 3); i++)
					fprintf(fichier, "  ");
				fprintf(fichier, " ");
				fprintf(fichier, "%s\n", (source) && (i=4) ? source : "");
			} else {
			for ( ; (i & 3); i++)
				fprintf(fichier, "  ");
			fprintf(fichier, " \n");
			}
			break;

		case D_HALF:
			fprintf(fichier, "%04X     ", donnee_p->valeur.demiNS);
			fprintf(fichier, "%s\n", (source) && (k=1) ? source : "");
			noeud_p = suivant_liste (liste_p);
			break;
		case D_WORD:

			if (donnee_p->lexeme_p->nature==L_SYMBOLE) {
				if (table_p && (etiquette_p = donnee_table(table_p, donnee_p->lexeme_p->data))) {
					fprintf(fichier, "%08X ", etiquette_p->decalage);
				} else
					fprintf(fichier, "00000000 ");
			} else {
				fprintf(fichier, "%08X ", donnee_p->valeur.motNS);
			}
			fprintf(fichier, "%s\n", (source) && (!k) ? source : "");
			noeud_p = suivant_liste (liste_p);
			break;
		case D_ASCIIZ:

			for (i=0; i < str_unesc_len(donnee_p->lexeme_p->data)-1; ) {
				j = (unsigned char)(donnee_p->valeur.chaine[i++]);
				fprintf(fichier, "%02X",j);
				if (!(i & 3)  && (i<str_unesc_len(donnee_p->lexeme_p->data)-1)) {
					if (i == 4)
						fprintf(fichier, " %s", (source) && !(k) ? source : "");
					else
						fprintf(fichier, " ");
					fprintf(fichier, "\n%3d %08X ", donnee_p->ligne, donnee_p->decalage+i);
				}
			}
			for ( ; (i & 3); i++)
				fprintf(fichier, "  ");
			fprintf(fichier, " ");
			if (str_unesc_len(donnee_p->lexeme_p->data)-2 < 4)
				fprintf(fichier, "%s\n", (source) && !(k) ? source : "");
			else
				fprintf(fichier, "\n");
			noeud_p = suivant_liste (liste_p);
			break;

		case D_SPACE:
			if (donnee_p->valeur.nbOctets>4)
				fprintf(fichier, "0000...  ");
			else if (donnee_p->valeur.nbOctets == 4)
				fprintf(fichier, "00000000 ");
			else if (donnee_p->valeur.nbOctets == 3)
				fprintf(fichier, "000000   ");
			else if (donnee_p->valeur.nbOctets == 2)
				fprintf(fichier, "0000     ");
			else if (donnee_p->valeur.nbOctets == 1)
				fprintf(fichier, "00       ");
			fprintf(fichier, "%s\n", (source) && (!k) ? source : "");
			noeud_p = suivant_liste (liste_p);
			break;

		default:
			fprintf(stderr, "type non défini\n");
		}
	}
}

/**
 * @param liste_p pointeur sur une liste générique des éléments de .data ou .bss à lire
 * @param table_p pointeur sur une table de donnees
 * @return Rien
 * @brief Cette fonction permet d'afficher la liste des éléments des sections .data ou .bss
 *
 */
void affiche_liste_donnee(FILE *fichier, struct Liste_s *liste_p, struct Table_s *table_p)
{
	struct Noeud_Liste_s* noeud_liste_p = NULL;
	if (!liste_p) {
		printf("La liste n'existe pas !\n");
	} else {
		if (!(liste_p->nb_elts)) {
			printf("La liste est vide\n");
		} else {
			for (noeud_liste_p=liste_p->debut_p ; (noeud_liste_p) ; noeud_liste_p=noeud_liste_p->suivant_p) {
				/*
				affiche_element_databss(fichier, (struct Donnee_s *)noeud_liste_p->donnee_p, table_p, NULL);
				*/
			}
		}
	}
	printf("\n\n");
}

/**
 * @param liste_p pointeur sur une liste générique d'instructions
 * @param table_p pointeur sur une table d'instructions
 * @return Rien
 * @brief Cette fonction permet d'afficher la liste des instructions de la section .text
 *
 */
void affiche_liste_instructions(FILE *fichier, struct Liste_s *liste_p, struct Table_s *table_p)
{
	struct Noeud_Liste_s* noeud_liste_p=NULL;
	if (!liste_p) {
		fprintf(stderr, "La liste n'existe pas !\n");
	} else {
		if (!(liste_p->nb_elts)) {
			fprintf(fichier, "La liste est vide\n");
		} else {
			for (noeud_liste_p=liste_p->debut_p ; (noeud_liste_p) ; noeud_liste_p=noeud_liste_p->suivant_p) {
				str_instruction(fichier, (struct Instruction_s *)noeud_liste_p->donnee_p, table_p);
			}
		}
	}
	fprintf(fichier, "\n");
}

/**
 * @param table_p pointeur sur la table d'étiquette à remplir
 * @return Rien
 * @brief Cette fonction permet d'ajouter une étiquette à une table d'étiquette
 *
 */
void affiche_table_etiquette(FILE *fichier, struct Liste_s *liste_etiquette_p, struct Table_s *table_etiquettes_p)
{
	struct Etiquette_s *etiquette_p = NULL;
	struct Noeud_Liste_s *noeud_p = NULL;
	struct Ligne_Chaine_s *sym_tab_elt_p;

	if (!liste_etiquette_p || !table_etiquettes_p) {
		printf("La table d'étiquette n'existe pas !\n");
	} else {
    	noeud_p = debut_liste (liste_etiquette_p);
    	while (noeud_p) {
    		sym_tab_elt_p = noeud_p->donnee_p;
    		etiquette_p = sym_tab_elt_p ? donnee_table (table_etiquettes_p, sym_tab_elt_p->chaine) : NULL;
    		if (etiquette_p && (etiquette_p->lexeme_p->ligne == sym_tab_elt_p->ligne)) {
    			if (etiquette_p->section != S_UNDEF)
    				fprintf(fichier, "%3d\t%-5s:%08X\t%s\n",etiquette_p->lexeme_p->ligne,type_section_to_str(etiquette_p->section),etiquette_p->decalage,etiquette_p->lexeme_p->data);
    			else
    				fprintf(fichier, "%3d\t[UNDEFINED]\t%s\n",etiquette_p->lexeme_p->ligne,etiquette_p->lexeme_p->data);
    		} else {
    			DEBUG_MSG("Ignore %d %s", sym_tab_elt_p ? sym_tab_elt_p->ligne : -1, sym_tab_elt_p ? sym_tab_elt_p->chaine : "");
    		}
			noeud_p = suivant_liste (liste_etiquette_p);
    	}
	}
}

/**
 * @param donnee_p pointeur sur un listage à détruire
 * @return Rien
 * @brief Cette fonction permet de détuire et libérer le contenu d'un listage
 *
 * Cela inclut en particulier la chaine représentation le contenu d'un ASCIIZ.
 * Ceci est nécessaire pour le mécanisque de gestion propre des liste génériques.
 */
void generer_listage (char *nom_fichier, struct Liste_s *liste_lignes_source_p, struct Liste_s *liste_text_p, struct Liste_s *liste_data_p, struct Liste_s *liste_bss_p, struct Liste_s *liste_etiquette_p, struct Table_s *table_etiquettes_p, struct Liste_s *liste_reloc_text_p, struct Liste_s *liste_reloc_data_p)
{
	struct Noeud_Liste_s *noeud_listage_p = NULL;
	struct Noeud_Liste_s *noeud_instruction_p = NULL;
	struct Noeud_Liste_s *noeud_data_p = NULL;
	struct Noeud_Liste_s *noeud_bss_p = NULL;
	struct Ligne_Chaine_s *listage_p = NULL;
	struct Instruction_s *instruction_p = NULL;
	struct Donnee_s *donnee_data_p = NULL;
	struct Donnee_s *donnee_bss_p = NULL;

    FILE *fichier = NULL;

    fichier = fopen(nom_fichier, "w");
    if ( NULL == fichier ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Impossible d'écrire le fichier \"%s\". Abandon du traitement",nom_fichier);
    }

	if (liste_lignes_source_p && liste_text_p && liste_data_p && liste_bss_p) {
		noeud_listage_p = debut_liste (liste_lignes_source_p);
		noeud_instruction_p = debut_liste (liste_text_p);
		noeud_data_p = debut_liste (liste_data_p);
		noeud_bss_p = debut_liste (liste_bss_p);
		while (noeud_listage_p) {
			listage_p = noeud_listage_p->donnee_p;
			noeud_data_p = courant_liste (liste_data_p);
			noeud_bss_p = courant_liste (liste_bss_p);
			while (noeud_instruction_p && (instruction_p = noeud_instruction_p->donnee_p) && (instruction_p->ligne < listage_p->ligne)) {
				noeud_instruction_p = suivant_liste (liste_text_p);
			}
			while (noeud_data_p && (donnee_data_p = noeud_data_p->donnee_p) && (donnee_data_p->ligne < listage_p->ligne)) {
				noeud_data_p = suivant_liste (liste_data_p);
			}
			while (noeud_bss_p && (donnee_bss_p = noeud_bss_p->donnee_p) && (donnee_bss_p->ligne < listage_p->ligne)) {
				noeud_bss_p = suivant_liste (liste_bss_p);
			}

			if (noeud_instruction_p && (instruction_p = noeud_instruction_p->donnee_p) && (instruction_p->ligne == listage_p->ligne)) {

				fprintf(fichier, "%3d %08X ",listage_p->ligne, instruction_p->decalage);
				if (instruction_p->definition_p)
					fprintf(fichier, "%08X %s\n", instruction_p->op_code, listage_p->chaine ? listage_p->chaine : "");
				while ((noeud_instruction_p  = suivant_liste (liste_text_p)) && (instruction_p = noeud_instruction_p->donnee_p) && (instruction_p->ligne == listage_p->ligne)) {
					if (noeud_instruction_p && (instruction_p = noeud_instruction_p->donnee_p) && (instruction_p->ligne == listage_p->ligne)) {
						fprintf(fichier, "%3d %08X ",listage_p->ligne, instruction_p->decalage);
									if (instruction_p->definition_p)
										fprintf(fichier, "%08X \n", instruction_p->op_code);
					}
				}

			} else if (noeud_data_p && (donnee_data_p = noeud_data_p->donnee_p) && (donnee_data_p->ligne == listage_p->ligne)) {
				affiche_element_databss (fichier, liste_data_p, table_etiquettes_p, listage_p->chaine ? listage_p->chaine : "", listage_p->ligne);
			} else if (noeud_bss_p && (donnee_bss_p = noeud_bss_p->donnee_p) && (donnee_bss_p->ligne == listage_p->ligne)) {
				affiche_element_databss (fichier, liste_bss_p, table_etiquettes_p, listage_p->chaine ? listage_p->chaine : "", listage_p->ligne);
			} else {
				fprintf(fichier, "%3d                   %s\n",listage_p->ligne, listage_p->chaine ? listage_p->chaine : "");
			}
			noeud_listage_p = suivant_liste (liste_lignes_source_p);
		}
    	fprintf(fichier, "\n.symtab\n");
    	affiche_table_etiquette(fichier, liste_etiquette_p, table_etiquettes_p);
    	fprintf(fichier, "\nrel.text\n");
    	affiche_liste_relocation(fichier, liste_reloc_text_p);
    	fprintf(fichier, "\nrel.data\n");
    	affiche_liste_relocation(fichier, liste_reloc_data_p);
    	fprintf(fichier, "\n");

	}
	fclose(fichier);
}
