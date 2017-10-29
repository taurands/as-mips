/**
 * @file syn.c
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Definition des fonctions liées au traitement syntaxique du fichier
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <errno.h>

#include <global.h>
#include <notify.h>
#include <str_utils.h>
#include <liste.h>
#include <table.h>
#include <dico.h>
#include <lex.h>
#include <syn.h>

const char *NOMS_SECTIONS[] = {"initial", ".text", ".data", ".bss"};
const char *NOMS_DATA[] = {".space", ".byte", ".word", ".asciiz"};

char *clefEtiquette(void *donnee_p)
{
	return (donnee_p ? ((struct Etiquette_s *)donnee_p)->lexeme_p->data : NULL);
}

void str_instruction(struct Instruction_s * instruction_p, struct Table_s *table_p)
{
	int i;

	printf("%5d %08x XXXXXXXX %8s %8s %8s %8s",
			instruction_p->ligne,
			instruction_p->decalage,
			instruction_p->definition_p->nom,
			instruction_p->operandes[0] ? instruction_p->operandes[0]->data : "    ",
			instruction_p->operandes[1] ? instruction_p->operandes[1]->data : "    ",
			instruction_p->operandes[2] ? instruction_p->operandes[2]->data : "    ");

	for (i=0 ; i<3 ; i++)
		if (instruction_p->operandes[i] && (instruction_p->operandes[i]->nature==L_SYMBOLE)){
			if (!donnee_table(table_p, instruction_p->operandes[i]->data)){
				printf("    0xXXXXXXXX : symbole %c[%d;%dm%s%c[%d;%dm inconnu dans la table des étiquettes",
						0x1B, STYLE_BOLD, COLOR_RED,
						instruction_p->operandes[i]->data,
						0x1B, STYLE_BOLD, 0);
			} else {
				printf("    0x%08x : symbole %c[%d;%dm%s%c[%d;%dm en section %s",
						((struct Etiquette_s *)donnee_table(table_p, instruction_p->operandes[i]->data))->decalage,
						0x1B, STYLE_BOLD, COLOR_GREEN,
						instruction_p->operandes[i]->data,
						0x1B, STYLE_BOLD, 0,
						NOMS_SECTIONS[((struct Etiquette_s *)donnee_table(table_p, instruction_p->operandes[i]->data))->section]);
			}
		}
	printf("\n");
}

/* Fonction permettant d’afficher les éléments d’une donnée des setions .data ou .bss */
void affiche_element_databss(struct Donnee_s *donnee_p, struct Table_s *table_p)
{
	/* printf("nom directive %s  numero de ligne %d  decalage %d",donnee_p->lexeme_p->data, donnee_p->ligne, donnee_p->decalage); */
	printf("%5d %08x ", donnee_p->ligne, donnee_p->decalage);
	switch(donnee_p->type) {
	case D_BYTE:
		printf("      %02x : byte", donnee_p->valeur.octetNS);
		break;
	case D_WORD:
		if (donnee_p->lexeme_p->nature==L_SYMBOLE) {
			if (table_p) {
				if (!donnee_table(table_p, donnee_p->lexeme_p->data)) {
					printf("XXXXXXXX : symbole %c[%d;%dm%s%c[%d;%dm est inconnu dans la table des étiquettes",
							0x1B, STYLE_BOLD, COLOR_RED,
							donnee_p->lexeme_p->data,
							0x1B, STYLE_BOLD, 0);
				} else {
					printf("%08x : symbole %c[%d;%dm%s%c[%d;%dm en section %s",
							((struct Etiquette_s *)donnee_table(table_p, donnee_p->lexeme_p->data))->decalage,
							0x1B, STYLE_BOLD, COLOR_GREEN,
							donnee_p->lexeme_p->data,
							0x1B, STYLE_BOLD, 0,
							NOMS_SECTIONS[((struct Etiquette_s *)donnee_table(table_p, donnee_p->lexeme_p->data))->section]);
				}
			} else
				printf("0xXXXXXXXX : symbole %s", donnee_p->lexeme_p->data);
		} else {
			printf("%08x : word", donnee_p->valeur.motNS);
		}
		break;
	case D_ASCIIZ:
		printf("\"%s\" : asciiz", donnee_p->valeur.chaine);
		break;
	case D_SPACE:
		printf("%08x : space (nombre d'octets réservés)", donnee_p->valeur.nbOctets);
		break;
	default:
		printf("type non défini\n");
	}

	printf("\n");
}

