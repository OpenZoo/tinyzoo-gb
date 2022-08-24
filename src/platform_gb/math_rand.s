;
; Pseudorandom number generator, adapted for TinyZoo/GB
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

	.globl	_rand
	.globl	_rand_mask8
	.globl  _rand_mod
	.globl	_rand_np1
	.globl	_rand_seed

	.area	_CODE
_rand:
.rand_bc:
        ld hl, #(_rand_seed)
        ld a, (hl)
        add a, #0xB3
        ld (hl+), a
        adc a, (hl)
        ld (hl+), a
        adc a, (hl)
        ld (hl+), a
        ld b, a
        adc a, (hl)
        ld (hl), a
        ld c, a
	ret
.rand_de:
        ld hl, #(_rand_seed)
        ld a, (hl)
        add a, #0xB3
        ld (hl+), a
        adc a, (hl)
        ld (hl+), a
        adc a, (hl)
        ld (hl+), a
        ld d, a
        adc a, (hl)
        ld (hl), a
        ld e, a
	ret

_rand_mask8:
	call .rand_bc
	and a, c
	ret

_rand_mod:
	ld b, d
	ld c, e
	call .rand_de
	jp __moduint

_rand_np1:
	ld b, a
	call .rand_de
	; de = 0 + d + e
	; ld a, e (implied)
	add a, d
	ld e, a
	ld a, #0
	adc a, #0
	ld d, a
	ld a, b
	jp _zoo_modu16_8
