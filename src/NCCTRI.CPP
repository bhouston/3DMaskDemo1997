/*
 *		ncctri.cpp	- NCC Triangle Fillers
 *
 * 	Nothing Comes Close Library
 * 	Written By Ben Houston [plash / AZuRe]
 *
 *		Started: Friday, February 28, 1997
 *		Updated: n/a
 */


#include "ncctypes.h"
#include "ncctri.h"

extern	byte			shadetable[4096];

screentype	*leftarray[3], *rightarray[3];
int			leftsection, rightsection;
int			leftsectionheight, rightsectionheight;
int			dudx, dudy, leftu, deltaleftu, leftv, deltaleftv;
int			dadx, dady, lefta, deltalefta, leftb, deltaleftb;
int			leftx, rightx;
word			deltaleftx, deltarightx;

int	getrightsection ( void );
int	getleftsection ( void );

void	envirotriangle ( screentype *v1, screentype *v2, screentype *v3, word enviromap, word destscreen )
{
	long	yratio;
	int	temp, xlongest;

	// This polygon drawer is not that great.  Its fairly speedy but it
	// is not a quality drawer.  It has the shakies but it the object its
	// drawing is moving fast enough you really won't notice the shaking much.

	// sort the triangle so that v1 is the top and v3 is the bottom
	if ( v1->y > v2->y ) { screentype *temp_ptr = v1; v1 = v2; v2 = temp_ptr; }
	if ( v1->y > v3->y ) { screentype *temp_ptr = v1; v1 = v3; v3 = temp_ptr; }
	if ( v2->y > v3->y ) { screentype *temp_ptr = v2; v2 = v3; v3 = temp_ptr; }

	// determine the length of the longest horizontal scanline
	temp = (v3->y - v1->y);
	if ( temp == 0 ) return;
	yratio	= ((v2->y - v1->y) << 6) / temp;
	xlongest	= ((v1->x - v2->x) << 6) + (v3->x - v1->x) * (int)yratio;
	if ( xlongest == 0 ) return;

/*	height = v3->y - v1->y;
	if ( height == 0 )
		return;
	temp		= ((v2->y - v1->y) << 6 ) / height;
	longest	= temp * (v3->x - v1->x) + ((v1->x - v2->x) << 6);
	if ( longest == 0 )
		return;*/

	// increase the magnitude to prevent overflow
	if ( xlongest < 0 )	{ if ( xlongest > -64 ) xlongest = -64; }
	else						{ if ( xlongest <  64 ) xlongest =  64; }

	if ( xlongest < 0 ) {
		rightarray[0] = v3;
		rightarray[1] = v2;
		rightarray[2] = v1;
		rightsection = 2;
		leftarray[0] = v3;
		leftarray[1] = v1;
		leftsection = 1;

		if ( getleftsection () <= 0 )
			return;
		if ( getrightsection () <= 0 ) {
			rightsection--;
			if ( getrightsection () <= 0 )
				return;
			}
		//if ( xlongest > -64 )
		//	xlongest	= -64;
		}
	else {
		leftarray[0] = v3;
		leftarray[1] = v2;
		leftarray[2] = v1;
		leftsection = 2;
		rightarray[0] = v3;
		rightarray[1] = v1;
		rightsection = 1;

		if ( getrightsection () <= 0 )
			return;
		if ( getleftsection () <= 0 ) {
			leftsection--;
			if ( getleftsection () <= 0 )
				return;
			}
		//if ( xlongest < 64 )
		//	xlongest	= 64;
		}

	yratio <<= 8;
	word	dudx = (int) ((( (long) (v1->u - v2->u) << 14) + (long)(v3->u - v1->u) * yratio) / xlongest);
	word	dvdx = (int) ((( (long) (v1->v - v2->v) << 14) + (long)(v3->v - v1->v) * yratio) / xlongest);
//	word	dudx = (int) ((((long)temp<<8)*(v3->u - v1->u)+(((long)v1->u - v2->u)<<14))/longest);
//	word	dvdx = (int) ((((long)temp<<8)*(v3->v - v1->v)+(((long)v1->v - v2->v)<<14))/longest);
	word	destptr = v1->y * 320;

	while ( 1 ) {
		int	width = (rightx >> 6) - (leftx >> 6);

		if ( width > 0 ) {
			asm	push	ds

			// setup es:[di]  -->  destination
			asm	mov	es, word ptr [destscreen]
			asm	mov	ax, word ptr [leftx]
			asm	shr	ax, 6
			asm	add	ax, word ptr [destptr]
			asm	mov	di, ax

			asm	mov	ax, word ptr [leftu]
			asm	mov	bx, word ptr [leftv]
			asm	mov	ds, word ptr [enviromap]

			asm	mov	cx, word ptr [width]

			pixel_loop:
				asm	mov	dh, ah
				asm	mov	dl, bh
				asm	mov	si, dx
				asm	mov	dl, ds:[si]
				asm	mov	es:[di], dl
				asm	add	ax, word ptr [dudx]
				asm	add	bx, word ptr [dvdx]
				asm	inc	di
				asm	dec	cx
				asm	jnz	pixel_loop

			asm	pop	ds
			}

		destptr += 320;

		if ( -- leftsectionheight <= 0 ) {
			if ( -- leftsection <= 0 )
				return;
			if ( getleftsection () <= 0 )
				return;
			}
		else {
			leftx += deltaleftx;
			leftu += deltaleftu;
			leftv += deltaleftv;
			}

		if ( -- rightsectionheight <= 0 ) {
			if ( -- rightsection <= 0 )
				return;
			if ( getrightsection () <= 0 )
				return;
			}
		else {
			rightx += deltarightx;
			}
		}
}

