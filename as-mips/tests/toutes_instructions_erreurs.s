.directive_non_valide_dans_init
.text
ODD
ADD
ADD $0,$4,$13, $19
ADD $0,$4
ADD $0,$4,,$13
ADD $0,17,$13
ADD $0,$sp4,$13
ADD 13,$4,$13
ADD toti,$4,$13
ADDI $1, $3, 21,22
ADDI $1, nombre
ADDI $1, nombre, $9
ADDI nombre, $10, $2
ADDI nombre, $10,( $2
SUB $2,$5,$6,$4
SUB $2, $5
SUB $2, $5 "$7
SUB 31, $4, $7
SUB 31, $5
tata:MULT $7,$8,$9
MULT
MULT $3
MULT 10,$7
MULT $7, 10
MULT 10, 12
DIV $zero
DIV $a0, $a10, $a5
AND $11,$12,$13, $14
AND $11, $12
AND $12 -$13
AND -12 13 $5
OR $12, $4
OR $1
OR $1, $2, $3, $4
OR 11
XOR $17,$18,$19,$20
XOR $1, $1
XOR XOR
XOR etiquette:
XOR ()
XOR -
XOR +
#commentaire XOR $12, $13, $14
ROTR $20, $21, 0, $3
ROTR $20, $21
ROTR $20, toto, $toto
SLL $22, $23, 12, tata
SLL tata, $24, $25 
SRL $26, $27, tata, ()
SRL $26, $27, 30, .word 12
SLT $28,$29,$30, $31
SLT $28, 14
LW $31, 0x60,$10
LW $31, $1, $2
LW $4, $5(32) #Inversion registre nombre non detectée
LW $31, lunchtime($10) #commentaire
LW $31, lunchtime($10)
SW $zero, 45($a0)($a1)
MFHI $s2, $3
MFHI $2, 90
MFHI
BEQ 
BEQ $gp
BNE $ra, $t9, nombre, nombre
BGTZ $23, 14, 30
BLEZ $t5
J 0x3847, 90
J
J $7
NOP NOP
NOP 0xa32
NOP $21 #Affiche mauvais type au lieu de opérande en trop
NOP #commentaire
MOVE $1, $2, $4
MOVE $1, $2 NOP #Ne détecte pas le NOP en trop
.data
.byte ""
.byte 0x1A,0FFFFFFFF
.word 32000, ""
.word -1000,-0400,-0x1999
.space 15
.asciiz "test" #La détection de lexeme de type chaine de caractère n'est pas encore active
.asciiz "", "\"""
.bss
.space -3 #Valeur négative acceptée
.space "3"
.text
seb:ADD $1, $2, $33
.space 12
