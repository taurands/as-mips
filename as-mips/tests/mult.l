  1                   # TEST_RETURN_CODE=PASS
  2                   ## Fichier mult.s : un programme qui fait une division et une multiplication
  3                   .text
  4                   .set noreorder		#pas de rÃ©ordonnancement
  5                   
  6                   ##----------------------------------------------------------------------------
  7                   # initialise les variables
  8 00000000 20190054   ADDI  $t9,$zero,EXIT
  9 00000004 200AFFD5   addi  $t2,$zero,-	43     	# $t2 <- -43
 10 00000008 200BFFFF   addi  $t3,	$0,0xffff      	# $t3 <- 0xff
 11                   
 12                   #fait la division
 13 0000000C 014B001A   DIV  $t2,$t3			# divise les deux nombres
 14 00000010 00004012   mflo $t0 			# prend le quotient du rÃ©sultat de la division
 15 00000014 00004810   mfhi $t1 			# prend le reste du rÃ©sultat de la division
 16 00000018 11200002   BEQ $t1,$zero, mult           # si il n'y a pas de reste alors on peut tester dans l'autre sens
 17 0000001C 00005020   add $t2,$zero,$zero		# si pas rÃ©ussi on set $t2 Ã  0
 18 00000020 08000015   J EXIT			# saut Ã  la sortie sinon
 19                   
 20                   #fait la multiplication (remarquez le nom de l'Ã©tiquette)
 21                   mult:
 22 00000024 00000000   NOP				# quelques non opÃ©rations pour respecter la consigne
 23 00000028 00000000   nop				# de la doc concernant le MFHI
 24 0000002C 010B0018   mult $t0,$t3			# on essaye de retrouver le nombre de dÃ©part
 25 00000030 00004012   MFLO $t0 			# prend la partie basse de la multiplication
 26 00000034 00004810   MFHI $t1 			# prend la partie haute de la multiplication
 27 00000038 00006820   add $t5,$zero,$zero		# astuce pour Ã©viter les optimisation de boucle
 28 0000003C 152D0005   BNE $t1,$t5,EXIT		# si le rÃ©sultat est trop grand on sort
 29 00000040 110A0002   BEQ $t0,$t2,reussi		# si $t2 et $t0 sont Ã©gaux on a retrouvÃ© le rÃ©sultat
 30 00000044 00005020   add $t2,$zero,$zero		# si pas rÃ©ussi on set $t2 Ã  0 et on sort
 31 00000048 08000015   J EXIT
 32                   reussi:
 33 0000004C 200A0001   addi $t2,$zero,0x1		# si rÃ©ussi on set $t2 Ã  1
 34 00000050 08000015   J EXIT			# et on sort
 35                   
 36                   
 37                   EXIT	:
 38 00000054 0000000C syscall
 39                   ## The End
 40                   
 41                   .data
 42 00000000 0CAABBCC .byte 12,0xAA,0xBB,0xCC,0xdd
 42 00000004 DD       
 43 00000005 01       .byte 1
 44 00000006 0102     .byte 1,2
 45 00000008 010203   .byte 1,2,3
 46 0000000B 01020304 .byte 1,2,3,4
 47 0000000F 01020304 .byte 1,2,3,4,5
 47 00000013 05       
 48 00000014 01020304 .byte 1,2,3,4,5,6
 48 00000018 0506     
 49 0000001A 01020304 .byte 1,2,3,4,5,6,7
 49 0000001E 050607   
 50 00000021 01020304 .byte 1,2,3,4,5,6,7,8
 50 00000025 05060708 
 51 00000029 01020304 .byte 1,2,3,4,5,6,7,8,9
 51 0000002D 05060708 
 51 00000031 09       
 52                   
 53 00000034 AABBCCDD .word 0xAABBCCDD

.symtab
 21	.text:00000024	mult
 32	.text:0000004C	reussi
 37	.text:00000054	EXIT

rel.text
00000000	R_MIPS_LO16	.text:00000054	EXIT
00000020	R_MIPS_26	.text:00000054	EXIT
00000048	R_MIPS_26	.text:00000054	EXIT
00000050	R_MIPS_26	.text:00000054	EXIT

rel.data

