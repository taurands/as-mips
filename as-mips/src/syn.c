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
#include <lex.h>
#include <syn.h>

const char *NOMS_SECTIONS[] = {"initial", ".text", ".data", ".bss"};
const char NATURE_INSTRUCTION[]= {'P', 'R', 'D', 'I', 'r', 'a'};
const char *NOMS_DATA[] = {".space", ".byte", ".word", ".asciiz"};

char *clefDefinitionInstruction(void *donnee_p)
{
	return (donnee_p ? ((struct DefinitionInstruction_s *)donnee_p)->nom : NULL);
}

char *clefEtiquette(void *donnee_p)
{
	return (donnee_p ? ((struct Etiquette_s *)donnee_p)->nom_p->data : NULL);
}

struct Dictionnaire_s *chargeDictionnaire(char *nomFichierDictionnaire)
{
	char *nomInstruction=calloc(128, sizeof(char));
	/* char carNature; */
	int nombreOperandes=0;
	int i=0;

	struct Dictionnaire_s* dictionnaireLu_p=calloc(1,sizeof(*dictionnaireLu_p));
	if (!dictionnaireLu_p) ERROR_MSG("Plus assez de mémoire pour créer un dictionnaire");

	FILE* f_p=fopen(nomFichierDictionnaire,"r"); /* Ouverture du dictionnaire d'instruction */
	if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");

	if (1!=fscanf(f_p,"%d",&(dictionnaireLu_p->nbMots))) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */

	dictionnaireLu_p->mots=calloc(dictionnaireLu_p->nbMots,sizeof(struct DefinitionInstruction_s));
	if (!dictionnaireLu_p->mots) ERROR_MSG("Plus assez de mémoire pour créer un dictionnaire");