void affiche_liste_donnee(struct Liste_s *liste_p, struct Table_s *table_p, char *titre_liste)
{
	struct Noeud_Liste_s* noeud_liste_p = NULL;
	if (!liste_p) {
		printf("%s n'existe pas !\n", titre_liste);
	} else {
		if (!(liste_p->nb_elts)) {
			printf("%s est vide\n", titre_liste);
		} else {
			printf("%s\n", titre_liste);
			for (noeud_liste_p=liste_p->debut_p ; (noeud_liste_p) ; noeud_liste_p=noeud_liste_p->suivant_p) {
				affiche_element_databss((struct Donnee_s *)noeud_liste_p->donnee_p, table_p);
			}
		}
	}
	printf("\n\n");
}

void affiche_liste_instructions(struct Liste_s *liste_p, struct Table_s *table_p, char *titre_liste)
{
	struct Noeud_Liste_s* noeud_liste_p=NULL;
	if (!liste_p) {
		printf("%s n'existe pas !\n", titre_liste);
	} else {
		if (!(liste_p->nb_elts)) {
			printf("%s est vide\n", titre_liste);
		} else {
			printf("%s\n", titre_liste);
			for (noeud_liste_p=liste_p->debut_p ; (noeud_liste_p) ; noeud_liste_p=noeud_liste_p->suivant_p) {
				str_instruction((struct Instruction_s *)noeud_liste_p->donnee_p, table_p);
			}
		}
	}
	printf("\n\n");
}

void affiche_table_etiquette(struct Table_s *table_p, char *titre)
{
	size_t i, j;

	struct Etiquette_s *etiquette_p=NULL;
	if (!table_p) {
		printf("%s n'existe pas !\n", titre);
	} else {
		if (!(table_p->nbElts)) {
			printf("%s est vide\n", titre);
		} else {
			printf("%s\n", titre);
			if (table_p) {
				j=0;
				for (i=0; i<table_p->nbEltsMax; i++)
					if (table_p->table[i]) {
						j++;
						etiquette_p=table_p->table[i];

						printf("%08x section %8s  %32s\n",etiquette_p->decalage, NOMS_SECTIONS[etiquette_p->section], etiquette_p->lexeme_p->data);
					}
			}
		}
	}
	printf("\n\n");
}

void aligner_decalage(uint32_t *decalage_p)
{
	const uint32_t masqueAlignement = 0x00000003; /* Les deux derniers bits doivent être à zéro pour avoir un aligement par mot de 32 bits */
	if (*decalage_p & masqueAlignement)
		*decalage_p=(*decalage_p + masqueAlignement) & ~masqueAlignement;

}

int enregistrer_etiquette(
		struct Lexeme_s *lexeme_p,
		enum Section_e section,
		uint32_t *decalage_p,
		struct Table_s *tableEtiquettes_p,
		struct Etiquette_s **mem_etiq_pp,
		char *msg_err)
{
	int erreur=SUCCESS;
	struct Etiquette_s *etiquetteCourante_p = NULL;

	etiquetteCourante_p = calloc (1, sizeof(*etiquetteCourante_p));
	if (!etiquetteCourante_p) {
		WARNING_MSG ("Impossible de créer une nouvelle étiquette");
		return FAIL_ALLOC;
	}

	/* XXX Revoir méthode d'alignement
	if ((section == S_DATA) && (suite_est_directive_word((*noeud_lexeme_pp)->suivant_p)))
		aligner_decalage(decalage_p);
	*/

	etiquetteCourante_p->lexeme_p = lexeme_p;
	etiquetteCourante_p->section = section;
	etiquetteCourante_p->decalage = *decalage_p;
	etiquetteCourante_p->ligne = lexeme_p->ligne;

	erreur = ajouter_table(tableEtiquettes_p, etiquetteCourante_p);
	if (SUCCESS== erreur) {
		INFO_MSG("Insertion de l'étiquette %zu : %s au decalage %u", tableEtiquettes_p->nbElts, (*lexeme_pp)->data, *decalage_p);
		if (section == S_DATA)
			*mem_etiq_pp = etiquetteCourante_p;
		return SUCCESS;
	} else {
		sprintf(msg_err, "est une étiquette déjà présente ligne %d", ((struct Etiquette_s *)donnee_table(tableEtiquettes_p, etiquetteCourante_p->lexeme_p->data))->ligne);
		free(etiquetteCourante_p);
		etiquetteCourante_p=NULL;
		return FAILURE;
	}
}

