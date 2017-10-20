/**
 * @file table.c
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition de fonctions de dictionnaire par fonction de hachage
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <global.h>
#include <str_utils.h>
#include <notify.h>
#include <table.h>

size_t nombrePremierGET(size_t nombre) {
	size_t diviseur;
	if (!(nombre%2)) nombre++; /* si pair, +1 le rend impair (les multiples de 2 ne sont pas premiers) */

	for (diviseur = 3; diviseur*diviseur <= nombre; diviseur+=2) {
		if (!(nombre%diviseur)) {
			nombre+=2;
			diviseur=1;
		}
	}
	return nombre;
}

size_t tailleTableHachageRecommandee(size_t nbElementsPrevus) {
	/* le facteur de charge recommandé pour un double hachage est entre 50% et 80% */
	/* Le choix suivant le place à 2/3 (2/(2+1)). La taille de la table doit être un nombre premier */
	/* afin d'avoir un groupe cyclique pour la deuxième clef de hachage */
	return nombrePremierGET(nbElementsPrevus + (nbElementsPrevus >> 1));
}

size_t hashKR2(char *chaine) {
	size_t hachage = 0;
	size_t caractere;

    if (chaine)
    	while ((caractere = (unsigned char)*(chaine++)))
        	hachage = ((hachage << 5) - hachage) + caractere; /* hash * 31 + caractere */

    return hachage;
}

size_t hashBernstein(char *chaine) {
	size_t hachage = 5381;
	size_t caractere;

    if (chaine)
    	while ((caractere = (unsigned char)*(chaine++)))
        	hachage = ((hachage << 5) + hachage) + caractere; /* hash * 33 + caractere */

    return hachage;
}

void *copiePointeur(void *original_p, size_t taille) {
	void *copie_p=NULL;
	if (original_p) {
		if (!(copie_p=malloc(taille))) ERROR_MSG("Impossible de dupliquer la structure de données");
		memcpy(copie_p, original_p, taille);
	}
	return copie_p;
}

/*
 * Function to Initialize Table
 */
struct Table_s *creeTable(size_t nbElementsMax, fonctionClef *fnClef_p, fonctionDestruction *fnDestruction_p) {
	struct Table_s *table_p = calloc(1, sizeof(*table_p));

    if (!table_p) ERROR_MSG("Impossible d'obtenir la mémoire pour la création de la structure de la table de hachage");

    table_p->nbEltsMax = nbElementsMax;
    table_p->nbElts=0;
    table_p->fnClef_p=fnClef_p;
    table_p->fnDestruction_p=fnDestruction_p;

    table_p->table = calloc(table_p->nbEltsMax, sizeof(*table_p->table));
    if (!table_p->table) ERROR_MSG("Impossible d'obtenir la mémoire pour la création de la table de hachage");

    return table_p;
}

/*
 * Function to Release Table
 */
struct Table_s *detruitTable(struct Table_s *table_p) {
	size_t i;

	if (table_p) {
		for (i=0; i<table_p->nbEltsMax; i++)
			if (table_p->table[i]) {
				if (table_p->fnDestruction_p)
					table_p->fnDestruction_p(table_p->table[i]);
				else
					free(table_p->table[i]);
			}

		free(table_p->table);
		free(table_p);
	}
	return NULL;
}

/*
 *
 */
void afficher_table(struct Table_s *table_p)
{
	size_t i, j;
	char *clef_p;

	if (table_p) {
		j=0;
		for (i=0; i<table_p->nbEltsMax; i++)
			if (table_p->table[i]) {
				j++;
				clef_p = table_p->fnClef_p ? table_p->fnClef_p(table_p->table[i]) : (char *)table_p->table[i];
				printf("%zu : [%zu] = %s\n", j, i, clef_p);
			}
	}
}

/*
 * Function to Find Element from the table
 */
size_t indexTable(struct Table_s *table_p, char *clef)
{
	size_t hachage = hashKR2(clef) % table_p->nbEltsMax;
	size_t pasCyclique = (hashBernstein(clef) % (table_p->nbEltsMax - 1)) + 1;
    /* la clef existe dans la table si le pointeur n'est pas null et que la clef pointée est identique (pour pour gérer les collisions) */
    while ((table_p->table[hachage]) &&
    		(strcmp(clef, (table_p->fnClef_p ? table_p->fnClef_p(table_p->table[hachage]) : (char *)table_p->table[hachage])))) {
        hachage = (hachage + pasCyclique) % table_p->nbEltsMax;
    }
    return hachage;
}

void *donneeTable(struct Table_s *table_p, char *clef)
{
    size_t position = indexTable(table_p, clef);
    return table_p->table[position];
}

/*
 * Function to Insert Element into the table - L'élément donnee_p n'est pas recopié et devra rester permanent
 */
int insereElementTable(struct Table_s *table_p, void *donnee_p)
{
	char *clef = (table_p->fnClef_p ? table_p->fnClef_p(donnee_p) : donnee_p);
    size_t position = indexTable(table_p, clef);

    if (!table_p->table[position]) {
    	table_p->table[position] = donnee_p;
    	table_p->nbElts++;
    	return TRUE;
    }
    else
    	return FALSE;
}

int supprimeElementTable(struct Table_s *table_p, char *clef)
{
    size_t position = indexTable(table_p, clef);

    if (table_p->table[position] && table_p->fnDestruction_p) {
    	table_p->fnDestruction_p(table_p->table[position]);
    	table_p->table[position] = NULL;
    	table_p->nbElts--;
    	return 1;
    }
    else
    	return 0;
}

/*
 * Function to redimensionneTable the table
 */
/*
HashTable_t *redimensionneTable(HashTable_t *table_p, size_t newSize) {
	int i;
    int size = table_p->nbElementsMax;
    void **table = table_p->table;
    table_p = initializeTable(newSize, table_p->fnClef_p, table_p->fnDestruction_p);
    for (i = 0; i < size; i++) {
        if (table[i])
            Insert(table_p, table[i]);
    }
    free(table);
    return table_p;
}
*/