void	envirotexturetriangle ( screentype *v1, screentype *v2, screentype *v3, word enviromap, word texturemap, word destscreen )
{
	long	yratio;
	int	temp, xlongest;

	// This polygon drawer is not that great.  Its fairly speedy but it
	// is not a quality drawer.  It has the shakies but it the object its
	// drawing is moving fast enough you really won't notice the shaking much.

	// sort the triangle so that v1 is the top and v3 is the bottom
	if ( v1->y > v2->y ) { screentype *temp_ptr = v1; v1 = v2; v2 = temp_ptr; }
	if ( v1->y > v3->y ) { screentype *temp_ptr = v1; v1 = v3; v3 = temp_ptr; }
	if ( v2->y > v3->y ) { screentype *temp_ptr = v2; v2 = v3; v3 = temp_ptr; }

	// determine the length of the longest horizontal scanline
	temp = (v3->y - v1->y);
	if ( temp == 0 ) return;
	yratio	= ((v2->y - v1->y) << 6) / temp;
	xlongest	= ((v1->x - v2->x) << 6) + (v3->x - v1->x) * (int)yratio;
	if ( xlongest == 0 ) return;

/*	height = v3->y - v1->y;
	if ( height == 0 )
		return;
	temp		= ((v2->y - v1->y) << 6 ) / height;
	longest	= temp * (v3->x - v1->x) + ((v1->x - v2->x) << 6);
	if ( longest == 0 )
		return;*/

	// increase the magnitude to prevent overflow
	if ( xlongest < 0 )	{ if ( xlongest > -64 ) xlongest = -64; }
	else						{ if ( xlongest <  64 ) xlongest =  64; }

	if ( xlongest < 0 ) {
		rightarray[0] = v3;
		rightarray[1] = v2;
		rightarray[2] = v1;
		rightsection = 2;
		leftarray[0] = v3;
		leftarray[1] = v1;
		leftsection = 1;

		if ( getleftsection () <= 0 )
			return;
		if ( getrightsection () <= 0 ) {
			rightsection--;
			if ( getrightsection () <= 0 )
				return;
			}
		//if ( xlongest > -64 )
		//	xlongest	= -64;
		}
	else {
		leftarray[0] = v3;
		leftarray[1] = v2;
		leftarray[2] = v1;
		leftsection = 2;
		rightarray[0] = v3;
		rightarray[1] = v1;
		rightsection = 1;

		if ( getrightsection () <= 0 )
			return;
		if ( getleftsection () <= 0 ) {
			leftsection--;
			if ( getleftsection () <= 0 )
				return;
			}
		//if ( xlongest < 64 )
		//	xlongest	= 64;
		}

	yratio <<= 8;
	word dadx = (int) ((( (long) (v1->a - v2->a) << 14) + (v3->a - v1->a) * yratio) / xlongest);
	word dbdx = (int) ((( (long) (v1->b - v2->b) << 14) + (v3->b - v1->b) * yratio) / xlongest);
	word dudx = (int) ((( (long) (v1->u - v2->u) << 14) + (v3->u - v1->u) * yratio) / xlongest);
	word dvdx = (int) ((( (long) (v1->v - v2->v) << 14) + (v3->v - v1->v) * yratio) / xlongest);
//	word	dudx = (int) ((((long)temp<<8)*(v3->u - v1->u)+(((long)v1->u - v2->u)<<14))/longest);
//	word	dvdx = (int) ((((long)temp<<8)*(v3->v - v1->v)+(((long)v1->v - v2->v)<<14))/longest);
	word	destptr = v1->y * 320;

	while ( 1 ) {
		int	width = (rightx >> 6) - (leftx >> 6);

		if ( width > 0 ) {
			word u, v, a, b, poffset;
			byte	index, texel, shade;

			a = lefta;
			b = leftb;
			u = leftu;
			v = leftv;
			poffset = destptr + (leftx >> 6);

			do {

				//printf ( "texel -> (%i %i) ", a>>8, b>>8 );
				asm	mov	es, word ptr [texturemap]
				asm	mov	ah, byte ptr [a+1]
				asm	mov	al, byte ptr [b+1]
				asm	mov	di, ax
				asm	mov	al, es:[di]
				asm	mov	[texel], al

				//printf ( "shade -> (%i %i)\n", u>>8, v>>8 );
				asm	mov	es, word ptr [enviromap]
				asm	mov	ah, byte ptr [u+1]
				asm	mov	al, byte ptr [v+1]
				asm	mov	di, ax
				asm	mov	al, es:[di]
				asm	mov	[shade], al

				//printf ( "texel (%i) shade (%i) = (%i) at offset (%i) ",
				//	texel, shade, ((texel << 6) + shade), poffset );

				//flushall();


				index = shadetable[texel*128 + shade];
	//			printf ( "shade (%i) texel (%i)\n", (int)shade, (int)texel );
				//printf ( "index (%i)\n", index );

				//index = shade;

				asm	mov	es, [destscreen]
				asm	mov	di, [poffset]
				asm	mov	al, [index]
				asm	mov	es:[di], al

				poffset ++;
				a += dadx;
				b += dbdx;
				u += dudx;
				v += dvdx;
				width --;
				} while ( width > 0 );
			}
		destptr += 320;

		if ( -- leftsectionheight <= 0 ) {
			if ( -- leftsection <= 0 )
				return;
			if ( getleftsection () <= 0 )
				return;
			}
		else {
			leftx += deltaleftx;
			leftu += deltaleftu;
			leftv += deltaleftv;
			lefta += deltalefta;
			leftb += deltaleftb;
			}

		if ( -- rightsectionheight <= 0 ) {
			if ( -- rightsection <= 0 )
				return;
			if ( getrightsection () <= 0 )
				return;
			}
		else {
			rightx += deltarightx;
			}
		}
}

