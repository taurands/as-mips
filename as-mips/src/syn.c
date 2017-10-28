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

enum M_E_S_e {
		INIT,
		FIN,
		ERREUR,
		COMMENT,
		EOL,

		OPTION,
		NOREORD,

		SECTION,

		ETIQUET,
		INSTRUC,

		BYTE,
		WORD,
		ASCIIZ,
		SPACE,

		DONNEE_A,
		DONNEE_W,
		DONNEE_B,
		DONNEE_S,
		VIRG_A,
		VIRG_W,
		VIRG_B,
		VIRG_S,

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

		DONNEE
		};


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
	struct NoeudListe_s* noeud_liste_p=NULL;
	if (!liste_p) {
		printf("%s n'existe pas !\n", titre_liste);
	} else {
		if (!(liste_p->nbElements)) {
			printf("%s est vide\n", titre_liste);
		} else {
			printf("%s\n", titre_liste);
			for (noeud_liste_p=liste_p->debut_liste_p ; (noeud_liste_p) ; noeud_liste_p=noeud_liste_p->suivant_p) {
				affiche_element_databss((struct Donnee_s *)noeud_liste_p->donnee_p, table_p);
			}
		}
	}
	printf("\n\n");
}


void affiche_liste_instructions(struct Liste_s *liste_p, struct Table_s *table_p, char *titre_liste)
{
	struct NoeudListe_s* noeud_liste_p=NULL;
	if (!liste_p) {
		printf("%s n'existe pas !\n", titre_liste);
	} else {
		if (!(liste_p->nbElements)) {
			printf("%s est vide\n", titre_liste);
		} else {
			printf("%s\n", titre_liste);
			for (noeud_liste_p=liste_p->debut_liste_p ; (noeud_liste_p) ; noeud_liste_p=noeud_liste_p->suivant_p) {
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

void mef_suivant(struct NoeudListe_s **noeud_lexeme_pp, struct Lexeme_s **lexeme_pp)
{
	if (noeud_lexeme_pp) {
		if ((*noeud_lexeme_pp) && (lexeme_pp)) {
			*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p;
			if (*noeud_lexeme_pp)
				*lexeme_pp=(struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p);
			else
				*lexeme_pp=NULL;
		}
	}
}

int suite_est_directive_word(struct NoeudListe_s *noeud_lexeme_p)
{
	int resultat=FALSE;
	struct Lexeme_s *lexeme_p;

	while (noeud_lexeme_p) {
		lexeme_p=(struct Lexeme_s *)noeud_lexeme_p->donnee_p;

		if ((lexeme_p->nature == L_COMMENTAIRE) || (lexeme_p->nature == L_FIN_LIGNE) ||	(lexeme_p->nature == L_ETIQUETTE))
			noeud_lexeme_p=noeud_lexeme_p->suivant_p;
		else if ((lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_WORD]))) {
			resultat=TRUE;
			break;
		}
		else
			break;
	}
	return resultat;
}

void aligner_decalage(uint32_t *decalage_p)
{
	const uint32_t masqueAlignement = 0x00000003; /* Les deux derniers bits doivent être à zéro pour avoir un aligement par mot de 32 bits */
	if (*decalage_p & masqueAlignement)
		*decalage_p=(*decalage_p + masqueAlignement) & ~masqueAlignement;

}
int enregistrer_etiquette(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum Section_e section,
		uint32_t *decalage_p,
		struct Table_s *tableEtiquettes_p,
		char *msg_err)
{

	struct Etiquette_s *etiquetteCourante_p=malloc(sizeof(*etiquetteCourante_p));
	if (!etiquetteCourante_p) ERROR_MSG("Impossible de créer une nouvelle étiquette");

	if ((section == S_DATA) && (suite_est_directive_word((*noeud_lexeme_pp)->suivant_p)))
		aligner_decalage(decalage_p);

	etiquetteCourante_p->lexeme_p=*lexeme_pp;
	etiquetteCourante_p->section=section;
	etiquetteCourante_p->decalage=*decalage_p;
	etiquetteCourante_p->ligne=(*lexeme_pp)->ligne;

	if (SUCCESS==ajouter_table(tableEtiquettes_p, etiquetteCourante_p)) {
		DEBUG_MSG("Insertion de l'étiquette %zu : %s au decalage %u", tableEtiquettes_p->nbElts, (*lexeme_pp)->data, *decalage_p);
		return SUCCESS;
	} else {
		sprintf(msg_err, "est une étiquette déjà présente ligne %d", ((struct Etiquette_s *)donnee_table(tableEtiquettes_p, etiquetteCourante_p->lexeme_p->data))->ligne);
		free(etiquetteCourante_p); etiquetteCourante_p=NULL;
		return FAILURE;
	}
}


enum M_E_S_e etat_comm_eol(struct Lexeme_s *lexeme_p, char *msg_err, char *msg)
{
	enum M_E_S_e etat;
	if (!lexeme_p) etat=ERREUR;
	else if (lexeme_p->nature==L_FIN_LIGNE) etat=EOL;
	else if (lexeme_p->nature==L_COMMENTAIRE) etat=COMMENT;
	else {
		etat=ERREUR;
		strcpy(msg_err, msg);
	}
	return etat;
}

enum M_E_S_e etat_sera_nombre_ou_symbole(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum M_E_S_e etat_normal_suiv,
		char *msg_err)
{
	enum M_E_S_e etat;
	mef_suivant(noeud_lexeme_pp, lexeme_pp);
	if (!(*lexeme_pp)) etat=ERREUR;
	else if (((*lexeme_pp)->nature==L_NOMBRE) || ((*lexeme_pp)->nature==L_SYMBOLE)) etat=etat_normal_suiv;
	else {
		etat=ERREUR;
		strcpy(msg_err, "n'est pas un nombre ou un symbole");
	}
	return etat;
}

enum M_E_S_e etat_sera_registre(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum M_E_S_e etat_normal_suiv,
		char *msg_err)
{
	enum M_E_S_e etat;

	mef_suivant(noeud_lexeme_pp, lexeme_pp);
	if (!(*lexeme_pp)) etat=ERREUR;
	else if ((*lexeme_pp)->nature==L_REGISTRE) etat=etat_normal_suiv;
	else {
		etat=ERREUR;
		strcpy(msg_err, "n'est pas un registre");
	}
	return etat;
}

enum M_E_S_e etat_traitement_registre(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		struct Table_s *def_reg_p,
		struct Instruction_s *instruction_p,
		int indice,
		enum Nature_lexeme_e nature_attendue,
		enum M_E_S_e etat_normal_suiv,
		char *msg_err,
		char *msg)
{
	enum M_E_S_e etat;
	if (!donnee_table(def_reg_p, (*lexeme_pp)->data)) {
		etat=ERREUR;
		strcpy(msg_err, "n'est pas un registre valide");
	} else {
		instruction_p->operandes[indice]=*lexeme_pp;

		mef_suivant(noeud_lexeme_pp, lexeme_pp);
		if (!(*lexeme_pp)) etat=ERREUR;
		else if ((*lexeme_pp)->nature==nature_attendue) etat=etat_normal_suiv;
		else {
			etat=ERREUR;
			strcpy(msg_err, msg);
		}
	}
	return etat;
}

int mef_lire_nombre(
		enum M_E_S_e etat,
		struct Lexeme_s *lexeme_p,
		struct Donnee_s **donnee_pp,
		uint32_t *decalage_p,
		char *msg_err)
{
	long int nombre;

	(*donnee_pp)->decalage=*decalage_p;
	(*donnee_pp)->lexeme_p=lexeme_p;
	(*donnee_pp)->ligne=lexeme_p->ligne;

	errno=0; /* remet le code d'erreur à 0 */
	nombre=strtol(lexeme_p->data, NULL, 0); /* Convertit la chaine en nombre, avec base automatique */
	if (errno) {
		strcpy(msg_err, "n'a pas pu être évalué numériquement");
		free((*donnee_pp));
		*donnee_pp=NULL;
		return ERREUR;
	} else {
		if ((((*donnee_pp)->type==D_BYTE) && ((nombre>UINT8_MAX) || (nombre<INT8_MIN))) ||
			(((*donnee_pp)->type==D_WORD) && ((nombre>UINT32_MAX) || (nombre<INT32_MIN))) ||
			(((*donnee_pp)->type==D_SPACE) && (nombre<=0) && (nombre+*decalage_p>=UINT32_MAX))) {

			strcpy(msg_err, "est au delà des valeurs permises");
			free((*donnee_pp));
			*donnee_pp=NULL;
			return ERREUR;
		} else {
			if ((*donnee_pp)->type==D_BYTE) {
				if (nombre>=0)
					(*donnee_pp)->valeur.octetNS=(uint8_t)nombre;
				else
					(*donnee_pp)->valeur.octet=(int8_t)nombre;
				(*decalage_p)++;
			} else if ((*donnee_pp)->type==D_WORD) {
				if (nombre>=0)
					(*donnee_pp)->valeur.motNS=(uint32_t)nombre;
				else
					(*donnee_pp)->valeur.mot=(int32_t)nombre;
				(*decalage_p)+=4;
			} else { /* typeDonnee==D_SPACE */
				(*donnee_pp)->valeur.nbOctets=(uint32_t)nombre;
				(*decalage_p)+=(*donnee_pp)->valeur.nbOctets;
			}
		}
	}
	return etat;
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
		struct Liste_s *liste_text_p,				/**< Pointeur sur la liste des instructions de la section .text */
		uint32_t *decalage_p,
		struct Liste_s *liste_p,
		char *msg_err
		)
{
	struct NoeudListe_s *noeud_lexeme_p=NULL;
	struct Lexeme_s *lexeme_p=NULL;

	enum M_E_S_e etat=INIT;
	enum Section_e section=S_INIT;
	int resultat=SUCCESS;

	struct DefinitionInstruction_s *def_p=NULL;
	struct Instruction_s *instruction_p=NULL;

	return SUCCESS;
}

/**
 * @return Rien, si ce n'est les données mises à jour par les pointeurs paramètres associés
 * @brief effectue l'analyse syntaxique de premier niveau d'une liste de lexemes
 *
 *	@dot
 *	digraph Machine_Etat_Syn {
 *		concentrate=false;
 *		graph [label="\nMachine à états finis d'analyse syntaxique"; fontname = "arial"; fontsize = 16;];
 *		edge [fontname = "arial"; fontsize = 10;];
 *		node [shape="ellipse";  fontname = "arial"; fontsize = 10; color = "black";] INIT; FIN;
 *		node [shape="ellipse";  fontname = "arial"; fontsize = 10; color = "red";] ERREUR;
 *		node [shape="ellipse";  fontname = "arial"; fontsize = 10; color = "black";] OPTION; NOREORD;
 *		node [shape="tab";  fontname = "arial"; fontsize = 10; color = "black";] BYTE; WORD; ASCIIZ; SPACE; ETIQUET; INSTRUC; SECTION;
 *		node [shape="ellipse";  fontname = "arial"; fontsize = 10; color = "black";] DONNEE_A; DONNEE_W; DONNEE_B; DONNEE_S; VIRG_A; VIRG_W; VIRG_B; VIRG_S;
 *		node [shape="ellipse";  fontname = "arial"; fontsize = 10; color = "black";] COMMENT; EOL;
 *		node [shape="ellipse";  fontname = "arial"; fontsize = 10; color = "black";] I_RN_3OP; I_RN_V32; I_RN_2OP; I_RN_V21; I_R_1OP; I_N_OP;
 *		node [shape="ellipse";  fontname = "arial"; fontsize = 10; color = "black";] I_B_REG; I_B_VIR; I_B_OFFS; I_B_PO; I_B_BASE; I_B_PF;
 *		rankdir=LR; // de gauche vers la droite
 *
 *		INIT -> {rank=same INSTRUC BYTE WORD ASCIIZ SPACE ETIQUET SECTION OPTION} [style=invis]
 *		INSTRUC -> {rank=same I_B_REG I_RN_3OP DONNEE_A DONNEE_W DONNEE_B DONNEE_S NOREORD} [style=invis]
 *		I_B_REG -> {rank=same I_B_VIR I_RN_V32 VIRG_A VIRG_W VIRG_B VIRG_S} [style=invis]
 *		I_B_VIR -> {rank=same I_B_OFFS I_RN_2OP} [style=invis]
 *		I_B_OFFS -> {rank=same I_B_PO I_RN_V21} [style=invis]
 *		I_B_PO -> {rank=same I_B_BASE I_R_1OP I_N_OP} [style=invis]
 *		I_B_BASE -> {rank=same I_B_PF} [style=invis]
 *		I_B_PF -> {rank=same COMMENT} [style=invis]
 *		COMMENT -> {rank=same EOL } [style=invis]
 *		EOL -> {rank=same ERREUR } [style=invis]
 *		ERREUR -> {rank=same FIN } [style=invis]
 *
 *		INIT -> OPTION [label = "DIR .set"]
 *		INIT -> SECTION [label = "DIR .text ou .data ou .bss"]
 *		INIT -> BYTE [label = "(section=data) et (DIR .byte)"]
 *		INIT -> WORD [label = "(section=data) et (DIR .word)"]
 *		INIT -> ASCIIZ [label = "(section=data) et (DIR .asciiz)"]
 *		INIT -> SPACE [label = "(section=data ou bss) et (DIR .space)"]
 *		INIT -> INSTRUC [label = "(section=text) et (ETQ)"]
 *		INIT -> COMMENT [label = "COMMENT"]
 *		INIT -> EOL [label = "EOL"]
 *		INIT -> ERREUR [label = "sinon"]
 *
 *		OPTION -> NOREORD [label = "SYM noreoder"]
 *		OPTION -> COMMENT [label = "COMMENT"]
 *		OPTION -> EOL [label = "EOL"]
 *		OPTION -> ERREUR [label = "sinon"]
 *
 *		SECTION -> COMMENT [label = "COMMENT"]
 *		SECTION -> EOL [label = "EOL"]
 *		SECTION -> ERREUR [label = "sinon"]
 *
 *		BYTE -> DONNEE_B [label = "NB"]
 *		BYTE -> ERREUR [label = "sinon"]
 *
 *		DONNEE_B -> VIRG_B [label = "VIR"]
 *		DONNEE_B -> COMMENT [label = "COMMENT"]
 *		DONNEE_B -> EOL [label = "EOL"]
 *		DONNEE_B -> ERREUR [label = "si non valide ou autre"]
 *
 *		VIRG_B -> DONNEE_B [label = "NB"]
 *		VIRG_B -> ERREUR [label = "sinon"]
 *
 *		WORD -> DONNEE_W [label = "NB ou SYM"]
 *		WORD -> ERREUR [label = "sinon"]
 *
 *		DONNEE_W -> VIRG_W [label = "VIR"]
 *		DONNEE_W -> COMMENT [label = "COMMENT"]
 *		DONNEE_W -> EOL [label = "EOL"]
 *		DONNEE_W -> ERREUR [label = "si non valide ou autre"]
 *
 *		VIRG_W -> DONNEE_W [label = "NB ou SYM"]
 *		VIRG_W -> ERREUR [label = "sinon"]
 *
 *		ASCIIZ -> DONNEE_A [label = "NB"]
 *		ASCIIZ -> ERREUR [label = "sinon"]
 *
 *		DONNEE_A -> VIRG_A [label = "VIR"]
 *		DONNEE_A -> COMMENT [label = "COMMENT"]
 *		DONNEE_A -> EOL [label = "EOL"]
 *		DONNEE_A -> ERREUR [label = "si non valide ou autre"]
 *
 *		VIRG_A -> DONNEE_A [label = "NB"]
 *		VIRG_A -> ERREUR [label = "sinon"]
 *
 *		SPACE -> DONNEE_S [label = "NB"]
 *		SPACE -> ERREUR [label = "sinon"]
 *
 *		DONNEE_S -> VIRG_S [label = "VIR"]
 *		DONNEE_S -> COMMENT [label = "COMMENT"]
 *		DONNEE_S -> EOL [label = "EOL"]
 *		DONNEE_S -> ERREUR [label = "si non valide ou autre"]
 *
 *		VIRG_S -> DONNEE_S [label = "NB"]
 *		VIRG_S -> ERREUR [label = "sinon"]
 *
 *		INSTRUC -> ERREUR [label = "mauvaise instruction"]
 *		INSTRUC -> I_B_REG [label = "base offset"]
 *		INSTRUC -> I_RN_3OP [label = "3 ops"]
 *		INSTRUC -> I_RN_2OP [label = "2 ops"]
 *		INSTRUC -> I_R_1OP [label = "1 ops et R"]
 *		INSTRUC -> I_N_OP [label = "1 ops et I"]
 *		INSTRUC -> COMMENT [label = "0 ops et COMMENT"]
 *		INSTRUC -> EOL [label = "0 ops et EOL"]
 *		INSTRUC -> ERREUR [label = "sinon"]
 *
 *		I_B_REG -> ERREUR [label = "pas bon REG"]
 *		I_B_REG -> I_B_VIR [label = "VIR"]
 *		I_B_REG -> ERREUR [label = "sinon"]
 *
 *		I_B_VIR -> I_B_OFFS [label = "NB ou SYM"]
 *		I_B_VIR -> ERREUR [label = "sinon"]
 *
 *		I_B_OFFS -> I_B_PO [label = "PO"]
 *		I_B_OFFS -> ERREUR [label = "sinon"]
 *
 *		I_B_PO -> I_B_BASE [label = ""]
 *		I_B_PO -> ERREUR [label = "sinon"]
 *
 *		I_B_BASE -> ERREUR [label = "pas bon REG"]
 *		I_B_BASE -> I_B_PF [label = "PF"]
 *		I_B_BASE -> ERREUR [label = "sinon"]
 *
 *		I_B_PF -> I_B_VIR [label = ""]
 *		I_B_PF -> COMMENT [label = "COMMENT"]
 *		I_B_PF -> EOL [label = "EOL"]
 *		I_B_PF -> ERREUR [label = "sinon"]
 *
 *	}
 *  @enddot
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

	uint32_t *decalage_p=NULL;
	struct Liste_s *liste_p=NULL;

	char msg_err[2*STRLEN];

	struct NoeudListe_s *noeud_lexeme_p=NULL;
	struct Lexeme_s *lexeme_p=NULL;

	enum M_E_S_e etat=INIT;
	enum Section_e section=S_INIT;
	int resultat=SUCCESS;

	struct DefinitionInstruction_s *def_p=NULL;
	struct Instruction_s *instruction_p=NULL;
	struct Donnee_s *donnee_p=NULL;

	if (lignes_lexemes_p) {
		msg_err[0]='\0';
		noeud_lexeme_p=lignes_lexemes_p->debut_liste_p;
		if (noeud_lexeme_p)
			lexeme_p=(struct Lexeme_s *)noeud_lexeme_p->donnee_p;
		else
			lexeme_p=NULL;
		while ((lexeme_p) && (etat != FIN)) {
			switch(etat) {
			case ERREUR:
				if (lexeme_p) {
					fprintf(stderr, "Erreur de syntaxe ligne %d, ", lexeme_p ? lexeme_p->ligne : 0);
					fprintf(stderr, "%c[%d;%dm%s%c[%d;%dm ", 0x1B, STYLE_BOLD, COLOR_RED, (!(lexeme_p->data) ? "Fin_de_ligne" : lexeme_p->data),0x1B, STYLE_OFF, 0);
					fprintf(stderr, "%s.\n", msg_err); msg_err[0]='\0';
				} else ERROR_MSG("fin de liste de lexème inatendue");

				free(instruction_p);
				instruction_p=NULL;
				free(donnee_p);
				donnee_p=NULL;

				while ((lexeme_p) && (lexeme_p->nature!=L_FIN_LIGNE))
					mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=FIN;
				else etat=EOL;

				resultat=FAILURE;
				break;
			case INIT:
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature==L_FIN_LIGNE) etat=EOL;
				else if (lexeme_p->nature==L_COMMENTAIRE) etat=COMMENT;
				else if ((lexeme_p->nature==L_DIRECTIVE) && ((!strcmp(lexeme_p->data, NOMS_SECTIONS[S_TEXT])) ||
					(!strcmp(lexeme_p->data, NOMS_SECTIONS[S_DATA])) || (!strcmp(lexeme_p->data, NOMS_SECTIONS[S_BSS]))))
					etat=SECTION;
				else if ((section==S_INIT) && (lexeme_p->nature==L_DIRECTIVE) && (!strcmp(lexeme_p->data, ".set"))) etat=OPTION;
				else if ((section!=S_INIT) && (lexeme_p->nature==L_ETIQUETTE)) etat=ETIQUET;
				else if ((((section==S_BSS) || (section==S_DATA)) && (lexeme_p->nature==L_DIRECTIVE) &&((!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE])))) ||
						(((section==S_DATA) && (lexeme_p->nature==L_DIRECTIVE) && ((!strcmp(lexeme_p->data, NOMS_DATA[D_BYTE])) ||
						(!strcmp(lexeme_p->data, NOMS_DATA[D_WORD])) || (!strcmp(lexeme_p->data, NOMS_DATA[D_ASCIIZ]))))))
					etat=DONNEE;

				/* les4 suivants devraient pouvoir remplacés par le précédent */
				else if ((section==S_DATA) && (lexeme_p->nature==L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_ASCIIZ]))) {
					etat=DONNEE;
					WARNING_MSG("On ne devrait pas passer là");
				}
				else if ((section==S_DATA) && (lexeme_p->nature==L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_BYTE]))) {
					etat=DONNEE;
					WARNING_MSG("On ne devrait pas passer là");
				}
				else if ((section==S_DATA) && (lexeme_p->nature==L_DIRECTIVE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_WORD]))) {
					etat=DONNEE;
					WARNING_MSG("On ne devrait pas passer là");
				}
				else if (((section==S_BSS) || (section==S_DATA)) && (lexeme_p->nature==L_DIRECTIVE) && ((!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE]))))	{
					etat=DONNEE;
					WARNING_MSG("On ne devrait pas passer là");
				}

				else if ((section==S_TEXT) && (lexeme_p->nature==L_INSTRUCTION)) etat=INSTRUC;
				else {
					etat=ERREUR;
					strcpy(msg_err, "n'est pas valide ici");
				}
				break;
			case EOL:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=FIN;
				else etat=INIT;
				break;
			case COMMENT:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature==L_FIN_LIGNE) etat=EOL;
				else {
					etat=ERREUR;
					strcpy(msg_err, "ne devait pas se trouver après un commentaire");
				}
				break;
			case SECTION:
				if (!strcmp(lexeme_p->data, NOMS_SECTIONS[S_TEXT])) {
					section = S_TEXT;
					decalage_p=&decalage_text;
					liste_p=liste_text_p;
				} else if (!strcmp(lexeme_p->data, NOMS_SECTIONS[S_DATA])) {
					section = S_DATA;
					decalage_p=&decalage_data;
					liste_p=liste_data_p;
				} else if (!strcmp(lexeme_p->data, NOMS_SECTIONS[S_BSS])) {
					section = S_BSS;
					decalage_p=&decalage_bss;
					liste_p=liste_bss_p;
				} else ERROR_MSG("erreur automate : nom de section");

				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				etat=etat_comm_eol(lexeme_p, msg_err, "ne devrait pas être après la directive de changement de section");
				break;
			case OPTION:
				etat=ERREUR;
				strcpy(msg_err, "n'est pas encore traité (OPTION)");
				break;
			case ETIQUET:
				if (SUCCESS!=enregistrer_etiquette(&noeud_lexeme_p, &lexeme_p, section, decalage_p, table_etiquettes_p, msg_err))
					etat=ERREUR;
				else {
					mef_suivant(&noeud_lexeme_p, &lexeme_p);
					if (!lexeme_p) etat=ERREUR;
					else etat=INIT;
				}
				break;
			/*
			case DONNEE_B:
				if (!(donnee_p=calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
				donnee_p->type=D_BYTE;
				etat=mef_lire_nombre(etat, lexeme_p, &donnee_p, decalage_p, msg_err);
				if (etat!=ERREUR) mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature!=L_VIRGULE) etat=etat_comm_eol(lexeme_p, msg_err, "ne devrait pas être là, ou il manque une virgule");
				else etat=VIRG_B;

				if (etat!=ERREUR) {
					ajouter_fin_liste(liste_p, donnee_p);
					donnee_p=NULL;
				}
				break;
				*/

			case DONNEE_A: /* XXX Créer une fonction mef_lire_chaine? */
				if (!(donnee_p=calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
				donnee_p->decalage=*decalage_p;
				donnee_p->type=D_ASCIIZ;
				donnee_p->lexeme_p=lexeme_p;
				donnee_p->ligne=lexeme_p->ligne;
				donnee_p->valeur.chaine=NULL;
				/* il manque à récupérer un pointeur de chaine */

				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature!=L_VIRGULE) etat=etat_comm_eol(lexeme_p, msg_err, "ne devrait pas être là, ou il manque une virgule");
				else etat=VIRG_A;

				if (etat!=ERREUR) {
					ajouter_fin_liste(liste_p, donnee_p);
					(*decalage_p)+=(donnee_p->valeur.chaine ? 1+strlen(donnee_p->valeur.chaine) : 0);
					donnee_p=NULL; /* XXX il faudra tester l'insertion */
				}
				break;
			case VIRG_A:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature==L_CHAINE) etat=DONNEE_A;
				else {
					etat=ERREUR;
					strcpy(msg_err, "ne devrait pas être là");
				}
				break;

			case DONNEE_W:
				if (!(donnee_p=calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
				aligner_decalage(decalage_p);
				donnee_p->type=D_WORD;
				etat=mef_lire_nombre(etat, lexeme_p, &donnee_p, decalage_p, msg_err);
				if (etat!=ERREUR) mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature!=L_VIRGULE) etat=etat_comm_eol(lexeme_p, msg_err, "ne devrait pas être là, ou il manque une virgule");
				else etat=VIRG_W;

				if (etat!=ERREUR) {
					ajouter_fin_liste(liste_p, donnee_p);
					donnee_p=NULL; /* XXX il faudra tester l'insertion */
				}
				break;
			case VIRG_W:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if ((lexeme_p->nature==L_NOMBRE) || (lexeme_p->nature==L_SYMBOLE)) etat=DONNEE_W;
				else {
					etat=ERREUR;
					strcpy(msg_err, "ne devrait pas être là");
				}
				break;

			case DONNEE_B:
				if (!(donnee_p=calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
				donnee_p->type=D_BYTE;
				etat=mef_lire_nombre(etat, lexeme_p, &donnee_p, decalage_p, msg_err);
				if (etat!=ERREUR) mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature!=L_VIRGULE) etat=etat_comm_eol(lexeme_p, msg_err, "ne devrait pas être là, ou il manque une virgule");
				else etat=VIRG_B;

				if (etat!=ERREUR) {
					ajouter_fin_liste(liste_p, donnee_p);
					donnee_p=NULL; /* XXX il faudra tester l'insertion */
				}
				break;
			case VIRG_B:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature==L_NOMBRE) etat=DONNEE_B;
				else {
					etat=ERREUR;
					strcpy(msg_err, "ne devrait pas être là");
				}
				break;

			case DONNEE_S:
				if (!(donnee_p=calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
				donnee_p->type=D_SPACE;
				etat=mef_lire_nombre(etat, lexeme_p, &donnee_p, decalage_p, msg_err);
				if (etat!=ERREUR) mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature!=L_VIRGULE) etat=etat_comm_eol(lexeme_p, msg_err, "ne devrait pas être là, ou il manque une virgule");
				else etat=VIRG_S;

				if (etat!=ERREUR) {
					ajouter_fin_liste(liste_p, donnee_p);
					donnee_p=NULL; /* XXX il faudra tester l'insertion */
				}
				break;
			case VIRG_S:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature==L_NOMBRE) etat=DONNEE_S;
				else {
					etat=ERREUR;
					strcpy(msg_err, "ne devrait pas être là");
				}
				break;

			case DONNEE:
				if (!noeud_lexeme_p->suivant_p->donnee_p) etat=ERREUR;
				else if ((section==S_DATA) && (((struct Lexeme_s *)(noeud_lexeme_p->suivant_p->donnee_p))->nature==L_SYMBOLE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_WORD]))) etat=DONNEE_W;
				else if ((section==S_DATA) && (((struct Lexeme_s *)(noeud_lexeme_p->suivant_p->donnee_p))->nature==L_NOMBRE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_WORD]))) etat=DONNEE_W;
				else if ((section==S_DATA) && (((struct Lexeme_s *)(noeud_lexeme_p->suivant_p->donnee_p))->nature==L_NOMBRE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_ASCIIZ]))) etat=DONNEE_A;
				else if ((section==S_DATA) && (((struct Lexeme_s *)(noeud_lexeme_p->suivant_p->donnee_p))->nature==L_NOMBRE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_BYTE]))) etat=DONNEE_B;
				else if ((section==S_DATA) && (((struct Lexeme_s *)(noeud_lexeme_p->suivant_p->donnee_p))->nature==L_NOMBRE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE]))) etat=DONNEE_S;
				else if ((section==S_BSS)  && (((struct Lexeme_s *)(noeud_lexeme_p->suivant_p->donnee_p))->nature==L_NOMBRE) && (!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE]))) etat=DONNEE_S;
				else {
					etat=ERREUR;
					strcpy(msg_err, "ne devrait pas être là");
				}
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				break;

			case INSTRUC:
				def_p=(struct DefinitionInstruction_s *)donnee_table(table_def_instructions_p, lexeme_p->data);
				if ((!def_p) || (strcmp(def_p->nom, lexeme_p->data))) {
					etat=ERREUR;
					strcpy(msg_err, "n'est pas une instruction connue");
				} else {
					INFO_MSG("Prise en compte de l'instruction %s à %d opérandes au décalage %d", def_p->nom, def_p->nb_ops, *decalage_p);
					instruction_p=calloc(1,sizeof(*instruction_p));
					instruction_p->definition_p=def_p;
					instruction_p->ligne=lexeme_p->ligne;
					instruction_p->decalage=*decalage_p;

					mef_suivant(&noeud_lexeme_p, &lexeme_p);
					if (!lexeme_p) etat=ERREUR;
					else {
						if ((def_p->nb_ops==0) && ((lexeme_p->nature == L_FIN_LIGNE) || (lexeme_p->nature==L_COMMENTAIRE)))  {
							ajouter_fin_liste(liste_p, instruction_p);
							instruction_p=NULL; /* XXX il faudra tester l'insertion */
							(*decalage_p)+=4;
						}

						if ((lexeme_p->nature!=L_REGISTRE) && (((def_p->type_ops==I_OP_R) && (def_p->nb_ops>0)) || ((def_p->type_ops==I_OP_N) && (def_p->nb_ops>1)) || (def_p->type_ops==I_OP_B))) {
							etat=ERREUR;
							strcpy(msg_err, "n'est pas un registre");
						}
						else if (def_p->type_ops==I_OP_B)
							etat=I_B_REG;
						else if (def_p->nb_ops==3) etat=I_RN_3OP;
						else if (def_p->nb_ops==2) etat=I_RN_2OP;
						else if ((def_p->nb_ops==1) && (def_p->type_ops==I_OP_R)) etat=I_R_1OP;
						else if ((def_p->nb_ops==1) && (def_p->type_ops==I_OP_N)) etat=I_N_OP;
						else if (def_p->nb_ops!=0) ERROR_MSG("Si on est là c'est qu'il y a un bug");
						else if (lexeme_p->nature == L_FIN_LIGNE) etat=EOL;
						else if (lexeme_p->nature==L_COMMENTAIRE) etat=COMMENT;
						else if (def_p->nb_ops==0) {
							etat=ERREUR;
							strcpy(msg_err, "est en trop pour cette instruction");
						}
						else ERROR_MSG("Si on est là c'est qu'il y a un bug");
					}
				}
				break;
			case I_RN_3OP:
				etat=etat_traitement_registre(&noeud_lexeme_p, &lexeme_p, table_def_registres_p, instruction_p, def_p->nb_ops-3, L_VIRGULE, I_RN_V32, msg_err, "à la place d'une virgule");
				break;
			case I_RN_V32:
				etat=etat_sera_registre(&noeud_lexeme_p, &lexeme_p, I_RN_2OP, msg_err);
				break;
			case I_RN_2OP:
				etat=etat_traitement_registre(&noeud_lexeme_p, &lexeme_p, table_def_registres_p, instruction_p, def_p->nb_ops-2, L_VIRGULE, I_RN_V21, msg_err, "à la place d'une virgule");
				break;
			case I_RN_V21:
				if (def_p->type_ops==I_OP_R) etat=etat_sera_registre(&noeud_lexeme_p, &lexeme_p, I_R_1OP, msg_err);
				else etat=etat_sera_nombre_ou_symbole(&noeud_lexeme_p, &lexeme_p, I_N_OP, msg_err);
				break;
			case I_R_1OP:
				if (!donnee_table(table_def_registres_p, lexeme_p->data)) {
					etat=ERREUR;
					strcpy(msg_err, "n'est pas un registre valide");
				} else {
					instruction_p->operandes[def_p->nb_ops-1]=lexeme_p;

					mef_suivant(&noeud_lexeme_p, &lexeme_p);
					etat=etat_comm_eol(lexeme_p, msg_err, "est en trop pour cette instruction");
					if (etat!=ERREUR) {
						ajouter_fin_liste(liste_p, instruction_p);
						instruction_p=NULL; /* XXX il faudra tester l'insertion */
						(*decalage_p)+=4;
					}
				}
				break;
			case I_N_OP:
				instruction_p->operandes[def_p->nb_ops-1]=lexeme_p;

				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				etat=etat_comm_eol(lexeme_p, msg_err, "est en trop pour cette instruction");
				if (etat!=ERREUR) {
					ajouter_fin_liste(liste_p, instruction_p);
					instruction_p=NULL; /* XXX il faudra tester l'insertion */
					(*decalage_p)+=4;
				}
				break;
			case I_B_REG:
				etat=etat_traitement_registre(&noeud_lexeme_p, &lexeme_p, table_def_registres_p, instruction_p, 0, L_VIRGULE, I_B_VIR, msg_err, "à la place d'une virgule");
				break;
			case I_B_VIR:
				etat=etat_sera_nombre_ou_symbole(&noeud_lexeme_p, &lexeme_p, I_B_OFFS, msg_err);
				break;
			case I_B_OFFS:
				instruction_p->operandes[1]=lexeme_p;

				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=ERREUR;
				else if (lexeme_p->nature==L_PARENTHESE_OUVRANTE) etat=I_B_PO;
				else {
					etat=ERREUR;
					strcpy(msg_err, "à la place d'une parenthèse ouvrante");
				}
				break;
			case I_B_PO:
				etat=etat_sera_registre(&noeud_lexeme_p, &lexeme_p, I_B_BASE, msg_err);
				break;
			case I_B_BASE:
				etat=etat_traitement_registre(&noeud_lexeme_p, &lexeme_p, table_def_registres_p, instruction_p, 2, L_PARENTHESE_FERMANTE, I_B_PF, msg_err, "à la place d'une parenthèse fermante");
				break;
			case I_B_PF:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				etat=etat_comm_eol(lexeme_p, msg_err, "est en trop pour cette instruction");
				if (etat!=ERREUR) {
					ajouter_fin_liste(liste_p, instruction_p);
					instruction_p=NULL; /* XXX il faudra tester l'insertion */
					(*decalage_p)+=4;
				}
				break;
			default:
				etat=ERREUR;
				strcpy(msg_err, "ne devrait pas être là");
			}
		}
	} else
		resultat=FAILURE;

	return resultat;
}

