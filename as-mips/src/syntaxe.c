/**
 * @file syntaxe.c
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Definition des fonctions liées au traitement syntaxique du fichier
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <syntaxe.h>
#include <str_utils.h>

const char *NOMS_SECTIONS[] = {"initial", ".text", ".data", ".bss"};
const char NATURE_INSTRUCTION[]= {'P', 'R', 'D', 'I', 'r', 'a'};
const char *NOMS_DATA[] = {".space", ".byte", ".word", ".asciiz"};

char *clefDefinitionInstruction(void *donnee_p) {
	return (donnee_p ? ((DefinitionInstruction_t *)donnee_p)->nom : NULL);
}

char *clefEtiquette(void *donnee_p) {
	return (donnee_p ? ((Etiquette_t *)donnee_p)->nom_p->data : NULL);
}

Dictionnaire_t *chargeDictionnaire(char *nomFichierDictionnaire) {
	char *nomInstruction=calloc(128, sizeof(char));
	/* char carNature; */
	int nombreOperandes=0;
	int i=0;

	Dictionnaire_t* dictionnaireLu_p=calloc(1,sizeof(*dictionnaireLu_p));
	if (!dictionnaireLu_p) ERROR_MSG("Plus assez de mémoire pour créer un dictionnaire");

	FILE* f_p=fopen(nomFichierDictionnaire,"r"); /* Ouverture du dictionnaire d'instruction */
	if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");

	if (1!=fscanf(f_p,"%d",&(dictionnaireLu_p->nbMots))) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */

	dictionnaireLu_p->mots=calloc(dictionnaireLu_p->nbMots,sizeof(DefinitionInstruction_t));
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

void effaceContenuDictionnaire(Dictionnaire_t *unDictionnaire_p) {
	int i;
	for (i=0;i<unDictionnaire_p->nbMots; i++)
		free((*unDictionnaire_p->mots)[i].nom);
	free(unDictionnaire_p->mots);
	unDictionnaire_p->mots=NULL;
	unDictionnaire_p->nbMots=0;
}

