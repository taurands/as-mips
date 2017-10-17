/**
 * @file table_hachage.c
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition de fonctions de dictionnaire par fonction de hachage
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <str_utils.h>
#include <notify.h>
#include <table_hachage.h>

#define MIN_TABLE_SIZE 5

unsigned int nombrePremierGET(unsigned int nombre) {
	unsigned int diviseur;
	if (!(nombre%2)) nombre++; /* si pair, +1 le rend impair (les multiples de 2 ne sont pas premiers */

	for (diviseur = 3; diviseur*diviseur <= nombre; diviseur+=2) {
		if (!(nombre%diviseur)) {
			nombre+=2;
			diviseur=1;
		}
	}
	return nombre;
}

unsigned int tailleTableHachageRecommandee(unsigned int nbElementsPrevus) {
	/* le facteur de charge recommandé pour un double hachage est entre 50% et 80% */
	/* Le choix suivant le place à 2/3 (2/(2+1)). La taille de la table doit être un nombre premier */
	/* afin d'avoir un groupe cyclique pour le deuxième hachage */
	return nombrePremierGET(nbElementsPrevus + (nbElementsPrevus >> 1));
}

unsigned int hashKR2(char *chaine) {
    unsigned int hachage = 0;
    unsigned int caractere;

    if (!chaine)
    	return 0;
    else {
        while ((caractere = (unsigned char)*(chaine++)))
        	hachage = ((hachage << 5) - hachage) + caractere; /* hash * 31 + caractere */
        return hachage;
    }
}

unsigned int hashBernstein(char *chaine) {
    unsigned int hachage = 5381;
    unsigned int caractere;

    if (!chaine)
    	return 0;
    else {
        while ((caractere = (unsigned char)*(chaine++)))
        	hachage = ((hachage << 5) + hachage) + caractere; /* hash * 33 + caractere */
        return hachage;
    }
}

char *clefStr(void *uneStr) {
	return (char *)uneStr;
}

void destructionStr(void *uneStr) {
	free(uneStr);
}

/*
 * Function to Initialize Table
 */
HashTable_t *creeTable(unsigned int nbElementsMax, fonctionClef *fnClef_p, fonctionDestruction *fnDestruction_p) {
	HashTable_t *htable = calloc(1, sizeof(*htable));

    if (!htable) ERROR_MSG("Impossible d'obtenir la mémoire pour la création de la structure de la table de hachage");

    htable->nbElementsMax = nbElementsMax;
    htable->nbElements=0;
    htable->fnClef_p=fnClef_p;
    htable->fnDestruction_p=fnDestruction_p;

    htable->table = calloc(htable->nbElementsMax, sizeof(*htable->table));
    if (!htable->table) ERROR_MSG("Impossible d'obtenir la mémoire pour la création de la table de hachage");

    return htable;
}

/*
 * Function to Release Table
 */
HashTable_t *detruitTable(HashTable_t *htable) {
	unsigned int i;

	if (htable->fnDestruction_p)
		for (i=0; i<htable->nbElementsMax; i++)
			if (htable->table[i])
				htable->fnDestruction_p(htable->table[i]);

	free(htable->table);
	free(htable);

	return NULL;
}

/*
 * Function to Find Element from the table
 */
unsigned int trouve(HashTable_t *htable_p, char *key) {
    unsigned int hashVal = hashKR2(key) % htable_p->nbElementsMax;
    unsigned int stepSize = (hashBernstein(key) % (htable_p->nbElementsMax - 1)) + 1;
    while ((htable_p->table[hashVal]) && (strcmp(key, htable_p->fnClef_p(htable_p->table[hashVal])))) {
        hashVal = (hashVal + stepSize) % htable_p->nbElementsMax;
    }
    return hashVal;
}

/*
 * Function to Insert Element into the table - L'élément donnee_p n'est pas recopié et devra rester permanent
 */
int insere(HashTable_t *htable_p, void *donnee_p) {
	char *key=htable_p->fnClef_p(donnee_p);
    unsigned int pos = trouve(htable_p, key);
    if (!htable_p->table[pos]) {
    	htable_p->table[pos] = donnee_p;
    	htable_p->nbElements++;
    	return 1;
    }
    else
    	return 0;
}

/*
 * Function to redimensionneTable the table
 */
/*
HashTable_t *redimensionneTable(HashTable_t *htable_p, int newSize) {
	int i;
    int size = htable_p->nbElementsMax;
    void **table = htable_p->table;
    htable_p = initializeTable(newSize, htable_p->fnClef_p, htable_p->fnDestruction_p);
    for (i = 0; i < size; i++) {
        if (table[i])
            Insert(htable_p, table[i]);
    }
    free(table);
    return htable_p;
}
*/

/*
 * Function to Retrieve the table
 */
void afficheTableHachage(HashTable_t *htable_p) {
	int i;
    for (i = 0; i < htable_p->nbElementsMax; i++) {
        char *value = htable_p->fnClef_p(htable_p->table[i]);
        printf("Position: %d Element: %s\n", (i + 1), value);
    }
}
/*
 * test du fonctionnement
 */
int test_hachage() {
    char *value = calloc(1,128);

	int size, i = 1;
    int choice;
    HashTable_t *htable_p;

    /*
    printf("premier >10 = %u\n", nombrePremierGET(10));
    printf("premier >11 = %u\n", nombrePremierGET(11));
    printf("premier >1000 = %u\n", nombrePremierGET(1000));
    printf("premier >10000 = %u\n", nombrePremierGET(10000));
    printf("premier >100000 = %u\n", nombrePremierGET(100000));
    printf("premier >26 = %u\n", nombrePremierGET(26));
    */
    while (1) {
        printf("\n----------------------\n");
        printf("Operations on Double Hashing\n");
        printf("\n----------------------\n");
        printf("1.Initialize size of the table\n");
        printf("2.Insert element into the table\n");
        printf("3.Display Hash Table\n");
        printf("4.redimensionneTable The Table\n");
        printf("5.Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
        case 1:
            printf("Enter nbElementsMax of the Hash Table: ");
            scanf("%d", &size);
            htable_p = creeTable(tailleTableHachageRecommandee(size), clefStr, NULL);
            break;
        case 2:
            if (i > htable_p->nbElementsMax) {
                printf("Table is Full, redimensionneTable the table\n");
                continue;
            }
            printf("Enter element to be inserted: ");
            scanf("%s", value);
            insere(htable_p, strdup(value));
            i++;
            break;
        case 3:
            afficheTableHachage(htable_p);
            break;
        case 4:
            printf("Enter new nbElementsMax of the Hash Table: ");
            scanf("%d", &size);
            /*
            htable_p = redimensionneTable(htable_p, size);
            */
            break;
        case 5:
            exit(0);
        default:
            printf("\nEnter correct option\n");
        }
    }

    free(value);
    return 0;
}
