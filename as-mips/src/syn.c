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

enum Etat_Syn_e {
		MES_INIT,
		MES_FIN,
		MES_ERREUR,
		MES_EOL,
		MES_COMMENT,
		MES_SECTION,
		MES_OPTION,
		MES_ETIQUET,
		MES_DONNEE,
		MES_INSTRUC,
		MES_I_RN_3OP,
		MES_I_RN_V32,
		MES_I_RN_2OP,
		MES_I_RN_V21,
		MES_I_R_1OP,
		MES_I_N_OP,
		MES_I_B_REG,
		MES_I_B_VIR,
		MES_I_B_OFFS,
		MES_I_B_PO,
		MES_I_B_BASE,
		MES_I_B_PF
	};

const char *NOMS_SECTIONS[] = {"initial", ".text", ".data", ".bss"};
const char *NOMS_DATA[] = {".space", ".byte", ".word", ".asciiz"};

char *clefEtiquette(void *donnee_p)
{
	return (donnee_p ? ((struct Etiquette_s *)donnee_p)->lexeme_p->data : NULL);
}

void str_instruction(struct Instruction_s * instruction_p, char *str)
{
	sprintf(str, "%5d 0x%08x %s %s, %s, %s",
			instruction_p->ligne,
			instruction_p->decalage,
			instruction_p->definition_p->nom,
			instruction_p->operandes[0] ? instruction_p->operandes[0]->data : "NULL",
			instruction_p->operandes[1] ? instruction_p->operandes[1]->data : "NULL",
			instruction_p->operandes[2] ? instruction_p->operandes[2]->data : "NULL");
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

void mef_directive_section(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum Section_e *section_p,
		char *msg_err)
{
	enum Section_e i;

	if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err)) {
		if ((*lexeme_pp)->nature==L_DIRECTIVE) {
			for (i=S_TEXT; i<=S_BSS; i++) { /* on regarde si la directive correspond à un nom de section valide */
				if (strcmp((*lexeme_pp)->data, NOMS_SECTIONS[i])==0) {
					*section_p=i;
					INFO_MSG("La directive \"%s\" a été reconnue. Changement de nature de section pour %d : %s",
							(*lexeme_pp)->data, *section_p, NOMS_SECTIONS[*section_p]);
					mef_suivant(noeud_lexeme_pp, lexeme_pp);

					mef_commentaire(noeud_lexeme_pp, lexeme_pp, msg_err);
					break;
				}
			}
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

int enregistrer_etiquette(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum Section_e section,
		uint32_t *decalage_p,
		struct Table_s *tableEtiquettes_p,
		char *msg_err)
{

	const uint32_t masqueAlignement = 0x00000003; /* Les deux derniers bits doivent être à zéro pour avoir un aligement par mot de 32 bits */


	struct Etiquette_s *etiquetteCourante_p=malloc(sizeof(*etiquetteCourante_p));
	if (!etiquetteCourante_p) ERROR_MSG("Impossible de créer une nouvelle étiquette");

	if ((*decalage_p & masqueAlignement) && (section == S_DATA) && (suite_est_directive_word((*noeud_lexeme_pp)->suivant_p)))
		*decalage_p=(*decalage_p + masqueAlignement) & ~masqueAlignement;

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

void mef_etiquette(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum Section_e section,
		uint32_t *decalage_p,
		struct Table_s *tableEtiquettes_p,
		char *msg_err)
{

	const uint32_t masqueAlignement = 0x00000003; /* Les deux derniers bits doivent être à zéro pour avoir un aligement par mot de 32 bits */


	while (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && ((*lexeme_pp)->nature == L_ETIQUETTE)) {
		struct Etiquette_s *etiquetteCourante_p=malloc(sizeof(*etiquetteCourante_p));
		if (!etiquetteCourante_p) ERROR_MSG("Impossible de créer une nouvelle étiquette");

		if ((*decalage_p & masqueAlignement) && (section == S_DATA) && (suite_est_directive_word((*noeud_lexeme_pp)->suivant_p))) {
			*decalage_p=(*decalage_p + masqueAlignement) & ~masqueAlignement;
		}
		etiquetteCourante_p->lexeme_p=*lexeme_pp;
		etiquetteCourante_p->section=section;
		etiquetteCourante_p->decalage=*decalage_p;
		etiquetteCourante_p->ligne=(*lexeme_pp)->ligne;

		if (SUCCESS==ajouter_table(tableEtiquettes_p, etiquetteCourante_p)) {
			DEBUG_MSG("Insertion de l'étiquette %zu : %s au decalage %u", tableEtiquettes_p->nbElts, (*lexeme_pp)->data, *decalage_p);
		}
		else {
			sprintf(msg_err, "l'étiquette %s est déjà présente ligne %d", (*lexeme_pp)->data,
					((struct Etiquette_s *)donnee_table(tableEtiquettes_p, etiquetteCourante_p->lexeme_p->data))->ligne);
			free(etiquetteCourante_p);
			etiquetteCourante_p=NULL;
		}

		mef_suivant(noeud_lexeme_pp, lexeme_pp);
		mef_commentaire(noeud_lexeme_pp, lexeme_pp, msg_err);
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

void mef_section_text(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		uint32_t *decalage_p,
		struct Liste_s *liste_p,
		struct Table_s *tableEtiquettes_p,
		struct Table_s *tableDefinitionInstructions_p,
		char *msg_err)
{

	struct DefinitionInstruction_s *def_p;
	struct Instruction_s *instruction_p;
	int op_a_lire;
	int index_op;

	if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err)) {
		mef_etiquette(noeud_lexeme_pp, lexeme_pp, S_TEXT, decalage_p, tableEtiquettes_p, msg_err);
		if ((*lexeme_pp)->nature == L_INSTRUCTION) {
			def_p=(struct DefinitionInstruction_s *)donnee_table(tableDefinitionInstructions_p, (*lexeme_pp)->data);
			if ((!def_p) || (strcmp(def_p->nom, (*lexeme_pp)->data))) {
				sprintf(msg_err, "l'instruction \"%s\" est inconnue", (*lexeme_pp)->data);
			} else {
				INFO_MSG("Prise en compte de l'instruction %s à %d opérandes au décalage %d", def_p->nom, def_p->nb_ops, *decalage_p);

				instruction_p=calloc(1,sizeof(*instruction_p));
				instruction_p->definition_p=def_p;
				instruction_p->ligne=(*lexeme_pp)->ligne;
				instruction_p->decalage=*decalage_p;

				op_a_lire=def_p->nb_ops;
				index_op=instruction_p->definition_p->nb_ops-op_a_lire;
				mef_suivant(noeud_lexeme_pp, lexeme_pp);

				/* Vérification du nombre d'opérande */
				while (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && (op_a_lire > 0)) {
					/* cas où on ne peut pas avoir un registre */
					if (instruction_p->definition_p->type_ops==I_OP_B) { /* base offset */
						/* ici, on va verifier la syntaxe correcte pas lexeme de façon "brute" */
						op_a_lire=6;
						if (!mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && ((*lexeme_pp)->nature != L_REGISTRE)) { /* Registre */
							if ((*lexeme_pp)->nature == L_FIN_LIGNE)
								sprintf(msg_err, "il manque un opérande de type registre");
							else
								sprintf(msg_err, "l'opérande %s n'est pas un registre", (*lexeme_pp)->data);
							free(instruction_p);
							instruction_p=NULL;
							break;
						} else {
							instruction_p->operandes[0]=*lexeme_pp;
							op_a_lire--;
							mef_suivant(noeud_lexeme_pp, lexeme_pp);
						}
						if (!mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && ((*lexeme_pp)->nature != L_VIRGULE)) { /* Virgule */
							if ((*lexeme_pp)->nature == L_FIN_LIGNE)
								sprintf(msg_err, "il manque une virgule");
							else
								sprintf(msg_err, "\"%s\" à la place d'une virgule", (*lexeme_pp)->data);
							free(instruction_p);
							instruction_p=NULL;
							break;
						} else {
							op_a_lire--;
							mef_suivant(noeud_lexeme_pp, lexeme_pp);
						}
						if (!mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && (((*lexeme_pp)->nature != L_NOMBRE) && ((*lexeme_pp)->nature != L_SYMBOLE))) { /* Nombre ou Symbole */
							if ((*lexeme_pp)->nature == L_FIN_LIGNE)
								sprintf(msg_err, "il manque un opérande de type nombre ou symbole");
							else
								sprintf(msg_err, "l'opérande %s n'est pas un nombre ou un symbole", (*lexeme_pp)->data);
							free(instruction_p);
							instruction_p=NULL;
							break;
						} else {
							instruction_p->operandes[1]=*lexeme_pp;
							op_a_lire--;
							mef_suivant(noeud_lexeme_pp, lexeme_pp);
						}
						if (!mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && ((*lexeme_pp)->nature != L_PARENTHESE_OUVRANTE)) { /* ( */
							if ((*lexeme_pp)->nature == L_FIN_LIGNE)
								sprintf(msg_err, "il manque une parenthèse ouvrante");
							else
								sprintf(msg_err, "\"%s\" à la place d'une \"(\"", (*lexeme_pp)->data);
							free(instruction_p);
							instruction_p=NULL;
							break;
						} else {
							op_a_lire--;
							mef_suivant(noeud_lexeme_pp, lexeme_pp);
						}
						if (!mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && ((*lexeme_pp)->nature != L_REGISTRE)) { /* Registre */
							if ((*lexeme_pp)->nature == L_FIN_LIGNE)
								sprintf(msg_err, "il manque un opérande de type registre");
							else
								sprintf(msg_err, "l'opérande %s n'est pas un registre", (*lexeme_pp)->data);
							free(instruction_p);
							instruction_p=NULL;
							break;
						} else {
							instruction_p->operandes[2]=*lexeme_pp;
							op_a_lire--;
							mef_suivant(noeud_lexeme_pp, lexeme_pp);
						}
						if (!mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && ((*lexeme_pp)->nature != L_PARENTHESE_FERMANTE)) { /* ) */
							if ((*lexeme_pp)->nature == L_FIN_LIGNE)
								sprintf(msg_err, "il manque une parenthèse fermante");
							else
								sprintf(msg_err, "\"%s\" à la place d'une \")\"", (*lexeme_pp)->data);
							free(instruction_p);
							instruction_p=NULL;
							break;
						} else {
							op_a_lire--;
							mef_suivant(noeud_lexeme_pp, lexeme_pp);
						}
					} else { /* Instructions de type registre ou immediat */
						if ((instruction_p->definition_p->type_ops==I_OP_N) && (op_a_lire==1) && (((*lexeme_pp)->nature != L_NOMBRE) && ((*lexeme_pp)->nature != L_SYMBOLE))) {
							sprintf(msg_err, "l'opérande %s n'est ni un nombre, ni un symbole", (*lexeme_pp)->data);
							free(instruction_p);
							instruction_p=NULL;
							break;
						} else if ((((instruction_p->definition_p->type_ops==I_OP_N) && (op_a_lire>1)) || (instruction_p->definition_p->type_ops==I_OP_R))  && ((*lexeme_pp)->nature != L_REGISTRE)) {
							sprintf(msg_err, "l'opérande %s n'est pas un registre", (*lexeme_pp)->data);
							free(instruction_p);
							instruction_p=NULL;
							break;
						} else { /* l'opérande est donc correct */
							instruction_p->operandes[index_op]=*lexeme_pp;
							index_op++;
							op_a_lire--;
							mef_suivant(noeud_lexeme_pp, lexeme_pp);

							if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && (op_a_lire > 0) && ((*lexeme_pp)->nature != L_VIRGULE)) {
								sprintf(msg_err, "\"%s\" à la place d'une virgule", (*lexeme_pp)->data);
								free(instruction_p);
								instruction_p=NULL;
								break;
							} else if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err)) {
								mef_suivant(noeud_lexeme_pp, lexeme_pp);
							}
						}
					}
				}
				if (instruction_p && op_a_lire) {
					sprintf(msg_err, "il manque %d opérande(s)", op_a_lire);
					free(instruction_p);
					instruction_p=NULL;
				} else if (instruction_p && (((*lexeme_pp)->nature == L_COMMENTAIRE) || ((*lexeme_pp)->nature == L_FIN_LIGNE))) {
					char str[STRLEN];
					str_instruction(instruction_p, str);
					DEBUG_MSG("%s", str);
					ajouter_fin_liste(liste_p, instruction_p); /* XXX tester */
					(*decalage_p)+=4;
					mef_commentaire(noeud_lexeme_pp, lexeme_pp, msg_err);
				} else if (instruction_p) {
					sprintf(msg_err, "l'opérande %s n'est pas de type attendu", (*lexeme_pp)->data);
					free(instruction_p);
					instruction_p=NULL;
				}
			}
		}
	}
}

