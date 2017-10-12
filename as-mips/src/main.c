
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

    unsigned int 	nlines 	= 0;
    char         	 *file 	= NULL;
    
    Liste_t *lignesLexeme_p=NULL;

    if ( argc != 2 ) {
        print_usage(argv[0]);
        exit( EXIT_FAILURE );
    }


    file  	= argv[argc-1];


    if ( NULL == file ) {
        fprintf( stderr, "Pas de mon pour le fichier assembleur. Abandon du traitement.\n" );
        exit( EXIT_FAILURE );
    }


    /* ---------------- do the lexical analysis -------------------*/
    lignesLexeme_p=lex_load_file( file, &nlines);

    /* ---------------- print the lexical analysis -------------------*/
    DEBUG_MSG("Le fichier source comporte %d lignes",nlines);
	visualisationLignesLexemes(lignesLexeme_p);

	Dictionnaire_t* mon_dictionnaire_p=chargeDictionnaire("src/dictionnaire_instruction.txt");
	DEBUG_MSG("index de ADD: %d",indexDictionnaire(mon_dictionnaire_p, "ADD"));
	DEBUG_MSG("index de XOR: %d",indexDictionnaire(mon_dictionnaire_p, "XOR"));
	DEBUG_MSG("index de TOTO: %d",indexDictionnaire(mon_dictionnaire_p, "TOTO"));
	DEBUG_MSG("index de 0: %d",indexDictionnaire(mon_dictionnaire_p, "0"));
	DEBUG_MSG("index de Z: %d",indexDictionnaire(mon_dictionnaire_p, "Z"));
	effaceContenuDictionnaire(mon_dictionnaire_p);
	free(mon_dictionnaire_p);

    /* ---------------- Free memory and terminate -------------------*/
    detruitListe(lignesLexeme_p);
    free(lignesLexeme_p);

    exit( EXIT_SUCCESS );
}

