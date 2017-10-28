/**
 * @file lex.c
 * @author BERTRAND Antoine TAURAND Sébastien sur base de François Portet <francois.portet@imag.fr>
 * @brief Lexical analysis routines for MIPS assembly syntax.
 *
 * These routines perform the analysis of the lexeme of an assembly source code file.
 */

#define _POSIX_C_SOURCE 200112L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <global.h>
#include <notify.h>
#include <str_utils.h>
#include <lex.h>
#include <liste.h>
#include <table.h>

/**
 * @enum Etat_lex_e
 * @brief Constantes de définition des états de la machine à états finis d'analyse lexicale
 *
 */
enum Etat_lex_e {
	ERREUR=L_ERREUR,

	SYMBOLE=L_SYMBOLE,
	INSTRUCTION=L_INSTRUCTION,
	DIRECTIVE=L_DIRECTIVE,
	ETIQUETTE=L_ETIQUETTE,

	REGISTRE=L_REGISTRE,
	VIRGULE=L_VIRGULE,
	PARENTHESE_OUVRANTE=L_PARENTHESE_OUVRANTE,
	PARENTHESE_FERMANTE=L_PARENTHESE_FERMANTE,

	NOMBRE=L_NOMBRE,

	CHAINE=L_CHAINE,

	COMMENTAIRE=L_COMMENTAIRE,
	FIN_LIGNE=L_FIN_LIGNE,

	DECIMAL,
	OCTAL,
	HEXADECIMAL,

	DEBUT_CHAINE,

	INIT,					/**< Etat initial */
	MOINS,
	PLUS,
	DEBUT_HEXADECIMAL,		/**< On a lu "0x" et on attend le premier caractère hexadécimal */
	DECIMAL_ZERO,			/**< On a lu un "0" */
	POINT					/**< On a lu un point */
};

/**
 * @param etat etat de la machine à états finis lexicale
 * @return chaine de caractères contenant le nom de l'état
 * @brief Cette fonction permet de donne le mon correspondant à un état
 *
 */	
char *nature_lex_to_str(enum Nature_lexeme_e nature)
{
	switch(nature) {
		case L_ERREUR:					return "ERREUR";
		case L_SYMBOLE:					return "SYMBOLE";
		case L_INSTRUCTION:				return "INSTRUCTION";
		case L_DIRECTIVE:				return "DIRECTIVE";
		case L_ETIQUETTE:				return "ETIQUETTE";
		case L_REGISTRE:				return "REGISTRE";
		case L_VIRGULE:					return "VIRGULE";
		case L_PARENTHESE_OUVRANTE:		return "PARENTHESE_OUVRANTE";
		case L_PARENTHESE_FERMANTE:		return "PARENTHESE_FERMANTE";
		case L_NOMBRE:					return "NOMBRE";
		case L_CHAINE:					return "CHAINE";
		case L_COMMENTAIRE:				return "COMMENTAIRE";
		case L_FIN_LIGNE:				return "FIN_LIGNE";
		default :
			ERROR_MSG("Erreur de résolution du nom de la nature du lexème... Il manque donc au moins un nom à rajouter pour %d", nature);
	}
	return NULL;
}

/**
 * @param etat etat de la machine à états finis lexicale
 * @return chaine de caractères contenant le nom de l'état
 * @brief Cette fonction permet de donne le mon correspondant à un état
 *
 */
