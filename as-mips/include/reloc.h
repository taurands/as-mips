/**
 * @file syn.h
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition de type et de prototypes de fonctions liées au traitement des relocations
 */

#ifndef _RELOC_H_
#define _RELOC_H_

#include <stdint.h>

enum Reloc_e {
 	R_MIPS_32=0,				/**< Dans liste_data_p si .word et si opérande est un SYMBOLE */
 	R_MIPS_26=1,				/**< Dans liste_text_p si instruction = J ou JAL et argument = SYMBOLE */
 	R_MIPS_HI16=2,				/**< Dans liste_text_p si instruction = LWI ou SWI et argument = SYMBOLE */
 	R_MIPS_LO16=3,				/**< Dans liste_text_p si instruction = LWI ou SWI et argument = SYMBOLE */
};

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

char *type_reloc_to_str(enum Reloc_e type_reloc);
char *type_enum_to_str(enum Section_e section);

void affiche_relocateur(struct Relocateur_s *relocateur_p);
void affiche_liste_relocation_data(struct Liste_s *liste_reloc_data_p);

#endif /* _RELOC_H_ */
