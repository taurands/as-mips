/**
 * @file listage.h
 * @author TAURAND Sébastien
 * @brief Définitions de types et structures ainsi que des prototypes des fonctions de travail avec les listes génériques simplement chaînées.
 */

#ifndef _LISTAGE_H_
#define _LISTAGE_H_

#include <liste.h>

struct Listage_s {
	unsigned int ligne;
	char *source;
	char decalage[8+1];
	char code[8+1];
};

void detruit_listage (void *donnee_p);
void generer_listage (struct Liste_s *liste_lignes_source_p, struct Liste_s *liste_text_p, struct Liste_s *liste_data_p, struct Liste_s *liste_bss_p);

#endif /* _LISTAGE_H_ */