/*
char *etat_lex_to_str(enum Etat_lex_e etat)
{
	switch(etat) {
		case INIT:					return "INIT";
		case POINT:					return "POINT";
		case VIRGULE:				return "VIRGULE";
		case NOMBRE:				return "NOMBRE";
		case DECIMAL:				return "DECIMAL";
		case HEXADECIMAL:			return "HEXADECIMAL";
		case OCTAL:					return "OCTAL";
		case CHAINE:				return "CHAINE";
		case MOINS:					return "MOINS";
		case PLUS:					return "PLUS";
		case REGISTRE:				return "REGISTRE";
		case SYMBOLE:				return "SYMBOLE";
		case INSTRUCTION:			return "INSTRUCTION";
		case DIRECTIVE:				return "DIRECTIVE";
		case ETIQUETTE:				return "ETIQUETTE";
		case COMMENTAIRE:			return "COMMENTAIRE";
		case DEBUT_HEXADECIMAL:		return "DEBUT_HEXADECIMAL";
		case DECIMAL_ZERO:			return "DECIMAL_ZERO";
		case PARENTHESE_OUVRANTE:	return "PARENTHESE_OUVRANTE";
		case PARENTHESE_FERMANTE:	return "PARENTHESE_FERMANTE";
		case ERREUR:				return "ERREUR";
		case FIN_LIGNE:				return "FIN_LIGNE";
		default : 	ERROR_MSG("Erreur de résolution du nom de l'état %d de la machine à états finis lexicale... Il manque donc au moins un nom d'état à rajouter", etat);
	}
	return NULL;
}
*/

/**
 * @param etat Etat antérieur de la machine à états finis d'analyse lexicale
 * @param c caractère à analyser qui va faire évoluer l'état.
 * @return Etat après l'analyse du caractère.
 * @brief Cette fonction catégorise un lexème en analysant le(s) caractère(s) le composant.
 *
 */
enum Etat_lex_e machine_etats_finis_lexicale(enum Etat_lex_e etat, char c)
{

/**	@dot
 *	digraph Machine_Etat_Lex {
 *		concentrate=true;
 *		graph [label="\nMachine à états finis d'analyse lexicale"; fontname = "helvetica"; fontsize = 16;];
 *		edge [fontname = "helvetica"; fontsize = 10;];
 *		node [shape="circle";  fontname = "helvetica"; fontsize = 10;]; ERREUR; INIT;
 *		node [shape="ellipse";  fontname = "helvetica"; fontsize = 10;]; DECIMAL_ZERO; DEBUT_HEXADECIMAL; POINT; PLUS; MOINS;
 *		node [shape="ellipse"; fontname = "helvetica"; fontsize = 10; color=green;];
 *		rankdir=LR; // de gauche vers la droite
 * 		INIT -> DECIMAL [label="1 à 9"];
 *		INIT -> DECIMAL_ZERO [label="0"];
 *		INIT -> MOINS [label="-"];
 *		INIT -> PLUS [label="+"];
 *		INIT -> POINT [label="."];
 *		INIT -> VIRGULE [label=","];
 *		INIT -> COMMENTAIRE [label="#"];
 *		INIT -> REGISTRE [label="$"];
 *		INIT -> PARENTHESE_OUVRANTE [label="("];
 *		INIT -> PARENTHESE_FERMANTE [label=")"];
 *		INIT -> SYMBOLE [label="_alpha"];
 *		INIT -> ERREUR [label="sinon"];
 *
 *		COMMENTAIRE -> COMMENTAIRE;
 *		ERREUR -> ERREUR;
 *
 *		MOINS -> DECIMAL_ZERO [label="0"];
 *		MOINS -> DECIMAL [label="1 à 9"];
 *		MOINS -> ERREUR [label="sinon"];
 *		PLUS -> DECIMAL_ZERO [label="0"];
 *		PLUS -> DECIMAL [label="1 à 9"];
 *		PLUS -> ERREUR [label="sinon"];
 *		VIRGULE -> ERREUR;
 *		PARENTHESE_OUVRANTE -> ERREUR;
 *		PARENTHESE_FERMANTE -> ERREUR;
 *
 *		DECIMAL_ZERO -> OCTAL [label="0 à 7---"];
 *		DECIMAL_ZERO -> DEBUT_HEXADECIMAL [label="x ou X"];
 *		DECIMAL_ZERO -> ERREUR [label="sinon"];
 *
 *		DEBUT_HEXADECIMAL -> HEXADECIMAL [label="hexa"];
 *		DEBUT_HEXADECIMAL -> ERREUR [label="sinon"];
 *
 *		HEXADECIMAL -> HEXADECIMAL [label="hexa"];
 *		HEXADECIMAL -> ERREUR [label="sinon"];
 *
 *		DECIMAL -> DECIMAL [label="0 à 9"];
 *		DECIMAL -> ERREUR [label="sinon"];
 * 
 *		OCTAL -> OCTAL [label="0 à 7"];
 *		OCTAL -> ERREUR [label="sinon"];
 *		
 *		SYMBOLE -> SYMBOLE [label="_alphanum"];
 *		SYMBOLE -> ETIQUETTE [label=":"];
 *		SYMBOLE -> ERREUR [label="sinon"];
 * 
 *		POINT -> DIRECTIVE [label="alpha"];
 *		POINT -> ERREUR [label="sinon"];
 * 
 *		ETIQUETTE -> ERREUR;
 * 
 *		DIRECTIVE -> DIRECTIVE [label="alpha"];
 *		DIRECTIVE -> ERREUR [label="sinon"];
 *
 *		REGISTRE -> REGISTRE [label="alphanum"];
 *		REGISTRE -> ERREUR [label="sinon"];
 *	}
 *  @enddot
 *
 * alpha indique un caractère compris de 'A' à 'Z' ou compris de 'a' à 'z'\n
 * \_alpha indique un caractère '\_' ou compris de 'A' à 'Z' ou compris de 'a' à 'z'\n
 * \_alphanum indique un caractère '\_' ou compris de 'A' à 'Z' ou compris de 'a' à 'z' ou compris de '0' à '9'\n
 * hexa indique un caractère compris de '0' à '9' ou compris de 'A' à 'F' ou compris de 'a' à 'f'\n
 * \n
 * Seuls les états verts correspondent à des états de sortie corrects en fin de lexème.
 *
 */