int indexDictionnaire(Dictionnaire_t *unDictionnaire_p, char *unMot) {
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

void analyseSyntaxeSauteCommentaire(ElementListe_t **elementListeLexeme_pp) {
	if ((*elementListeLexeme_pp) && ((Lexeme_t *)((*elementListeLexeme_pp)->donnees_p))->nature == L_COMMENTAIRE) {
		*elementListeLexeme_pp=(*elementListeLexeme_pp)->suivant_p;
		DEBUG_MSG("Saute le commentaire");
	}
}

void analyseSyntaxeVaFinLigne(ElementListe_t **elementListeLexeme_pp) {
	while ((*elementListeLexeme_pp) && ((Lexeme_t *)((*elementListeLexeme_pp)->donnees_p))->nature != L_FIN_LIGNE) {
		DEBUG_MSG("Saute le lexème %s", ((Lexeme_t *)(*elementListeLexeme_pp)->donnees_p)->data);
		*elementListeLexeme_pp=(*elementListeLexeme_pp)->suivant_p;
	}
}

void analyseSyntaxeSection(ElementListe_t **elementListeLexeme_pp, enum Section_e *section_p) {
	enum Section_e i;
	if (*elementListeLexeme_pp && ((Lexeme_t *)((*elementListeLexeme_pp)->donnees_p))->nature != L_FIN_LIGNE) {
		Lexeme_t *lexeme_p=(Lexeme_t *)(*elementListeLexeme_pp)->donnees_p;
		if (lexeme_p->nature==L_DIRECTIVE) {
			lexeme_p->data=strlwr(lexeme_p->data);

			for (i=S_TEXT; i<=S_BSS; i++) { /* on regarde si la directive coorespond à un nom de section */
				if (strcmp(lexeme_p->data, NOMS_SECTIONS[i])==0) {
					*section_p=i;
					DEBUG_MSG("La directive \"%s\" a été reconnue. Changement de nature de section pour %d : %s",
							lexeme_p->data, *section_p, NOMS_SECTIONS[*section_p]);
					*elementListeLexeme_pp=(*elementListeLexeme_pp)->suivant_p;
					analyseSyntaxeSauteCommentaire(elementListeLexeme_pp);
					break;
				}
			}
		}
	}
}

void analyseSyntaxeInit(
		ElementListe_t **elementListeLexeme_pp,
		enum Section_e *section_p) {

	analyseSyntaxeSection(elementListeLexeme_pp, section_p);
	if (*elementListeLexeme_pp && ((Lexeme_t *)((*elementListeLexeme_pp)->donnees_p))->nature != L_FIN_LIGNE) {
		Lexeme_t *lexeme_p=(Lexeme_t *)(*elementListeLexeme_pp)->donnees_p;
		if (lexeme_p->nature==L_DIRECTIVE) {
			lexeme_p->data=strlwr(lexeme_p->data);
			if (strcmp(lexeme_p->data, ".set")==0) {
				DEBUG_MSG("La directive \".set\" a été reconnue dans la section initiale");
				*elementListeLexeme_pp=(*elementListeLexeme_pp)->suivant_p;
				lexeme_p=(Lexeme_t *)(*elementListeLexeme_pp)->donnees_p;
				if (lexeme_p->nature==L_SYMBOLE) {
					if (strcmp(strlwr(lexeme_p->data), "noreorder")==0) {
						DEBUG_MSG("le symbole \"noreoder\" est bien présent");
						*elementListeLexeme_pp=(*elementListeLexeme_pp)->suivant_p;
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
		analyseSyntaxeSauteCommentaire(elementListeLexeme_pp);
	}
}

void analyseSyntaxeText(
		ElementListe_t **elementListeLexeme_pp,
		enum Section_e *section_p,
		uint32_t *decalage_p,
		Liste_t *liste_p,
		TableHachage_t *tableEtiquettes_p,
		Dictionnaire_t *dictionnaireInstructions_p) {

	analyseSyntaxeSection(elementListeLexeme_pp, section_p);
	if (*elementListeLexeme_pp && ((Lexeme_t *)((*elementListeLexeme_pp)->donnees_p))->nature != L_FIN_LIGNE) {

	}
}

void analyseSyntaxeData(
		ElementListe_t **elementListeLexeme_pp,
		enum Section_e *section_p,
		uint32_t *decalage_p,
		Liste_t *liste_p,
		TableHachage_t *tableEtiquettes_p) {

	analyseSyntaxeSection(elementListeLexeme_pp, section_p);
	if (*elementListeLexeme_pp && ((Lexeme_t *)((*elementListeLexeme_pp)->donnees_p))->nature != L_FIN_LIGNE) {

	}
}

void analyseSyntaxeBss(
		ElementListe_t **elementListeLexeme_pp,
		enum Section_e *section_p,
		uint32_t *decalage_p,
		Liste_t *liste_p,
		TableHachage_t *tableEtiquettes_p) {

	analyseSyntaxeSection(elementListeLexeme_pp, section_p);
	if (*elementListeLexeme_pp && ((Lexeme_t *)((*elementListeLexeme_pp)->donnees_p))->nature != L_FIN_LIGNE) {

	}
}

void analyseSyntaxe(
		Liste_t *lignesLexemes_p,
		Dictionnaire_t *dictionnaireInstructions_p,
		TableHachage_t *tableEtiquettes_p,
		Liste_t *listeText_p,
		Liste_t *listeData_p,
		Liste_t *listeBss_p) {

	uint32_t decalageText=0;
	uint32_t decalageData=0;
	uint32_t decalageBss=0;

	ElementListe_t *elementListeLexeme_p=NULL;

	enum Section_e section=S_INIT;

	if (lignesLexemes_p) {
		elementListeLexeme_p=lignesLexemes_p->debut_liste_p;
		while (elementListeLexeme_p) { /* Boucle sur la liste des lexèmes */

			/* On va analyser la syntaxe en fonction de la section dans lequelle on se trouve */
			switch(section) {
			case S_INIT:
				analyseSyntaxeInit(&elementListeLexeme_p, &section);
				break;

			case S_TEXT:
				analyseSyntaxeText(&elementListeLexeme_p, &section, &decalageText, listeText_p, tableEtiquettes_p, dictionnaireInstructions_p);
				break;

			case S_DATA:
				analyseSyntaxeData(&elementListeLexeme_p, &section, &decalageData, listeData_p, tableEtiquettes_p);
				break;

			case S_BSS:
				analyseSyntaxeBss(&elementListeLexeme_p, &section, &decalageBss, listeBss_p, tableEtiquettes_p);
				break;

			default: ERROR_MSG("Cas non prévu, section inconnue");
			}

			DEBUG_MSG("Ignore les lexèmes suivants non traités");
			analyseSyntaxeVaFinLigne(&elementListeLexeme_p); /* On s'assure de bien être en fin de ligne */
			DEBUG_MSG("Passage à la ligne suivante");
			elementListeLexeme_p=elementListeLexeme_p->suivant_p; /* puis on passe à la nouvelle */
		}
	}
}

