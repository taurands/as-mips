  1                   
  2                   # allons au ru
  3                   
  4                   
  5                   .set noreorder
  6                   .data
  7 00000000 00000000 .word dollar, menu, quid
  7 00000004 00000007 
  7 00000008 00000000 
  8 0000000C 00000000 .word dollar, menu, quid
  8 00000010 00000007 
  8 00000014 00000000 
  9 00000018 00000000 .word dollar, menu, quid
  9 0000001C 00000007 
  9 00000020 00000000 
 10                   
 11                   .text
 12 00000000 00000000     NOP
 13 00000004 00000000     Lw $t0, lunchtime
 13 00000008 00000000
 14 0000000c 00000000     LW $6, -0x200($7)
 15 00000010 00000000     ADDI $t1,$zero,8
 16                   
 17                   boucle:
 18 00000014 00000000     BEQ $t0 , $t1 , byebye
 19 00000018 00000000     addi $t1 , $t1 , 1
 20 0000001c 00000000     J boucle
 21 00000020 00000000     BLT $at, $sp, boucle
 21 00000024 00000000
 22                   byebye:
 23 00000028 00000000 toto: tata:    JAL viteviteauru
 24 0000002c 00000000 	JAL encoreunindefini
 25                   
 26                   .data
 27 00000024 0000000C lunchtime : .word 12
 28 00000028 00000007 	.word menu, quid
 28 0000002C 00000000 
 29                   
 30                   .bss
 31 00000000 0000...  .space 5,2
 31 00000005 0000     
 32 00000007 0000...  menu:.space 24

.symtab
  7	[UNDEFINED]	dollar
  7	[UNDEFINED]	quid
 17	.text:00000014	boucle
 22	.text:00000028	byebye
 23	.text:00000028	toto
 23	.text:00000028	tata
 23	[UNDEFINED]	viteviteauru
 24	[UNDEFINED]	encoreunindefini
 27	.data:00000024	lunchtime
 32	.bss :00000007	menu

rel.text
00000004	R_MIPS_HI16	.data:00000024	lunchtime
00000008	R_MIPS_LO16	.data:00000024	lunchtime
0000001c	R_MIPS_26	.text:00000014	boucle
00000028	R_MIPS_26	[UNDEFINED]	viteviteauru
0000002c	R_MIPS_26	[UNDEFINED]	encoreunindefini

rel.data
00000000	R_MIPS_32	[UNDEFINED]	dollar
00000004	R_MIPS_32	.bss :00000007	menu
00000008	R_MIPS_32	[UNDEFINED]	quid
0000000c	R_MIPS_32	[UNDEFINED]	dollar
00000010	R_MIPS_32	.bss :00000007	menu
00000014	R_MIPS_32	[UNDEFINED]	quid
00000018	R_MIPS_32	[UNDEFINED]	dollar
0000001c	R_MIPS_32	.bss :00000007	menu
00000020	R_MIPS_32	[UNDEFINED]	quid
00000028	R_MIPS_32	.bss :00000007	menu
0000002c	R_MIPS_32	[UNDEFINED]	quid