int lire_nombre(
		struct Lexeme_s *lexeme_p,
		struct Donnee_s *donnee_p,
		uint32_t *decalage_p,
		char *msg_err)
{
	long int nombre;

	donnee_p->decalage = *decalage_p;
	donnee_p->lexeme_p = lexeme_p;
	donnee_p->ligne = lexeme_p->ligne;

	errno = 0; /* remet le code d'erreur à 0 */
	nombre = strtol(lexeme_p->data, NULL, 0); /* Convertit la chaine en nombre, avec base automatique */
	if (errno) {
		strcpy(msg_err, "n'a pas pu être évalué numériquement");
		return FAILURE;
	} else {
		if (((donnee_p->type==D_BYTE) && ((nombre>UINT8_MAX) || (nombre<INT8_MIN))) ||
			((donnee_p->type==D_WORD) && ((nombre>UINT32_MAX) || (nombre<INT32_MIN))) ||
			((donnee_p->type==D_SPACE) && (nombre<=0) && (nombre+*decalage_p>=UINT32_MAX))) {

			strcpy(msg_err, "est au delà des valeurs permises");
			return FAILURE;
		} else {
			if (donnee_p->type==D_BYTE) {
				if (nombre>=0)
					donnee_p->valeur.octetNS=(uint8_t)nombre;
				else
					donnee_p->valeur.octet=(int8_t)nombre;
				(*decalage_p)++;
			} else if (donnee_p->type==D_WORD) {
				if (nombre>=0)
					donnee_p->valeur.motNS=(uint32_t)nombre;
				else
					donnee_p->valeur.mot=(int32_t)nombre;
				(*decalage_p)+=4;
			} else { /* typeDonnee==D_SPACE */
				donnee_p->valeur.nbOctets=(uint32_t)nombre;
				(*decalage_p)+=donnee_p->valeur.nbOctets;
			}
		}
	}
	return SUCCESS;
}

