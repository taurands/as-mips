# allons au ru
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
	.byte 255, +4, 077, -066, 0xAA, -0xFF # un petit commentaire pour voir
.space 512

lunchtime :toto:toto:.word 12
.word 9,byebye
            .asciiz "", "\"\"", "a a a"
            .byte 209,-127
.bss
    .space 12,6,7
