;
; Controller reading for Game Boy, adapted for TinyZoo/GB
;
; Copyright 2018, 2020 Damian Yerrick
; Copyright (c) 2020, 2021, 2022 Adrian Siekierka
; 
; This software is provided 'as-is', without any express or implied
; warranty.  In no event will the authors be held liable for any damages
; arising from the use of this software.
; 
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, and to alter it and redistribute it
; freely, subject to the following restrictions:
; 
; 1. The origin of this software must not be misrepresented; you must not
;    claim that you wrote the original software. If you use this software
;    in a product, an acknowledgment in the product documentation would be
;    appreciated but is not required.
; 2. Altered source versions must be plainly marked as such, and must not be
;    misrepresented as being the original software.
; 3. This notice may not be removed or altered from any source distribution.
;

	.include	"global.s"

	.globl	_input_update_vbl
	.globl	_input_keys
	.globl	_input_keys_repressed
	.globl	_input_keys_released
	.globl	_input_held

	.area	_CODE
_input_update_vbl:
	di

	; Poll half the controller
	ld a, #P1F_GET_BTN
	call .onenibble
	swap a
	ld b, a ; B7-4 = 1; B3-0 = unpressed buttons

	; Poll the other half
	ld a, #P1F_GET_DPAD
	call .onenibble

	xor b   ; A = pressed buttons + directions
	ld b, a ; B = pressed buttons + directions

	; And release the controller
	ld a, #P1F_GET_NONE
	ldh (rP1), a

	ei
	
	; Combine with input_keys
	ld a, (_input_keys)
	or a, b
	ld (_input_keys), a
	xor a, #0xFF
	ld e, a ; E = ~keys

	; Load input_keys_released
	ld a, (_input_keys_released)
	ld d, a ; D = input_keys_released
	and a, b
	ld c, a ; C = input_keys_released & keys

	; Combine with input_keys_repressed
	ld a, (_input_keys_repressed)
	or a, c
	ld (_input_keys_repressed), a

	; Combine with input_keys_released
	ld a, (_input_held)
	and a, e ; A = input_held & (~keys)
	or a, d
	ld (_input_keys_released), a

	ret

.onenibble:
	ldh (rP1), a   ; switch the key matrix
	call .knownret ; burn 10 cycles calling a known ret
	ldh a, (rP1)   ; ignore value while waiting for the key matrix to settle
	ldh a, (rP1)
	ldh a, (rP1)   ; this read counts
	or #0xF0 ; A7-4 = 1; A3-0 = unpressed keys
.knownret:
	ret