int analyser_donnee(
		struct Liste_s *lignes_lexemes_p,			/**< Pointeur sur la liste des lexèmes */
		struct Liste_s *liste_p,					/**< Pointeur sur la liste des instructions de la section .text */
		uint32_t *decalage_p,
		char *msg_err)
{
	struct Noeud_Liste_s *noeud_courant_p = NULL;
	struct Lexeme_s *lexeme_p = NULL;

	enum Donnee_e type_donnee = D_UNDEF;
	struct Donnee_s *donnee_p = NULL;

	int code_retour = SUCCESS;

	enum Etat_e {
		ERREUR,
		INIT,
		SUITE,
		EOL,

		DONNEE
	} etat = INIT;

	if (!lignes_lexemes_p || !liste_p || !decalage_p || !msg_err)
		return FAILURE;
	else if ((noeud_courant_p = courant_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p)) {
		do {
			switch(etat) {
			case INIT:
				if ((!strcmp(lexeme_p->data, NOMS_DATA[D_BYTE])))
					type_donnee = D_BYTE;
				else if ((!strcmp(lexeme_p->data, NOMS_DATA[D_WORD])))
					type_donnee = D_WORD;
				else if ((!strcmp(lexeme_p->data, NOMS_DATA[D_ASCIIZ])))
					type_donnee = D_ASCIIZ;
				else if ((!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE])))
					type_donnee = D_SPACE;
				etat = DONNEE;
				break;
			case DONNEE:
				if (((type_donnee != D_ASCIIZ) && (lexeme_p->nature == L_NOMBRE)) ||
					((type_donnee == D_ASCIIZ) && (lexeme_p->nature == L_CHAINE)) ||
					((type_donnee == D_WORD) && (lexeme_p->nature == L_SYMBOLE))) {

					if (!(donnee_p = calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
					donnee_p->type = type_donnee;

					if (type_donnee == D_ASCIIZ) {
						donnee_p->decalage =* decalage_p;
						donnee_p->type = D_ASCIIZ;
						donnee_p->lexeme_p = lexeme_p;
						donnee_p->ligne = lexeme_p->ligne;
						donnee_p->valeur.chaine=NULL; /* XXX Il faut lire... */

						(*decalage_p)+=(donnee_p->valeur.chaine ? 1+strlen(donnee_p->valeur.chaine) : 0);
					} else {
						code_retour = lire_nombre(lexeme_p, donnee_p, decalage_p, msg_err);
						if (code_retour != SUCCESS) {
							free(donnee_p);
							return code_retour;
						}
					}

					code_retour = ajouter_fin_liste(liste_p, donnee_p);
					if (code_retour != SUCCESS) {
						free(donnee_p);
						donnee_p = NULL;
						strcpy(msg_err, "");
						return code_retour;
					} else {
						donnee_p = NULL;
						etat = SUITE;
					}
				} else {
					strcpy(msg_err, "n'a pas le type attendu");
					etat = ERREUR;
				}
				break;
			case SUITE:
				if (lexeme_p->nature == L_VIRGULE)
					etat = DONNEE;
				else if (lexeme_p->nature == L_COMMENTAIRE)
					etat = SUITE;
				else if (lexeme_p->nature == L_FIN_LIGNE)
					etat = EOL;
				else {
					strcpy(msg_err, "n'est pas valide ici");
					etat = ERREUR;
				}
				break;
			default:
				etat=ERREUR;
				strcpy(msg_err, "ne devrait pas être là");
			}
		} while ((etat != ERREUR) && (etat != EOL) && (noeud_courant_p = suivant_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p));
		return (etat == ERREUR) ? FAILURE : SUCCESS;
	} else {
		return FAILURE;
	}
}

