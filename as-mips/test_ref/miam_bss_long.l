  1                   # TEST_RETURN_CODE=PASS
  2                   # allons au ru
  3                   
  4                   
  5                   .set noreorder
  6                   .text
  7 00000000 3C018765     Lw $t0 , menu
  7 00000004 8C284321 
  8 00000008 8CE6FE00     LW $6, -0x200($7)
  9 0000000C 20090008     ADDI $t1,$zero,8
 10 00000010 3C088765     LUI $t0, menu
 11                   
 12                   boucle:
 13 00000014 11090004     BEQ $t0 , $t1 , byebye
 14 00000018 00000000     NOP
 15 0000001C 21290001     addi $t1 , $t1 , 1
 16 00000020 08000005     J boucle 
 17 00000024 00000000     NOP
 18                   byebye:
 19 00000028 0C000000     JAL viteviteauru
 20                   
 21                   .data
 22 00000000 0000000C lunchtime: .word 12
 23 00000004 87654321 .word menu
 24                   
 25                   .bss
 26 00000000 0000...  .space 0x87654321
 27 87654321 0000...  menu:.space 24

.symtab
 12	.text:00000014	boucle
 18	.text:00000028	byebye
 19	[UNDEFINED]	viteviteauru
 22	.data:00000000	lunchtime
 27	.bss :87654321	menu

rel.text
00000000	R_MIPS_HI16	.bss :87654321	menu
00000004	R_MIPS_LO16	.bss :87654321	menu
00000010	R_MIPS_HI16	.bss :87654321	menu
00000020	R_MIPS_26	.text:00000014	boucle
00000028	R_MIPS_26	[UNDEFINED]	viteviteauru

rel.data
00000004	R_MIPS_32	.bss :87654321	menu

