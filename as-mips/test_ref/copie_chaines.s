# TEST_RETURN_CODE=PASS



.bss
.set noreorder
copy : .space 256 # some space to copy the strings

.data# the 2 strings to copy with their length  
text1: .asciiz "bonjour!","au revoir"
text2: .asciiz "il a dit, \"bonjour\" 'une fois' \n" 
taille1: .word 8
taille2: .word 32

.text 
j main # go to the main procedure
nop
copy_text:    
              # suppose that $t0 contains the @ of the destination space
              #              $t1 contains the size of the text to read
              #              $t2 contains the @ of the source text 
              #              $t3 the indice of the word of text to save to the destination
              #              $t4 is the destitation @ of the word of text to save  
              #              $t5 is the word of text  
              #              $t6 the indice of the word of text to load from the source


   add $t4,$t0,$zero     # init $4 and $t6
   add $t6,$t2,$zero     
boucle:
   slt $s1,$t3,$t1       # if $t3>$t1
   bgtz $s1,sortie   # then get out of the procedure   
   nop
   lw $t5,0($t2)         # load word to save in $t5   
   sw $t5,0($t4)         # store word in $t5 at @ indicated by $t4
   addi $t3,$t3,4        # increment $t3 of the size of a word
   add $t4,$t0,$t3       # increase destination address 
   add $t6,$t2,$t3       # increase destination address 
   b boucle              # continue
   nop 
sortie: 
   jr $ra	         # go back to main
   nop
main:

lw $t0,copy              # fill function arguments in 
lw $t1,taille1
addi $t1,$t1,1
lw $t2,text1
addi $t3,$zero,0
jal copy_text            # call copy_text on text1
nop
addi $t1,$t1,1
add $t0,$t0,$t1
lw $t1,taille2
lw $t2,text2
addi $t3,$zero,0
jal copy_text            # call copy_text on text2 
nop

