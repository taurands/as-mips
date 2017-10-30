
/**
 * @file main.c
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr> from François Cayre
 * @brief Main entry point for MIPS assembler.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <global.h>
#include <notify.h>
#include <lex.h>
#include <str_utils.h>
#include <syn.h>
#include <dico.h>
#include <table.h>

char NOM_DICO_INSTRUCTIONS[] = "src/dictionnaire_instructions.txt";
char NOM_DICO_REGISTRES[] = "src/dictionnaire_registres.txt";

/**
 * @param exec Name of executable.
 * @return Nothing.
 * @brief Print usage.
 *
 */
void print_usage (char *exec)
{
    fprintf(stderr, "Usage: %s file.s\n", exec);
}

/**
 * @param argc Number of arguments on the command line.
 * @param argv Value of arguments on the command line.
 * @return Whether this was a success or not.
 * @brief Main entry point for MIPS assembler.
 *
 */
int main (int argc, char *argv[])
{
	int code_retour=SUCCESS;

    unsigned int nb_lignes = 0;
    unsigned int nb_etiquettes = 0;
    unsigned int nb_instructions = 0;

    char *nom_fichier_asm = NULL;
    
    struct Table_s *table_def_instructions_p=NULL;
    struct Table_s *table_def_registres_p=NULL;
    struct Table_s *table_etiquettes_p=NULL;
    struct Liste_s *liste_lexemes_p=NULL;
    struct Liste_s *liste_text_p=NULL;
    struct Liste_s *liste_data_p=NULL;
    struct Liste_s *liste_bss_p=NULL;

    if (argc != 2) {
        print_usage(argv[0]);
        exit (EXIT_FAILURE);
    }
    nom_fichier_asm = argv[argc-1];
    if (NULL == nom_fichier_asm) {
        fprintf(stderr, "Pas de nom pour le fichier assembleur. Abandon du traitement.\n" );
        exit (EXIT_FAILURE);
    }

    do { /* Do While pour permettre de sortir proprement en cas de problème d'allocation mémoire */
    	if ((code_retour = creer_liste (&liste_lexemes_p, (fonctionDestructeur *)detruit_lexeme)))
    		break;
    	if ((code_retour = creer_liste (&liste_text_p, NULL)))
    		break;
    	if ((code_retour = creer_liste (&liste_data_p, NULL)))
    		break;
    	if ((code_retour = creer_liste (&liste_bss_p, NULL)))
    		break;

        /* ---------------- effectue l'analyse lexicale  -------------------*/
        lex_load_file(nom_fichier_asm, liste_lexemes_p, &nb_lignes, &nb_etiquettes, &nb_instructions);

        /* ---------------- print the lexical analysis -------------------*/
        DEBUG_MSG("Le fichier source comporte %u lignes, %u étiquettes et %u instructions", nb_lignes, nb_etiquettes, nb_instructions);
    	visualisation_liste_lexemes(liste_lexemes_p);

    	/* Crée la table d'étiquettes pour pouvoir contenir toutes celles identifiées lors de l'analyse lexicale */
    	if ((code_retour = creer_table(&table_etiquettes_p, nb_etiquettes, clefEtiquette, NULL)))
    		break;
    	if ((code_retour = charge_def_instruction(&table_def_instructions_p, NOM_DICO_INSTRUCTIONS)))
    		break;
    	if ((code_retour = charge_def_registre(&table_def_registres_p, NOM_DICO_REGISTRES)))
    		break;

    	/* effectue l'analyse syntaxique */
    	analyser_syntaxe(liste_lexemes_p, table_def_instructions_p, table_def_registres_p, table_etiquettes_p, liste_text_p, liste_data_p, liste_bss_p);

    	/* affiche les résultats de l'analyse syntaxique */
    	affiche_table_etiquette(table_etiquettes_p, "Table des étiquettes");
    	affiche_liste_instructions(liste_text_p, table_etiquettes_p, "Table des instructions de .text");
    	affiche_liste_donnee(liste_data_p, table_etiquettes_p, "Table des données de la section .data");
    	affiche_liste_donnee(liste_bss_p, table_etiquettes_p, "Table des données de la section .bss");
    } while (FALSE);


    /* ---------------- Libération de la mémoire allouée -------------------*/
	detruire_table (&table_etiquettes_p);
    detruire_table (&table_def_instructions_p);
    detruire_table (&table_def_registres_p);

	detruire_liste (&liste_text_p);
    detruire_liste (&liste_data_p);
    detruire_liste (&liste_bss_p);
	detruire_liste (&liste_lexemes_p);

	exit (code_retour);
}