void mef_section_data_bss(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum Section_e section,
		uint32_t *decalage_p,
		struct Liste_s *liste_p,
		struct Table_s *tableEtiquettes_p,
		char *msg_err)
{

	struct Donnee_s *donnee_p;
	enum Nature_lexeme_e nature;
	enum Donnee_e typeDonnee;
	long int nombre;

	if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err)) {
		mef_etiquette(noeud_lexeme_pp, lexeme_pp, section, decalage_p, tableEtiquettes_p, msg_err);

		if ((*lexeme_pp)->nature == L_DIRECTIVE) {
			typeDonnee=D_UNDEF;
			if (!strcmp((*lexeme_pp)->data, NOMS_DATA[D_BYTE]))
				typeDonnee=D_BYTE;
			if (!strcmp((*lexeme_pp)->data, NOMS_DATA[D_WORD]))
				typeDonnee=D_WORD;
			if (!strcmp((*lexeme_pp)->data, NOMS_DATA[D_ASCIIZ]))
				typeDonnee=D_ASCIIZ;
			if (!strcmp((*lexeme_pp)->data, NOMS_DATA[D_SPACE]))
				typeDonnee=D_SPACE;

			if (typeDonnee != D_UNDEF) {
				mef_suivant(noeud_lexeme_pp, lexeme_pp); /* Passe au lexème suivant pour récupérer les arguments */

				while (	((*lexeme_pp) && ((nature=(*lexeme_pp)->nature) || TRUE)) &&
						((((typeDonnee==D_SPACE) || ((section==S_DATA) && (typeDonnee==D_BYTE))) && (nature==L_NOMBRE)) ||
						 ((section==S_DATA) && (typeDonnee==D_WORD) && ((nature==L_NOMBRE) || (nature==L_SYMBOLE))) ||
						 ((section==S_DATA) && ((typeDonnee==D_ASCIIZ)) && ((nature==L_CHAINE))))) {

					/* ajouter la donnée dans la liste ici */
					if (!(donnee_p=calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
					donnee_p->decalage=*decalage_p;
					donnee_p->type=typeDonnee;
					donnee_p->lexeme_p=(*lexeme_pp);
					donnee_p->ligne=(*lexeme_pp)->ligne;

					if ((*lexeme_pp)->nature != L_SYMBOLE) {
						if (typeDonnee!=D_ASCIIZ) {
							errno=0; /* remet le code d'erreur à 0 */
							nombre=strtol((*lexeme_pp)->data, NULL, 0); /* Convertit la chaine en nombre, avec base automatique */
							if (errno) {
								sprintf(msg_err, "conversion numérique de %s non valide", (*lexeme_pp)->data);
								free(donnee_p);
							} else {
								if (((typeDonnee==D_BYTE) && ((nombre>UINT8_MAX) || (nombre<INT8_MIN))) ||
									((typeDonnee==D_WORD) && ((nombre>UINT32_MAX) || (nombre<INT32_MIN))) ||
									((typeDonnee==D_SPACE) && (nombre<=0) && (nombre+*decalage_p>=UINT32_MAX))) {

									sprintf(msg_err, "le nombre %s est au delà des valeurs permises", (*lexeme_pp)->data);
									free(donnee_p);
								} else {
									if (typeDonnee==D_BYTE) {
										if (nombre>=0)
											donnee_p->valeur.octetNS=(uint8_t)nombre;
										else
											donnee_p->valeur.octet=(int8_t)nombre;
										(*decalage_p)++;
									} else if (typeDonnee==D_WORD) {
										if (nombre>=0)
											donnee_p->valeur.motNS=(uint32_t)nombre;
										else
											donnee_p->valeur.mot=(int32_t)nombre;
										(*decalage_p)+=4;
									} else { /* typeDonnee==D_SPACE */
										donnee_p->valeur.nbOctets=(uint32_t)nombre;
										(*decalage_p)+=donnee_p->valeur.nbOctets;
									}
									ajouter_fin_liste(liste_p, donnee_p);
								}
							}
						} else { /* On est sur une chaine ascii */
							/* XXX tester si la chaine n'est pas trop longue ... */
							donnee_p->valeur.chaine=NULL; /* XXX Ajouter le traitement des chaines de caractère. Nécessite une modification du parser lexical */
							(*decalage_p)+=1+strlen(donnee_p->valeur.chaine);
						}
					} else { /* C'est un symbole (possible uniquement sous .word, on ne peut pas encore calculer sa valeur mais on met quand même dans la liste*/
						DEBUG_MSG("Ajout d'un mot symbole \"%s\" au décalage %" SCNu32, (*lexeme_pp)->data, *decalage_p);
						ajouter_fin_liste(liste_p, donnee_p);
						(*decalage_p)+=4;
					}

					mef_suivant(noeud_lexeme_pp, lexeme_pp);
					if ((*lexeme_pp)->nature == L_VIRGULE)
						mef_suivant(noeud_lexeme_pp, lexeme_pp);
					else if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err) && ((*lexeme_pp)->nature != L_COMMENTAIRE)) {
						sprintf(msg_err, "Il manque une virgule avant %s", (*lexeme_pp)->data);
						break;
					}
				} /* fin du while des opérandes valides */
				mef_commentaire(noeud_lexeme_pp, lexeme_pp, msg_err);
			} else { /* Directive inconnue */
				if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err)) {
					sprintf(msg_err, "le terme %s est incorrect", (*lexeme_pp)->data);
				}
			}
		} /* Pas une directive */
		if (mef_valide(noeud_lexeme_pp, lexeme_pp, msg_err)) {
			sprintf(msg_err, "le terme \"%s\" est incorrect ici", (*lexeme_pp)->data);
		}
	}
}