void	envirobumptexturetriangle ( screentype *v1, screentype *v2, screentype *v3,
					 word enviromap, word texturemap, word bumpmap, word destscreen )
{
	long	xratio, yratio;
	int	xlongest, ylongest, temp;
	word	dudx, dudy, dvdx, dvdy, dadx, dady, dbdx, dbdy;
	word	lineoffset, pixeloffset;
	int	linewidth;

	// This polygon drawer is not that great.  Its fairly speedy but it
	// is not a quality drawer.  It has the shakies but it the object its
	// drawing is moving fast enough you really won't notice the shaking much.

	// sort the triangle so that v1 is the top and v3 is the bottom
	if ( v1->y > v2->y ) { screentype *temp_ptr = v1; v1 = v2; v2 = temp_ptr; }
	if ( v1->y > v3->y ) { screentype *temp_ptr = v1; v1 = v3; v3 = temp_ptr; }
	if ( v2->y > v3->y ) { screentype *temp_ptr = v2; v2 = v3; v3 = temp_ptr; }

	// determine the length of the longest horizontal scanline
	temp = (v3->y - v1->y);
	if ( temp == 0 ) return;
	yratio	= ((v2->y - v1->y) << 6) / temp;
	xlongest	= ((v1->x - v2->x) << 6) + (v3->x - v1->x) * (int)yratio;
	if ( xlongest == 0 ) return;

	// increase the magnitude to prevent overflow
	if ( xlongest < 0 )	{ if ( xlongest > -64 ) xlongest = -64; }
	else						{ if ( xlongest <  64 ) xlongest =  64; }

	if ( xlongest < 0 ) {
		rightarray[0] = v3;
		rightarray[1] = v2;
		rightarray[2] = v1;
		rightsection = 2;
		leftarray[0] = v3;
		leftarray[1] = v1;
		leftsection = 1;

		if ( getleftsection () <= 0 )
			return;
		if ( getrightsection () <= 0 ) {
			rightsection--;
			if ( getrightsection () <= 0 )
				return;
			}
		}
	else {
		leftarray[0] = v3;
		leftarray[1] = v2;
		leftarray[2] = v1;
		leftsection = 2;
		rightarray[0] = v3;
		rightarray[1] = v1;
		rightsection = 1;

		if ( getrightsection () <= 0 )
			return;
		if ( getleftsection () <= 0 ) {
			leftsection--;
			if ( getleftsection () <= 0 )
				return;
			}
		}

	yratio <<= 8;
	dudx = (int) ((( (long) (v1->u - v2->u) << 14) + (v3->u - v1->u) * yratio) / xlongest);
	dvdx = (int) ((( (long) (v1->v - v2->v) << 14) + (v3->v - v1->v) * yratio) / xlongest);
	dadx = (int) ((( (long) (v1->a - v2->a) << 14) + (v3->a - v1->a) * yratio) / xlongest);
	dbdx = (int) ((( (long) (v1->b - v2->b) << 14) + (v3->b - v1->b) * yratio) / xlongest);

	lineoffset = v1->y * 320;

	// sort the triangle so that v1 is the left and v3 is the right
	if ( v1->x > v2->x ) { screentype *temp_ptr = v1; v1 = v2; v2 = temp_ptr; }
	if ( v1->x > v3->x ) { screentype *temp_ptr = v1; v1 = v3; v3 = temp_ptr; }
	if ( v2->x > v3->x ) { screentype *temp_ptr = v2; v2 = v3; v3 = temp_ptr; }

	// determine the lenght of the longest vertical scanline
	temp = (v3->x - v1->x);
	if ( temp == 0 ) return;
	xratio	= ((v2->x - v1->x) << 6) / temp;
	ylongest	= ((v1->y - v2->y) << 6) + (v3->y - v1->y) * (int)xratio;
	if ( ylongest == 0 ) return;

	// increase the magnitude to prevent overflow
	if ( ylongest < 0 )	{ if ( ylongest > -64 ) ylongest = -64; }
	else						{ if ( ylongest <  64 ) ylongest =  64; }

	xratio <<= 8;

//	dudy = (int) ((( (long) (v2->u - v1->u) << 14) + (v3->u - v2->u) * xratio) / ylongest);
//	dvdy = (word) ((( (long) (v2->v - v1->v) << 14) + (v3->v - v2->v) * xratio) / ylongest);
	dady = (int) ((( (long) (v1->a - v2->a) << 14) + (v3->a - v1->a) * xratio) / ylongest);
	dbdy = (int) ((( (long) (v1->b - v2->b) << 14) + (v3->b - v1->b) * xratio) / ylongest);
//	dbdx = (int) ((( (long) (v1->b - v2->b) << 14) + (v3->b - v1->b) * yratio) / xlongest);

	while ( 1 ) {
		byte	index, texel, shade;
		word	a1, a2, b1, b2;
		int	ah1,ah2,bh1,bh2;
		word	u, v, a, b;
		int bmpu, bmpv;

		linewidth = (rightx >> 6) - (leftx >> 6);

		if ( linewidth > 0 ) {

			u = leftu;	v = leftv;
			a = lefta;	b = leftb;

			pixeloffset = lineoffset + (leftx >> 6);

			do {
				//printf ( "texel -> (%i %i) ", a>>8, b>>8 );
				a1 = ((a + dadx) & 0xff00) + ((b + dbdx) >> 8);
				a2 = ((a - dadx) & 0xff00) + ((b - dbdx) >> 8);
				b1 = ((a + dady) & 0xff00) + ((b + dbdy) >> 8);
				b2 = ((a - dady) & 0xff00) + ((b - dbdy) >> 8);

				asm	xor	ax, ax
				asm	mov	es, [bumpmap]
				// the horizontal bumpmapping
				asm	mov	di, [a1]
				asm	mov	al, es:[di]
				asm	mov   [ah1], ax
				asm	mov	di, [a2]
				asm	mov	al, es:[di]
				asm	mov   [ah2], ax
				// the vertical bumpmapping
				asm	mov	di, [b1]
				asm	mov	al, es:[di]
				asm	mov   [bh1], ax
				asm	mov	di, [b2]
				asm	mov	al, es:[di]
				asm	mov   [bh2], ax

				bmpu = ah1 - ah2 + (u >> 8);
				bmpv = bh1 - bh2 + (v >> 8);

				if ( bmpu >= 0 && bmpu < 256 && bmpv >= 0 && bmpv < 256 )
				{
					asm	mov	es, word ptr [texturemap]
					asm	mov	ah, byte ptr [a+1]
					asm	mov	al, byte ptr [b+1]
					asm	mov	di, ax
					asm	mov	al, es:[di]
					asm	mov	[texel], al

					//printf ( "shade -> (%i %i)\n", u>>8, v>>8 );
					asm	mov	es, word ptr [enviromap]
					asm	mov	ah, byte ptr [bmpu]
					asm	mov	al, byte ptr [bmpv]
					asm	mov	di, ax
					asm	mov	al, es:[di]
					asm	mov	[shade], al

					index = shadetable[texel*128 + shade];

					asm	mov	es, [destscreen]
					asm	mov	di, [pixeloffset]
					asm	mov	al, [index]
					asm	mov	es:[di], al
				}
				else {
					asm	mov	es, [destscreen]
					asm	mov	di, [pixeloffset]
					asm	xor	al, al
					asm	mov	es:[di], al
				}
				pixeloffset ++;
				a += dadx;
				b += dbdx;
				u += dudx;
				v += dvdx;
				linewidth --;
				} while ( linewidth > 0 );
			}

		lineoffset += 320;

		if ( -- leftsectionheight <= 0 ) {
			if ( -- leftsection <= 0 )
				return;
			if ( getleftsection () <= 0 )
				return;
			}
		else {
			leftx += deltaleftx;
			leftu += deltaleftu;
			leftv += deltaleftv;
			lefta += deltalefta;
			leftb += deltaleftb;
			}

		if ( -- rightsectionheight <= 0 ) {
			if ( -- rightsection <= 0 )
				return;
			if ( getrightsection () <= 0 )
				return;
			}
		else {
			rightx += deltarightx;
			}
		}
}

