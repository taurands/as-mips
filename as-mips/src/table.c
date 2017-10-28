/**
 * @file table.c
 * @author BERTRAND Antoine, TAURAND Sébastien
 * @brief Definition de fonctions pour les tables de hachage génériques
 *
 * L'implémentation utilise un double hachage afin d'éviter de gérer les collisions avec des listes chainées.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <table.h>

/**
 * @param nombre
 * @return le nombre premier recherché
 * @brief Renvoie le plus petit nombre premier supérieur ou égal à nombre
 *
 * Algorithme basique mais utilisant quand même un crible d'Ératosthène sur les multiples de 2 ainsi que
 * l'arrêt de la recherche des diviseurs au delà de la racine carrée du nombre (sans calculer la racine bien sûr).
 */
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

/**
 * @param nbElementsPrevus nombre d'éléments prévus
 * @return La taille recommandée
 * @brief Donne le "bon" dimensionnement pour une table à double hachage
 *
 * le facteur de charge recommandé pour un double hachage est entre 50% et 80%.
 * Le choix effectué ici le place à 2/3. La taille de la table est un nombre premier
 * afin d'avoir un groupe cyclique pour la deuxième clef de hachage
 */
size_t tailleTableHachageRecommandee(size_t nbElementsPrevus) {
	return nombrePremierGET(nbElementsPrevus + (nbElementsPrevus >> 1) + 1);
}

/**
 * @param chaine chaine de caractères contenant la clef à hacher
 * @return valeur du hachage
 * @brief Renvoie une valeur de hachage suivant la définition de la deuxième version du livre de K & R.
 *
 * Cette fonction est très similaire à la fonction de hachage de chaine de JAVA
 */
size_t hashKR2(char *chaine) {
	size_t hachage = 0;
	size_t caractere;

    if (chaine)
    	while ((caractere = (unsigned char)*(chaine++)))
        	hachage = ((hachage << 5) - hachage) + caractere; /* hash * 31 + caractere */

    return hachage;
}

/**
 * @param chaine chaine de caractères contenant la clef à hacher
 * @return valeur du hachage
 * @brief Renvoie une valeur de hachage suivant la définition de Bernstein.
 */
size_t hashBernstein(char *chaine) {
	size_t hachage = 5381;
	size_t caractere;

    if (chaine)
    	while ((caractere = (unsigned char)*(chaine++)))
        	hachage = ((hachage << 5) + hachage) + caractere; /* hash * 33 + caractere */

    return hachage;
}

/**
 * @param nb_elts nombre d'élements minimal que la table devra pouvoir acceuillir
 * @param fn_clef_p pointeur sur une fonction permettant de renvoyer la clef d'identification à partir du pointeur sur l'élément
 * @param fn_destruction_p p pointeur sur une fonction permettant de détruire les données liées aux éléments eux-mêmes
 * @return pointeur sur la table de hachage générique créée
 * @brief Crée une table de hachage générique
 */
int creer_table(struct Table_s **table_pp, size_t nb_elts, fonctionClef *fn_clef_p, fonctionDestruction *fn_destruction_p)
{
	if (table_pp && !*table_pp) {
		(*table_pp)=calloc (1, sizeof(**table_pp));
		if (!*table_pp)
			return FAIL_ALLOC;

	    (*table_pp)->nbEltsMax = tailleTableHachageRecommandee(nb_elts);
	    (*table_pp)->nbElts=0;
	    (*table_pp)->fnClef_p=fn_clef_p;
	    (*table_pp)->fnDestruction_p=fn_destruction_p;
	    (*table_pp)->table = calloc ((*table_pp)->nbEltsMax, sizeof(*(*table_pp)->table));
	    if (!(*table_pp)->table) {
	    	free (*table_pp);
	    	*table_pp = NULL;
	    	return FAIL_ALLOC;
	    }

		return SUCCESS;
	} else
		return FAILURE;
}

/**
 * @param table_p pointeur sur une table de hachage générique
 * @return SUCCESS si la table existe, NULL sinon
 * @brief Supprime et libère la table de hachage et tout son contenu lié
 *
 * Si une fonction de destuction a été passée à la création de la table, elle sera utilisée pour libérer
 * la mémoire occupée par les éléments et leurs dépendances éventuelles. Sinon, seul les éléments pointés seront
 * enlevés de la table et libérés.
 */
