# TEST_RETURN_CODE=PASS


.data 
text1: .asciiz "j'adore les textes avec des accents ê é ǜ, des symboles $ µ % \\, des MAJUSCULES et du code assembleur .asciiz text1:", "\n"
.word 0xFFFFFFFF
.asciiz "est-ce que le .word est bien \"aligné\" ?" 
.word 0x00000000
