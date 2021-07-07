	.include	"global.s"

	.globl	_hblank_isr_jp
	.globl	_hblank_isr_ip
	.globl	_hblank_isr_ctr

	.area	_HEADER_LCD (ABS)
	.org	0x48
.int_LCD:
	push	af
	push	hl
	jp	_hblank_isr_jp

	.area	_BSS
_hblank_isr_jp:
	.ds 1
_hblank_isr_ip:
	.ds 2
_hblank_isr_ctr:
	.ds 1
