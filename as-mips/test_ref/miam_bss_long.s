# TEST_RETURN_CODE=PASS
# allons au ru


.set noreorder
.text
    Lw $t0 , menu
    LW $6, -0x200($7)
    ADDI $t1,$zero,8
    LUI $t0, menu

boucle:
    BEQ $t0 , $t1 , byebye
    NOP
    addi $t1 , $t1 , 1
    J boucle 
    NOP
byebye:
    JAL viteviteauru

.data
lunchtime: .word 12
.word menu

.bss
.space 0x87654321
menu:.space 24
