
# allons au ru


.set noreorder
.text
    NOP
    Lw $t0, lunchtime
    LW $6, -0x200($7)
    ADDI $t1,$zero,8

boucle:
    BEQ $t0 , $t1 , byebye
    addi $t1 , $t1 , 1
    J boucle
    BLT $at, $sp, boucle
byebye:
    JAL viteviteauru

.data
lunchtime : .word 12
	.word menu

.bss
.space 5,2
menu:.space 24
