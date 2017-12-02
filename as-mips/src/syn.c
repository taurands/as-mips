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
#include <reloc.h>

const char *NOMS_SECTIONS[] = {"initial", ".text", ".data", ".bss", "undef"};
const char *NOMS_DATA[] = {".space", ".byte", ".half", ".word", ".float", ".asciiz"};

char *clefEtiquette(void *donnee_p)
{
	return (donnee_p ? ((struct Etiquette_s *)donnee_p)->lexeme_p->data : NULL);
}

/**
 * @param donnee_p pointeur sur une donnée à détruire
 * @return Rien
 * @brief Cette fonction permet de détuire et libérer le contenu d'une donnée
 *
 * Cela inclut en particulier la chaine représentation le contenu d'un ASCIIZ.
 * Ceci est nécessaire pour le mécanisque de gestion propre des liste génériques.
 */
void detruit_donnee(void *donnee_p)
{
	if (donnee_p) {
		if (((struct Donnee_s *)donnee_p)->type == D_ASCIIZ)
			free(((struct Donnee_s *)donnee_p)->valeur.chaine);
		free(donnee_p);
	}
}

/**
 * @param donnee défini le type de donnée d'une donnée
 * @return chaine de caractères contenant le nom du type de donnée
 * @brief Cette fonction permet de donner le nom correspondant à un type de donnée
 *
 */
char *type_donnee_to_str(enum Donnee_e donnee)
{
	switch(donnee) {
		case D_UNDEF:					return "undef";
		case D_SPACE:					return ".space";
		case D_BYTE:						return ".byte";
		case D_HALF:						return ".half";
		case D_WORD:						return ".word";
		case D_FLOAT:						return ".float";
		case D_ASCIIZ:						return ".asciiz";
		default :
			ERROR_MSG("Erreur de résolution du nom du type de donnee... Il manque donc au moins un nom à rajouter pour %d", donnee);
	}
	return NULL;
}

/**
 * @param instruction_p pointeur sur une instruction à lire
 * @param table_p pointeur sur une table d'instructions
 * @return Rien
 * @brief Cette fonction permet d'afficher une instruction
 *
 */
void str_instruction(struct Instruction_s * instruction_p, struct Table_s *table_p)
{
	int i;

	printf("%3d %08x ",
			instruction_p->ligne,
			instruction_p->decalage);
	if (instruction_p->definition_p) {
		printf("%08x %-40s",
			instruction_p->op_code,
			instruction_p->source ? instruction_p->source : "");

		printf("%s %s %s %s",
		instruction_p->definition_p->nom,
		instruction_p->operandes[0] ? instruction_p->operandes[0]->data : "",
		instruction_p->operandes[1] ? instruction_p->operandes[1]->data : "",
		instruction_p->operandes[2] ? instruction_p->operandes[2]->data : "");

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
	} else
		printf("%*s %s", 8, "", "");

	printf("\n");
}

/**
 * @param donnee_p pointeur sur un element de .data ou .bss à lire
 * @param table_p pointeur sur une table de donnees
 * @return Rien
 * @brief Cette fonction permet d'afficher un element des sections .data ou .bss
 *
 */