enum Etat_Syn_e etat_comm_eol(struct Lexeme_s *lexeme_p, char *msg_err, char *msg)
{
	enum Etat_Syn_e etat;
	if (!lexeme_p) etat=MES_ERREUR;
	else if (lexeme_p->nature==L_FIN_LIGNE) etat=MES_EOL;
	else if (lexeme_p->nature==L_COMMENTAIRE) etat=MES_COMMENT;
	else {
		etat=MES_ERREUR;
		strcpy(msg_err, msg);
	}
	return etat;
}

enum Etat_Syn_e etat_sera_nombre_ou_symbole(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum Etat_Syn_e etat_normal_suiv,
		char *msg_err)
{
	enum Etat_Syn_e etat;
	mef_suivant(noeud_lexeme_pp, lexeme_pp);
	if (!(*lexeme_pp)) etat=MES_ERREUR;
	else if (((*lexeme_pp)->nature==L_NOMBRE) || ((*lexeme_pp)->nature==L_SYMBOLE)) etat=etat_normal_suiv;
	else {
		etat=MES_ERREUR;
		strcpy(msg_err, "n'est pas un registre");
	}
	return etat;
}

enum Etat_Syn_e etat_sera_registre(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		enum Etat_Syn_e etat_normal_suiv,
		char *msg_err)
{
	enum Etat_Syn_e etat;

	mef_suivant(noeud_lexeme_pp, lexeme_pp);
	if (!(*lexeme_pp)) etat=MES_ERREUR;
	else if ((*lexeme_pp)->nature==L_REGISTRE) etat=etat_normal_suiv;
	else {
		etat=MES_ERREUR;
		strcpy(msg_err, "n'est pas un registre");
	}
	return etat;
}