/**
 * @return Rien, si ce n'est les données mises à jour par les pointeurs paramètres associés
 * @brief effectue l'analyse syntaxique de premier niveau d'une ligne de lexemes pour une instruction
 *
 *	@dot
 *	digraph Machine_Etat_Instruc {
 *		concentrate=false;
 *		graph [label="\nMachine à états finis d'analyse syntaxique - Traitement des instructions"; fontname = "arial"; fontsize = 16;];
 *		edge [fontname = "arial"; fontsize = 10;];
 *		node [shape="ellipse";  fontname = "arial"; fontsize = 10; color = "black";]
 *
 *		rankdir=LR; // de gauche vers la droite
 *		INSTRUC -> ERREUR [label = "mauvaise instruction"]
 *		INSTRUC -> I_B_REG [label = "base offset"]
 *		INSTRUC -> I_RN_3OP [label = "3 ops"]
 *		INSTRUC -> I_RN_2OP [label = "2 ops"]
 *		INSTRUC -> I_R_1OP [label = "1 ops et R"]
 *		INSTRUC -> I_N_OP [label = "1 ops et N"]
 *		INSTRUC -> ERREUR [label = "sinon"]
 *
 *		I_B_REG -> I_B_VIR [label = "VIR"]
 *		I_B_VIR -> I_B_OFFS [label = "NB ou SYM"]
 *		I_B_OFFS -> I_B_PO [label = "PO"]
 *		I_B_PO -> I_B_BASE [label = "REG"]
 *		I_B_BASE -> I_B_PF [label = "PF"]
 *		I_B_PF -> COMMENT [label = "COMMENT"]
 *		I_B_PF -> EOL [label = "EOL"]

 *		I_RN_3OP -> I_RN_V32 [label = "VIR"]
 *		I_RN_V32 -> I_RN_2OP [label = "REG"]
 *		I_RN_2OP -> I_RN_V21 [label = "VIR"]
 *		I_RN_V21 -> I_R_1OP [label = "R et REG"]
 *		I_R_1OP -> ERREUR [label = "pas bon REG"]
 *		I_R_1OP -> COMMENT [label = "COMMENT"]
 *		I_R_1OP -> EOL [label = "EOL"]
 *		I_RN_V21 -> I_N_OP [label = "N et (NB ou SYM)"]
 *		I_N_OP -> COMMENT [label = "COMMENT"]
 *		I_N_OP -> EOL [label = "EOL"]
 *
 *		I_B_REG -> ERREUR [label = "pas bon REG"]
 *		I_B_REG -> ERREUR [label = "sinon"]
 *
 *		I_B_VIR -> ERREUR [label = "sinon"]
 *
 *		I_B_OFFS -> ERREUR [label = "sinon"]
 *
 *		I_B_PO -> ERREUR [label = "sinon"]
 *
 *		I_B_BASE -> ERREUR [label = "pas bon REG"]
 *		I_B_BASE -> ERREUR [label = "sinon"]
 *
 *		I_B_PF -> ERREUR [label = "sinon"]
 *
 *
 *
 *		I_RN_3OP -> ERREUR [label = "pas bon REG"]
 *		I_RN_3OP -> ERREUR [label = "sinon"]
 *
 *		I_RN_V32 -> ERREUR [label = "sinon"]
 *
 *		I_RN_2OP -> ERREUR [label = "pas bon REG"]
 *		I_RN_2OP -> ERREUR [label = "sinon"]
 *
 *		I_RN_V21 -> ERREUR [label = "sinon"]
 *
 *		I_R_1OP -> ERREUR [label = "sinon"]
 *
 *		I_N_OP -> ERREUR [label = "sinon"]
 *
 *		INSTRUC -> COMMENT [label = "0 ops et COMMENT"]
 *		INSTRUC -> EOL [label = "0 ops et EOL"]
 *
 *		COMMENT -> EOL [label=EOL]
 *		COMMENT -> ERREUR [label = "sinon"]
 *	}
 *  @enddot
 *
 */