void affiche_element_databss(struct Donnee_s *donnee_p, struct Table_s *table_p)
{
	unsigned int i,j;
	struct Etiquette_s *etiquette_p = NULL;

	/* printf("nom directive %s  numero de ligne %d  decalage %d",donnee_p->lexeme_p->data, donnee_p->ligne, donnee_p->decalage); */
	if (!(donnee_p->decalage & 3) || (donnee_p->type == D_ASCIIZ)|| (donnee_p->type == D_SPACE)) {
		printf("%3d %08X ", donnee_p->ligne, donnee_p->decalage);
	}
	switch(donnee_p->type) {
	case D_BYTE:
		printf("%02X", donnee_p->valeur.octetNS);
		break;
	case D_HALF:
		printf("%04X", donnee_p->valeur.demiNS);
		break;
	case D_WORD:
		if (donnee_p->lexeme_p->nature==L_SYMBOLE) {
			if (table_p && (etiquette_p = donnee_table(table_p, donnee_p->lexeme_p->data))) {
				printf("%08X", etiquette_p->decalage);
			} else
				printf("00000000");
		} else {
			printf("%08X %s %s", donnee_p->valeur.motNS, type_donnee_to_str(donnee_p->type), donnee_p->lexeme_p->data);
		}
		break;
	case D_ASCIIZ:
		for (i=0; i<str_unesc_len(donnee_p->lexeme_p->data)-1; ) {
			j = (unsigned char)(donnee_p->valeur.chaine[i++]);
			printf("%02X",j);
			if (!(i&3)  && (i<str_unesc_len(donnee_p->lexeme_p->data)-1))
				printf("\n%3d %08X ", donnee_p->ligne, donnee_p->decalage+i);
		}
		break;
	case D_SPACE:
		if (donnee_p->valeur.nbOctets>4)
			printf("0000...");
		else
			printf("00?");
		break;
	default:
		printf("type non défini\n");
	}

	printf("\n");
}

/**
 * @param liste_p pointeur sur une liste générique des éléments de .data ou .bss à lire
 * @param table_p pointeur sur une table de donnees
 * @return Rien
 * @brief Cette fonction permet d'afficher la liste des éléments des sections .data ou .bss
 *
 */