enum Etat_Syn_e etat_traitement_registre(
		struct NoeudListe_s **noeud_lexeme_pp,
		struct Lexeme_s **lexeme_pp,
		struct Table_s *def_reg_p,
		struct Instruction_s *instruction_p,
		int indice,
		enum Nature_lexeme_e nature_attendue,
		enum Etat_Syn_e etat_normal_suiv,
		char *msg_err,
		char *msg)
{
	enum Etat_Syn_e etat;
	if (!donnee_table(def_reg_p, (*lexeme_pp)->data)) {
		etat=MES_ERREUR;
		strcpy(msg_err, "n'est pas un registre valide");
	} else {
		instruction_p->operandes[indice]=*lexeme_pp;

		mef_suivant(noeud_lexeme_pp, lexeme_pp);
		if (!(*lexeme_pp)) etat=MES_ERREUR;
		else if ((*lexeme_pp)->nature==nature_attendue) etat=etat_normal_suiv;
		else {
			etat=MES_ERREUR;
			strcpy(msg_err, msg);
		}
	}
	return etat;
}


/**
 * @return Rien, si ce n'est les données mises à jour par les pointeurs paramètres associés
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

	uint32_t *decalage_p=NULL;
	struct Liste_s *liste_p=NULL;

	char msg_err[2*STRLEN];

	struct NoeudListe_s *noeud_lexeme_p=NULL;
	struct Lexeme_s *lexeme_p=NULL;

	enum Etat_Syn_e etat=MES_INIT;
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
		while ((lexeme_p) && (etat != MES_FIN)) {
			switch(etat) {
			case MES_INIT:
				if (!lexeme_p) etat=MES_ERREUR;
				else if (lexeme_p->nature==L_FIN_LIGNE) etat=MES_EOL;
				else if (lexeme_p->nature==L_COMMENTAIRE) etat=MES_COMMENT;
				else if ((lexeme_p->nature==L_DIRECTIVE) && ((!strcmp(lexeme_p->data, NOMS_SECTIONS[S_TEXT])) ||
					(!strcmp(lexeme_p->data, NOMS_SECTIONS[S_DATA])) || (!strcmp(lexeme_p->data, NOMS_SECTIONS[S_BSS]))))
					etat=MES_SECTION;
				else if ((section==S_INIT) && (lexeme_p->nature==L_DIRECTIVE) && (!strcmp(lexeme_p->data, "noreorder"))) etat=MES_OPTION;
				else if ((section!=S_INIT) && (lexeme_p->nature==L_ETIQUETTE)) etat=MES_ETIQUET;
				else if ((((section==S_BSS) || (section==S_DATA)) && (lexeme_p->nature==L_DIRECTIVE) &&((!strcmp(lexeme_p->data, NOMS_DATA[D_SPACE])))) ||
						(((section==S_DATA) && (lexeme_p->nature==L_DIRECTIVE) && ((!strcmp(lexeme_p->data, NOMS_DATA[D_BYTE])) ||
						(!strcmp(lexeme_p->data, NOMS_DATA[D_WORD])) || (!strcmp(lexeme_p->data, NOMS_DATA[D_ASCIIZ]))))))
					etat=MES_DONNEE;
				else if ((section==S_TEXT) && (lexeme_p->nature==L_INSTRUCTION)) etat=MES_INSTRUC;
				else {
					etat=MES_ERREUR;
					strcpy(msg_err, "n'est pas valide ici");
				}
				break;
			case MES_EOL:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=MES_FIN;
				else etat=MES_INIT;
				break;
			case MES_COMMENT:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=MES_ERREUR;
				else if (lexeme_p->nature==L_FIN_LIGNE) etat=MES_EOL;
				else {
					etat=MES_ERREUR;
					strcpy(msg_err, "ne devait pas se trouver après un commentaire");
				}
				break;
			case MES_SECTION:
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
			case MES_OPTION:

				break;
			case MES_ETIQUET:
				if (SUCCESS!=enregistrer_etiquette(&noeud_lexeme_p, &lexeme_p, section, decalage_p, table_etiquettes_p, msg_err))
					etat=MES_ERREUR;
				else {
					mef_suivant(&noeud_lexeme_p, &lexeme_p);
					if (!lexeme_p) etat=MES_ERREUR;
					else etat=MES_INIT;
				}
				break;
			case MES_DONNEE:
				break;
			case MES_INSTRUC:
				def_p=(struct DefinitionInstruction_s *)donnee_table(table_def_instructions_p, lexeme_p->data);
				if ((!def_p) || (strcmp(def_p->nom, lexeme_p->data))) {
					etat=MES_ERREUR;
					strcpy(msg_err, "n'est pas une instruction connue");
				} else {
					INFO_MSG("Prise en compte de l'instruction %s à %d opérandes au décalage %d", def_p->nom, def_p->nb_ops, *decalage_p);
					instruction_p=calloc(1,sizeof(*instruction_p));
					instruction_p->definition_p=def_p;
					instruction_p->ligne=lexeme_p->ligne;
					instruction_p->decalage=*decalage_p;

					if (def_p->nb_ops==0) {
						ajouter_fin_liste(liste_p, instruction_p);
						instruction_p=NULL; /* XXX il faudra tester l'insertion */
						(*decalage_p)+=4;
					}

					mef_suivant(&noeud_lexeme_p, &lexeme_p);
					if (!lexeme_p) etat=MES_ERREUR;
					else {
						if ((lexeme_p->nature!=L_REGISTRE) && (((def_p->type_ops==I_OP_R) && (def_p->nb_ops>0)) || ((def_p->type_ops==I_OP_N) && (def_p->nb_ops>1)) || (def_p->type_ops==I_OP_B))) {
							etat=MES_ERREUR;
							strcpy(msg_err, "n'est pas un registre");
						}
						else if (def_p->type_ops==I_OP_B) etat=MES_I_B_REG;
						else if (def_p->nb_ops==3) etat=MES_I_RN_3OP;
						else if (def_p->nb_ops==2) etat=MES_I_RN_2OP;
						else if ((def_p->nb_ops==1) && (def_p->type_ops==I_OP_R)) etat=MES_I_R_1OP;
						else if ((def_p->nb_ops==1) && (def_p->type_ops==I_OP_N)) etat=MES_I_N_OP;
						else if (def_p->nb_ops!=0) ERROR_MSG("Si on est là c'est qu'il y a un bug");
						else if (lexeme_p->nature == L_FIN_LIGNE) etat=MES_EOL;
						else if (lexeme_p->nature==L_COMMENTAIRE) etat=MES_COMMENT;
						else ERROR_MSG("Si on est là c'est qu'il y a un bug");
					}
				}
				break;
			case MES_I_RN_3OP:
				etat=etat_traitement_registre(&noeud_lexeme_p, &lexeme_p, table_def_registres_p, instruction_p, def_p->nb_ops-3, L_VIRGULE, MES_I_RN_V32, msg_err, "à la place d'une virgule");
				break;
			case MES_I_RN_V32:
				etat=etat_sera_registre(&noeud_lexeme_p, &lexeme_p, MES_I_RN_2OP, msg_err);
				break;
			case MES_I_RN_2OP:
				etat=etat_traitement_registre(&noeud_lexeme_p, &lexeme_p, table_def_registres_p, instruction_p, def_p->nb_ops-2, L_VIRGULE, MES_I_RN_V21, msg_err, "à la place d'une virgule");
				break;
			case MES_I_RN_V21:
				if (def_p->type_ops==I_OP_R) etat=etat_sera_registre(&noeud_lexeme_p, &lexeme_p, MES_I_R_1OP, msg_err);
				else etat=etat_sera_nombre_ou_symbole(&noeud_lexeme_p, &lexeme_p, MES_I_N_OP, msg_err);
				break;
			case MES_I_R_1OP:
				if (!donnee_table(table_def_registres_p, lexeme_p->data)) {
					etat=MES_ERREUR;
					strcpy(msg_err, "n'est pas un registre valide");
				} else {
					instruction_p->operandes[def_p->nb_ops-1]=lexeme_p;

					mef_suivant(&noeud_lexeme_p, &lexeme_p);
					etat=etat_comm_eol(lexeme_p, msg_err, "est en trop pour cette instruction");
					if (etat!=MES_ERREUR) {
						ajouter_fin_liste(liste_p, instruction_p);
						instruction_p=NULL; /* XXX il faudra tester l'insertion */
						(*decalage_p)+=4;
					}
				}
				break;
			case MES_I_N_OP:
				break;
			case MES_I_B_REG:
				etat=etat_traitement_registre(&noeud_lexeme_p, &lexeme_p, table_def_registres_p, instruction_p, 0, L_VIRGULE, MES_I_B_VIR, msg_err, "à la place d'une virgule");
				break;
			case MES_I_B_VIR:
				etat=etat_sera_nombre_ou_symbole(&noeud_lexeme_p, &lexeme_p, MES_I_B_OFFS, msg_err);
				break;
			case MES_I_B_OFFS:
				break;
			case MES_I_B_PO:
				etat=etat_sera_registre(&noeud_lexeme_p, &lexeme_p, MES_I_B_BASE, msg_err);
				break;
			case MES_I_B_BASE:
				etat=etat_traitement_registre(&noeud_lexeme_p, &lexeme_p, table_def_registres_p, instruction_p, 2, L_PARENTHESE_FERMANTE, MES_I_B_PF, msg_err, "à la place d'une parenthèse fermante");
				break;
			case MES_I_B_PF:
				mef_suivant(&noeud_lexeme_p, &lexeme_p);
				etat=etat_comm_eol(lexeme_p, msg_err, "est en trop pour cette instruction");
				break;
			case MES_ERREUR:
				if (lexeme_p) {
					fprintf(stderr, "Erreur de syntaxe ligne %d, ", lexeme_p ? lexeme_p->ligne : 0);
					fprintf(stderr, "\"%s\" ", !lexeme_p ? "pas de lexeme" : lexeme_p->data);
					fprintf(stderr, "%s.\n", msg_err); msg_err[0]='\0';
				} else ERROR_MSG("fin de liste de lexème innatendue");

				free(instruction_p);
				instruction_p=NULL;
				free(donnee_p);
				donnee_p=NULL;

				while ((lexeme_p) && (lexeme_p->nature!=L_FIN_LIGNE))
					mef_suivant(&noeud_lexeme_p, &lexeme_p);
				if (!lexeme_p) etat=MES_FIN;
				else etat=MES_EOL;

				resultat=FAILURE;
				break;
			default:
				etat=MES_ERREUR;
				strcpy(msg_err, "ne devrait pas être là");
			}
		}
	} else
		resultat=FAILURE;

	return resultat;
}
/**
 * @return Rien, si ce n'est les données mises à jour par les pointeurs paramètres associés
 * @brief effectue l'analyse syntaxique de premier niveau d'une liste de lexemes
 *
 */
