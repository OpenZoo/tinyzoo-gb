;
; Optimized math routines
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

	.globl	_signum8
	.globl	_difference8

	.area	_CODE
_signum8:
	; a = x
        bit 7, a
        jr nz, .Signum8ReturnMinusOne ; x < 0
        ld e, a
        xor a, a
        cp a, e ; x == 0?
        ret z
        ld a, #0x01 ; x > 0
        ret
.Signum8ReturnMinusOne:
        ld a, #0xFF
        ret

_difference8:
	ld d, a
        sub a, e
	ret nc

        ld a, e
        sub a, d
        ret
