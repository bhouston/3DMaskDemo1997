/*
 *		nccgraph.cpp	- NCC Simple Graphics Functions
 *
 * 	Nothing Comes Close Library
 * 	Written By Ben Houston [plash / AZuRe]
 *
 *		Started: Friday, February 28, 1997
 *		Updated: n/a
 */


#include "ncctypes.h"
#include "nccgraph.h"


void	fadepalette ( colortype *pal, byte fade )
{
	int color;

	asm	mov	dx, 0x3c8
	asm	xor	al, al
	asm	out	dx, al
	for ( int i = 0; i < 256; i ++ ) {
		color = pal[i].r * fade / 64;
		asm	mov	al, byte ptr [color]
		asm	mov	dx, 0x3c9
		asm	out	dx, al
		color = pal[i].g * fade / 64;
		asm	mov	al, byte ptr [color]
		asm	mov	dx, 0x3c9
		asm	out	dx, al
		color = pal[i].b * fade / 64;
		asm	mov	al, byte ptr [color]
		asm	mov	dx, 0x3c9
		asm	out	dx, al
		}
}

void	setpalette ( colortype *pal )
{
	asm	mov	dx, 0x3c8
	asm	xor	al, al
	asm	out	dx, al
	asm	mov	dx, 0x3c9
	asm	mov	cx, 768
	asm	mov	si, [pal]
	asm	rep	outsb
}

void	vretrace ( void )
{
	asm	mov	dx, 0x3da
	retrace_end:
		asm	in		al, dx
		asm	test	al, 0x8
		asm	jnz	retrace_end
	retrace_start:
		asm	in		al, dx
		asm	test	al, 0x8
		asm	jz		retrace_start
}

void	copyrect ( word source, word dest, recttype rect )
{
	int	copywidth, skipwidth, copyheight, startoffset;

	copywidth = ((rect.r + 3) >> 2) - (rect.l >> 2);
	skipwidth = 320 - (copywidth << 2);
	copyheight = rect.b - rect.t + 1;
	startoffset = rect.t * 320 + ((rect.l >> 2) << 2);

	asm	push	ds
	asm	mov	ds, [source]
	asm	mov	es, [dest]
	asm	mov	si, [startoffset]
	asm	mov	di, si
	asm	mov	dx, [skipwidth]
	asm	mov	bx, [copyheight]
	lineloop:
		asm	mov	cx, [copywidth]
		asm	rep	movsd
		asm	add	di, dx
		asm	add	si, dx
		asm	dec	bx
		asm	jnz	lineloop
	asm	pop	ds
}

void	copyscreen ( word source, word dest )
{
	asm	push	ds
	asm	mov	ds, [source]
	asm	mov	es, [dest]
	asm	xor	di, di
	asm	xor	si, si
	asm	mov	cx, 64000/4
	asm	rep	movsd
	asm	pop	ds
}

void	fillscreen ( word dest, byte color )
{
	asm	mov	es, [dest]
	asm	xor	di, di
	asm	mov	bl, [color]
	asm	mov	bh, bl
	asm	mov	ax, bx
	asm	shl	eax, 16
	asm	mov	ax, bx
	asm	mov	cx, 64000/4
	asm	rep	stosd
}

void	copypixel ( word x, word y, word u, word v, word source, word dest )
{
	asm	mov	es, [source]
	asm	mov	al, byte ptr [u]
	asm	mov	ah, byte ptr [v]
	asm	mov	di, ax
	asm	mov	al, es:[di]

	asm	mov	es, [dest]
	asm	mov	bx, [y]
	asm	shl	bx, 8
	asm	mov	di, [x]
	asm	add	di, bx
	asm	shr	bx, 2
	asm	add	di, bx
	asm	mov	es:[di], al
}


void	putpixel ( word x, word y, byte color, word destscreen )
{
	asm	mov	al, [color]
	asm	mov	es, [destscreen]
	asm	mov	bx, [y]
	asm	shl	bx, 8
	asm	mov	di, [x]
	asm	add	di, bx
	asm	shr	bx, 2
	asm	add	di, bx
	asm	mov	es:[di], al
}