	switch(etat) {
		case INIT:
			if (isdigit(c)) etat=(c=='0')? DECIMAL_ZERO : DECIMAL; 	
			else if (c=='-') etat=MOINS;
			else if (c=='+') etat=PLUS;
			else if (c=='.') etat=POINT;
			else if (c==',') etat=VIRGULE;
			else if (c=='#') etat=COMMENTAIRE;
			else if (c=='"') etat=DEBUT_CHAINE;
			else if (c=='$') etat=REGISTRE;
			else if (c=='(') etat=PARENTHESE_OUVRANTE;
			else if (c==')') etat=PARENTHESE_FERMANTE;
			else if (isalpha(c) || (c=='_')) etat=SYMBOLE;
			else etat=ERREUR;
			break;
		
		case VIRGULE:
		case PARENTHESE_OUVRANTE:
		case PARENTHESE_FERMANTE:
		case MOINS:
		case PLUS:
			if (isdigit(c)) etat=(c=='0')? DECIMAL_ZERO : DECIMAL;
			else etat=ERREUR; /* Ces états ont été standardisés et ne doivent pas être suivis d'autre caractère à la suite */
			break;
		
		case DECIMAL_ZERO: /* On va chercher si le prochain caractere est 'x' pour savoir si la valeur sera un nombre hexadecimal ou non */
			if(c=='x' || c=='X') etat=DEBUT_HEXADECIMAL;
			else if (isdigit(c) && c<'8') etat=OCTAL;
			else etat=ERREUR;
			break;
	
		case DEBUT_HEXADECIMAL:
			if(isxdigit(c)) etat=HEXADECIMAL;
			else etat=ERREUR;
			break;  

		case DEBUT_CHAINE:
			if (c=='"') etat=CHAINE;
			break; /* Pas d'erreur possible ici */
	
		case HEXADECIMAL: 
			if(!isxdigit(c)) etat=ERREUR;
			break;

		case DECIMAL: 
			if (!isdigit(c)) etat=ERREUR;
			break;

		case OCTAL: 
			if (!(isdigit(c)) && (c<'8')) etat=ERREUR;
			break;
	
		case SYMBOLE:
			if (c==':') etat=ETIQUETTE;
			else if (!(isalnum(c) || (c=='_'))) etat=ERREUR;
			break;

		case POINT:
			if (isalpha(c)) etat=DIRECTIVE;
			else etat=ERREUR;
			break;
			
		case ETIQUETTE:
			etat=ERREUR; /* Il ne doit pas y avoir d'autre caractère après le ':' dans une étiquette  */
			break;
	
		case DIRECTIVE:
			if (!isalpha(c)) etat=ERREUR;
			break;

		case REGISTRE:
			if (!isalnum(c)) etat=ERREUR;
		    break;	
		    								
		default :
			/* pour COMMENTAIRE et ERREUR tous les caractères suivants sont sans importance */
			;
	}		    
	return etat;
}