void affiche_liste_donnee(struct Liste_s *liste_p, struct Table_s *table_p)
{
	struct Noeud_Liste_s* noeud_liste_p = NULL;
	if (!liste_p) {
		printf("La liste n'existe pas !\n");
	} else {
		if (!(liste_p->nb_elts)) {
			printf("La liste est vide\n");
		} else {
			for (noeud_liste_p=liste_p->debut_p ; (noeud_liste_p) ; noeud_liste_p=noeud_liste_p->suivant_p) {
				affiche_element_databss((struct Donnee_s *)noeud_liste_p->donnee_p, table_p);
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
void affiche_liste_instructions(struct Liste_s *liste_p, struct Table_s *table_p)
{
	struct Noeud_Liste_s* noeud_liste_p=NULL;
	if (!liste_p) {
		printf("La liste n'existe pas !\n");
	} else {
		if (!(liste_p->nb_elts)) {
			printf("La liste est vide\n");
		} else {
			for (noeud_liste_p=liste_p->debut_p ; (noeud_liste_p) ; noeud_liste_p=noeud_liste_p->suivant_p) {
				str_instruction((struct Instruction_s *)noeud_liste_p->donnee_p, table_p);
			}
		}
	}
	printf("\n\n");
}

/**
 * @param table_p pointeur sur la table d'étiquette à remplir
 * @return Rien
 * @brief Cette fonction permet d'ajouter une étiquette à une table d'étiquette
 *
 */
void affiche_table_etiquette(struct Table_s *table_p)
{
	size_t i, j;

	struct Etiquette_s *etiquette_p=NULL;
	if (!table_p) {
		printf("La table d'étiquette n'existe pas !\n");
	} else {
		if (!(table_p->nbElts)) {
			printf("La table d'étiquette est vide\n");
		} else {
			if (table_p) {
				j=0;
				for (i=0; i<table_p->nbEltsMax; i++)
					if (table_p->table[i]) {
						j++;
						etiquette_p=table_p->table[i];

						if (etiquette_p->section != S_UNDEF)
							printf("%3d\t%-5s:%08x\t%s\n",etiquette_p->lexeme_p->ligne,type_enum_to_str(etiquette_p->section),etiquette_p->decalage,etiquette_p->lexeme_p->data);
						else
							printf("%3d\t[UNDEFINED]\t%s\n",etiquette_p->lexeme_p->ligne,etiquette_p->lexeme_p->data);
					}
			}
		}
	}
	printf("\n\n");
}

/**
 * @param decalage_p pointeur sur la valeur à décaler
 * @param nb_bits nombre de bits de décalage
 * @return Rien
 * @brief Cette fonction permet d'efectuer un décalage de nb_bits sur une adresse
 *
 */
void aligner_decalage(uint32_t *decalage_p, unsigned int nb_bits)
{
	uint32_t masqueAlignement = 0x00000001;
	masqueAlignement = (masqueAlignement << nb_bits) - 1;
	if (*decalage_p & masqueAlignement)
		*decalage_p=(*decalage_p + masqueAlignement) & ~masqueAlignement;
}

/**
 * @param lexeme_p pointeur sur le lexeme correspondant
 * @param section section où est définie l'étiquette
 * @param decalage_p decalage mémoire par rapport au début de section où est définie l'étiquette
 * @param tableEtiquettes_p pointeur sur la table où sera ajouter l'étiquette
 * @param mem_etiq_pp pointeur de pointeur permettant de garder en mémoire l'étiquette intacte
 * @param msg_err message d'erreur le cas échant
 * @return SUCCESS si l'étiquette à bien été ajoutée à la table d'étiquette
 * @brief Cette fonction permet de remplir tous les champs d'une étiquette et d'ajouter l'étiquette à la table d'étiquettes
 *
 */
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

/**
 * @param lexeme_p pointeur sur le lexeme (nombre) à lire
 * @param donnee_p pointeur sur la donnée qui sera crée
 * @param decalage_p pointeur sur la valeur du decalage mémoire par rapport au début de section
 * @param msg_err message d'erreur le cas échant
 * @return SUCCESS si le nombre a pu être évalué
 * @brief Cette fonction permet de créer une donnée à partir d'un nombre
 *
 */
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
			((donnee_p->type==D_HALF) && ((nombre>UINT16_MAX) || (nombre<INT16_MIN))) ||
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
			} else if (donnee_p->type==D_HALF) {
				if (nombre>=0)
					donnee_p->valeur.demiNS=(uint16_t)nombre;
				else
					donnee_p->valeur.demi=(int16_t)nombre;
				(*decalage_p)+=2;
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

/**
 * @param lignes_lexemes_p pointeur sur la liste de lxèmes obtenu après analyse lexicale
 * @param liste_p pointeur sur une liste de données qui sera rempli
 * @param decalage_p pointeur sur la valeur du decalage mémoire par rapport au début de section
 * @param msg_err message d'erreur le cas échant
 * @return SUCCESS si le remplissage de la liste liste_p s'est bien déroulé
 * @brief Cette fonction permet de remplir la liste de données par analyse sur la liste de lexèmes
 *
 */
int analyser_donnee(
		struct Liste_s *lignes_lexemes_p,			/**< Pointeur sur la liste des lexèmes */
		struct Liste_s *liste_p,					/**< Pointeur sur la liste des donnees de la section .data ou .bss */
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
				else if ((!strcmp(lexeme_p->data, NOMS_DATA[D_HALF])))
					type_donnee = D_HALF;
				else if ((!strcmp(lexeme_p->data, NOMS_DATA[D_WORD])))
					type_donnee = D_WORD;
				else if ((!strcmp(lexeme_p->data, NOMS_DATA[D_FLOAT])))
					type_donnee = D_FLOAT;
				else if ((!strcmp(lexeme_p->data, NOMS_DATA[D_ASCIIZ])))
					type_donnee = D_ASCIIZ;
				else if ((!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE])))
					type_donnee = D_SPACE;
				etat = DONNEE;
				break;
			case DONNEE:
				if (((type_donnee != D_ASCIIZ) && (lexeme_p->nature == L_NOMBRE)) ||
					((type_donnee == D_ASCIIZ) && (lexeme_p->nature == L_CHAINE)) ||
					((type_donnee == D_BYTE) && (lexeme_p->nature == L_CAR)) ||
					((type_donnee == D_WORD) && (lexeme_p->nature == L_SYMBOLE))) {

					if (!(donnee_p = calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
					donnee_p->type = type_donnee;

					if (type_donnee == D_ASCIIZ) {
						donnee_p->decalage = *decalage_p;
						donnee_p->type = D_ASCIIZ;
						donnee_p->lexeme_p = lexeme_p;
						donnee_p->ligne = lexeme_p->ligne;
						donnee_p->valeur.chaine = dqstr_unesc_str (lexeme_p->data);
						(*decalage_p)+=(donnee_p->valeur.chaine ? str_unesc_len (lexeme_p->data) - 1 : 0);
					} else if ((type_donnee == D_BYTE) && (lexeme_p->nature == L_CAR)) {
						donnee_p->decalage = *decalage_p;
						donnee_p->type = D_BYTE;
						donnee_p->lexeme_p = lexeme_p;
						donnee_p->ligne = lexeme_p->ligne;
						donnee_p->valeur.car = sqstr_unesc_char (lexeme_p->data);
						(*decalage_p)++;
					} else{
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

/**
 * @param lignes_lexemes_p pointeur sur la liste de lxèmes obtenu après analyse lexicale
 * @param lexemes_supl_p pointeur sur la liste des lexemes à rajouter si nécessaire
 * @param table_def_instructions_p pointeur sur la table "dico" des instructions
 * @param table_def_pseudo_p pointeur sur la table "dico" des pseudo instructions
 * @param table_def_registres_p pointeur sur la table "dico" des registres
 * @param liste_p pointeur sur une liste de données qui sera rempli
 * @param decalage_p pointeur sur la valeur du decalage mémoire par rapport au début de section
 * @param msg_err message d'erreur le cas échant
 * @return SUCCESS si le remplissage de la liste liste_p s'est bien déroulé
 * @brief Cette fonction permet de remplir la liste des instructions par analyse sur la liste de lexèmes
 *
 */
int analyser_instruction(
		struct Liste_s *lignes_lexemes_p,			/**< Pointeur sur la liste des lexèmes */
		struct Liste_s *lexemes_supl_p,				/**< Pointeur sur la liste des lexemes à rajouter si nécessaire */
		struct Table_s *table_def_instructions_p,	/**< Pointeur sur la table "dico" des instructions */
		struct Table_s *table_def_pseudo_p,			/**< Pointeur sur la table "dico" des pseudo instructions */
		struct Table_s *table_def_registres_p,		/**< Pointeur sur la table "dico" des registres */
		struct Liste_s *liste_p,					/**< Pointeur sur la liste des instructions de la section .text */
		uint32_t *decalage_p,
		char *msg_err)
{
	struct Noeud_Liste_s *noeud_courant_p = NULL;
	struct Lexeme_s *lexeme_p = NULL;
	struct DefinitionInstruction_s *def_p = NULL;
	struct Instruction_s *instruction_p = NULL;
	struct Instruction_s *instr_supl_p = NULL;
	struct DefinitionPseudoInstruction_s *def_pseudo_p = NULL;

	long nb;
	unsigned int i;
	unsigned int j;

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
				if ((lexeme_p->nature != L_NOMBRE) && (lexeme_p->nature != L_SYMBOLE) && (lexeme_p->nature != L_PARENTHESE_OUVRANTE)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est ni un nombre, ni un registre");
				} else if (lexeme_p->nature == L_PARENTHESE_OUVRANTE) {
					instruction_p->operandes[1] = NULL;
					etat = I_B_BASE;
				} else {
					instruction_p->operandes[1] = lexeme_p;
					etat = I_B_PO;
				}
				break;
			case I_B_PO:
				if ((lexeme_p->nature != L_PARENTHESE_OUVRANTE) && (lexeme_p->nature != L_COMMENTAIRE) && (lexeme_p->nature != L_FIN_LIGNE)) {
					etat = ERREUR;
					strcpy(msg_err, "n'est pas une parenthèse ouvrante");
				} else if (lexeme_p->nature == L_COMMENTAIRE) {
					instruction_p->operandes[2] = NULL;
					etat = SUITE;
				} else if (lexeme_p->nature == L_FIN_LIGNE) {
					instruction_p->operandes[2] = NULL;
					etat = EOL;
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

		/* XXX tester le null, fin ligne, ... */
		if ((etat == EOL) && (instruction_p) && (def_p->type_ops == I_OP_B)) {
			if (!instruction_p->operandes[1]) {
				creer_lexeme(&(instruction_p->operandes[1]),"0", L_NOMBRE, instruction_p->ligne);
				ajouter_fin_liste(lexemes_supl_p,instruction_p->operandes[1]);
			}
			if (!instruction_p->operandes[2]) {
				creer_lexeme(&(instruction_p->operandes[2]),"$zero", L_REGISTRE, instruction_p->ligne);
				ajouter_fin_liste(lexemes_supl_p,instruction_p->operandes[2]);
			}
			if (instruction_p->operandes[1]->nature == L_SYMBOLE) {
				/* Insertion d'une instruction LUI $at, symbole */
				instr_supl_p = calloc (1, sizeof(*instr_supl_p));
				instr_supl_p->definition_p=donnee_table(table_def_instructions_p, "LUI");
				instr_supl_p->ligne = instruction_p->ligne;
				instr_supl_p->decalage = *decalage_p;
				instr_supl_p->source = lexeme_p->data;
				creer_lexeme(&(instr_supl_p->operandes[0]), "$at", L_REGISTRE, instruction_p->ligne);
				ajouter_fin_liste(lexemes_supl_p, instr_supl_p->operandes[0]);

				creer_lexeme(&(instr_supl_p->operandes[1]), instruction_p->operandes[1]->data, L_SYMBOLE, instruction_p->ligne);
				ajouter_fin_liste(lexemes_supl_p, instr_supl_p->operandes[1]);

				ajouter_fin_liste(liste_p, instr_supl_p);
				(*decalage_p)+=4;

				/* Insertion d'une instruction LW/SW registre, symbole($at) */
				instr_supl_p = calloc (1, sizeof(*instr_supl_p));
				instr_supl_p->definition_p = donnee_table(table_def_instructions_p, instruction_p->definition_p->nom);
				instr_supl_p->ligne = instruction_p->ligne;
				instr_supl_p->decalage = *decalage_p;
				creer_lexeme(&(instr_supl_p->operandes[0]), instruction_p->operandes[0]->data, L_REGISTRE, instruction_p->ligne);
				ajouter_fin_liste(lexemes_supl_p, instr_supl_p->operandes[0]);
				creer_lexeme(&(instr_supl_p->operandes[1]), instruction_p->operandes[1]->data, L_SYMBOLE, instruction_p->ligne);
				ajouter_fin_liste(lexemes_supl_p, instr_supl_p->operandes[1]);
				creer_lexeme(&(instr_supl_p->operandes[2]), "$at", L_REGISTRE, instruction_p->ligne);
				ajouter_fin_liste(lexemes_supl_p, instr_supl_p->operandes[2]);
				ajouter_fin_liste(liste_p, instr_supl_p);
				(*decalage_p)+=4;

				free(instruction_p);
				return SUCCESS;
			} else {
				instruction_p->source = lexeme_p->data;
				ajouter_fin_liste(liste_p, instruction_p);
				instruction_p=NULL;
				(*decalage_p)+=4;
				return SUCCESS;

			}


		} else if ((etat == EOL) && (instruction_p->definition_p->reloc == R_MIPS_PSEUDO)) {
			def_pseudo_p = donnee_table(table_def_pseudo_p, instruction_p->definition_p->nom);
			for (i=0;i<def_pseudo_p->nb_instruction;i++) {
				instr_supl_p = calloc (1, sizeof(*instr_supl_p));
				instr_supl_p->definition_p = donnee_table(table_def_instructions_p, def_pseudo_p->rempl[i].instruction);
				instr_supl_p->ligne = instruction_p->ligne;
				instr_supl_p->decalage = *decalage_p;
				if (i == 0)
					instr_supl_p->source = lexeme_p->data;

				for (j=0; j<3; j++) {
					if (def_pseudo_p->rempl[i].arg[j][0] != DQ_CHAR) {
						nb=strtol(def_pseudo_p->rempl[i].arg[j],NULL,0);
						if (nb>0) {
							instr_supl_p->operandes[j] = instruction_p->operandes[nb-1];
						} else {
							instr_supl_p->operandes[j] = NULL;
						}
					} else {
						creer_lexeme(&(instr_supl_p->operandes[j]), def_pseudo_p->rempl[i].arg[j]+1, (def_pseudo_p->rempl[i].arg[j][1] == '$' ? L_REGISTRE : L_NOMBRE), instruction_p->ligne);
						ajouter_fin_liste(lexemes_supl_p, instr_supl_p->operandes[j]);
						instr_supl_p->operandes[j]->data[strlen(instr_supl_p->operandes[j]->data)-1] = '\0';
					}
				}
				ajouter_fin_liste(liste_p, instr_supl_p);
				(*decalage_p)+=4;
			}
		free (instruction_p);
		return SUCCESS;
		} else if ((etat == EOL) && (SUCCESS == ajouter_fin_liste(liste_p, instruction_p))) {
			instruction_p->source = lexeme_p->data;

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
 * @param lignes_lexemes_p pointeur sur la liste des lexèmes
 * @param lexemes_supl_p pointeur sur la liste de lexèmes supplémentaires
 * @param table_def_instructions_p pointeur sur la table "dico" des instructions
 * @param table_def_pseudo_p pointeur sur la table "dico" des pseudos instructions
 * @param table_def_registres_p pointeur sur la table "dico" des registres
 * @param table_etiquettes_p pointeur sur la table des étiquettes
 * @param liste_text_p pointeur sur la liste des instructions de la section .text
 * @param liste_data_p pointeur sur la liste des données de la section .data
 * @param liste_bss_p pointeur sur la liste des réservations des .space de la section .bss
 * @return SUCCESS si l'analyse syntaxique s'est bien déroulée
 * @brief effectue l'analyse syntaxique de premier niveau d'une liste de lexemes
 *
 */
int analyser_syntaxe(
		struct Liste_s *lignes_lexemes_p,			/**< Pointeur sur la liste des lexèmes */
		struct Liste_s *lexemes_supl_p,				/**< Pointeur sur la liste de lexèmes supplémentaires */
		struct Table_s *table_def_instructions_p,	/**< Pointeur sur la table "dico" des instructions */
		struct Table_s *table_def_pseudo_p,			/**< Pointeur sur la table "dico" des pseudos instructions */
		struct Table_s *table_def_registres_p,		/**< Pointeur sur la table "dico" des registres */
		struct Table_s *table_etiquettes_p,			/**< Pointeur sur la table des étiquettes */
		struct Liste_s *liste_text_p,				/**< Pointeur sur la liste des instructions de la section .text */
		struct Liste_s *liste_data_p,				/**< Pointeur sur la liste des données de la section .data */
		struct Liste_s *liste_bss_p)				/**< Pointeur sur la liste des réservations des .space de la section .bss */
{
	uint32_t decalage_text=0;
	uint32_t decalage_data=0;
	uint32_t decalage_bss=0;

	char msg_err[2*STRLEN+1];

	struct Noeud_Liste_s *noeud_courant_p=NULL;
	struct Lexeme_s *lexeme_p=NULL;

	enum Section_e section=S_INIT;
	struct Etiquette_s **mem_etiq_table = NULL;
	size_t index_mem = 0;

	int code_erreur=SUCCESS;
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
						code_erreur = enregistrer_etiquette(lexeme_p, section, &decalage_text, table_etiquettes_p, NULL, msg_err);
					else if (section == S_DATA) {
						code_erreur = enregistrer_etiquette(lexeme_p, section, &decalage_data, table_etiquettes_p, mem_etiq_table+index_mem, msg_err);
						if ((code_erreur == SUCCESS) && (mem_etiq_table[index_mem]))
							index_mem++;
					}
					else if (section == S_BSS)
						code_erreur = enregistrer_etiquette(lexeme_p, section, &decalage_bss, table_etiquettes_p, NULL, msg_err);
					if (code_erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					}
					else if (code_erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_TEXT) && (lexeme_p->nature==L_INSTRUCTION)) {
					code_erreur = analyser_instruction(lignes_lexemes_p, lexemes_supl_p, table_def_instructions_p, table_def_pseudo_p, table_def_registres_p, liste_text_p, &decalage_text, msg_err);
					if (code_erreur == FAIL_ALLOC)
						return FAIL_ALLOC;
					else if (code_erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_BYTE]))) {
					for (index_mem = 0; mem_etiq_table[index_mem]; )
						mem_etiq_table[index_mem++] = NULL;
					index_mem = 0;
					code_erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (code_erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (code_erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_HALF]))) {
					aligner_decalage(&decalage_data, 1);
					for (index_mem = 0; mem_etiq_table[index_mem]; ) {
						mem_etiq_table[index_mem]->decalage = decalage_data;
						mem_etiq_table[index_mem++] = NULL;
					}
					index_mem = 0;
					code_erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (code_erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (code_erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_WORD]))) {
					aligner_decalage(&decalage_data, 2);
					for (index_mem = 0; mem_etiq_table[index_mem]; ) {
						mem_etiq_table[index_mem]->decalage = decalage_data;
						mem_etiq_table[index_mem++] = NULL;
					}
					index_mem = 0;
					code_erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (code_erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (code_erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_FLOAT]))) {
					aligner_decalage(&decalage_data, 2);
					for (index_mem = 0; mem_etiq_table[index_mem]; ) {
						mem_etiq_table[index_mem]->decalage = decalage_data;
						mem_etiq_table[index_mem++] = NULL;
					}
					index_mem = 0;
					code_erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (code_erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (code_erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_ASCIIZ]))) {
					for (index_mem = 0; mem_etiq_table[index_mem]; )
						mem_etiq_table[index_mem++] = NULL;
					index_mem = 0;
					code_erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (code_erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (code_erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_DATA) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE]))) {
					for (index_mem = 0; mem_etiq_table[index_mem]; )
						mem_etiq_table[index_mem++] = NULL;
					index_mem = 0;
					code_erreur = analyser_donnee(lignes_lexemes_p, liste_data_p, &decalage_data, msg_err);
					if (code_erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (code_erreur == SUCCESS)
						etat = INIT;
					else
						etat = ERREUR;
				} else if ((section==S_BSS) && (lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE]))) {
					code_erreur = analyser_donnee(lignes_lexemes_p, liste_bss_p, &decalage_bss, msg_err);
					if (code_erreur == FAIL_ALLOC) {
						free(mem_etiq_table);
						return FAIL_ALLOC;
					} else if (code_erreur == SUCCESS)
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
					fprintf(stderr, "%s :\n", msg_err);
					msg_err[0]='\0';
				} else
					ERROR_MSG("fin de liste de lexème innatendue");
				while ((noeud_courant_p = suivant_liste(lignes_lexemes_p)) && (lexeme_p = noeud_courant_p->donnee_p) && (lexeme_p->nature != L_FIN_LIGNE))
					; /* Passe tous les lexèmes jusqu'à la fin de ligne */
				if (lexeme_p)
					fprintf(stderr, "%s\n\n", lexeme_p->data);
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
