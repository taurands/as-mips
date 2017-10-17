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

int machineEtatsFinisSyntaxique(int etat, Lexeme_t *lexeme_p) {
	return etat;
}

Liste_t *analyseSyntaxe(Liste_t *lignesLexemes_p, Dictionnaire_t *monDictionnaire_p, TableHachage_t *tableEtiquettes_p) {
	ElementListe_t *elementListeLigneLexeme=NULL;
	Liste_t *listeLexeme_p=NULL;
	ElementListe_t *elementListeLexeme_p=NULL;
	Lexeme_t *lexeme_p=NULL;

	uint32_t decalageText=0;
	uint32_t decalageData=0;
	uint32_t decalageBss=0;

	int ligneSource=0;

	int etat;
	enum Section_e i, section=S_INIT;

	Liste_t *lignesCode_p=NULL;

	if (lignesLexemes_p) {
		elementListeLigneLexeme=lignesLexemes_p->debut_liste_p;
		while (elementListeLigneLexeme) {
			ligneSource++;
			listeLexeme_p=(Liste_t *)elementListeLigneLexeme->donnees_p;
			elementListeLexeme_p=listeLexeme_p->debut_liste_p;
			while (elementListeLexeme_p) {
				lexeme_p=(Lexeme_t *)elementListeLexeme_p->donnees_p;

				etat=machineEtatsFinisSyntaxique(etat, lexeme_p);
				if (lexeme_p->nature==L_DIRECTIVE) {
					lexeme_p->data=strlwr(lexeme_p->data);
					if (strcmp(lexeme_p->data, ".set")==0) {
						DEBUG_MSG("La directive \".set\" a été reconnue");
						elementListeLexeme_p=elementListeLexeme_p->suivant_p;
						lexeme_p=(Lexeme_t *)elementListeLexeme_p->donnees_p;
						if (lexeme_p->nature==L_SYMBOLE) {
							if (strcmp(strlwr(lexeme_p->data), "noreorder")==0) {
								DEBUG_MSG("Reconnu le symbole \"noreoder\"");
							}
							else { /* Le symbole n'est pas "noreorder" */
								ERROR_MSG("Symbole inconnu après la directive \".set\"");
							}
						}
						else { /* Le lexème n'est pas un symbole */
							DEBUG_MSG("Pas de symbole après la directive \".set\"");
						}
					}
					else { /* La directive n'est pas un ".set" */
						for (i=S_TEXT; i<=S_BSS; i++) { /* on regarde si la directive coorespond à un nom de section */
							if (strcmp(lexeme_p->data, NOMS_SECTIONS[i])==0) {
								section=i;
								DEBUG_MSG("La directive \"%s\" a été reconnue. Changement de nature de section pour %d : %s", lexeme_p->data, section, NOMS_SECTIONS[section]);
								break;
							}
						}
						if (i>S_BSS) { /* La directive n'est pas un nom de section */
							DEBUG_MSG("La directive \"%s\" est inconnue dans la section %d : \"%s\"", lexeme_p->data, section, NOMS_SECTIONS[section]);
						}
					}

				}
				else /* Le lexème n'est pas une directive */ if (section==S_TEXT) {

				}
				else {

				}

				elementListeLexeme_p=elementListeLexeme_p->suivant_p;
			}

			elementListeLigneLexeme=elementListeLigneLexeme->suivant_p;
		}
	}
	return lignesCode_p;
}

/**
 * @param liste_lexemes_p
 * @return nothing
 * @brief Cette fonction permet de dire s'il y a une instruction du dictionnaire valable sur une ligne
 */
void bonneInstruction(Liste_t* ligne_lexemes_p) {

	ElementListe_t* p_1=ligne_lexemes_p->debut_liste_p;
	ElementListe_t* p_2=ligne_lexemes_p->debut_liste_p;

	/*
	Dictionnaire_t* mon_dictionnaire_p=chargeDictionnaire("dictionnaire_instruction.txt");
	int i=0;
	int nombre_instruction;
	 */
	/* Instruction_t instruction_courante; */

	char* nom_instruction="";
	int nombre_argument;

	int compteur_lexeme_utile=0;

	while (((Lexeme_t*)(p_1->donnees_p))->nature!=L_SYMBOLE) { /* On cherche le premier symbole (seul lexeme sucsecptible d'être une instruction) */
		p_1=p_1->suivant_p;
		p_2=p_2->suivant_p;
	}
	/* On arrive à un lexeme SYMBOLE */
	if (((Lexeme_t*)(p_1->donnees_p))->nature==L_SYMBOLE) {

		/* Comparaison du nom de symbole à l'ensemble des instructions possibles du dictionnaire */

		while (0==1/* (i<nombre_instruction) */) { /* Tant que l'on a pas lu l'enemble du dictionnaire */


			if (strcmp(nom_instruction,((Lexeme_t*)(p_1->donnees_p))->data)==0) { /* Si le lexeme correspond à une instruction du dictionnaire */
				INFO_MSG("Le lexeme correspond à une instruction du dictionnaire");
				while (p_1) {
					if (((Lexeme_t*)(p_1->donnees_p))->nature != (L_VIRGULE || L_PARENTHESE_OUVRANTE || L_PARENTHESE_FERMANTE || L_COMMENTAIRE)) {   /* Si le lexeme est un lexeme utile */
						compteur_lexeme_utile++;
					}
					p_1=p_1->suivant_p;
				}
				if (compteur_lexeme_utile==nombre_argument) {
					INFO_MSG("L'instruction possède bien le bon nombre de d'argument");
					/* Ajout du lexeme à la liste d'instruction */
					/*
					ajouteElementFinListe(liste_instruction_p, p_2->donnees_p);
					for (i=0;i<nombre_argument;i++){
						while (((Lexeme_t*)(p_1->donnees_p))->nature != (L_VIRGULE || L_PARENTHESE_OUVRANTE || L_PARENTHESE_FERMANTE || L_COMMENTAIRE)) {
							p_2=p_2->suivant_p;
						}
						ajouteElementFinListe(liste_instruction_p, p_2->donnees_p);
					}
					 */
				}
			}
		}

	}
}