/**
 * @param ligne chaine contenant la ligne du fichier source assembleur à analyser
 * @param liste_lexemes_p pointeur sur la liste de lexèmes servant à stoquer les lexèmes qui seront extraits dans l'analyses
 * @param num_ligne le numéro de la ligne dans le fichier source
 * @param nb_etiquettes_p un pointeur sur le nombre d'étiquette que l'on trouvera dans l'analyse
 * @param nb_instructions_p un pointeur sur le nombre d'instructions que l'on trouvera à priori dans l'analyse
 * @return Rien, si ce n'est la liste de lexème mise à jour ainsi les nombres d'étiquettes et d'instructions
 * @brief Cette fonction fait l'analyse lexicale d'un ligne ayant subi une "standardisation" (pre-processing)
 */
void lex_read_line(char *ligne, struct Liste_s *liste_lexemes_p, unsigned int num_ligne, unsigned int *nb_etiquettes_p, unsigned int *nb_instructions_p)
{
    struct Lexeme_s *lexeme_p;

	enum Etat_lex_e etat;
	char c;
	int i;
	int debutLigne = 1;
	
    char *seps = " ";
    char *token = NULL;
    char save[2*STRLEN];

    /* copy the input line so that we can do anything with it without impacting outside world*/
    memcpy( save, ligne, 2*STRLEN );

    /* get each token*/
    for( token = strtok( save, seps ); NULL != token; token = strtok( NULL, seps )) {
    	etat=INIT;
		i=0; /* On se place sur le premier caractère du token en cours */
		while('\0'!=(c=token[i])) { /* Tant que la ligne n'est pas terminee */
			etat=machine_etats_finis_lexicale(etat,c);
			i++;
        }
        
        if (etat==COMMENTAIRE) {
         	char * diese_p=strchr(ligne,'#');

         	lexeme_p = malloc(sizeof(*lexeme_p));
         	if (!(lexeme_p->data = (char *)malloc(strlen(diese_p)+1*sizeof(char)))) ERROR_MSG("Impossible de dupliquer le contenu du nouveau commentaire");
    		strcpy(lexeme_p->data, diese_p);
         	lexeme_p->nature=L_COMMENTAIRE;
         	lexeme_p->ligne=num_ligne;
         	ajouter_fin_liste(liste_lexemes_p, lexeme_p);
         	break;
        }


        else {
        	/* en cas d'états intermédiaires que l'on n'attend pas en sortie, on passe en erreur */
        	switch(etat) {
        		case DECIMAL_ZERO:
        			etat=DECIMAL;
        			break;

    			case MOINS:
    			case PLUS:
    				etat=ERREUR; /* les signes doivent être accolés à un nombre */
    				break;
        			
    			case DEBUT_HEXADECIMAL:
    				etat=ERREUR;
    				break;
    				
    			case DEBUT_CHAINE:
    				etat=ERREUR;
    				break;

				case POINT:
					etat=ERREUR;
					break;
					
				case INIT:
					etat=ERREUR;
					break;
					
				default :
					;		
        	}
        	if (etat==ETIQUETTE) {
        		token[strlen(token)-1]='\0'; /* enlève des deux points à la fin de l'étiquette */
        		(*nb_etiquettes_p)++;
        	}
         	else { /* Tout symbole en début de ligne précédé éventuellement de une ou plusieurs étiquettes est une instruction */
        		if (debutLigne && etat==SYMBOLE) {
        			etat=INSTRUCTION;
        			token=strupr(token);
        			(*nb_instructions_p)++;
        		}
            	if (etat==DIRECTIVE) {
            		token=strlwr(token);
            	}
            	if ((etat==DECIMAL) || (etat==OCTAL) || (etat==HEXADECIMAL)) {
            		etat=NOMBRE;
            	}
        		debutLigne=0;
        	}
        	lexeme_p = malloc(sizeof(*lexeme_p));

         	if (!(lexeme_p->data = (char *)malloc(strlen(token)+1*sizeof(char)))) ERROR_MSG("Impossible de dupliquer le contenu du nouveau lexeme");
    		strcpy(lexeme_p->data, token);
         	lexeme_p->nature=etat;
         	lexeme_p->ligne=num_ligne;
         	ajouter_fin_liste(liste_lexemes_p, lexeme_p);
    	}
    }

    /* rajoute un lexeme marqueur de fin de ligne */
    lexeme_p = malloc(sizeof(*lexeme_p));

    lexeme_p->data=NULL;
    lexeme_p->nature=L_FIN_LIGNE;
    lexeme_p->ligne=num_ligne;
    ajouter_fin_liste(liste_lexemes_p, lexeme_p);
}

