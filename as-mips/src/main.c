
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
#include <syntaxe.h>
#include <str_utils.h>
#include <table_hachage.h>

/**
 * @param exec Name of executable.
 * @return Nothing.
 * @brief Print usage.
 *
 */
void print_usage( char *exec ) {
    fprintf(stderr, "Usage: %s file.s\n",
            exec);
}



/**
 * @param argc Number of arguments on the command line.
 * @param argv Value of arguments on the command line.
 * @return Whether this was a success or not.
 * @brief Main entry point for MIPS assembler.
 *
 */
int main ( int argc, char *argv[] ) {

    unsigned int nbLignes 	= 0;
    unsigned int nbEtiquettes = 0;
    unsigned int nbInstructions = 0;

    char         *file 	= NULL;
    
    TableHachage_t *tableDefinitionInstructions_p=NULL;
    TableHachage_t *tableDefinitionRegistres_p=NULL;
    TableHachage_t *tableEtiquettes_p=NULL;
    Liste_t *listeLexemes_p=NULL;
    Liste_t *listeText_p=NULL;
    Liste_t *listeData_p=NULL;
    Liste_t *listeBss_p=NULL;

    if ( argc != 2 ) {
        print_usage(argv[0]);
        exit( EXIT_FAILURE );
    }


    file  	= argv[argc-1];


    if ( NULL == file ) {
        fprintf( stderr, "Pas de nom pour le fichier assembleur. Abandon du traitement.\n" );
        exit( EXIT_FAILURE );
    }

    listeLexemes_p=creeListe(sizeof(Lexeme_t), (fonctionDestructeur *)detruitLexeme);
    listeText_p=creeListe(sizeof(Instruction_t), NULL);
    listeData_p=creeListe(sizeof(Donnee_t), NULL);
    listeBss_p=creeListe(sizeof(Donnee_t), NULL);

	Dictionnaire_t* mon_dictionnaire_p=chargeDictionnaire("src/dictionnaire_instruction.txt");

    /* ---------------- do the lexical analysis -------------------*/
    lex_load_file(file, listeLexemes_p, &nbLignes, &nbEtiquettes, &nbInstructions);

    /* ---------------- print the lexical analysis -------------------*/
    DEBUG_MSG("Le fichier source comporte %u lignes, %u étiquettes et %u instructions", nbLignes, nbEtiquettes, nbInstructions);
	visualisationListeLexemes(listeLexemes_p);


	tableEtiquettes_p=creeTable(nbEtiquettes, clefEtiquette, NULL);
	/* */
	analyseSyntaxe(listeLexemes_p, mon_dictionnaire_p, tableEtiquettes_p, listeText_p, listeData_p, listeBss_p);
	/* */
	tableEtiquettes_p=detruitTable(tableEtiquettes_p);
    tableDefinitionInstructions_p=detruitTable(tableDefinitionInstructions_p);
    tableDefinitionRegistres_p=detruitTable(tableDefinitionRegistres_p);

	/*
	DEBUG_MSG("index de ADD: %d",indexDictionnaire(mon_dictionnaire_p, "ADD"));
	DEBUG_MSG("index de XOR: %d",indexDictionnaire(mon_dictionnaire_p, "XOR"));
	DEBUG_MSG("index de TOTO: %d",indexDictionnaire(mon_dictionnaire_p, "TOTO"));
	DEBUG_MSG("index de 0: %d",indexDictionnaire(mon_dictionnaire_p, "0"));
	DEBUG_MSG("index de Z: %d",indexDictionnaire(mon_dictionnaire_p, "Z"));
	*/

    /* ---------------- Free memory and terminate -------------------*/

	effaceContenuDictionnaire(mon_dictionnaire_p);
	free(mon_dictionnaire_p);

	listeText_p=detruitListe(listeText_p);
    listeData_p=detruitListe(listeData_p);
    listeBss_p=detruitListe(listeBss_p);
	listeLexemes_p=detruitListe(listeLexemes_p);

    /*
    printf("Hachage B null : %x\n", hashBernstein(NULL));
    printf("Hachage B vide : %x\n", hashBernstein(""));
    printf("Hachage B t : %x\n", hashBernstein("t"));
    printf("Hachage B to : %x\n", hashBernstein("to"));

    printf("Hachage KR2 null : %x\n", hashKR2(NULL));
    printf("Hachage KR2 vide : %x\n", hashKR2(""));
    printf("Hachage KR2 t : %x\n", hashKR2("t"));
    printf("Hachage KR2 to : %x\n", hashKR2("to"));
    */

    /* test_hachage(); */
    exit( EXIT_SUCCESS );
}

