#include <regAT89C51RC2.inc>
#include <twi.h>

SCL BIT 0xB4
SDA BIT 0xB5

VARS SEGMENT DATA OVERLAYABLE
	RSEG VARS
	datavar: ds 1
	num: ds 1
	p_data: ds 1
	temp: ds 1

TWI_WRITE_CODE SEGMENT CODE
	RSEG TWI_WRITE CODE
PUBLIC _TWI_WRITE;, _TWI_READ

_TWI_WRITE:
	setb SCL
	setb SDA
	mov A, R7		;device_addr
	clr C			;0 in R/W bit
	rlc A
	mov datavar, A
	mov A, R5
	mov num, A
	mov A, R3		;if mem specific ptr
	mov p_data, A
	mov R7, #0x01	;bus busy error
	mov C, SCL
	jnc exist
	mov C, SDA
	jnc exit
next_byte:
	clr SDA
	mov bit_cnt, #8
	mov delay_cnt, #2
	acall delay
next_bit:
	clr SCL
	mov A, datavar
	clr C
	rlc A
	mov datavar, A
	mov SDA, C
	mov A, #0
	addc A, #0		;zero ACL
	mov temp, A
	mov delay_cnt, #2
	acall delay
	setb SCL
	mov delay_cnt, #2
	acall delay
clock1:
	mov C, SCL
	jnc clock1
	mov delay_cnt, #2
	acall delay
	mov A, temp
	mov C, SDA
	addc A, #0		;essentially XOR
	anl A, #0x01	;bus busy error
	jnz exit
	djnz bit_cnt, nextbit
	;TODO check for ACK
	dec num
	mov A, num
	mov R7, 0x00
	jz exit
	dec num
	mov R0, p_data
	mov A, @R0
	inc p_data
	sjmp next_byte

END