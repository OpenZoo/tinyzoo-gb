; Copyright (c) 2020, 2021, 2022 Adrian Siekierka
;
; TinyZoo/GB is free software: you can redistribute it and/or modify it under
; the terms of the GNU General Public License as published by the Free
; Software Foundation, either version 3 of the License, or (at your option)
; any later version.
;
; TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
; FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
; more details.
;
; You should have received a copy of the GNU General Public License along
; with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
;
; TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
; the following license terms:
;
; Copyright (c) 2020 Adrian Siekierka
;
; Based on a reconstruction of code from ZZT,
; Copyright 1991 Epic MegaGames, used with permission.
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
; SOFTWARE.

	.include	"global.s"

	.globl	_dhsecs
	.globl  _timer_state
	.globl  _timer_speed_base
;	.globl  _timer_speed_drum

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
	jr z, .timer_handler_end

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
	ld (__current_bank), a
.ifdef __TPP1__
	ld (0x0000), a
.else
	ld (0x2000), a
.endif

	call (_sound_update)

	pop af
	ld (__current_bank), a
.ifdef __TPP1__
	ld (0x0000), a
.else
	ld (0x2000), a
.endif

.timer_handler_end:
	pop		hl
	pop		af
	reti

	.area	_BSS
_timer_state:
	.ds 1
_timer_speed_base:
	.ds 1
; _timer_speed_drum:
;	.ds 1
