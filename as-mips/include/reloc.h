/**
 * @file syn.h
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition de type et de prototypes de fonctions liées au traitement des relocations
 */

#ifndef _RELOC_H_
#define _RELOC_H_

#include <stdint.h>

/**
 * @struct Relocateur_s
 * @brief Elément définissant une relocation
 */
struct Relocateur_s {
	uint32_t decalage;								/**< Décalage de l'instruction */
	enum Reloc_e type_reloc;						/**< Type de relocation à réaliser */
	struct Etiquette_s *etiquette_p;				/**< Pointeur sur la définition de l'étiquette associée */
};

int relocation_data(struct Liste_s *liste_data_p, struct Liste_s *liste_reloc_data_p, struct Table_s *table_etiquettes_p);
int relocation_texte(struct Liste_s *liste_text_p, struct Liste_s *liste_reloc_text_p, struct Table_s *table_etiquettes_p);

char *type_reloc_to_str(enum Reloc_e type_reloc);
char *type_enum_to_str(enum Section_e section);

void affiche_relocateur(struct Relocateur_s *relocateur_p);
void affiche_liste_relocation(struct Liste_s *liste_reloc_data_p);

#endif /* _RELOC_H_ */
