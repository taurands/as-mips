
# allons au ru


.set noreorder
.data
.word dollar, menu, quid
.word dollar, menu, quid
.word dollar, menu, quid

.text
    NOP
    Lw $t0, lunchtime
    LW $6, -0x200($7)
    SW $t1, menu
    ADDI $t1,$zero,8

boucle:
    BEQ $t0 , $t1 , byebye
    addi $t1 , $t1 , 1
    J boucle
    BLT $at, $sp, boucle
byebye:
toto: tata:    JAL viteviteauru
	JAL encoreunindefini

.data
lunchtime : .word 12
	.word menu, quid

.bss
.space 5,2
menu:.space 24
