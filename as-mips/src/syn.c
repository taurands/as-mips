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

		if (insereElementTable(tableEtiquettes_p, etiquetteCourante_p)) {
			DEBUG_MSG("Insertion de l'étiquette %zu : %s au decalage %u", tableEtiquettes_p->nbElts, (*lexeme_pp)->data, *decalage_p);
		}
		else {
			sprintf(msg_err, "l'étiquette %s est déjà présente ligne %d", (*lexeme_pp)->data,
					((struct Etiquette_s *)donneeTable(tableEtiquettes_p, etiquetteCourante_p->lexeme_p->data))->ligne);
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
			def_p=(struct DefinitionInstruction_s *)donneeTable(tableDefinitionInstructions_p, (*lexeme_pp)->data);
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

void analyse_syntaxe(
		struct Liste_s *lignesLexemes_p,
		struct Table_s *tableDefinitionInstructions_p,
		struct Table_s *tableDefinitionRegistres_p,
		struct Table_s *tableEtiquettes_p,
		struct Liste_s *listeText_p,
		struct Liste_s *listeData_p,
		struct Liste_s *listeBss_p)
{

	uint32_t decalageText=0;
	uint32_t decalageData=0;
	uint32_t decalageBss=0;

	char msg_err[2*STRLEN];

	struct NoeudListe_s *noeud_lexeme_p=NULL;
	struct Lexeme_s *lexeme_p=NULL;

	enum Section_e section=S_INIT;

	if (lignesLexemes_p) {
		msg_err[0]='\0';
		noeud_lexeme_p=lignesLexemes_p->debut_liste_p;
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
				mef_section_text(&noeud_lexeme_p, &lexeme_p, &decalageText, listeText_p, tableEtiquettes_p, tableDefinitionInstructions_p, msg_err);
				break;

			case S_DATA:
				mef_section_data_bss(&noeud_lexeme_p, &lexeme_p, section, &decalageData, listeData_p, tableEtiquettes_p, msg_err);
				break;

			case S_BSS:
				mef_section_data_bss(&noeud_lexeme_p, &lexeme_p, section, &decalageBss, listeBss_p, tableEtiquettes_p, msg_err);
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