/* les fonctions suivantes seront à supprimer */
void mef_suivant_sur(struct NoeudListe_s **noeud_lexeme_pp, struct Lexeme_s **lexeme_pp)
{
	if (noeud_lexeme_pp) {
		if ((*noeud_lexeme_pp) && (*noeud_lexeme_pp=((*noeud_lexeme_pp)->suivant_p)) && (lexeme_pp))
			*lexeme_pp=(struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p);
	}
}

int mef_valide(struct NoeudListe_s **noeud_lexeme_pp, struct Lexeme_s **lexeme_pp, char * msg_err)
{
	return (noeud_lexeme_pp) && (*noeud_lexeme_pp) && (lexeme_pp) && (*lexeme_pp) && ((!msg_err) || ('\0' == msg_err[0])) && ((*lexeme_pp)->nature != L_FIN_LIGNE);
}

void mef_commentaire(struct NoeudListe_s **noeud_lexeme_pp, struct Lexeme_s **lexeme_pp, char *msg_err)
{
	if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && ((*lexeme_pp)->nature == L_COMMENTAIRE)) {
		INFO_MSG("Passe le commentaire \"%s\"", (*lexeme_pp)->data);
		mef_suivant(noeud_lexeme_pp, lexeme_pp);
	}
}

void mef_section_init(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		char *msg_err)
{
	if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err)) {
		if (((*lexeme_pp)->nature == L_DIRECTIVE) && (0==strcmp((*lexeme_pp)->data, ".set"))) {
			INFO_MSG("La directive \".set\" a été reconnue dans la section initiale");
			mef_suivant(noeud_lexeme_pp, lexeme_pp);

			if (((*lexeme_pp)->nature==L_SYMBOLE) && (0==strcmp(strlwr((*lexeme_pp)->data), "noreorder"))) {
				INFO_MSG("le symbole \"noreoder\" est bien présent");
				/* on ne fait rien de plus dans notre cas c'est notre mode par défaut */
				mef_suivant(noeud_lexeme_pp, lexeme_pp);
				mef_commentaire(noeud_lexeme_pp, lexeme_pp, msg_err);
			} else if ((*lexeme_pp)->nature==L_SYMBOLE) {
				sprintf(msg_err, "\"%s\" n'est pas valide, seule l'option \"noreorder\" est acceptée", (*lexeme_pp)->data);
			} else {
				sprintf(msg_err, "il manque l'option \"noreorder\" après le \".set\"");
			}
		} else {
			sprintf(msg_err, "\"%s\" n'est pas valide, seule la directive \".set\" est autorisée dans cette section", (*lexeme_pp)->data);
		}
	}
}