void	texturetriangle ( screentype *v1, screentype *v2, screentype *v3, word texturemap, word destscreen )
{
	long	yratio;
	int	temp, xlongest;

	// This polygon drawer is not that great.  Its fairly speedy but it
	// is not a quality drawer.  It has the shakies but it the object its
	// drawing is moving fast enough you really won't notice the shaking much.

	// sort the triangle so that v1 is the top and v3 is the bottom
	if ( v1->y > v2->y ) { screentype *temp_ptr = v1; v1 = v2; v2 = temp_ptr; }
	if ( v1->y > v3->y ) { screentype *temp_ptr = v1; v1 = v3; v3 = temp_ptr; }
	if ( v2->y > v3->y ) { screentype *temp_ptr = v2; v2 = v3; v3 = temp_ptr; }

	// determine the length of the longest horizontal scanline
	temp = (v3->y - v1->y);
	if ( temp == 0 ) return;
	yratio	= ((v2->y - v1->y) << 6) / temp;
	xlongest	= ((v1->x - v2->x) << 6) + (v3->x - v1->x) * (int)yratio;
	if ( xlongest == 0 ) return;

/*	height = v3->y - v1->y;
	if ( height == 0 )
		return;
	temp		= ((v2->y - v1->y) << 6 ) / height;
	longest	= temp * (v3->x - v1->x) + ((v1->x - v2->x) << 6);
	if ( longest == 0 )
		return;*/

	// increase the magnitude to prevent overflow
	if ( xlongest < 0 )	{ if ( xlongest > -64 ) xlongest = -64; }
	else						{ if ( xlongest <  64 ) xlongest =  64; }

	if ( xlongest < 0 ) {
		rightarray[0] = v3;
		rightarray[1] = v2;
		rightarray[2] = v1;
		rightsection = 2;
		leftarray[0] = v3;
		leftarray[1] = v1;
		leftsection = 1;

		if ( getleftsection () <= 0 )
			return;
		if ( getrightsection () <= 0 ) {
			rightsection--;
			if ( getrightsection () <= 0 )
				return;
			}
		//if ( xlongest > -64 )
		//	xlongest	= -64;
		}
	else {
		leftarray[0] = v3;
		leftarray[1] = v2;
		leftarray[2] = v1;
		leftsection = 2;
		rightarray[0] = v3;
		rightarray[1] = v1;
		rightsection = 1;

		if ( getrightsection () <= 0 )
			return;
		if ( getleftsection () <= 0 ) {
			leftsection--;
			if ( getleftsection () <= 0 )
				return;
			}
		//if ( xlongest < 64 )
		//	xlongest	= 64;
		}

	yratio <<= 8;
	word dadx = (int) ((( (long) (v1->a - v2->a) << 14) + (v3->a - v1->a) * yratio) / xlongest);
	word dbdx = (int) ((( (long) (v1->b - v2->b) << 14) + (v3->b - v1->b) * yratio) / xlongest);
//	word	dudx = (int) ((( (long) (v1->u - v2->u) << 14) + (long)(v3->u - v1->u) * yratio) / xlongest);
//	word	dvdx = (int) ((( (long) (v1->v - v2->v) << 14) + (long)(v3->v - v1->v) * yratio) / xlongest);
//	word	dudx = (int) ((((long)temp<<8)*(v3->u - v1->u)+(((long)v1->u - v2->u)<<14))/longest);
//	word	dvdx = (int) ((((long)temp<<8)*(v3->v - v1->v)+(((long)v1->v - v2->v)<<14))/longest);
	word	destptr = v1->y * 320;

	while ( 1 ) {
		int	width = (rightx >> 6) - (leftx >> 6);

		if ( width > 0 ) {
			asm	push	ds

			// setup es:[di]  -->  destination
			asm	mov	es, word ptr [destscreen]
			asm	mov	ax, word ptr [leftx]
			asm	shr	ax, 6
			asm	add	ax, word ptr [destptr]
			asm	mov	di, ax

			asm	mov	ax, word ptr [lefta]
			asm	mov	bx, word ptr [leftb]
			asm	mov	ds, word ptr [texturemap]

			asm	mov	cx, word ptr [width]

			pixel_loop:
				asm	mov	dh, ah
				asm	mov	dl, bh
				asm	mov	si, dx
				asm	mov	dl, ds:[si]
				asm	mov	es:[di], dl
				asm	add	ax, word ptr [dadx]
				asm	add	bx, word ptr [dbdx]
				asm	inc	di
				asm	dec	cx
				asm	jnz	pixel_loop

			asm	pop	ds
			}

		destptr += 320;

		if ( -- leftsectionheight <= 0 ) {
			if ( -- leftsection <= 0 )
				return;
			if ( getleftsection () <= 0 )
				return;
			}
		else {
			leftx += deltaleftx;
			lefta += deltalefta;
			leftb += deltaleftb;
			}

		if ( -- rightsectionheight <= 0 ) {
			if ( -- rightsection <= 0 )
				return;
			if ( getrightsection () <= 0 )
				return;
			}
		else {
			rightx += deltarightx;
			}
		}
}