	while (f_p && (i<dictionnaireLu_p->nbMots)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

		if (1 != fscanf(f_p,"%s", nomInstruction)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");
		if (1 != fscanf(f_p,"%d", &nombreOperandes)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");
		/* if (1 != fscanf(f_p,"%c", &carNature)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours"); */
		nomInstruction=strupr(nomInstruction);
		(*dictionnaireLu_p->mots)[i].nom=strdup(nomInstruction);
		(*dictionnaireLu_p->mots)[i].nbOperandes=nombreOperandes;
		if (i) if (strcmp((*dictionnaireLu_p->mots)[i-1].nom, (*dictionnaireLu_p->mots)[i].nom)>=0) ERROR_MSG("Le fichier dictionnaire d'instructions n'est par rangé par ordre alphabétique");
		i++;
	}
	fclose(f_p);

	/* Il faut traiter les erreurs correspondant à : */
	/* Un nombre d'instructions incorrect */
	/*  - une absence d'instruction */
	/*  - une absence de nombre de paramètre */
	/*  - les données doivent être dans l'ordre alphabétique pour permettre une recherche rapide dichotomique */

	free(nomInstruction);
	return dictionnaireLu_p;
}

void effaceContenuDictionnaire(struct Dictionnaire_s *unDictionnaire_p)
{
	int i;
	for (i=0;i<unDictionnaire_p->nbMots; i++)
		free((*unDictionnaire_p->mots)[i].nom);
	free(unDictionnaire_p->mots);
	unDictionnaire_p->mots=NULL;
	unDictionnaire_p->nbMots=0;
}

int indexDictionnaire(struct Dictionnaire_s *unDictionnaire_p, char *unMot)
{
	/* fonction de recherche dichotomique qui renvoie l'indice où se trouve unMot dans unDictionnaire_p */
	/* si elle est absente, renvoie -1 */

	/* initialisation de ces variables avant la boucle de recherche */
	int trouve=-1;							/* vaut -1 tant que la valeur n'aura pas été trouvée */
	int debut=0;  							/* indice de début */
	int fin=unDictionnaire_p->nbMots-1;  	/* indice de fin */

	int milieu;  							/* indice de "milieu" */
	int comp; 								/* résultat de la comparaison */

	while((trouve==-1) && (fin>=debut)) {
		milieu = (debut + fin)/2;  /* on détermine l'indice de milieu */
		comp=strcmp((*unDictionnaire_p->mots)[milieu].nom,unMot);
		if (comp>0)
			fin=milieu-1;
		else if (comp<0)
			debut=milieu+1;
		else
			trouve=milieu;
	}
	return trouve;
}

void mef_commentaire(struct NoeudListe_s **noeud_lexeme_pp)
{
	if ((*noeud_lexeme_pp) && ((struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p))->nature == L_COMMENTAIRE) {
		*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p;
		DEBUG_MSG("Passe le commentaire");
	}
}

void mef_erreur(struct NoeudListe_s **noeud_lexeme_pp)
{
	struct Lexeme_s *lexeme_p;
	while ((*noeud_lexeme_pp) && (lexeme_p=(struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p))->nature != L_FIN_LIGNE) {
		DEBUG_MSG("Ignore le lexème (%s|%s)", etat_lex_to_str(lexeme_p->nature), lexeme_p->data);
		*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p;
	}
}

void mef_directive_section(struct NoeudListe_s **noeud_lexeme_pp, enum Section_e *section_p)
{
	enum Section_e i;

	if (*noeud_lexeme_pp && ((struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p))->nature != L_FIN_LIGNE) {
		struct Lexeme_s *lexeme_p=(struct Lexeme_s *)(*noeud_lexeme_pp)->donnee_p;
		if (lexeme_p->nature==L_DIRECTIVE) {
			lexeme_p->data=strlwr(lexeme_p->data);

			for (i=S_TEXT; i<=S_BSS; i++) { /* on regarde si la directive coorespond à un nom de section */
				if (strcmp(lexeme_p->data, NOMS_SECTIONS[i])==0) {
					*section_p=i;
					DEBUG_MSG("La directive \"%s\" a été reconnue. Changement de nature de section pour %d : %s",
							lexeme_p->data, *section_p, NOMS_SECTIONS[*section_p]);
					*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p;
					mef_commentaire(noeud_lexeme_pp);
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

		if ((lexeme_p->nature == L_COMMENTAIRE) ||
			(lexeme_p->nature == L_FIN_LIGNE) ||
			(lexeme_p->nature == L_ETIQUETTE))

			noeud_lexeme_p=noeud_lexeme_p->suivant_p;

		else if ((lexeme_p->nature == L_DIRECTIVE) && (!strcmp(lexeme_p->data, ".word"))) {
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
		enum Section_e section,
		uint32_t *decalage_p,
		struct Table_s *tableEtiquettes_p)
{

	const uint32_t masqueAlignement = 0x00000003; /* Les deux derniers bits doivent être à zéro pour avoir un aligement par mot de 32 bits */

	struct Lexeme_s *lexeme_p;

	while (*noeud_lexeme_pp && (lexeme_p=(struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p))->nature == L_ETIQUETTE) {
		struct Etiquette_s *etiquetteCourante_p=malloc(sizeof(*etiquetteCourante_p));
		if (!etiquetteCourante_p) ERROR_MSG("Impossible de créer une nouvelle étiquette");

		if ((*decalage_p & masqueAlignement) && (section == S_DATA) && (suite_est_directive_word((*noeud_lexeme_pp)->suivant_p))) {
			*decalage_p=(*decalage_p + masqueAlignement) & ~masqueAlignement;
		}
		etiquetteCourante_p->nom_p=lexeme_p;
		etiquetteCourante_p->section=section;
		etiquetteCourante_p->decalage=*decalage_p;
		etiquetteCourante_p->ligneSource=lexeme_p->ligne;

		if (insereElementTable(tableEtiquettes_p, etiquetteCourante_p)) {
			DEBUG_MSG("Insertion de l'étiquette %zu : %s au decalage %u", tableEtiquettes_p->nbElts, lexeme_p->data, *decalage_p);
		}
		else {
			DEBUG_MSG("Erreur, l'étiquette %s était déjà présente", lexeme_p->data);
			free(etiquetteCourante_p);
		}

		*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p; /* On passe au lexème suivant */
	}
}

void mef_section_init(
		struct NoeudListe_s **noeud_lexeme_pp,
		enum Section_e *section_p)
{

	mef_directive_section(noeud_lexeme_pp, section_p);

	if (*noeud_lexeme_pp && ((struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p))->nature != L_FIN_LIGNE) {
		struct Lexeme_s *lexeme_p=(struct Lexeme_s *)(*noeud_lexeme_pp)->donnee_p;
		if (lexeme_p->nature==L_DIRECTIVE) {
			lexeme_p->data=strlwr(lexeme_p->data);
			if (strcmp(lexeme_p->data, ".set")==0) {
				DEBUG_MSG("La directive \".set\" a été reconnue dans la section initiale");
				*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p;
				lexeme_p=(struct Lexeme_s *)(*noeud_lexeme_pp)->donnee_p;
				if (lexeme_p->nature==L_SYMBOLE) {
					if (strcmp(strlwr(lexeme_p->data), "noreorder")==0) {
						DEBUG_MSG("le symbole \"noreoder\" est bien présent");
						*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p;
					}
					else { /* Le symbole n'est pas "noreorder" */
						DEBUG_MSG("Symbole inconnu après la directive \".set\" dans la section initiale");
					}
				}
				else { /* Le lexème n'est pas un symbole */
					DEBUG_MSG("Pas de symbole après la directive \".set\" dans la section initiale");
				}
			}
		}
		mef_commentaire(noeud_lexeme_pp);
	}
}

void mef_section_text(
		struct NoeudListe_s **noeud_lexeme_pp,
		enum Section_e *section_p,
		uint32_t *decalage_p,
		struct Liste_s *liste_p,
		struct Table_s *tableEtiquettes_p,
		struct Dictionnaire_s *dictionnaireInstructions_p)
{

	mef_directive_section(noeud_lexeme_pp, section_p);

	if (*noeud_lexeme_pp && ((struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p))->nature != L_FIN_LIGNE) {

	}
}

void mef_section_data_bss(
		struct NoeudListe_s **noeud_lexeme_pp,
		enum Section_e *section_p,
		uint32_t *decalage_p,
		struct Liste_s *liste_p,
		struct Table_s *tableEtiquettes_p)
{

	struct Lexeme_s *lexeme_p=NULL;
	struct Donnee_s *donnee_p;
	enum Nature_lexeme_e nature;
	enum Donnee_e typeDonnee;
	long int nombre;


	mef_directive_section(noeud_lexeme_pp, section_p);

	if (*noeud_lexeme_pp && (lexeme_p=(struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p))->nature != L_FIN_LIGNE) {
		mef_etiquette(noeud_lexeme_pp, *section_p, decalage_p, tableEtiquettes_p);

		if (lexeme_p->nature == L_DIRECTIVE) {
			typeDonnee=D_UNDEF;
			if (!strcmp(lexeme_p->data, ".byte"))
				typeDonnee=D_BYTE;
			if (!strcmp(lexeme_p->data, ".word"))
				typeDonnee=D_WORD;
			if (!strcmp(lexeme_p->data, ".asciiz"))
				typeDonnee=D_ASCIIZ;
			if (!strcmp(lexeme_p->data, ".space"))
				typeDonnee=D_SPACE;

			if (typeDonnee != D_UNDEF) {
				*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p; /* Passe au lexème suivant pour récupérer les arguments */

				while (	((lexeme_p=(struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p)) && ((nature=lexeme_p->nature) || TRUE)) &&
						((((typeDonnee==D_SPACE) || ((*section_p==S_DATA) && (typeDonnee==D_BYTE))) && ((nature==L_NOMBRE_DECIMAL) || (nature==L_NOMBRE_HEXADECIMAL) ||	(nature==L_NOMBRE_OCTAL))) ||
						 ((*section_p==S_DATA) && (typeDonnee==D_WORD) && ((nature==L_NOMBRE_DECIMAL) || (nature==L_NOMBRE_HEXADECIMAL) || (nature==L_NOMBRE_OCTAL) || (nature==L_SYMBOLE))) ||
						 ((*section_p==S_DATA) && ((typeDonnee==D_ASCIIZ)) && ((nature==L_CHAINE))
						)
						)) {

					/* ajouter la donnée dans la liste ici */
					if (!(donnee_p=calloc(1, sizeof(*donnee_p)))) ERROR_MSG("Impossible de créer une donnée");
					donnee_p->decalage=*decalage_p;
					donnee_p->type=typeDonnee;
					donnee_p->nom_p=lexeme_p;
					donnee_p->ligneSource=lexeme_p->ligne;

					if (lexeme_p->nature != L_SYMBOLE) {
						if (typeDonnee!=D_ASCIIZ) {
							errno=0; /* remet le code d'erreur à 0 */
							nombre=strtol(lexeme_p->data, NULL, 0); /* Convertit la chaine en nombre, avec base automatique */
							if (errno) {
								DEBUG_MSG("Erreur de conversion numérique");
								free(donnee_p);
							} else {
								if ((typeDonnee==D_BYTE) && (nombre>=0) && (nombre<=UINT8_MAX)) {
									donnee_p->valeur.octetNS=(uint8_t)nombre;
									DEBUG_MSG("Ajout d'un byte non signé (%s=%ld) de valeur %" SCNu8 " au décalage %" SCNu32, lexeme_p->data, nombre, donnee_p->valeur.octetNS, *decalage_p);
									ajouter_fin_liste(liste_p, donnee_p);
									(*decalage_p)++;
								} else if ((typeDonnee==D_BYTE) && (nombre<0) && (nombre>=INT8_MIN)) {
									donnee_p->valeur.octet=(int8_t)nombre;
									DEBUG_MSG("Ajout d'un byte signé (%s=%ld) de valeur %" SCNi8 " au décalage %" SCNu32, lexeme_p->data, nombre, donnee_p->valeur.octet, *decalage_p);
									ajouter_fin_liste(liste_p, donnee_p);
									(*decalage_p)++;
								} else if ((typeDonnee==D_WORD) && (nombre>=0) && (nombre<=UINT32_MAX)) {
									donnee_p->valeur.motNS=(uint32_t)nombre;
									DEBUG_MSG("Ajout d'un mot non signé (%s=%ld) de valeur %" SCNu32 " au décalage %" SCNu32, lexeme_p->data, nombre, donnee_p->valeur.motNS, *decalage_p);
									ajouter_fin_liste(liste_p, donnee_p);
									(*decalage_p)+=4;
								} else if ((typeDonnee==D_WORD) && (nombre<0) && (nombre>=INT32_MIN)) {
									donnee_p->valeur.mot=(int32_t)nombre;
									DEBUG_MSG("Ajout d'un mot signé (%s=%ld) de valeur %" SCNi32 " au décalage %" SCNu32, lexeme_p->data, nombre, donnee_p->valeur.mot, *decalage_p);
									ajouter_fin_liste(liste_p, donnee_p);
									(*decalage_p)+=4;
								} else if ((typeDonnee==D_SPACE) && (nombre>=0) && (nombre+*decalage_p<=UINT32_MAX)) {
									donnee_p->valeur.nbOctets=(uint32_t)nombre;
									DEBUG_MSG("Ajout d'un espace de (%s=%ld) %" SCNu32 " octets au décalage %" SCNu32, lexeme_p->data, nombre, donnee_p->valeur.nbOctets, *decalage_p);
									ajouter_fin_liste(liste_p, donnee_p);
									(*decalage_p)+=donnee_p->valeur.nbOctets;
								} else {
									DEBUG_MSG("Format numérique hors limites (errno=%d)", errno); /* XXX A compléter*/
									free(donnee_p);
								}
							}
						} else { /* On est sur une chaine ascii */

						}
					} else { /* C'est un symbole, on ne peut pas encore calculer sa valeur mais on met quand même dans la liste*/
						DEBUG_MSG("Ajout d'un mot symbole %s au décalage %" SCNu32, lexeme_p->data, *decalage_p);
						ajouter_fin_liste(liste_p, donnee_p);
						(*decalage_p)+=4;
					}

					*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p; /* Passe au lexème suivant pour récupérer les arguments */
					if (((struct Lexeme_s *)((*noeud_lexeme_pp)->donnee_p))->nature == L_VIRGULE)
						*noeud_lexeme_pp=(*noeud_lexeme_pp)->suivant_p;
					else
						break;
				}

				mef_commentaire(noeud_lexeme_pp);
			} else {

			}
		}
	}
}

void analyse_syntaxe(
		struct Liste_s *lignesLexemes_p,
		struct Dictionnaire_s *dictionnaireInstructions_p,
		struct Table_s *tableEtiquettes_p,
		struct Liste_s *listeText_p,
		struct Liste_s *listeData_p,
		struct Liste_s *listeBss_p)
{

	uint32_t decalageText=0;
	uint32_t decalageData=0;
	uint32_t decalageBss=0;

	struct NoeudListe_s *noeud_lexeme_p=NULL;

	enum Section_e section=S_INIT;

	if (lignesLexemes_p) {
		noeud_lexeme_p=lignesLexemes_p->debut_liste_p;
		while (noeud_lexeme_p) { /* Boucle sur la liste des lexèmes */

			/* On va analyser la syntaxe en fonction de la section dans lequelle on se trouve */
			switch(section) {
			case S_INIT:
				mef_section_init(&noeud_lexeme_p, &section);
				break;

			case S_TEXT:
				mef_section_text(&noeud_lexeme_p, &section, &decalageText, listeText_p, tableEtiquettes_p, dictionnaireInstructions_p);
				break;

			case S_DATA:
				mef_section_data_bss(&noeud_lexeme_p, &section, &decalageData, listeData_p, tableEtiquettes_p);
				break;

			case S_BSS:
				mef_section_data_bss(&noeud_lexeme_p, &section, &decalageBss, listeBss_p, tableEtiquettes_p);
				break;

			default: ERROR_MSG("Cas non prévu, section inconnue");
			}

			if (((struct Lexeme_s *)(noeud_lexeme_p->donnee_p))->nature != L_FIN_LIGNE) {
				DEBUG_MSG("Ignore les lexèmes suivants non traités");
				mef_erreur(&noeud_lexeme_p); /* On s'assure de bien être en fin de ligne */
			}

			DEBUG_MSG("Passage à la ligne suivante");
			noeud_lexeme_p=noeud_lexeme_p->suivant_p; /* puis on passe à la nouvelle */
		}
	}
}

