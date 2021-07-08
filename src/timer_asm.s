	.include	"global.s"

	.globl	_dhsecs
	.globl  _timer_state
	.globl  _timer_speed_base
	.globl  _timer_speed_drum

	.area	_HEADER_TIM (ABS)
	.org	0x50
.int_TIM:
	ei
	push	af
	push	hl
	jp	.timer_handler

	.area	_BASE

.timer_handler:
	; only run every other tick
	ld hl, #(_timer_state)
	ld a, (hl)
	xor a, #0x01
	ld (hl), a
	jp z, .timer_handler_end

	; increment dhsecs
	ld hl, #(_dhsecs)
	ld a, (hl)
	add a, #11
	ld (hl+), a
	ld a, (hl)
	adc a, #0
	ld (hl), a

	ld a, (__current_bank)
	push af
	ld a, #0x02
	ld (0x2000), a
	call (_sound_update)
	pop af
	ld (0x2000), a

.timer_handler_end:
	pop		hl
	pop		af
	reti

	.area	_BSS
_dhsecs:
	.ds 2
_timer_state:
	.ds 1
_timer_speed_base:
	.ds 1
_timer_speed_drum:
	.ds 1