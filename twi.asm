;#include <regAT89C51RC2.inc>

SCL BIT 0xB4
SDA BIT 0xB5

VARS SEGMENT DATA OVERLAYABLE
	RSEG VARS
	datavar: ds 1
	num: ds 1
	p_data: ds 1
	temp: ds 1
	bit_cnt: ds 1
	delay_cnt: ds 1

TWI_WRITE_CODE SEGMENT CODE
	RSEG TWI_WRITE_CODE
PUBLIC _TWI_WRITE

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
	mov R7, #0x01		;bus busy error
	mov C, SCL
	jnc exit
	mov C, SDA
	jnc exit
next_byte:
	clr SDA
	mov bit_cnt, #8
	mov delay_cnt, #3
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
	mov delay_cnt, #3
	acall delay
	setb SCL
	mov delay_cnt, #3
	acall delay
clock1:
	mov C, SCL
	jnc clock1
	mov delay_cnt, #3
	acall delay
	mov A, temp
	mov C, SDA
	addc A, #0		;essentially XOR
	anl A, #0x01		;bus busy error
	jnz exit
	djnz bit_cnt, next_bit
	mov delay_cnt, #3
	acall delay
	clr SCL
	mov delay_cnt, #3
	acall delay
	setb SDA
	mov delay_cnt, #3
	acall delay
	setb SCL
	mov delay_cnt, #3
	acall delay
	sjmp wait_ack
exit:
	ret				;here for sjmp range limits
wait_ack:
	mov C, SCL
	jnc wait_ack
	mov C, SDA
	jc exit
	;dec num			;??? why two dec?
	mov A, num
	mov R7, 0x00
	jz exit
	dec num
	mov R0, p_data
	mov A, @R0
	inc p_data
	sjmp next_byte
	clr SCL
	mov delay_cnt, #3
	acall delay
	clr SDA
	mov delay_cnt, #3
	acall delay
	setb SCL
	mov delay_cnt, #3
	acall delay
wait_something:
	mov C, SCL
	jnc wait_something
	mov delay_cnt, #3
	acall delay
	setb SDA
	sjmp exit

delay:
	nop
	djnz delay_cnt, delay
	ret

TWI_READ_CODE SEGMENT CODE
	RSEG TWI_READ_CODE
PUBLIC _TWI_READ

_TWI_READ:
	setb SCL
	setb SDA
	mov A, R7		;device_addr
	setb C			;1 in R/W bit
	rlc A
	mov datavar, A
	mov A, R5
	mov num, A
	mov A, R3		;if mem specific ptr
	mov p_data, A
	mov R7, #0x01		;bus busy error
	mov C, SCL
	jnc exit2
	mov C, SDA
	jnc exit
next_byte2:
	mov temp, #0
	mov bit_cnt, #8
next_bit2:
	clr SCL
	setb SDA
	mov delay_cnt, #6
	acall delay
	setb SCL
	mov delay_cnt, #3
	acall delay
wait1:
	mov C, SCL
	jnc wait1
	mov A, temp
	mov C, SDA
	rlc A
	mov temp, A
	dec bit_cnt
	mov A, bit_cnt
	jz next_bit2
	mov R0, p_data
	mov A, temp
	mov @R0, A
	inc p_data
	dec num
	mov A, num
	jz finish
	clr SCL
	mov delay_cnt, #6
	acall delay
	clr SDA
	mov delay_cnt, #3
	acall delay
	setb SCL
	mov delay_cnt, #3
	acall delay
	sjmp wait2
exit2:				;here for sjmp range limits
	ret
wait2:
	mov C, SCL
	jnc wait2
	mov delay_cnt, #3
	acall delay
	sjmp next_byte2
finish:
	clr SCL
	mov delay_cnt, #3
	acall delay
	setb SDA
	mov delay_cnt, #3
	lcall delay
	setb SCL
	mov delay_cnt, #3
	lcall delay
wait3:
	mov C, SCL
	jnc wait3
	mov delay_cnt, #3
	lcall delay
	clr SCL
	mov delay_cnt, #3
	lcall delay
	clr SDA
	mov delay_cnt, #3
	acall delay
	setb SCL
	mov delay_cnt, #3
	lcall delay
wait4:
	mov C, SCL
	jnc wait4
	setb SDA
	sjmp exit2

END