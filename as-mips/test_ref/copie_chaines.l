  1                   # TEST_RETURN_CODE=PASS
  2                   
  3                   
  4                   
  5                   .bss
  6                   .set noreorder
  7 00000000 0000...  copy : .space 256 # some space to copy the strings
  8                   
  9                   .data# the 2 strings to copy with their length  
 10 00000000 626F6E6A text1: .asciiz "bonjour!","au revoir"
 10 00000004 6F757221 
 10 00000008 00       
 10 00000009 61752072 
 10 0000000D 65766F69 
 10 00000011 7200     
 11 00000013 696C2061 text2: .asciiz "il a dit, \"bonjour\" 'une fois' \n" 
 11 00000017 20646974 
 11 0000001B 2C202262 
 11 0000001F 6F6E6A6F 
 11 00000023 75722220 
 11 00000027 27756E65 
 11 0000002B 20666F69 
 11 0000002F 7327200A 
 11 00000033 00       
 12 00000034 00000008 taille1: .word 8
 13 00000038 00000020 taille2: .word 32
 14                   
 15                   .text 
 16 00000000 0800000F j main # go to the main procedure
 17 00000004 00000000 nop
 18                   copy_text:    
 19                                 # suppose that $t0 contains the @ of the destination space
 20                                 #              $t1 contains the size of the text to read
 21                                 #              $t2 contains the @ of the source text 
 22                                 #              $t3 the indice of the word of text to save to the destination
 23                                 #              $t4 is the destitation @ of the word of text to save  
 24                                 #              $t5 is the word of text  
 25                                 #              $t6 the indice of the word of text to load from the source
 26                   
 27                   
 28 00000008 01006020    add $t4,$t0,$zero     # init $4 and $t6
 29 0000000C 01407020    add $t6,$t2,$zero     
 30                   boucle:
 31 00000010 0169882A    slt $s1,$t3,$t1       # if $t3>$t1
 32 00000014 1E200007    bgtz $s1,sortie   # then get out of the procedure   
 33 00000018 00000000    nop
 34 0000001C 83E00000    lw $t5,0($t2)         # load word to save in $t5   
 35 00000020 AD8D0000    sw $t5,0($t4)         # store word in $t5 at @ indicated by $t4
 36 00000024 216B0004    addi $t3,$t3,4        # increment $t3 of the size of a word
 37 00000028 010B6020    add $t4,$t0,$t3       # increase destination address 
 38 0000002C 014B7020    add $t6,$t2,$t3       # increase destination address 
 39                      b boucle              # continue
 40 00000030 00000000    nop 
 41                   sortie: 
 42 00000034 03E00008    jr $ra	         # go back to main
 43 00000038 00000000    nop
 44                   main:
 45                   
 46 0000003C 3C200000 lw $t0,copy              # fill function arguments in 
 46 00000040 83200000
 47 00000044 3C200034 lw $t1,taille1
 47 00000048 83200034
 48 0000004C 21290001 addi $t1,$t1,1
 49 00000050 3C200000 lw $t2,text1
 49 00000054 83600000
 50 00000058 200B0000 addi $t3,$zero,0
 51 0000005C 0C000002 jal copy_text            # call copy_text on text1
 52 00000060 00000000 nop
 53 00000064 21290001 addi $t1,$t1,1
 54 00000068 01094020 add $t0,$t0,$t1
 55 0000006C 3C200038 lw $t1,taille2
 55 00000070 83200038
 56 00000074 3C200013 lw $t2,text2
 56 00000078 83600013
 57 0000007C 200B0000 addi $t3,$zero,0
 58 00000080 0C000002 jal copy_text            # call copy_text on text2 
 59 00000084 00000000 nop
 60                   

.symtab
  7	.bss :00000000	copy
 10	.data:00000000	text1
 11	.data:00000013	text2
 12	.data:00000034	taille1
 13	.data:00000038	taille2
 18	.text:00000008	copy_text
 30	.text:00000010	boucle
 41	.text:00000034	sortie
 44	.text:0000003C	main

rel.text
00000000	R_MIPS_26	.text:0000003c	main
0000003c	R_MIPS_HI16	.bss :00000000	copy
00000040	R_MIPS_LO16	.bss :00000000	copy
00000044	R_MIPS_HI16	.data:00000034	taille1
00000048	R_MIPS_LO16	.data:00000034	taille1
00000050	R_MIPS_HI16	.data:00000000	text1
00000054	R_MIPS_LO16	.data:00000000	text1
0000005c	R_MIPS_26	.text:00000008	copy_text
0000006c	R_MIPS_HI16	.data:00000038	taille2
00000070	R_MIPS_LO16	.data:00000038	taille2
00000074	R_MIPS_HI16	.data:00000013	text2
00000078	R_MIPS_LO16	.data:00000013	text2
00000080	R_MIPS_26	.text:00000008	copy_text

rel.data

