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
#include <table_hachage.h>

#define MIN_TABLE_SIZE 11

/*
 * Table Declaration
 */
typedef struct HashTable_t {
    int nbElementsMax;
    int nbElements;
    void **table;
    fonctionClef *fnClef_p;
} HashTable_t;

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

unsigned int hashKR2(char *chaine) {
    unsigned int hachage = 0;
    unsigned int caractere;

    if (!chaine)
    	return 0;
    else {
        while ((caractere = (unsigned char)*(chaine++)))
        	hachage = 31*hachage + caractere;
        return hachage;
    }
}

char *clefStr(void *uneStr) {
	return (char *)uneStr;
}

/*
 * Function to Initialize Table
 */
HashTable_t *initializeTable(int nbElementsMax, fonctionClef *fnClef_p) {
	HashTable_t *htable;
    if (nbElementsMax < MIN_TABLE_SIZE) {
        printf("Table Size Too Small\n");
        return NULL;
    }
    htable = calloc(1, sizeof(*htable));
    if (htable == NULL) {
        printf("Out of Space\n");
        return NULL;
    }
    htable->nbElementsMax = nbElementsMax;
    htable->nbElements=0;
    htable->fnClef_p=fnClef_p;
    htable->table = calloc(htable->nbElementsMax, sizeof(*htable->table));
    if (htable->table == NULL) {
        printf("Table Size Too Small\n");
        return NULL;
    }
    return htable;
}
/*
 * Function to Release Table
 */
void releaseTable(HashTable_t *htable) {
	/*
	int i;
    htable = malloc(sizeof(struct HashTable));
    if (htable == NULL) {
        printf("Out of Space\n");
        return NULL;
    }
    htable->size = size;
    htable->table = malloc(sizeof(struct HashNode[htable->size]));
    if (htable->table == NULL) {
        printf("Table Size Too Small\n");
        return NULL;
    }
    for (i = 0; i < htable->size; i++) {
        htable->table[i].info = Empty;
        htable->table[i].element = NULL;
    }
    */
}
/*
 * Function to Find Element from the table
 */
int Find(char *key, HashTable_t *htable_p) {
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
void Insert(void *donnee_p, HashTable_t *htable_p) {
	char *key=htable_p->fnClef_p(donnee_p);
    unsigned int pos = Find(key, htable_p);
    if (!htable_p->table[pos]) {
    	htable_p->table[pos] = donnee_p;
    }
}
/*
 * Function to Rehash the table
 */
HashTable_t *Rehash(HashTable_t *htable_p, int newSize) {
	int i;
    int size = htable_p->nbElementsMax;
    void **table = htable_p->table;
    htable_p = initializeTable(newSize, htable_p->fnClef_p);
    for (i = 0; i < size; i++) {
        if (table[i])
            Insert(table[i], htable_p);
    }
    free(table);
    return htable_p;
}
/*
 * Function to Retrieve the table
 */
void Retrieve(HashTable_t *htable_p) {
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
    while (1) {
        printf("\n----------------------\n");
        printf("Operations on Double Hashing\n");
        printf("\n----------------------\n");
        printf("1.Initialize size of the table\n");
        printf("2.Insert element into the table\n");
        printf("3.Display Hash Table\n");
        printf("4.Rehash The Table\n");
        printf("5.Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
        case 1:
            printf("Enter nbElementsMax of the Hash Table: ");
            scanf("%d", &size);
            htable_p = initializeTable(size, clefStr);
            break;
        case 2:
            if (i > htable_p->nbElementsMax) {
                printf("Table is Full, Rehash the table\n");
                continue;
            }
            printf("Enter element to be inserted: ");
            scanf("%s", value);
            Insert(strdup(value), htable_p);
            i++;
            break;
        case 3:
            Retrieve(htable_p);
            break;
        case 4:
            printf("Enter new nbElementsMax of the Hash Table: ");
            scanf("%d", &size);
            htable_p = Rehash(htable_p, size);
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
