/*
 *		nccmisc.cpp	- NCC Miscellanious Functions
 *
 * 	Nothing Comes Close Library
 * 	Written By Ben Houston [plash / AZuRe]
 *
 *		Started: Friday, February 28, 1997
 *		Updated: n/a
 */


#include "ncctypes.h"
#include "nccmisc.h"

#include <string.h>

char	gstring[25];

dword	gettickcount ( void )
{
	dword	tickcount;

	// Returns the number of times the timer interrupt handler has been
	//  called.  Therefore divide this timer by 18.2 to get seconds.
	//  The most accurate way to time without reprogramming the PIC.

	asm	mov	ax, 0x0040
	asm	mov	es, ax
	asm	mov	di, 0x006C
	asm	mov	eax, dword ptr es:[di]
	asm	mov	dword ptr [tickcount], eax

	return tickcount;
}

/*
void	loadscreen ( char *filename, word destscreen )
{
	char	errorstring[256];

	// copy the name to a global variable.
	strcpy ( gstring, filename );

	// open the file with read only access (ax=handle on return)
	asm	push	bp
	asm	mov	ax, 0x3d00
	asm	lea	dx, [gstring]
	asm	int	0x21
	asm	mov	bp, ax
	asm	jnc	file_ok
	asm	pop	bp

	sprintf ( errorstring, "   *** error, cannot open file '%s'", gstring );
	fatalerror ( errorstring );

	// read the raw data
	file_ok:
	asm	mov	ah, 0x3f
	asm	mov   bx, bp
	asm	mov	cx, 64000
	asm	push	ds
	asm	mov	ds, [destscreen]
	asm	xor	dx, dx
	asm	int	0x21
	asm	pop	ds

	// close the file
	asm	mov	bx, bp
	asm	mov	ah, 0x3e
	asm	int	0x21
	asm	pop	bp
}

*/
