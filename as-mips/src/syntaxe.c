#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <gen_list.h>
#include <syntaxe.h>

char* duplicateStr(char* sourceStr) {
	char* destStr=NULL;
	if (sourceStr) {
		destStr=malloc(sizeof(*destStr)*(1+strlen(sourceStr)));
		if (!destStr) ERROR_MSG("Allocation impossible");
		strcpy(destStr,sourceStr);
	}
	return destStr;
}

/**
 * @param liste_lexemes_p
 * @return nothing
 * @brief Cette fonction permet de dire s'il y a une instruction du dictionnaire valable sur une ligne
 */
void bonneInstruction(Liste_t* ligne_lexemes_p) {

	ElementListe_t* p_1=ligne_lexemes_p->debut_liste_p;
	ElementListe_t* p_2=ligne_lexemes_p->debut_liste_p;

	Dictionnaire_t* mon_dictionnaire_p=NULL;
	/* Instruction_t instruction_courante; */

	int i=0;

	int nombre_instruction;
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

		FILE* f_p=fopen("dictionnaire_instruction.txt","r"); /* Ouverture du dictionnaire d'instruction */
		if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");
		if (1!=fscanf(f_p,"%d",&nombre_instruction)) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */
		mon_dictionnaire_p=calloc(nombre_instruction,sizeof(Mot_Dictionnaire_t));
		if (!mon_dictionnaire_p) ERROR_MSG("Plus assez de mémoire pour créer un dictionnaire");

		/* Il faut traiter les erreurs correspondant à : */
		/* Un nombre d'instructions incorrect */
		/*  - une absence d'instruction */
		/*  - une absence de nombre de paramètre */
		while (f_p && (i<nombre_instruction)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

			if(2!=fscanf(f_p,"%s %d",nom_instruction,&nombre_argument)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");
			(*mon_dictionnaire_p)[i].instruction=duplicateStr(nom_instruction);
			(*mon_dictionnaire_p)[i++].nb_arg=nombre_argument;


			if (strcmp(nom_instruction,((Lexeme_t*)(p_1->donnees_p))->data)==0) { /* Si le lexeme correspond à une instruction du dictionnaire */
				INFO_MSG("Le lexeme correspond à une instruction du dictionnaire");
				while (p_1) {
					if (((Lexeme_t*)(p_1->donnees_p))->nature != (L_VIRGULE || L_PARANTHESE_OUVRANTE || L_PARANTHESE_FERMANTE || L_COMMENTAIRE)) {   /* Si le lexeme est un lexeme utile */
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
						while (((Lexeme_t*)(p_1->donnees_p))->nature != (L_VIRGULE || L_PARANTHESE_OUVRANTE || L_PARANTHESE_FERMANTE || L_COMMENTAIRE)) {
							p_2=p_2->suivant_p;
						}
						ajouteElementFinListe(liste_instruction_p, p_2->donnees_p);
					}
					*/
				}
			}
		}

		fclose(f_p);
	}
}