int analyser_instruction(
		struct Liste_s *lignes_lexemes_p,			/**< Pointeur sur la liste des lexèmes */
		struct Table_s *table_def_instructions_p,	/**< Pointeur sur la table "dico" des instructions */
		struct Table_s *table_def_registres_p,		/**< Pointeur sur la table "dico" des registres */
		struct Liste_s *liste_p,				/**< Pointeur sur la liste des instructions de la section .text */
		uint32_t *decalage_p,
		char *msg_err)
{
	struct Noeud_Liste_s *noeud_courant_p = NULL;
	struct Lexeme_s *lexeme_p = NULL;
	struct DefinitionInstruction_s *def_p = NULL;
	struct Instruction_s *instruction_p = NULL;

	enum Etat_e {
		ERREUR,
		INIT,
		SUITE,
		EOL,

		I_RN_3OP,
		I_RN_V32,
		I_RN_2OP,
		I_RN_V21,
		I_R_1OP,
		I_N_OP,

		I_B_REG,
		I_B_VIR,
		I_B_OFFS,
		I_B_PO,
		I_B_BASE,
		I_B_PF,
	} etat = INIT;

	if (!lignes_lexemes_p || !table_def_instructions_p || !table_def_registres_p || !liste_p || !decalage_p || !msg_err)
		return FAILURE;
	else if ((noeud_courant_p = courant_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p)) {
		do {
			switch(etat) {
			case INIT:
				def_p=donnee_table(table_def_instructions_p, lexeme_p->data);
				if (!def_p) {
					etat=ERREUR;
					strcpy(msg_err, "n'est pas une instruction connue");
				} else {
					INFO_MSG("Prise en compte de l'instruction %s à %d opérandes au décalage %d", def_instruction_p->nom, def_instruction_p->nb_ops, *decalage_p);
					if (!(instruction_p = calloc (1,sizeof(*instruction_p)))) {
						return FAIL_ALLOC;
					}
					instruction_p->definition_p = def_p;
					instruction_p->ligne = lexeme_p->ligne;
					instruction_p->decalage = *decalage_p;

					if (def_p->type_ops==I_OP_B)
						etat = I_B_REG;
					else if (def_p->nb_ops==3)
						etat = I_RN_3OP;
					else if (def_p->nb_ops==2)
						etat = I_RN_2OP;
					else if ((def_p->nb_ops==1) && (def_p->type_ops==I_OP_R))
						etat = I_R_1OP;
					else if ((def_p->nb_ops==1) && (def_p->type_ops==I_OP_N))
						etat = I_N_OP;
					else if (def_p->nb_ops==0)
						etat = SUITE;
					else
						ERROR_MSG("Si on est là, c'est qu'il y a un bug sur le nombre d'opérandes de la définition d'instruction");
				}
				break;
			case I_RN_3OP:
				if (lexeme_p->nature != L_REGISTRE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre");
				} else if (!donnee_table(table_def_registres_p, lexeme_p->data)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre valide");
				} else {
					instruction_p->operandes[def_p->nb_ops-3] = lexeme_p;
					etat = I_RN_V32;
				}
				break;
			case I_RN_V32:
				if (lexeme_p->nature != L_VIRGULE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas une virgule");
				} else {
					etat = I_RN_2OP;
				}
				break;
			case I_RN_2OP:
				if (lexeme_p->nature != L_REGISTRE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre");
				} else if (!donnee_table(table_def_registres_p, lexeme_p->data)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre valide");
				} else {
					instruction_p->operandes[def_p->nb_ops-2] = lexeme_p;
					etat = I_RN_V21;
				}
				break;
			case I_RN_V21:
				if (lexeme_p->nature != L_VIRGULE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas une virgule");
				} else if (def_p->type_ops==I_OP_R){
					etat = I_R_1OP;
				} else {
					etat = I_N_OP;
				}
				break;
			case I_R_1OP:
				if (lexeme_p->nature != L_REGISTRE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre");
				} else if (!donnee_table(table_def_registres_p, lexeme_p->data)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre valide");
				} else {
					instruction_p->operandes[def_p->nb_ops-1] = lexeme_p;
					etat = SUITE;
				}
				break;
			case I_N_OP:
				if ((lexeme_p->nature != L_NOMBRE) && (lexeme_p->nature != L_SYMBOLE)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est ni un nombre, ni un registre");
				} else {
					instruction_p->operandes[def_p->nb_ops-1] = lexeme_p;
					etat = SUITE;
				}
				break;

			case I_B_REG:
				if (lexeme_p->nature != L_REGISTRE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre");
				} else if (!donnee_table(table_def_registres_p, lexeme_p->data)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre valide");
				} else {
					instruction_p->operandes[0] = lexeme_p;
					etat = I_B_VIR;
				}
				break;
			case I_B_VIR:
				if (lexeme_p->nature != L_VIRGULE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas une virgule");
				} else {
					etat = I_B_OFFS;
				}
				break;
			case I_B_OFFS:
				if ((lexeme_p->nature != L_NOMBRE) && (lexeme_p->nature != L_SYMBOLE)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est ni un nombre, ni un registre");
				} else {
					instruction_p->operandes[1] = lexeme_p;
					etat = I_B_PO;
				}
				break;
			case I_B_PO:
				if (lexeme_p->nature != L_PARENTHESE_OUVRANTE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas une parenthèse ouvrante");
				} else {
					etat = I_B_BASE;
				}
				break;
			case I_B_BASE:
				if (lexeme_p->nature != L_REGISTRE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre");
				} else if (!donnee_table(table_def_registres_p, lexeme_p->data)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas un registre valide");
				} else {
					instruction_p->operandes[2] = lexeme_p;
					etat = I_B_PF;
				}
				break;
			case I_B_PF:
				if (lexeme_p->nature != L_PARENTHESE_FERMANTE) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas une parenthèse fermante");
				} else {
					etat = SUITE;
				}
				break;
			case SUITE:
				if ((lexeme_p->nature != L_COMMENTAIRE) && (lexeme_p->nature != L_FIN_LIGNE)) {
					etat=ERREUR;
					strcpy(msg_err, "ne devait pas se trouver après cette instruction");
				} else if (lexeme_p->nature == L_COMMENTAIRE) {
					etat=SUITE;
				} else {
					etat=EOL;
				}
				break;
			default:
				etat=ERREUR;
				strcpy(msg_err, "ne devrait pas être là");
			}
		} while ((etat != ERREUR) && (etat != EOL) && (noeud_courant_p = suivant_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p));

		/* tester le null, fin ligne, ... */
		if ((etat == EOL) && (SUCCESS == ajouter_fin_liste(liste_p, instruction_p))) {
			instruction_p=NULL;
			(*decalage_p)+=4;
			return SUCCESS;
		} else {
			free(instruction_p);
			return FAILURE;
		}
	} else {
		return FAILURE;
	}
}

