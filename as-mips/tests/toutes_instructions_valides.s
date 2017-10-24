.data
.space 6
.byte 1, 2
mot1:.word 1, nombre1
.byte 1
mot2:.word nombre
nombre:.word 1
.space 1, 2
.text
ADD $0,$4,$13
instruc1:ADDI $1, $3, 21
ADDI $1, $3, nombre
SUB $2,$5,$6
MULT $7,$8
DIV $9,$10
AND $11,$12,$13
OR $14,$15,$16
XOR $17,$18,$19
ROTR $20, $21, 0
ROTR $20, $21, 31
ROTR $20, $21, toto
SLL $22, $23, 12
SLL $24, $25, tata
SRL $26, $27, titi
SRL $26, $27, 30
SLT $28,$29,$30
LW $31, 0x60($10)
SW $zero, 45($a0)
LUI $t1, 12
tata:MFHI $s2
MFLO $k1
BEQ $1, $3, 2398
BEQ $gp, $sp, nombre
BNE $ra, $t9, nombre
BGTZ $23, 14
BLEZ $t5, 50
J 0x3847
J nombre
JAL 0x3847
JR $zero
NOP
MOVE $1, $2
#NEG $3, $4
#LI $sp, 100
#LI $sp, nmbre
#BLT $10, $11, 0x1356
.data
.byte 45
.byte 0x1A,023
.word 32000
.word -1000,-0400,-0x1
.space 15
.asciiz "test"
.asciiz "", "\"""
.bss
titi:.space 12
.space 05,0xF