void	envirobumptriangle ( screentype *v1, screentype *v2, screentype *v3, word enviromap, word bumpmap, word destscreen )
{
	long	xratio, yratio;
	int	xlongest, ylongest, temp;
	word	dudx, dudy, dvdx, dvdy, dadx, dady, dbdx, dbdy;
	word	lineoffset, pixeloffset;
	int	linewidth;

	// This polygon drawer is not that great.  Its fairly speedy but it
	// is not a quality drawer.  It has the shakies but it the object its
	// drawing is moving fast enough you really won't notice the shaking much.

	// sort the triangle so that v1 is the top and v3 is the bottom
	if ( v1->y > v2->y ) { screentype *temp_ptr = v1; v1 = v2; v2 = temp_ptr; }
	if ( v1->y > v3->y ) { screentype *temp_ptr = v1; v1 = v3; v3 = temp_ptr; }
	if ( v2->y > v3->y ) { screentype *temp_ptr = v2; v2 = v3; v3 = temp_ptr; }

	// determine the length of the longest horizontal scanline
	temp = (v3->y - v1->y);
	if ( temp == 0 ) return;
	yratio	= ((v2->y - v1->y) << 6) / temp;
	xlongest	= ((v1->x - v2->x) << 6) + (v3->x - v1->x) * (int)yratio;
	if ( xlongest == 0 ) return;

	// increase the magnitude to prevent overflow
	if ( xlongest < 0 )	{ if ( xlongest > -64 ) xlongest = -64; }
	else						{ if ( xlongest <  64 ) xlongest =  64; }

	if ( xlongest < 0 ) {
		rightarray[0] = v3;
		rightarray[1] = v2;
		rightarray[2] = v1;
		rightsection = 2;
		leftarray[0] = v3;
		leftarray[1] = v1;
		leftsection = 1;

		if ( getleftsection () <= 0 )
			return;
		if ( getrightsection () <= 0 ) {
			rightsection--;
			if ( getrightsection () <= 0 )
				return;
			}
		}
	else {
		leftarray[0] = v3;
		leftarray[1] = v2;
		leftarray[2] = v1;
		leftsection = 2;
		rightarray[0] = v3;
		rightarray[1] = v1;
		rightsection = 1;

		if ( getrightsection () <= 0 )
			return;
		if ( getleftsection () <= 0 ) {
			leftsection--;
			if ( getleftsection () <= 0 )
				return;
			}
		}

	yratio <<= 8;
	dudx = (int) ((( (long) (v1->u - v2->u) << 14) + (v3->u - v1->u) * yratio) / xlongest);
	dvdx = (int) ((( (long) (v1->v - v2->v) << 14) + (v3->v - v1->v) * yratio) / xlongest);
	dadx = (int) ((( (long) (v1->a - v2->a) << 14) + (v3->a - v1->a) * yratio) / xlongest);
	dbdx = (int) ((( (long) (v1->b - v2->b) << 14) + (v3->b - v1->b) * yratio) / xlongest);

	lineoffset = v1->y * 320;

	// sort the triangle so that v1 is the left and v3 is the right
	if ( v1->x > v2->x ) { screentype *temp_ptr = v1; v1 = v2; v2 = temp_ptr; }
	if ( v1->x > v3->x ) { screentype *temp_ptr = v1; v1 = v3; v3 = temp_ptr; }
	if ( v2->x > v3->x ) { screentype *temp_ptr = v2; v2 = v3; v3 = temp_ptr; }

	// determine the lenght of the longest vertical scanline
	temp = (v3->x - v1->x);
	if ( temp == 0 ) return;
	xratio	= ((v2->x - v1->x) << 6) / temp;
	ylongest	= ((v1->y - v2->y) << 6) + (v3->y - v1->y) * (int)xratio;
	if ( ylongest == 0 ) return;

	// increase the magnitude to prevent overflow
	if ( ylongest < 0 )	{ if ( ylongest > -64 ) ylongest = -64; }
	else						{ if ( ylongest <  64 ) ylongest =  64; }

	xratio <<= 8;

//	dudy = (int) ((( (long) (v2->u - v1->u) << 14) + (v3->u - v2->u) * xratio) / ylongest);
//	dvdy = (word) ((( (long) (v2->v - v1->v) << 14) + (v3->v - v2->v) * xratio) / ylongest);
	dady = (int) ((( (long) (v1->a - v2->a) << 14) + (v3->a - v1->a) * xratio) / ylongest);
	dbdy = (int) ((( (long) (v1->b - v2->b) << 14) + (v3->b - v1->b) * xratio) / ylongest);
//	dbdx = (int) ((( (long) (v1->b - v2->b) << 14) + (v3->b - v1->b) * yratio) / xlongest);

	while ( 1 ) {

		word	a1, a2, b1, b2;
		int	ah1,ah2,bh1,bh2;
		word	u, v, a, b;
		int bmpu, bmpv;

		linewidth = (rightx >> 6) - (leftx >> 6);

		if ( linewidth > 0 ) {

			u = leftu;	v = leftv;
			a = lefta;	b = leftb;

			pixeloffset = lineoffset + (leftx >> 6);

			do {
				a1 = ((a + dadx) & 0xff00) + ((b + dbdx) >> 8);
				a2 = ((a - dadx) & 0xff00) + ((b - dbdx) >> 8);
				b1 = ((a + dady) & 0xff00) + ((b + dbdy) >> 8);
				b2 = ((a - dady) & 0xff00) + ((b - dbdy) >> 8);

				asm	xor	ax, ax
				asm	mov	es, [bumpmap]
				// the horizontal bumpmapping
				asm	mov	di, [a1]
				asm	mov	al, es:[di]
				asm	mov   [ah1], ax
				asm	mov	di, [a2]
				asm	mov	al, es:[di]
				asm	mov   [ah2], ax
				// the vertical bumpmapping
				asm	mov	di, [b1]
				asm	mov	al, es:[di]
				asm	mov   [bh1], ax
				asm	mov	di, [b2]
				asm	mov	al, es:[di]
				asm	mov   [bh2], ax

				bmpu = ah1 - ah2 + (u >> 8);
				bmpv = bh1 - bh2 + (v >> 8);

				if ( bmpu >= 0 && bmpu < 256 && bmpv >= 0 && bmpv < 256 )
				{
					asm	mov	es, [enviromap]
					asm	mov	ah, [bmpu]
					asm	mov	al, [bmpv]
					asm	mov	di, ax
					asm	mov	al, es:[di]
					asm	mov	es, [destscreen]
					asm	mov	di, [pixeloffset]
					asm	mov	es:[di], al
				}
				else
				{
					asm	xor	ax, ax
					asm	mov	es, [destscreen]
					asm	mov	di, [pixeloffset]
					asm	mov	es:[di], al
				}
				u += dudx;
				v += dvdx;
				a += dadx;
				b += dbdx;

				pixeloffset++;
				linewidth --;

				} while ( linewidth > 0 );

			}

		lineoffset += 320;

		if ( -- leftsectionheight <= 0 ) {
			if ( -- leftsection <= 0 )
				return;
			if ( getleftsection () <= 0 )
				return;
			}
		else {
			leftx += deltaleftx;
			lefta += deltalefta;
			leftb += deltaleftb;
			leftu += deltaleftu;
			leftv += deltaleftv;
			}

		if ( -- rightsectionheight <= 0 ) {
			if ( -- rightsection <= 0 )
				return;
			if ( getrightsection () <= 0 )
				return;
			}
		else {
			rightx += deltarightx;
			}
		}
}

