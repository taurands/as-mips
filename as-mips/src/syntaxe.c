#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <gen_list.h>
#include <syntaxe.h>
#include <str_utils.h>

Dictionnaire_t *chargeDictionnaire(char *nomFichierDictionnaire) {
	char *nomInstruction=calloc(128, sizeof(char));
	int nombreOperandes=0;
	int i=0;

	Dictionnaire_t* dictionnaireLu_p=calloc(1,sizeof(*dictionnaireLu_p));
	if (!dictionnaireLu_p) ERROR_MSG("Plus assez de mémoire pour créer un dictionnaire");

	FILE* f_p=fopen(nomFichierDictionnaire,"r"); /* Ouverture du dictionnaire d'instruction */
	if (!f_p) ERROR_MSG("Impossible d'ouvrir le fichier");

	if (1!=fscanf(f_p,"%d\n",&(dictionnaireLu_p->nbMots))) ERROR_MSG("Nombre d'instructions du dictionnaire introuvable"); /* Lecture de la première ligne du dictionnaire */

	dictionnaireLu_p->mots=calloc(dictionnaireLu_p->nbMots,sizeof(Mot_Dictionnaire_t));
	if (!dictionnaireLu_p->mots) ERROR_MSG("Plus assez de mémoire pour créer un dictionnaire");

	while (f_p && (i<dictionnaireLu_p->nbMots)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */

		if (1 != fscanf(f_p,"%s", nomInstruction)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");
		if (1 != fscanf(f_p,"%d\n", &nombreOperandes)) ERROR_MSG("La ligne du dictionnaire ne comprennait pas le nom et/ou le nombre d'arguments de l'instruction en cours");
		nomInstruction=strupr(nomInstruction);
		(*dictionnaireLu_p->mots)[i].instruction=strdup(nomInstruction);
		(*dictionnaireLu_p->mots)[i].nb_arg=nombreOperandes;
		if (i) if (strcmp((*dictionnaireLu_p->mots)[i-1].instruction, (*dictionnaireLu_p->mots)[i].instruction)>=0) ERROR_MSG("Le fichier dictionnaire d'instructions n'est par rangé par ordre alphabétique");
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
		free((*unDictionnaire_p->mots)[i].instruction);
	free(unDictionnaire_p->mots);
	unDictionnaire_p->mots=NULL;
	unDictionnaire_p->nbMots=0;
}

int indexDictionnaire(Dictionnaire_t *unDictionnaire_p, char *unMot) {
	/* fonction de recherche dichotomique qui renvoie l'indice où se trouve unMot dans unDictionnaire_p */
	/* si elle est absente, renvoie -1 */


	/* déclaration des variables locales à la fonction */
	int trouve;  /* vaut faux tant que la valeur "val" n'aura pas été trouvée */
	int id;  /* indice de début */
	int ifin;  /* indice de fin */
	int im;  /* indice de "milieu" */
	int comp; /* résultat de la comparaison */

	/* initialisation de ces variables avant la boucle de recherche */
	trouve = -1;  /* la valeur n'a pas encore été trouvée */
	id = 0;  /* intervalle de recherche compris entre 0... */
	ifin = unDictionnaire_p->nbMots-1;

	/* boucle de recherche */
	while((trouve==-1) && (ifin>=id)){

		im = (id + ifin)/2;  /* on détermine l'indice de milieu */

		comp=strcmp((*unDictionnaire_p->mots)[im].instruction,unMot);
		if (comp>0)
			ifin=im-1;
		else if (comp<0)
			id=im+1;
		else
			trouve=im;
	}
	return trouve;
}

/**
 * @param liste_lexemes_p
 * @return nothing
 * @brief Cette fonction permet de dire s'il y a une instruction du dictionnaire valable sur une ligne
 */
void bonneInstruction(Liste_t* ligne_lexemes_p) {

	ElementListe_t* p_1=ligne_lexemes_p->debut_liste_p;
	ElementListe_t* p_2=ligne_lexemes_p->debut_liste_p;

	Dictionnaire_t* mon_dictionnaire_p=chargeDictionnaire("dictionnaire_instruction.txt");
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

		while ((i<nombre_instruction)) { /* Tant que l'on a pas lu l'enemble du dictionnaire */


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

	}
}