void analyse_syntaxe(
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

	struct NoeudListe_s *noeud_lexeme_p=NULL;
	struct Lexeme_s *lexeme_p=NULL;

	enum Section_e section=S_INIT;

	if (lignes_lexemes_p) {
		msg_err[0]='\0';
		noeud_lexeme_p=lignes_lexemes_p->debut_liste_p;
		if (noeud_lexeme_p)
			lexeme_p=(struct Lexeme_s *)noeud_lexeme_p->donnee_p;
		else
			lexeme_p=NULL;

		while (noeud_lexeme_p) { /* Boucle sur la liste des lexèmes */
			mef_commentaire(&noeud_lexeme_p, &lexeme_p, msg_err);
			mef_directive_section(&noeud_lexeme_p, &lexeme_p, &section, msg_err);

			/* On va analyser la syntaxe en fonction de la section dans lequelle on se trouve */
			switch(section) {
			case S_INIT:
				mef_section_init(&noeud_lexeme_p, &lexeme_p, msg_err);
				break;

			case S_TEXT:
				mef_section_text(&noeud_lexeme_p, &lexeme_p, &decalage_text, liste_text_p, table_etiquettes_p, table_def_instructions_p, msg_err);
				break;

			case S_DATA:
				mef_section_data_bss(&noeud_lexeme_p, &lexeme_p, section, &decalage_data, liste_data_p, table_etiquettes_p, msg_err);
				break;

			case S_BSS:
				mef_section_data_bss(&noeud_lexeme_p, &lexeme_p, section, &decalage_bss, liste_bss_p, table_etiquettes_p, msg_err);
				break;

			default: ERROR_MSG("Cas non prévu, section inconnue");
			}

			if ('\0' != msg_err[0]) {
				fprintf(stderr, "Erreur de syntaxe ligne %d, %s.\n", lexeme_p->ligne, msg_err);
				msg_err[0]='\0';
			} else if (mef_valide(&noeud_lexeme_p, &lexeme_p, msg_err)) {
				WARNING_MSG("Erreur de syntaxe ligne %d, il manque un message d'erreur de syntaxe relatif au lexème (%s|%s)", (lexeme_p)->ligne, etat_lex_to_str((lexeme_p)->nature), (lexeme_p)->data);
			}
			while (mef_valide(&noeud_lexeme_p, &lexeme_p, msg_err)) /* va jusqu'en fin de ligne */
				mef_suivant(&noeud_lexeme_p, &lexeme_p);

			INFO_MSG("Fin de la ligne %d", lexeme_p->ligne);
			mef_suivant(&noeud_lexeme_p, &lexeme_p); /* puis on passe à la nouvelle ligne */
		}
	}
}

