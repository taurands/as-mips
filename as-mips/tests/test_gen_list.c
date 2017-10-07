#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <gen_list.h>


void list_with_ints();
void list_with_strings();
 
void free_string(void *data);
 
int main(int argc, char *argv[])
{
  printf("Loading int demo...\n");
  list_with_ints();
  list_with_strings();

  return 0;
}
 
void list_with_ints()
{
  int numbers = 10;
  printf("Generating list with the first %d positive numbers...\n", numbers);
 
  int i;
  Liste_t list;
  initialiseListe(&list, sizeof(int), NULL);
 
  for(i = 1; i <= numbers; i++) {
    ajouteElementFinListe(&list, &i);
  }

  ElementListe_t *elementCourant_p = list.debut_liste_p;
  while (elementCourant_p) {
	  printf("%d\n",*(int *)elementCourant_p->donnees_p);
	  elementCourant_p = elementCourant_p->suivant_p;
  }
 
  detruitListe(&list);
  printf("Successfully freed %d numbers...\n", numbers);
}
 
void list_with_strings()
{
  int numNames = 5;
  const char *names[] = { "David", "Kevin", "Michael", "Craig", "Jimi" };
 
  int i;
  Liste_t list;
  initialiseListe(&list, sizeof(char *), free_string);
 
  char *name;
  for(i = 0; i < numNames; i++) {
	name = (char *)malloc(sizeof(*name)*(1+strlen(names[i])));
    strcpy(name, names[i]);
    ajouteElementFinListe(&list, &name);
  }
 
  ElementListe_t *elementCourant_p = list.debut_liste_p;
  while (elementCourant_p) {
	  printf("%s\n",*(char **)elementCourant_p->donnees_p);
	  elementCourant_p = elementCourant_p->suivant_p;
  }
 
  detruitListe(&list);
  printf("Successfully freed %d strings...\n", numNames);
}
 
void free_string(void *data)
{
  free(*(char **)data);
}
