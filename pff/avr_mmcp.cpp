/*-------------------------------------------------------------------------*/
/* PFF - Low level disk control module for AVR            (C)ChaN, 2014    */
/*-------------------------------------------------------------------------*/

#include "pff.h"
#include "diskio.h"

/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/

#include <avr/io.h>			/* Device specific include files */
#include <util/delay.h>
#include "SPI.h"

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0	(0x40+0)	/* GO_IDLE_STATE */
#define CMD1	(0x40+1)	/* SEND_OP_COND (MMC) */
#define	ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(0x40+8)	/* SEND_IF_COND */
#define CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define CMD24	(0x40+24)	/* WRITE_BLOCK */
#define CMD55	(0x40+55)	/* APP_CMD */
#define CMD58	(0x40+58)	/* READ_OCR */


/* Card type flags (CardType) */
#define CT_MMC				0x01	/* MMC ver 3 */
#define CT_SD1				0x02	/* SD ver 1 */
#define CT_SD2				0x04	/* SD ver 2 */
#define CT_BLOCK			0x08	/* Block addressing */

/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* 1st byte (Start + Index) */
	DWORD arg		/* Argument (32 bits) */
)
{
	BYTE n, res;

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card */
	DESELECT();
	xmit_spi(0xFF);
	SELECT();
	xmit_spi(0xFF);

	/* Send a command packet */
	xmit_spi(cmd);						/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive a command response */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do {
		res = xmit_spi(0xFF);
	} while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}




/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void)
{
	BYTE n, cmd, ty, ocr[4];

	init_spi();		/* Initialize ports to control MMC */
	DESELECT();
	for (n = 10; n; n--) xmit_spi(0xFF);	/* 80 dummy clocks with CS=H */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* GO_IDLE_STATE */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2, assume valid vdd range */
			for (n = 0; n < 4; n++) xmit_spi(0xFF);	/* Get trailing return value of R7 resp */
			while (send_cmd(ACMD41, 1UL << 30)) _delay_us(100);
			if (send_cmd(CMD58, 0) == 0) { /* Check CCS bit in the OCR */
				ty = CT_SD2;
				if (xmit_spi(0xFF) & 0x40) ty |= CT_BLOCK; /* SDv2 (HC or SC) */
				for (n = 0; n < 3; n++) xmit_spi(0xFF);
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			while (send_cmd(cmd, 0)) _delay_us(100); /* Wait for leaving idle state */
			if (send_cmd(CMD16, 512) != 0)           /* Set R/W block length to 512 */
				ty = 0;
		}
	}
//	CardType = ty;
	DESELECT();
	xmit_spi(0xFF);

	return ty ? 0 : STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE *buff,		/* Pointer to the read buffer (NULL:Forward to the stream) */
	DWORD sector	/* Sector number (LBA) */
)
{
	DRESULT res;
	BYTE rc = 0;
	UINT count = 512; /* Number of bytes to read (ofs + cnt mus be <= 512) */


//	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	res = RES_ERROR;
	if (send_cmd(CMD17, sector) == 0) {	/* READ_SINGLE_BLOCK */
		
		while (rc != 0xFE) {
			rc = xmit_spi(0xFF);
		}

		/* A data packet arrived */
//		if (rc == 0xFE) {	
			/* Skip leading bytes */
//			while (offset--) xmit_spi(0xFF); Leading bytes always 0

			/* Receive a part of the sector */
			/* Store data to the memory */
		do {
			*buff++ = xmit_spi(0xFF);
		} while (--count);
				
		/* Skip trailing bytes and CRC */
		xmit_spi(0xFF);
		xmit_spi(0xFF);

		res = RES_OK;
	}

	DESELECT();
	xmit_spi(0xFF);

	return res;
}



/*-----------------------------------------------------------------------*/
/* Write partial sector                                                  */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_writep (
	WRITE_STATE* ws,
	const BYTE *buff,	/* Pointer to the bytes to be written (NULL:Initiate/Finalize sector write) */
	DWORD sc			/* Number of bytes to send, Sector number (LBA) or zero */
)
{
	DRESULT res;
	UINT bc;

	res = RES_ERROR;

	if (buff) {		/* Send data bytes */
		bc = sc;
		while (bc && ws->wc) {		/* Send data bytes to the card */
			xmit_spi(*buff++);
			ws->wc--; bc--;
		}
		res = RES_OK;
	} else {
		if (sc) {	/* Initiate sector write process */
//			if (!(CardType & CT_BLOCK)) sc *= 512;	/* Convert to byte address if needed */
			if (send_cmd(CMD24, sc) == 0) {			/* WRITE_SINGLE_BLOCK */
				xmit_spi(0xFF); xmit_spi(0xFE);		/* Data block header */
				ws->wc = 512;							/* Set byte counter */
				res = RES_OK;
			}
		} else {	/* Finalize sector write process */
			bc = ws->wc + 2;
			while (bc--) xmit_spi(0);	/* Fill left bytes and CRC with zeros */
			if ((xmit_spi(0xFF) & 0x1F) == 0x05) {	/* Receive data resp and wait for end of write process in timeout of 500ms */
				for (bc = 5000; xmit_spi(0xFF) != 0xFF && bc; bc--)	/* Wait for ready */
					_delay_us(100);
				if (bc) res = RES_OK;
			}
			DESELECT();
			xmit_spi(0xFF);
		}
	}

	return res;
}
#endif
