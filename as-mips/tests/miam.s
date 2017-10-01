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
lunchtime : .word 12
