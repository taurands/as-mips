
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
    
    Liste_t *lignesLexeme_p=NULL;
    Liste_t *lignesCode_p=NULL;
    /*
    Liste_t *listeEtiquettes=NULL;
    */
    TableHachage_t *tableEtiquettes_p;

    if ( argc != 2 ) {
        print_usage(argv[0]);
        exit( EXIT_FAILURE );
    }


    file  	= argv[argc-1];


    if ( NULL == file ) {
        fprintf( stderr, "Pas de nom pour le fichier assembleur. Abandon du traitement.\n" );
        exit( EXIT_FAILURE );
    }


    /* ---------------- do the lexical analysis -------------------*/
    lignesLexeme_p=lex_load_file( file, &nbLignes, &nbEtiquettes, &nbInstructions);

    /* ---------------- print the lexical analysis -------------------*/
    DEBUG_MSG("Le fichier source comporte %u lignes, %u étiquettes et %u instructions", nbLignes, nbEtiquettes, nbInstructions);
	visualisationLignesLexemes(lignesLexeme_p);

	Dictionnaire_t* mon_dictionnaire_p=chargeDictionnaire("src/dictionnaire_instruction.txt");

	tableEtiquettes_p=creeTable(nbEtiquettes, clefStr, destructionStr);

	lignesCode_p=analyseSyntaxe(lignesLexeme_p, mon_dictionnaire_p, tableEtiquettes_p);

	tableEtiquettes_p=detruitTable(tableEtiquettes_p);

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

	detruitListe(lignesCode_p);
	free(lignesCode_p);
    detruitListe(lignesLexeme_p);
    free(lignesLexeme_p);

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

