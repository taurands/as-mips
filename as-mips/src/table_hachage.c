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

#define MIN_TABLE_SIZE 10

/*
 * Node Type Declaration
 */
enum EntryType {
    Legitimate, Empty, Deleted
};
/*
 * Node Declaration
 */
struct HashNode {
    char *element;
    enum EntryType info;
};
/*
 * Table Declaration
 */
struct HashTable {
    int size;
    fonctionClef *fnClef_p;
    struct HashNode *table;
};

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
struct HashTable *initializeTable(int size, fonctionClef *fnClef_p) {
	int i;
    struct HashTable * htable;
    if (size < MIN_TABLE_SIZE) {
        printf("Table Size Too Small\n");
        return NULL;
    }
    htable = malloc(sizeof(struct HashTable));
    if (htable == NULL) {
        printf("Out of Space\n");
        return NULL;
    }
    htable->size = size;
    htable->fnClef_p=fnClef_p;
    htable->table = malloc(sizeof(struct HashNode[htable->size]));
    if (htable->table == NULL) {
        printf("Table Size Too Small\n");
        return NULL;
    }
    for (i = 0; i < htable->size; i++) {
        htable->table[i].info = Empty;
        htable->table[i].element = NULL;
    }
    return htable;
}
/*
 * Function to Release Table
 */
void releaseTable(struct HashTable *htable) {
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
int Find(char *key, struct HashTable *htable) {
    unsigned int hashVal = hashKR2(key) % htable->size;
    unsigned int stepSize = (hashBernstein(key) % (htable->size - 1)) + 1;
    while (htable->table[hashVal].info != Empty
            && (0 != strcmp(key, htable->table[hashVal].element))) {
        hashVal = (hashVal + stepSize) % htable->size;
    }
    return hashVal;
}
/*
 * Function to Insert Element into the table
 */
void Insert(char *key, struct HashTable *htable) {
    unsigned int pos = Find(key, htable);
    if (htable->table[pos].info != Legitimate) {
        htable->table[pos].info = Legitimate;
        htable->table[pos].element = strdup(key);
    }
}
/*
 * Function to Rehash the table
 */
struct HashTable *Rehash(struct HashTable *htable, int newSize) {
	int i;
    int size = htable->size;
    struct HashNode *table = htable->table;
    htable = initializeTable(newSize,htable->fnClef_p);
    for (i = 0; i < size; i++) {
        if (table[i].info == Legitimate)
            Insert(table[i].element, htable);
    }
    free(table);
    return htable;
}
/*
 * Function to Retrieve the table
 */
void Retrieve(struct HashTable *htable) {
	int i;
    for (i = 0; i < htable->size; i++) {
        char *value = htable->table[i].element;
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
    struct HashTable * htable;
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
            printf("Enter size of the Hash Table: ");
            scanf("%d", &size);
            htable = initializeTable(size, clefStr);
            break;
        case 2:
            if (i > htable->size) {
                printf("Table is Full, Rehash the table\n");
                continue;
            }
            printf("Enter element to be inserted: ");
            scanf("%s", value);
            Insert(value, htable);
            i++;
            break;
        case 3:
            Retrieve(htable);
            break;
        case 4:
            printf("Enter new size of the Hash Table: ");
            scanf("%d", &size);
            htable = Rehash(htable, size);
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
