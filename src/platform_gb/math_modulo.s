;
; Optimized modulo routines
;
; Copyright (c) 2022 Adrian Siekierka
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

	.globl	_zoo_mods16_8
	.globl	_zoo_modu16_8

; Based on https://bisqwit.iki.fi/story/howto/bitmath/#DivAndModDivisionAndModulo

	.area	_CODE
_zoo_mods16_8:
	; de = dividend
	; a = divisor, must be > 0
	bit 7, d
	jr z, _zoo_modu16_8

	ld c, a

	; negate dividend
	xor a, a
	sub a, e
	ld e, a
	ld a, #0
	sbc a, d
	ld  d, a

	; call unsigned modulo
	ld a, c
	call _zoo_modu16_8a

	; result = divisor - result - 1
	ld a, c
	sub a, e
	dec a
	ret

_zoo_modu16_8:
	; de = dividend
	; a = divisor, must be > 0

	; a = temp value
	; b = multiple
	; c = unused
	; de = dividend/remain
	; h = temp
	; l = divisor/scaled_divisor

	; a = divisor
_zoo_modu16_8a:
	ld b, #1 ; b = multiple
	; while(divisor < 0x80)
.fastmod_scale_divisor:
	cp a, #0x10
	jr nc, .fastmod_scale_divisor2
	swap b ; multiple <<= 4
	swap a ; scaled_divisor <<= 4
.fastmod_scale_divisor2:
	cp a, #0x20
	jr nc, .fastmod_scale_divisor3
	sla b ; multiple <<= 1
	sla b ; multiple <<= 1
	add a, a ; scaled_divisor <<= 1
	add a, a ; scaled_divisor <<= 1
.fastmod_scale_divisor3:
	cp a, #0x40
	jr nc, .fastmod_scale_divisor_end
	sla b ; multiple <<= 1
	add a, a ; scaled_divisor <<= 1	
.fastmod_scale_divisor_end:
	ld l, a ; l = divisor
.fastmod_remain:
	; remain -= divisor
	; if < 0, skip to remain2
	ld a, e
	sub a, l
	ld h, a
	ld a, d
	sbc #0
	jr c, .fastmod_remain2
	ld d, a
	ld e, h
	jr .fastmod_remain

.fastmod_remain2:
	srl l
	srl b
	jr nz, .fastmod_remain

.fastmod_remain3:
	ld a, e
	ret
