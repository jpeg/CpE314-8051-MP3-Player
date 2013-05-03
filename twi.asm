SCL BIT 0xB4 ;P3^4
SDA BIT 0xB5 ;P3^5

VARS SEGMENT DATA OVERLAYABLE
	RSEG VARS
	datavar: ds 1
	num: ds 1
	p_data: ds 1
	temp: ds 1
	bit_cnt: ds 1
	delay_cnt: ds 1

TWI_CODE SEGMENT CODE
	RSEG TWI_CODE

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
	clr SDA
	mov delay_cnt, #3
	acall delay
next_byte:
	mov bit_cnt, #8
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
wait_ack:
	mov C, SCL
	jnc wait_ack
	mov C, SDA
	jc exit
	;dec num			;??? why two dec?
	mov A, num
	mov R7, #0x00		;No error
	jz exit
	dec num
	mov R0, p_data
	mov A, @R0
	mov datavar, A
	inc p_data
	sjmp next_byte
exit:
	clr SCL
	mov delay_cnt, #3
	acall delay
	clr SDA
	mov delay_cnt, #3
	acall delay
	setb SCL
	mov delay_cnt, #3
	acall delay
wait_nack:
	mov C, SCL
	jnc wait_nack
	mov delay_cnt, #3
	acall delay
	setb SDA
	ret

delay:
	nop
	djnz delay_cnt, delay
	ret

PUBLIC _TWI_READ
_TWI_READ:
	setb SCL
	setb SDA
	mov A, R7		;device_addr
	setb C			;0 in R/W bit
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
	clr SDA
	mov delay_cnt, #3
	acall delay
	mov bit_cnt, #8
next_bit2:
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
clock2:
	mov C, SCL
	jnc clock2
	mov delay_cnt, #3
	acall delay
	mov A, temp
	mov C, SDA
	addc A, #0		;essentially XOR
	anl A, #0x01		;bus busy error
	jnz exit
	djnz bit_cnt, next_bit2
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
wait_ack2:
	mov C, SCL
	jnc wait_ack2
	mov C, SDA
	jnc next_byte3
	ljmp exit
next_byte3:
	mov temp, #0
	mov bit_cnt, #8
next_bit3:
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
	jnz next_bit3
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
wait_ack3:
	mov C, SCL
	jnc wait_ack3
	mov delay_cnt, #3
	acall delay
	sjmp next_byte3
finish:
	clr SCL
	mov delay_cnt, #3
	acall delay
	setb SDA
	mov delay_cnt, #3
	acall delay
	setb SCL
	mov delay_cnt, #3
	acall delay
wait_nack2:
	mov C, SCL
	jnc wait_nack2
	mov delay_cnt, #3
	lcall delay
	mov R7, #0x00		;No error
	ljmp exit

END