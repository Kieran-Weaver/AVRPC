;SD CARD PINS
#define	DDR_CS	_SFR_IO_ADDR(SD_CS_DDR), SD_CS_BIT
#define	PORT_CS	_SFR_IO_ADDR(SD_CS_PORT), SD_CS_BIT

;---------------------------------------------------------------------------;
.nolist
#include <avr/io.h>
.list
.text

;---------------------------------------------------------------------------
; Erase a flash page
;
; void flash_erase (DWORD flash_addr);

#ifndef SPMCSR
#define SPMCSR SPMCR
#endif

.globl flash_erase
.func flash_erase
flash_erase:

	movw	ZL, r22
#if FLASHEND >= 0x10000
	out	_SFR_IO_ADDR(RAMPZ), r24
#endif

	; Initiate erase operation
	ldi	r24, 0b00000011
	sts	_SFR_MEM_ADDR(SPMCSR), r24
	spm

	; Wait for end of erase operation
1:	lds	r24, _SFR_MEM_ADDR(SPMCSR)
	sbrc	r24, 0
	rjmp	1b

	; Re-enable read access to the flash
	ldi	r24, 0b00010001
	sts	_SFR_MEM_ADDR(SPMCSR), r24
	spm

9:	ret
.endfunc



;---------------------------------------------------------------------------
; Write a flash page
;
; void flash_write (DWORD flash_addr, const BYTE* data);

.globl flash_write
.func flash_write
flash_write:
	push	r0
	push	r1

#if FLASHEND >= 0x10000
	out	_SFR_IO_ADDR(RAMPZ), r24
#endif

	; Fill page buffer
	movw	ZL, r22
	movw	XL, r20
	ldi	r25, lo8(SPM_PAGESIZE/2)
1:	ld	r0, X+
	ld	r1, X+
	ldi	r24, 0b00000001
	sts	_SFR_MEM_ADDR(SPMCSR), r24
	spm
	adiw	ZL, 2
	dec	r25
	brne	1b

	; Initiate write operation
	movw	ZL, r22
	ldi	r24, 0b00000101
	sts	_SFR_MEM_ADDR(SPMCSR), r24
	spm

	; Wait for end of write operation
2:	lds	r24, _SFR_MEM_ADDR(SPMCSR)
	sbrc	r24, 0
	rjmp	2b

	; Re-enable read access to the flash
	ldi	r24, 0b00010001
	sts	_SFR_MEM_ADDR(SPMCSR), r24
	spm

9:	pop	r1
	pop	r0
	ret
.endfunc
