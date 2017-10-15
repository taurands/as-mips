/**
 * @file table_hachage.c
 * @author BERTRAND Antoine TAURAND Sébastien
 * @brief Definition de fonctions de dictionnaire par fonction de hachage
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
    int element;
    enum EntryType info;
};
/*
 * Table Declaration
 */
struct HashTable {
    int size;
    struct HashNode *table;
};

unsigned int hashBernstein(char *chaine) {
    unsigned int hachage = 5381;
    unsigned int caractere;

    if (!chaine)
    	return 0;
    else {
        while ((caractere = (unsigned char)*chaine++))
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
        while ((caractere = (unsigned char)*chaine++))
        	hachage = 31*hachage + caractere;
        return hachage;
    }
}

/*
 * Function to Genereate First Hash
 */
int HashFunc1(int key, int size) {
    return key % size;
}
/*
 * Function to Genereate Second Hash
 */
int HashFunc2(int key, int size) {
    return (key * size - 1) % size;
}
/*
 * Function to Initialize Table
 */
struct HashTable *initializeTable(int size) {
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
 * Function to Find Element from the table
 */
int Find(int key, struct HashTable *htable) {
    int hashVal = HashFunc1(key, htable->size);
    int stepSize = HashFunc2(key, htable->size);
    while (htable->table[hashVal].info != Empty
            && htable->table[hashVal].element != key) {
        hashVal = hashVal + stepSize;
        hashVal = hashVal % htable->size;
    }
    return hashVal;
}
/*
 * Function to Insert Element into the table
 */
void Insert(int key, struct HashTable *htable) {
    int pos = Find(key, htable);
    if (htable->table[pos].info != Legitimate) {
        htable->table[pos].info = Legitimate;
        htable->table[pos].element = key;
    }
}
/*
 * Function to Rehash the table
 */
struct HashTable *Rehash(struct HashTable *htable) {
	int i;
    int size = htable->size;
    struct HashNode *table = htable->table;
    htable = initializeTable(2 * size);
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
        int value = htable->table[i].element;
        if (!value)
            printf("Position: %d Element: Null\n", (i + 1));
        else
            printf("Position: %d Element: %d\n", (i + 1), value);
    }
}
/*
 * test du fonctionnement
 */
int test_hachage() {
    int value, size, pos, i = 1;
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
            htable = initializeTable(size);
            break;
        case 2:
            if (i > htable->size) {
                printf("Table is Full, Rehash the table\n");
                continue;
            }
            printf("Enter element to be inserted: ");
            scanf("%d", &value);
            Insert(value, htable);
            i++;
            break;
        case 3:
            Retrieve(htable);
            break;
        case 4:
            htable = Rehash(htable);
            break;
        case 5:
            exit(0);
        default:
            printf("\nEnter correct option\n");
        }
    }
    return 0;
}