int detruire_table(struct Table_s **table_pp) {
	size_t i;

	if (table_pp && *table_pp) {
		for (i=0; i<(*table_pp)->nbEltsMax; i++)
			if ((*table_pp)->table[i]) {
				if ((*table_pp)->fnDestruction_p)
					(*table_pp)->fnDestruction_p((*table_pp)->table[i]);
				else
					free((*table_pp)->table[i]);
			}

		free((*table_pp)->table);
		free((*table_pp));
		*table_pp = NULL;
		return SUCCESS;
	} else
		return FAILURE;
}

/**
 * @param table_p pointeur sur une table de hachage générique
 * @return Rien
 * @brief Affiche l'ensemble des clefs contenues dans la table
 *
 * Le premier nombre indique simplement la nième clef
 * Le nombre en [ ] indique l'index dans la table
 */
void afficher_clefs_table(struct Table_s *table_p)
{
	size_t i, j;
	char *clef_p;

	if (table_p) {
		j=0;
		for (i=0; i<table_p->nbEltsMax; i++)
			if (table_p->table[i]) {
				j++;
				clef_p = table_p->fnClef_p ? table_p->fnClef_p(table_p->table[i]) : (char *)table_p->table[i];
				printf("%3zu->[%3zu] = %s\n", j, i, clef_p);
			}
	}
}

/**
 * @param table_p pointeur sur une table de hachage générique
 * @param clef chaine de caractère représentant l'identifiant de l'élément dont on veut obtenir l'index
 * @return l'index de la table dans lequel on a trouvé l'élément assoicé à la clé ou dans lequel il serait mis s'il n'y existe pas
 * @brief Renvoie l'index de la table de hachage correspondant à la clef
 *
 * Attention, l'index peut correspondre à une case de la table qui pointe sur NULL s'il n'y a pas d'élément correspand dans la table
 */
size_t index_table(struct Table_s *table_p, char *clef)
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

/**
 * @param table_p pointeur sur une table de hachage générique
 * @param clef chaine de caractère représentant l'identifiant de l'élément recherché
 * @return pointeur sur l'élément de la table correspondant à la clef s'il existe, NULL sinon
 * @brief Renvoie un pointeur sur l'élément de la table identifié par la clef
 */
void *donnee_table(struct Table_s *table_p, char *clef)
{
	if (table_p && clef) {
		size_t position = index_table(table_p, clef);
		return table_p->table[position];
	}
	return NULL;
}

/**
 * @param table_p pointeur sur une table de hachage générique
 * @param donnee_p pointeur de la donnée à ajouter
 * @return SUCCESS si on ajoute vraiment un nouvel élément, FAILURE sinon (en particulier si l'élément existe déjà)
 * @brief Ajoute un élément à la table de hachage générique
 *
 * Si on a passé une fonction clé à la création, celle-ci sera utilisée pour extraire la clef qui lui sera associée,
 * sinon, l'élément sera sa propre clef (cas d'une chaine par exemple).
 */
int ajouter_table(struct Table_s *table_p, void *donnee_p)
{
	char *clef = NULL;
	if (table_p && donnee_p) {
		clef = (table_p->fnClef_p ? table_p->fnClef_p(donnee_p) : donnee_p);
		size_t position = index_table(table_p, clef);

		if (!table_p->table[position]) {
			table_p->table[position] = donnee_p;
			table_p->nbElts++;
			return SUCCESS;
		}
	}
	return FAILURE;
}

/**
 * @param table_p pointeur sur une table de hachage générique
 * @param clef chaine de caractère contenant la clef de reconnaissance de la donnée à supprimer
 * @return SUCCESS si on supprime vraiment un élément, FAILURE sinon
 * @brief Supprime un élément d'une table de hachage générique
 *
 * Si une fonction de destuction a été passée à la création de la table, elle sera utilisée pour libérer
 * la mémoire occupée par l'élément est ses dépendances éventuelles. Sinon, seul l'élément pointé sera
 * enlevé de la table et libéré.
 *
 * Si une fonction de Clef a été passée à l'initialisation, elle sera utilisée pour identifier l'élément,
 * sinon, l'élément sera la clef lui-même (cas d'une chaine de caractères).
 */
int supprimer_table(struct Table_s *table_p, char *clef)
{
    size_t position = index_table(table_p, clef);

    if (table_p && table_p->table[position] && table_p->fnDestruction_p) {
    	table_p->fnDestruction_p(table_p->table[position]);
    	table_p->table[position] = NULL;
    	table_p->nbElts--;
    	return SUCCESS;
    }
    else
    	return FAILURE;
}
