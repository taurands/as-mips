
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
#include <table.h>

/**
 * @param exec Name of executable.
 * @return Nothing.
 * @brief Print usage.
 *
 */
void print_usage( char *exec )
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
int main(int argc, char *argv[])
{

    unsigned int nbLignes 	= 0;
    unsigned int nbEtiquettes = 0;
    unsigned int nbInstructions = 0;

    char         *file 	= NULL;
    
    struct Table_s *tableDefinitionInstructions_p=NULL;
    struct Table_s *tableDefinitionRegistres_p=NULL;
    struct Table_s *tableEtiquettes_p=NULL;
    struct Liste_s *listeLexemes_p=NULL;
    struct Liste_s *listeText_p=NULL;
    struct Liste_s *listeData_p=NULL;
    struct Liste_s *listeBss_p=NULL;

    if (argc != 2) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    file = argv[argc-1];
    if (NULL == file) {
        fprintf(stderr, "Pas de nom pour le fichier assembleur. Abandon du traitement.\n" );
        exit(EXIT_FAILURE);
    }

    listeLexemes_p=creer_liste((fonctionDestructeur *)detruitLexeme);
    listeText_p=creer_liste(NULL);
    listeData_p=creer_liste(NULL);
    listeBss_p=creer_liste(NULL);

    /* ---------------- effectue l'analyse lexicale  -------------------*/
    lex_load_file(file, listeLexemes_p, &nbLignes, &nbEtiquettes, &nbInstructions);

    /* ---------------- print the lexical analysis -------------------*/
    DEBUG_MSG("Le fichier source comporte %u lignes, %u étiquettes et %u instructions", nbLignes, nbEtiquettes, nbInstructions);
	visualisationListeLexemes(listeLexemes_p);

	/* Crée la table d'étiquettes pour pouvoir contenir toutes celles identifiées lors de l'analyse lexicale */
	tableEtiquettes_p=creeTable(nbEtiquettes, clefEtiquette, NULL);
	charge_def_instruction(&tableDefinitionInstructions_p, "src/dictionnaire_instruction.txt");
	afficher_table(tableDefinitionInstructions_p);
	charge_def_instruction(&tableDefinitionRegistres_p, "src/dictionnaire_registre.txt");
	afficher_table(tableDefinitionRegistres_p);

	/* effectue l'analyse syntaxique */
	analyse_syntaxe(listeLexemes_p, tableDefinitionInstructions_p, tableDefinitionRegistres_p, tableEtiquettes_p, listeText_p, listeData_p, listeBss_p);

	/* affiche les résultats de l'analyse syntaxique */
	afficher_table(tableEtiquettes_p);
	/* XXX afficher les trois listes de text, data et bss */

    /* ---------------- Free memory and terminate -------------------*/
	tableEtiquettes_p=detruitTable(tableEtiquettes_p);
    tableDefinitionInstructions_p=detruitTable(tableDefinitionInstructions_p);
    tableDefinitionRegistres_p=detruitTable(tableDefinitionRegistres_p);

	listeText_p=detruire_liste(listeText_p);
    listeData_p=detruire_liste(listeData_p);
    listeBss_p=detruire_liste(listeBss_p);
	listeLexemes_p=detruire_liste(listeLexemes_p);

	exit( EXIT_SUCCESS );
}

