/**
 * @file listage.h
 * @author TAURAND Sébastien
 * @brief Définitions de types et structures ainsi que des prototypes des fonctions de travail avec les listes génériques simplement chaînées.
 */

#ifndef _LISTAGE_H_
#define _LISTAGE_H_

#include <stdint.h>
#include <table.h>
#include <liste.h>

struct Ligne_Chaine_s {
	unsigned int ligne;
	char *chaine;
};

void detruit_listage (void *donnee_p);

void affiche_table_etiquette(FILE *fichier, struct Liste_s *liste_etiquette_p, struct Table_s *table_etiquettes_p);
void affiche_liste_donnee (FILE *fichier, struct Liste_s *liste_p, struct Table_s *table_p);
void affiche_liste_instructions (FILE *fichier, struct Liste_s *liste_p, struct Table_s *table_p);

void generer_listage (char *nom_fichier, struct Liste_s *liste_lignes_source_p, struct Liste_s *liste_text_p, struct Liste_s *liste_data_p, struct Liste_s *liste_bss_p, struct Liste_s *liste_etiquette_p, struct Table_s *table_etiquettes_p, struct Liste_s *liste_reloc_text_p, struct Liste_s *liste_reloc_data_p);
int generer_objet (char *nom_fichier, struct Liste_s *liste_text_p, struct Liste_s *liste_data_p, uint32_t decalage_text, uint32_t decalage_data, uint32_t decalage_bss);

#endif /* _LISTAGE_H_ */
