# TEST_RETURN_CODE=PASS


.data
text1: .asciiz "j'adore les textes avec des accents ê é ǜ, des symboles $ µ % \\, des MAJUSCULES et du code assembleur .asciiz text1:", "\n"
.word 0xFFFFFFFF
.asciiz "est-ce que le .word est bien \"aligné\" ?"
.word 0x00000000
.asciiz "","1","12","123","1234","12345","123\0456"
.asciiz ""
.asciiz "1"
.asciiz "12"
.asciiz "123"
.asciiz "1234"
.asciiz "12345"
.asciiz "123456"
.asciiz "1234567"
.asciiz "12345678"
.asciiz "123456789"