/**
 * @param nom_fichier Le nom du fichier source asssembleur
 * @param liste_lexemes_p Un pointeur sur une liste (générique) de lexèmes
 * @param nb_lignes_p Le pointeur vers le compteur de nombre de lignes
 * @param nb_etiquettes_p Le pointeur vers le compteur de nombre d'étiquettes
 * @param nb_instructions_p Le pointeur vers le compteur de nombre d'instructions
 * @return Rien. Si ce n'est la liste générique de lexèmes ainsi que les nombres de lignes, d'instructions et d'étiquettes
 * @brief Cette fonction charge le fichier assembleur et effectue sont analyse lexicale
 *
 */
void lex_load_file(char *nom_fichier, struct Liste_s *liste_lexemes_p, unsigned int *nb_lignes_p, unsigned int *nb_etiquettes_p, unsigned int *nb_instructions_p)
{

    FILE        *fp   = NULL;
    char         line[STRLEN]; /* original source line */
    char         res[2*STRLEN]; /* standardised source line, can be longeur due to some possible added spaces*/
    
    fp = fopen( nom_fichier, "r" );
    if ( NULL == fp ) {
        /*macro ERROR_MSG : message d'erreur puis fin de programme ! */
        ERROR_MSG("Impossible d'ouvrir le fichier \"%s\". Abandon du traitement",nom_fichier);
    }
    *nb_lignes_p = 0;

    while(!feof(fp)) {
        /*read source code line-by-line */
        if ( NULL != fgets( line, STRLEN-1, fp ) ) {
            if (strlen(line)) if (line[strlen(line)-1] == '\n') line[strlen(line)-1] = '\0';  /* remove final '\n' */
            (*nb_lignes_p)++;

            if ( 0 != strlen(line) ) {
                lex_standardise( line, res );
                lex_read_line( res, liste_lexemes_p, *nb_lignes_p, nb_etiquettes_p, nb_instructions_p );
            }
        }       
    }

    fclose(fp);
    if (!*nb_lignes_p) WARNING_MSG("Attention, le fichier \"%s\" est vide", nom_fichier);
}

/**
 * @param in Input line of source code (possibly very badly written).
 * @param out Line of source code in a suitable form for further analysis.
 * @return nothing
 * @brief This function will prepare a line of source code for further analysis.
 *
 * Aucun changement de casse n'est effectué à ce niveau
 */
