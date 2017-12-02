/**
 * @file listage.c
 * @author TAURAND Sébastien
 * @brief Définitions des fonctions permettant de fabriquer le listage
 */

#include <stdlib.h>
#include <stdio.h>
#include <listage.h>
#include <syn.h>

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
		if (((struct Listage_s *)donnee_p)->source)
			free(((struct Listage_s *)donnee_p)->source);
		free(donnee_p);
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
void generer_listage (struct Liste_s *liste_lignes_source_p, struct Liste_s *liste_text_p, struct Liste_s *liste_data_p, struct Liste_s *liste_bss_p)
{
	struct Noeud_Liste_s *noeud_listage_p = NULL;
	struct Noeud_Liste_s *noeud_instruction_p = NULL;
	struct Noeud_Liste_s *noeud_data_p = NULL;
	struct Noeud_Liste_s *noeud_bss_p = NULL;
	struct Listage_s *listage_p = NULL;
	struct Instruction_s *instruction_p = NULL;
	struct Donnee_s *donnee_data_p = NULL;
	struct Donnee_s *donnee_bss_p = NULL;

	if (liste_lignes_source_p && liste_text_p && liste_data_p && liste_bss_p) {
		noeud_listage_p = debut_liste (liste_lignes_source_p);
		noeud_instruction_p = debut_liste (liste_text_p);
		noeud_data_p = debut_liste (liste_data_p);
		noeud_bss_p = debut_liste (liste_bss_p);
		while (noeud_listage_p) {
			listage_p = noeud_listage_p->donnee_p;
			printf ("%3d ",listage_p->ligne);
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
				printf("%08x ", instruction_p->decalage);
				if (instruction_p->definition_p)
					printf("%08x %s\n", instruction_p->op_code, listage_p->source ? listage_p->source : "");
				while ((noeud_instruction_p  = suivant_liste (liste_text_p)) && (instruction_p = noeud_instruction_p->donnee_p) && (instruction_p->ligne == listage_p->ligne)) {
					if (noeud_instruction_p && (instruction_p = noeud_instruction_p->donnee_p) && (instruction_p->ligne == listage_p->ligne)) {
									printf("%3d %08x ",listage_p->ligne, instruction_p->decalage);
									if (instruction_p->definition_p)
										printf("%08x\n", instruction_p->op_code);
					}
				}

			} else if (noeud_data_p && (donnee_data_p = noeud_data_p->donnee_p) && (donnee_data_p->ligne == listage_p->ligne)) {

			} else if (noeud_bss_p && (donnee_bss_p = noeud_bss_p->donnee_p) && (donnee_bss_p->ligne == listage_p->ligne)) {

			} else
				printf ("                  %s\n", listage_p->source ? listage_p->source : "");
			noeud_listage_p = suivant_liste (liste_lignes_source_p);

		}
	}
}