/**
 * @return XXX
 * @brief effectue l'analyse syntaxique de premier niveau d'une liste de lexemes
 *
 */
int analyser_syntaxe(
		struct Liste_s *lignes_lexemes_p,			/**< Pointeur sur la liste des lexèmes */
		struct Table_s *table_def_instructions_p,	/**< Pointeur sur la table "dico" des instructions */
		struct Table_s *table_def_registres_p,		/**< Pointeur sur la table "dico" des registres */
		struct Table_s *table_etiquettes_p,			/**< Pointeur sur la table des étiquettes */
		struct Liste_s *liste_text_p,				/**< Pointeur sur la liste des instructions de la section .text */
		struct Liste_s *liste_data_p,				/**< Pointeur sur la liste des données de la section .data */
		struct Liste_s *liste_bss_p)				/**< Pointeur sur la liste des réservations des .space de la section .bss */
{
	uint32_t decalage_text=0;
	uint32_t decalage_data=0;
	uint32_t decalage_bss=0;

	char msg_err[2*STRLEN];

	struct Noeud_Liste_s *noeud_courant_p=NULL;
	struct Lexeme_s *lexeme_p=NULL;

	enum Section_e section=S_INIT;
	struct Etiquette_s **mem_etiq_table = NULL;
	size_t index_mem = 0;

	int erreur=SUCCESS;
	int resultat=SUCCESS;

	enum Etat_e {
		ERREUR,
		INIT,
		SUITE,
		OPTION
	} etat = INIT;

