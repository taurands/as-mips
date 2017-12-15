# TEST_RETURN_CODE=PASS
.data
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
.byte 1
.byte 1,2
.byte 1,2,3
.byte 1,2,3,4
.byte 1,2,3,4,5
.byte 1,2,3,4,5,6
.byte 1,2,3,4,5,6,7
.byte 1,2,3,4,5,6,7,8
.byte 1,2,3,4,5,6,7,8,9
.byte 12,0xAA,0xBB,0xCC,0xdd
.byte '\'', '"', '\n', '\r', '\t', '\f', '\v', '\a', '\b', '\\', '\0'
.half 0x1234
.half 0x1234, 0x5678
.half 0x1234, 0x5678, 0x9abc
.half 0x1234, 0x5678, 0x9abc, -1
.half 0x1234, 0x5678, 0x9abc, -1, -2
.word 0x12345678
.word 0x12345678, 0x98765432
.word 0x12345678, 0x98765432, -1, -2, 3
.space 1
.space 2
.space 3
.space 4
.space 5
.space 1, 2
.space 1, 2, 3
.bss
.space 1
.space 2
.space 3
.space 4
.space 5
.space 1, 2
.space 1, 2, 3