void lex_standardise(char* in, char* out)
{
    unsigned int i, j;
    const char * ESPACE_AVANT = "#$,-()+";
    const char * PAS_ESPACE_AVANT = ":";
    const char * ESPACE_APRES = ":,()";
    const char * PAS_ESPACE_APRES = ".-+";

    DEBUG_MSG("in  = \"%s\"", in);
    
    for ( i= 0, j= 0; i < strlen(in); i++ ) {
        if (strchr(ESPACE_AVANT, in[i])) {
        	if (j>0) if (out[j-1] != ' ') out[j++]=' '; /* rajoute un espace avant les symboles '$', ',', '(', ')', '-', '+' # si pas en début de ligne et pas déjà un espace. */
			out[j++]=in[i]; /* Puis, recopie le symbole */      

        	/* Si c'est un commentaire, puis recopie tous les caractères suivants jusqu'à la fin de la ligne sans faire de standardisation aval */
		    if ( in[i] == '#') for ( i++; i < strlen(in); i++ ) out[j++]=in[i];
        }
        else {
		    /* translate all spaces (i.e., tab) into simple spaces */
		    if (isblank((int) in[i])) {
		    	/* on recopie un espace sauf si on est en debut de ligne ou après un caractère contenu dans PAS_ESPACE_APRES l'interdisant */
				if ( j > 0 ) if (!strchr(PAS_ESPACE_APRES, out[j-1])) out[j++]=' ';
		    }
		    
		    /* otherwise copy as is */
		    else {
		    	out[j]=in[i];        	
				if (strchr(PAS_ESPACE_AVANT, out[j])) {
					while (j>0) {
						/* enlève les espaces avant un ':' */
						if (out[j-1] == ' ') out[(j--)-1]=in[i];
						else break;
					}
				}
				j++;
			}
        }
        /* Ajoute un espace après les caractères contenus dans ESPACE_APRES sauf s'il y en a déjà en */
        if (strchr(ESPACE_APRES, in[i])) if ( i+1 < strlen(in) ) if (!isblank((int) in[i+1])) out[j++]=' ';
    }
    out[j]='\0';
    DEBUG_MSG("out = \"%s\"", out);
}

/**
 * @param lexeme_p pointeur sur un lexeme à détruire
 * @return Rien
 * @brief Cette fonction permet de détuire et libérer le contenu d'un lexème
 *
 * Cela inclut en particulier la chaine représentation le contenu du lexème.
 * Ceci est nécessaire pour le mécanisque de gestion propre des liste génériques.
 */
void detruit_lexeme(void *lexeme_p)
{
	INFO_MSG("Lexeme: %p ... %s",Lexeme_p,((struct Lexeme_s *)Lexeme_p)->data);
	free(((struct Lexeme_s *)lexeme_p)->data);
	free(lexeme_p);
}

/**
 * @param lexeme_p pointeur sur un lexeme
 * @return Rien, si ce n'est l'affichage
 * @brief Cette fonction permet de visualiser le contenu d'un lexeme
 *
 */
void visualisation_lexeme(struct Lexeme_s * lexeme_p)
{
	printf("(%s|%s|%d)", nature_lex_to_str(lexeme_p->nature), lexeme_p->data, lexeme_p->ligne);
}

/**
 * @param liste_p pointeur sur une liste de (générique) de lexèmes
 * @return rien
 * @brief Cette fonction permet de visualiser le contenu d'une liste de lexeme
 *
 */
void visualisation_liste_lexemes(struct Liste_s * liste_p)
{
	struct NoeudListe_s * lexemeCourant_p=liste_p->debut_liste_p;

	while (lexemeCourant_p) {
		visualisation_lexeme((struct Lexeme_s *)lexemeCourant_p->donnee_p);
		if (((struct Lexeme_s *)lexemeCourant_p->donnee_p)->nature == L_FIN_LIGNE)
			printf("\n");
		else
			if (lexemeCourant_p->suivant_p) printf(", ");
		lexemeCourant_p=lexemeCourant_p->suivant_p;
	}
}