	if (!lignes_lexemes_p || !table_def_instructions_p || !table_def_registres_p || !table_etiquettes_p || !liste_text_p || !liste_data_p || !liste_bss_p)
		return FAILURE;
	else if ((noeud_courant_p = debut_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p)) {
		msg_err[0]='\0';
		mem_etiq_table = calloc (table_etiquettes_p->nbEltsMax, sizeof(*mem_etiq_table));
		do {
			switch(etat) {
			case INIT:
				if (lexeme_p->nature==L_FIN_LIGNE)
					etat=INIT;
				else if (lexeme_p->nature==L_COMMENTAIRE)
					etat=INIT;
				else if ((section!=S_INIT) && (lexeme_p->nature==L_ETIQUETTE)) {
					if (section == S_TEXT)
						erreur = enregistrer_etiquette(lexeme_p, section, &decalage_text, table_etiquettes_p, NULL, msg_err);
					else if (section == S_DATA) {
						erreur = enregistrer_etiquette(lexeme_p, section, &decalage_data, table_etiquettes_p, mem_etiq_table+index_mem, msg_err);
						if ((erreur == SUCCESS) && (mem_etiq_table[index_mem]))
							index_mem++;
					}
					else if (section == S_BSS)
						erreur = enregistrer_etiquette(lexeme_p, section, &decalage_bss, table_etiquettes_p, NULL, msg_err);
					if (erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					}
					else if (erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_TEXT) && (lexeme_p->nature==L_INSTRUCTION)) {
					erreur = analyser_instruction(lignes_lexemes_p, table_def_instructions_p, table_def_registres_p, liste_text_p, &decalage_text, msg_err);
					if (erreur == FAIL_ALLOC)
						return FAIL_ALLOC;
					else if (erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_BYTE]))) {
					for (index_mem = 0; mem_etiq_table[index_mem]; )
						mem_etiq_table[index_mem++] = NULL;
					index_mem = 0;
					erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_WORD]))) {
					aligner_decalage(&decalage_data);
					for (index_mem = 0; mem_etiq_table[index_mem]; ) {
						mem_etiq_table[index_mem]->decalage = decalage_data;
						mem_etiq_table[index_mem++] = NULL;
					}
					index_mem = 0;
					erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_ASCIIZ]))) {
					for (index_mem = 0; mem_etiq_table[index_mem]; )
						mem_etiq_table[index_mem++] = NULL;
					index_mem = 0;
					erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE]))) {
					for (index_mem = 0; mem_etiq_table[index_mem]; )
						mem_etiq_table[index_mem++] = NULL;
					index_mem = 0;
					erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_BSS) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE]))) {
					erreur = analyser_donnee(lignes_lexemes_p, liste_bss_p, &decalage_bss, msg_err);
					if (erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((lexeme_p->nature==L_DIRECTIVE) && ((!strcmp(lexeme_p->data, NOMS_SECTIONS[S_TEXT])))) {
					section = S_TEXT;
					etat = SUITE;
				} else if ((lexeme_p->nature==L_DIRECTIVE) && ((!strcmp(lexeme_p->data, NOMS_SECTIONS[S_DATA])))) {
					section = S_DATA;
					etat = SUITE;
				} else if ((lexeme_p->nature==L_DIRECTIVE) && ((!strcmp(lexeme_p->data, NOMS_SECTIONS[S_BSS])))) {
					section = S_BSS;
					etat = SUITE;
				} else if ((section==S_INIT) && (lexeme_p->nature==L_DIRECTIVE) && (!strcmp(lexeme_p->data, ".set")))
					etat=OPTION;
				else {
					etat=ERREUR;
					strcpy(msg_err, "n'est pas valide ici");
				}
				break;
			case OPTION:
				if ((lexeme_p->nature == L_SYMBOLE) && (!strcmp(lexeme_p->data, "noreorder")))
					etat=SUITE;
				else {
					etat=ERREUR;
					strcpy(msg_err, "ne devait pas se trouver après la directive .set");
				}
				break;
			case SUITE:
				if ((lexeme_p->nature != L_COMMENTAIRE) && (lexeme_p->nature != L_FIN_LIGNE)) {
					etat=ERREUR;
					strcpy(msg_err, "ne devait pas se trouver après cette instruction");
				} else if (lexeme_p->nature == L_COMMENTAIRE) {
					etat=SUITE;
				} else {
					etat=INIT;
				}
				break;
			default:
				etat=ERREUR;
				strcpy(msg_err, "ne devrait pas être là (defaut switch principal)");
			}

			if (etat == ERREUR) {
				if ((noeud_courant_p = courant_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p)) {
					fprintf(stderr, "Erreur de syntaxe ligne %d, ", lexeme_p ? lexeme_p->ligne : 0);
					fprintf(stderr, "%c[%d;%dm%s%c[%d;%dm ", 0x1B, STYLE_BOLD, COLOR_RED, (!(lexeme_p->data) ? "une fin de ligne" : lexeme_p->data),0x1B, STYLE_OFF, 0);
					fprintf(stderr, "%s.\n", msg_err);
					msg_err[0]='\0';
				} else
					ERROR_MSG("fin de liste de lexème innatendue");
				while ((noeud_courant_p = suivant_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p) && (lexeme_p->nature != L_FIN_LIGNE))
					; /* Passe tous les lexèmes jusqu'à la fin de ligne */
				resultat=FAILURE;
				etat = INIT;
			}
		} while ((noeud_courant_p = suivant_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p));
		free(mem_etiq_table);
		return resultat;
	} else {
		return FAILURE;
	}
}