int	getrightsection ( void )
{
	int	height;

	screentype *v1 = rightarray[rightsection];
	screentype *v2 = rightarray[rightsection-1];

	height = v2->y - v1->y;
	if ( height == 0 )
		return 0;

	deltarightx = ((v2->x - v1->x) << 6) / height;
	rightx = v1->x << 6;
	rightsectionheight = height;

	return height;
}

int	getleftsection ( void )
{
	int height;

	screentype *v1 = leftarray[leftsection];
	screentype *v2 = leftarray[leftsection-1];

	height = v2->y - v1->y;
	if ( height == 0 )
		return 0;

	deltaleftx = ((v2->x - v1->x) << 6) / height;
	leftx = v1->x << 6;

	// texture coordinates
	deltalefta = (((v2->a - v1->a) << 6) / height) << 2;
	lefta = (word)v1->a << 8;
	deltaleftb = (((v2->b - v1->b) << 6) / height) << 2;
	leftb = (word)v1->b << 8;

	// phong coordinates
	deltaleftu = (((v2->u - v1->u) << 6) / height) << 2;
	leftu = (word)v1->u << 8;
	deltaleftv = (((v2->v - v1->v) << 6) / height) << 2;
	leftv = (word)v1->v << 8;
	leftsectionheight = height;

	return height;
}
