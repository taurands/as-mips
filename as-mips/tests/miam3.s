# allons au ru

etiquette_non_autorisee_dans_section_init:
.set
NOP
.byte 12
.set noreorder
.text
    Lw $t0 , lunchtime
    LW $6, -0x200($7)
    ADDI $t1,$zero,8
boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    B boucle 
    NOP
byebye:
    JAL viteviteauru
.data
titi:
titi:
	.byte 255, +4, 077, -066, 0xAA, -0xFF # un petit commentaire pour voir
.SPACE 512

lunchtime :toto:tutu:.word 12
.word 9,byebye
            .asciiz 7, "", "\"\"", "a a a"
            .asciiz toto
            .byte 209,-127
.bss
    .space 12,6, # commentaire
    .space 12 13 titi
    .space 12,
