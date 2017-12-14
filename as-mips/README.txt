Squelette de code de l'assembleur MIPS du projet info PHELMA.

Le répertoire est organisé ainsi : 
.
├── testing
│   ├── README.txt
│   └── simpleUnitTest.sh
├── test_ref
│   ├── copie_chaines.s
│   ├── donnees_simple.s
│   ├── miam.s
│   ├── mult.s
│   ├── erreur_lexicale.s
│   ├── erreur_syntaxique.s
│   ├── miam_bss_long.s
│   ├── tt_instr_err_limite.s
│   ├── tt_instr_valides_limites.s
│   ├── copie_chaines.res
│   ├── donnees_simple.res
│   ├── miam.res
│   ├── mult.res
│   ├── erreur_lexicale.res
│   ├── erreur_syntaxique.res
│   ├── miam_bss_long.res
│   ├── tt_instr_err_limite.res
│   ├── tt_instr_valides_limites.res
│   ├── copie_chaines.obj
│   ├── donnees_simple.obj
│   ├── miam.obj
│   └── mult.obj
├── tests
│   ├── vide.s
│   └── espaces.s 
├── src
│   ├── main.c
│   ├── listage.c
│   ├── reloc.c
│   ├── syn.c
│   ├── lex.c
│   ├── dico.c
│   ├── table.c
│   ├── liste.c
│   ├── str_utils.c
│   ├── dictionnaire_registres.txt
│   ├── dictionnaire_instructions.txt
│   └── dictionnaire_pseudo.txt
├── include
│   ├── reloc.h
│   ├── listage.h
│   ├── syn.h
│   ├── lex.h
│   ├── dico.h
│   ├── table.h
│   ├── liste.h
│   ├── str_utils.h
│   ├── notify.h
│   ├── lex.h
│   └── global.h
├── doc
│   └── livrables
│       ├── Livrable 4 - 2A SICOM - BERTRAND - TAURAND.pdf
│       ├── Livrable 3 - 2A SICOM - BERTRAND - TAURAND.pdf
│       ├── Livrable 2 - 2A SICOM - BERTRAND - TAURAND.pdf
│       └── Livrable 1 - 2A SICOM - BERTRAND - TAURAND.pdf
├── README.txt
├── Makefile
└── Doxyfile

- tests et test_ref : contiennent les fichiers pour tester le programme 
- testing : contient un script de vérification du bon fonctionnement du programme par rapport aux fichiers de référence contenus dans test_ref
- src : qui contient le code C de l'assembleur ainsi que les trois dictionnaires de registres, d'instructions et de pseudo-instructions
- include : qui contient toutes les définitions de types et prototypes de votre programme. Lisez attentivement les headers avant de commencer à coder et de les modifier
- doc : contient la documentation et le répertoire ou stocker les rapports de livrable.
- README.txt : ce document
- Makefile pour compiler soit en mode debug (afficher les traces du programme) soit en mode release (produit final)
- Doxyfile : fichier de configuration du générateur automatique de documentation doxygen

--- pour compiler le code en mode debug (il créé l'exécutable 'as-mips' qui affiche les traces)
$ make debug 



--- pour l'executer 
$ ./as-mips FICHIER.S



--- pour compiler le code en mode release (il créé l'exécutable 'as-mips' qui n'affiche pas les traces. En tant que développeur, vous utiliserez très rarement cette commande)
$ make release 


--- Génération de l'archive 

Tapez "make archive". Vous aurez produit un fichier se terminant par "tgz". C'est l'archive 
que vous nous enverrez par email. 



--- Documentation du code

Si vous voulez documenter votre code, regardez la syntaxe appropriée pour 
les tags Doxygen : 

http://www.stack.nl/~dimitri/doxygen/docblocks.html

Une fois votre code commente, tapez "make documentation" (nous vous fournissons
un Doxyfile qui ira bien pour ce que vous avez a faire). 

Note : il faut avoir installé le package doxygen. Sous Ubuntu / Debian, tapez : 
sudo apt-get install doxygen
