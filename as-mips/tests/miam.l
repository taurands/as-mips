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
 15 00000010 00000000     SW $t1, menu
 15 00000014 00000000
 16 00000018 00000000     ADDI $t1,$zero,8
 17                   
 18                   boucle:
 19 0000001c 00000000     BEQ $t0 , $t1 , byebye
 20 00000020 00000000     addi $t1 , $t1 , 1
 21 00000024 00000000     J boucle
 22 00000028 00000000     BLT $at, $sp, boucle
 22 0000002c 00000000
 23                   byebye:
 24 00000030 00000000 toto: tata:    JAL viteviteauru
 25 00000034 00000000 	JAL encoreunindefini
 26                   
 27                   .data
 28 00000024 0000000C lunchtime : .word 12
 29 00000028 00000007 	.word menu, quid
 29 0000002C 00000000 
 30                   
 31                   .bss
 32 00000000 0000...  .space 5,2
 32 00000005 0000     
 33 00000007 0000...  menu:.space 24

.symtab
  7	[UNDEFINED]	dollar
  7	[UNDEFINED]	quid
 18	.text:0000001c	boucle
 23	.text:00000030	byebye
 24	.text:00000030	toto
 24	.text:00000030	tata
 24	[UNDEFINED]	viteviteauru
 25	[UNDEFINED]	encoreunindefini
 28	.data:00000024	lunchtime
 33	.bss :00000007	menu

rel.text
00000004	R_MIPS_HI16	.data:00000024	lunchtime
00000008	R_MIPS_LO16	.data:00000024	lunchtime
00000010	R_MIPS_HI16	.bss :00000007	menu
00000014	R_MIPS_LO16	.bss :00000007	menu
00000024	R_MIPS_26	.text:0000001c	boucle
00000030	R_MIPS_26	[UNDEFINED]	viteviteauru
00000034	R_MIPS_26	[UNDEFINED]	encoreunindefini

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

