/*
 *		PTB_Demo.Cpp
 *			- An Phong-Texture-Bump Demostration
 *
 *		Coded by Ben Houston [plash | AZuRe]
 *
 *		Started :  Friday, July 26, 1996
 *		Released : Friday, March 14, 1997
 */


#include "ncctypes.h"
#include "nccmisc.h"
#include "nccgraph.h"
#include "ncctri.h"
#include "ncccolor.h"

#include <string.h>

#define	RS_PLASTIC	0
#define	RS_METAL		1
#define	RS_TEXTURE	2

#define	MAX_VERTEX		980
#define	MAX_FACET		1620
#define	MAX_ANGLE		512
#define	MAX_DELTA		25

#define	OPTPAL_SIZE			128
#define	OPTPAL_OFFSET		0
#define	PAL_SIZE			255
#define	PAL_OFFSET		1

#define	K_AMBIENT			0.2
#define	K_DIFFUSE         0.9
#define	K_SPECULAR        0.7
#define	K_ATTENUATION     1.0
#define	K_FALLOFF			150

#define	FACE_R			0.86
#define	FACE_G			0.23
#define	FACE_B			0.59

#define	AMBIENT_R		0.0
#define	AMBIENT_G		0.0
#define	AMBIENT_B		0.0

#define	LIGHT_R			0.83
#define	LIGHT_G			0.83
#define	LIGHT_B			0.83


char			vertexfile[15]			= "mask.vtx",		// vertices
				normalfile[15]			= "mask.nrm",		// vertex normals
				facetfile[15]			= "mask.fct",		// facets
				txtrcoordfile[15]		= "mask.txr",		// texture coords
				backgroundfile[15]	= "azure.raw",		// background bitmap
				reflectfile[15]		= "refmap.raw",	// reflection map
				bumpmapfile[15]		= "bumpmap.raw",  // bump map
				texturefile[15]		= "texture.raw",	// texture map
				texturepalfile[15]	= "texture.pal";	// texture palette

//extern	colortype	*basecolor;
vectortype	*objectvertex = (vectortype*)&basecolor[0];
vectortype	*objectnormal = (vectortype*)&basecolor[2048];
texturetype	*objecttexture = (texturetype*)objectnormal;
screentype	screenvertex[MAX_VERTEX];
facettype	objectfacet[MAX_FACET];

word			totalvertices, totalfacets;
int			xrotation, yrotation;
word			vscreen, bumpmap, facetorder, background;
word			enviromap, texturemap;
int			asinlookup[256], bincount[80];
int			zdepth, currentcolor;
colortype	reflectpal[256], *phongpal = reflectpal;
colortype	*currentpal = reflectpal, texturepal[32];
recttype		currentrect, screenrect = { 0, 0, 319, 199 },
				dirtyrect, invalidrect = { 319, 199, 0, 0 };
int			renderstyle;
boolean		bumpmapping, solid, rotation;
extern	numbasecolors;

// startup functions
void	init ( void );
real	phongillumination ( real diffusecolor, real specularcolor,
					real lightcolor,  real ambientcolor, real theta );
void	createbasecolors ( void );
void	initlookups ( void );
void	initobject ( void );
void	initbumpmap ( void );
void	inittexture ( void );
void	initgraphics ( void );
void	fatalerror ( char *msg );

// mapping schemes
void	createminiphongmap ( void );
void	createphongmap ( void );
void	loadreflectmap ( void );
void	loadbackground ( void );

// rendering style setups
void	setupreflectivemetal ( void );
void	setupplasticphong ( void );
void	setupphongtexture ( void );

// scene transistion functions
void	exitsequence ( void );
void	introsequence ( void );

// vertex and facet processing functions (or whatever...)
void	drawframe ( void );
void	transformvertices ( void );
void	sortfacets ( void );
void	drawfacets ( void );
void	drawvertices ( void );

// shutdown functions
void	close ( void );


/* -----------------------------------------------------------
 *
 * 		the azure mask 3d engine (don't fool yourself, it's shit!)
 *
 */

void	main ( void )
{
	dword		startcount, endcount, frames;
	int		xinc, yinc;

	clrscr ();

	printf ( "\n" );
	printf ( "   PTB_Demo.Exe (Needs around 540 kb of free memory)\n" );
	printf ( "      - Phong-Texture-Bump Demostration\n\n" );

	printf ( "   Coded by Ben Houston [ plash / AZuRe ]\n" );
	printf ( "      e-mail: 'w_houston@mail.public.net'\n" );
	printf ( "   Released Sunday, March 16, 1997\n\n" );

	printf ( "   Greetz: MRI, Rex Deathstar, Nooon, Miracle, Complex\n" );
	printf ( "      Asch, Mundane, OTM, Hornet !!!!\n\n" );

	printf ( "   Runtime Controls:\n" );
	printf ( "      'a' and 'z' to change object distance,\n" );
	printf ( "      'd' and 'f' to change x rotation,\n" );
	printf ( "      'c' and 'v' to change y rotation,\n" );
	printf ( "      'spacebar' to stop all rotation,\n" );
	printf ( "      'r' to use metal mapping (render style),\n" );
	printf ( "      'p' to use phong plastic (render style),\n" );
	printf ( "      't' to use phong texture (render style),\n" );
	printf ( "      'b' to toggle bumpmapping,\n" );
	printf ( "      's' to toggle polygon filling,\n" );
	printf ( "      'q' or 'esc' to quit.\n\n" );

	printf ( "   Press a key to start demo" );
	getch ();

	init ();

	rotation = true;
	bumpmapping = false;
	solid = true;

	xrotation = 127;
	yrotation = 20;
	zdepth = 200;
	dirtyrect = screenrect;
	currentrect = screenrect;

	setupplasticphong ();
//	setupplasticphong ();
	introsequence ();

	xinc = 3;
	yinc = -2;
	frames = 0;
	startcount = gettickcount ();

	do {

		if ( rotation ) {
			// update the x rotation while keeping it between 0 and MAX_ANGLE
			xrotation = xrotation + xinc;
			if ( xrotation >= MAX_ANGLE )
				xrotation -= MAX_ANGLE;
			if ( xrotation < 0 )
				xrotation += MAX_ANGLE;

			// update the y rotation while keeping it between 0 and MAX_ANGLE
			yrotation = yrotation + yinc;
			if ( yrotation >= MAX_ANGLE )
				yrotation -= MAX_ANGLE;
			if ( yrotation < 0 )
				yrotation += MAX_ANGLE;
			}

		drawframe ();

		// synconize with vertical retrace (it's just a slowdown!)
	//		vretrace ();

		// overwrite the dirty portion of the screen and write the
		// new object.
		copyrect ( vscreen, 0xa000, dirtyrect );
		frames ++;


		// if a key has been hit get it and check what it was.
		if ( kbhit () )
			switch ( getch () ) {
				// do you want to quit?
				case 'q':
				case 27 :
					goto done;
				// modify rotation
				case ' ':
					rotation = ( rotation == false );
					break;
				// toggle bumpmapping
				case 'b' :
					bumpmapping = ( bumpmapping == false );
					break;
				// pick the rendering style
				case 't' :
					if ( renderstyle != RS_TEXTURE ) {
						setupphongtexture ();
						setpalette ( currentpal );
						}
					break;
				case 'p' :
					if ( renderstyle != RS_PLASTIC ) {
						setupplasticphong ();
						setpalette ( currentpal );
						}
					break;
				case 'm' :
					if ( renderstyle != RS_METAL ) {
						setupreflectivemetal ();
						setpalette ( currentpal );
						}
					break;
				// toggle point or poly fill
				case 's' :
					solid = ( solid == false );
					break;
				// increase x rotation
				case 'd' :
					xinc ++;
					if ( xinc > MAX_DELTA )
						xinc = MAX_DELTA;
					break;
				// decrease x rotation
				case 'f' :
					xinc --;
					if ( xinc < -MAX_DELTA )
						xinc = -MAX_DELTA;
					break;
				// increase y rotation
				case 'c' :
					yinc ++;
					if ( yinc > MAX_DELTA )
						yinc = MAX_DELTA;
					break;
				// decrease y rotation
				case 'v' :
					yinc --;
					if ( yinc < -MAX_DELTA )
						yinc = -MAX_DELTA;
					break;
				// push the object father away
				case 'a' :
					zdepth += 10;
					if ( zdepth > 2000 )
						zdepth = 2000;
					break;
				// bring the object closer
				case 'z' :
					zdepth -= 10;
					if ( zdepth < 200 )
						zdepth = 200;
					break;
				}
		} while ( 1 );
	done:

	endcount = gettickcount ();

	exitsequence ();

	close ();

	clrscr ();

	printf ( "\n\n" );
	printf ( "   Coded by Ben Houston [ plash / AZuRe ]\n" );
	printf ( "      e-mail: 'w_houston@mail.public.net'\n\n" );

	printf ( "   Runtime Stats:\n" );
	printf ( "      Vertices in Object        - %i\n", totalvertices );
	printf ( "      Faces in Object           - %i\n", totalfacets );
	printf ( "      Average Frames Per Second - %f\n\n", (frames*18.2)/(endcount-startcount) );
}

void	drawframe ( void )
{
	// what was drawn last frame in the vscreen?
	dirtyrect = currentrect;

	// reset the current rect.
	currentrect = invalidrect;

	// copy the background to the dirty portion of the
	// virtual screen (drawing area)
	copyrect ( background, vscreen, dirtyrect );

	// rotate the vertices and calculate the phong enviro coords
	transformvertices ();

	// are we solid filling?
	if ( solid == true )
	{
		// do face hidding then sorting.
		sortfacets ();
		// draw all faces that were sorted.
		drawfacets ();
	}
	else
	{
		// just draw the vertices of the non hidden polys
		drawvertices ();
	}

	// get the combined rectangle containing both the current and
	//	the dirty rectangle.
	if ( currentrect.l < dirtyrect.l )
		dirtyrect.l = currentrect.l;
	if ( currentrect.r > dirtyrect.r )
		dirtyrect.r = currentrect.r;
	if ( currentrect.t < dirtyrect.t )
		dirtyrect.t = currentrect.t;
	if ( currentrect.b > dirtyrect.b )
		dirtyrect.b = currentrect.b;
}

void	setupreflectivemetal ( void )
{
	loadbackground ();
	copyscreen ( background, vscreen );

	for ( int loop = 0; loop < 256; loop ++ ) {
		reflectpal[loop].r = loop>>2;
		reflectpal[loop].g = loop>>2;
		reflectpal[loop].b = loop>>2;
		}

	loadreflectmap ();

	renderstyle = RS_METAL;

	drawframe ();

	vretrace ();
	fadepalette ( currentpal, 0 );
	vretrace ();
	copyscreen ( vscreen, 0xa000 );
	vretrace ();

	currentpal = reflectpal;

}

void	setupplasticphong ( void )
{
	int	loop, baseindex;
	real	incedent;

	loadbackground ();
	copyscreen ( background, vscreen );

	// create phong palette
	phongpal[0].r = 0;
	phongpal[0].g = 0;
	phongpal[0].b = 0;

	for ( loop = 0; loop < PAL_SIZE; loop ++ ) {

		// what is the incedent angle that we are calculating a color for?
		// Scale loop to 0 to 1, flip then scale it up to 0 to 90 degrees.
		// I say degrees but remember that the computer works in radians.
		incedent = ( (real)(PAL_SIZE - ( loop + 1 )) / PAL_SIZE ) * ( M_PI / 2 );

		baseindex = ( loop + PAL_OFFSET );

		// determine the rgb components
		phongpal[baseindex].r = 63 * phongillumination ( FACE_R, 1.0, LIGHT_R, AMBIENT_R, incedent );
		phongpal[baseindex].g = 63 * phongillumination ( FACE_G, 1.0, LIGHT_G, AMBIENT_G, incedent );
		phongpal[baseindex].b = 63 * phongillumination ( FACE_B, 1.0, LIGHT_B, AMBIENT_B, incedent );
		}

	createphongmap ();

	renderstyle = RS_PLASTIC;

	drawframe ();

	vretrace ();
	fadepalette ( currentpal, 0 );
	vretrace ();
	copyscreen ( vscreen, 0xa000 );
	vretrace ();

	currentpal = phongpal;
}

void	setupphongtexture ( void )
{
	// load the background(hopefully in the disk cache)
	loadbackground ();

	// adjust the background to use the yellow color of the bitmap
	asm	mov	es, [background]
	asm	xor	di, di
	asm	mov	cx, 64000
	pixel_l:
		asm	xor	bx, bx
		asm	mov	bl, es:[di]
		asm	shr	bx, 1
		asm	add	bx, 128*30
		asm	mov	al, shadetable[bx]
		asm   mov	es:[di], al
		asm	inc	di
		asm	dec	cx
		asm	jnz	pixel_l;

	// make sure that the vscreen is in sync with the background
	copyscreen ( background, vscreen );

	// this one a little slow :(
	createminiphongmap ();

	// we are now texturing
	renderstyle = RS_TEXTURE;

	drawframe ();

	// don't let anyone see the picture when the palette doesn't match
	vretrace ();
	fadepalette ( currentpal, 0 );
	vretrace ();
	copyscreen ( vscreen, 0xa000 );
	vretrace ();

	// use the optimal palette for the texture (used by shade table)
	currentpal = optimalpal;
}

/* -----------------------------------------------------------
 *
 * 		scene transistions
 *
 */

void	introsequence ( void )
{
	int	fade;

	// a slow fade in
	for ( fade = 0; fade < 64; fade ++ )
	{
		vretrace ();
		vretrace ();
		vretrace ();
		fadepalette ( currentpal, fade );
	}
	delay ( 500 );
}

void	exitsequence ( void )
{
	int	fade;

	// a quick fade out
	for ( fade = 0; fade < 64; fade ++ )
	{
		vretrace ();
		fadepalette ( currentpal, 63 - fade );
	}
}


/* -----------------------------------------------------------
 *
 * 		vertex and facet processing functions
 *
 */

void	transformvertices ( void )
{
	int	loop, xint, yint, zint;
	long	xtemp, ytemp, ztemp, x, y, z;
	long	xcos, xsin, ycos, ysin;

	// if your wondering it would have been better to use a 3x3 matrix
	//	rotation but I didn't feel like is at the time.

	// get the cos and sin values for the selected rotations
	// they are going to be used in the following linear rotations.

	xcos = (int) ( cos ( (float)xrotation / MAX_ANGLE * M_PI * 2 ) * 256 );
	xsin = (int) ( sin ( (float)xrotation / MAX_ANGLE * M_PI * 2 ) * 256 );
	ycos = (int) ( cos ( (float)yrotation / MAX_ANGLE * M_PI * 2 ) * 256 );
	ysin = (int) ( sin ( (float)yrotation / MAX_ANGLE * M_PI * 2 ) * 256 );

	for ( loop = 0; loop < totalvertices; loop ++ ) {
		// grab the object space vertex position vector
		x = objectvertex[loop].x;
		y = objectvertex[loop].y;
		z = objectvertex[loop].z;

		// rotating around the x axis
		ytemp = y;
		ztemp = z;
		y = ( ytemp * xcos - ztemp * xsin ) >> 8;
		z = ( ytemp * xsin + ztemp * xcos ) >> 8;


		// rotating around the y axis
		xtemp = x;
		ztemp = z;
		x = ( xtemp * ycos - ztemp * ysin ) >> 8;
		z = ( xtemp * ysin + ztemp * ycos ) >> 8;

		xint = (int)x;
		yint = (int)y;
		zint = ( (int)z >> 8 ) + zdepth;	// preparing for projection

		// do the projection to screen space
		xint = xint / zint + 160;		// there is distortion because I didn't
		yint = yint / zint + 100;		// take into account the non square screen pixels

		// update bounding box
		if ( yint < currentrect.t )
			currentrect.t = yint;
		else if ( yint > currentrect.b )
			currentrect.b = yint;

		if ( xint < currentrect.l )
			currentrect.l = xint;
		else if ( xint > currentrect.r )
			currentrect.r = xint;

		// remember the screen space vertex
		screenvertex[loop].x = xint;
		screenvertex[loop].y = yint;
		screenvertex[loop].z = (int)z;

		// grad the object space vertex normal
		x = objectnormal[loop].x;
		y = objectnormal[loop].y;
		z = objectnormal[loop].z;

		// rotate around the x axis
		ytemp = y;
		ztemp = z;
		y = ( ytemp * xcos - ztemp * xsin ) >> 8;
		z = ( ytemp * xsin + ztemp * xcos ) >> 8;

		// rotate around the y axis
		xtemp = x;
		ztemp = z;
		x = ( xtemp * ycos - ztemp * ysin ) >> 8;
//		z = ( xtemp * ysin + ztemp * ycos ) >> 8;  we don't need the z component.

		xint = (int)x;
		yint = (int)y;

		// determine the x and y angles that are using simular to
		// how voltair's fast phong except that we are doing both x and y
		// instead of interpolating just z.  WE get highlights inside polygons
		// now!!!!
		screenvertex[loop].u = asinlookup[(xint+256) >> 1];
		screenvertex[loop].v = asinlookup[(yint+256) >> 1];
		}
}

void	sortfacets ( void )
{
	int	facet, xnormal, depthbin;
	screentype	*v1, *v2, *v3;
	word far	*bin_ptr;

	// a bin sort is being used because more precision is not needed.
	// my compiler optimizes this loop well, thank god.

	for ( facet = 0; facet < totalfacets; facet++ ) {

		// grab pointers to the facets 3 vertices
		v1 = (screentype*)objectfacet[facet].a;
		v2 = (screentype*)objectfacet[facet].b;
		v3 = (screentype*)objectfacet[facet].c;

		// cross product to determine if the facet is facing the
		// veiwer
		xnormal = (v2->x - v1->x)*(v1->y - v3->y) - (v2->y - v1->y)*(v1->x - v3->x);
		if ( xnormal < 0 )
			continue;	// facet is not facing the viewer

		// determine which bin this facets is going to end up in by
		// find the z midpoint of the facet
		depthbin = ( v1->z >> 11) + (v2->z >> 11) + (v3->z >> 11) + 40;

		// if the facet depth inside the bin sort range
		if (( depthbin < 80 )&&( depthbin >= 0 )) {

			bin_full:

			// is the selected bin full ?
			if ( bincount[depthbin] < 128 ) {

				// no the bin isn't full to add this facets to this bin
				bin_ptr = (word far*)MK_FP ( facetorder,
								 ( depthbin << 8 ) + ( bincount[depthbin] << 1 ) );
				*bin_ptr = facet;	// being trick, just saving the facet offset
				bincount[depthbin]++;	// add one to the number of facets in the bin
				}
			else {

				// the bin is full to try the next one.
				depthbin++;
				goto	bin_full;
				}
			}
		else {
			// exit to text mode
			asm	mov	ax,	0x03
			asm	int	0x10
			// this part should never be reached but just in case I'd like to know.
			printf ( "\a\nfacet out of bin range %i %i", facet, depthbin );
			getch ();
			}
		}
}


void	drawfacets ( void )
{
	screentype	*v1, *v2, *v3;
	int	bin, bintotal, binfacet, facet;
	word far	*bin_ptr;

	// try all the bins from farest to nearest (painter's algorithm)
	// my compiler optimizes this loop well, thank god.
	for ( bin = 79; bin >= 0; bin -- ) {

		// get the number of facets in this bin
		bintotal = bincount[bin];

		for ( binfacet = 0; binfacet < bintotal; binfacet ++ ) {
			// get the offset of the facet to be drawn
			bin_ptr = (word far*)MK_FP ( facetorder, ( bin << 8 ) + ( binfacet << 1 ) );
			facet = *bin_ptr;

			// get the offsets to the vertices of this facet
			v1 = (screentype*)objectfacet[facet].a;
			v2 = (screentype*)objectfacet[facet].b;
			v3 = (screentype*)objectfacet[facet].c;

			// draw the facet
			if ( renderstyle != RS_TEXTURE ) {
				if ( bumpmapping == true )
					envirobumptriangle ( v1, v2, v3, enviromap, bumpmap, vscreen );
				else
					envirotriangle ( v1, v2, v3, enviromap, vscreen );
				}
			else {
				if ( bumpmapping == false )
					envirotexturetriangle ( v1, v2, v3, enviromap, texturemap, vscreen );
				else
					envirobumptexturetriangle ( v1, v2, v3, enviromap, texturemap, bumpmap, vscreen );
				}
			}

		// this bin's facets have now all be drawn
		bincount[bin] = 0;
		}
}

void drawvertices ( void )
{
	int	facet, xnormal;
	screentype	*v1, *v2, *v3;

	// a bin sort is being used because more precision is not needed.
	// my compiler optimizes this loop well, thank god.

	for ( facet = 0; facet < totalfacets; facet++ ) {

		// grab pointers to the facets 3 vertices
		v1 = (screentype*)objectfacet[facet].a;
		v2 = (screentype*)objectfacet[facet].b;
		v3 = (screentype*)objectfacet[facet].c;

		// cross product to determine if the facet is facing the
		// veiwer
		xnormal = (v2->x - v1->x)*(v1->y - v3->y) - (v2->y - v1->y)*(v1->x - v3->x);
		if ( xnormal < 0 )
			continue;	// facet is not facing the viewer

		copypixel ( v1->x, v1->y, v1->u, v1->v, enviromap, vscreen );
		copypixel ( v2->x, v2->y, v2->u, v2->v, enviromap, vscreen );
		copypixel ( v3->x, v3->y, v3->u, v3->v, enviromap, vscreen );
		}
}



/* -----------------------------------------------------------
 *
 * 		initialization functions
 *
 */

void	init ( void )
{
	putch ( '.' );
	if ( allocmem ( 4096, &vscreen ) != -1 )
		fatalerror ( "   *** out of memory #1" );
	if ( allocmem ( 4096, &background ) != -1 )
		fatalerror ( "   *** out of memory #2" );
	if ( allocmem ( 4096, &bumpmap ) != -1 )
		fatalerror ( "   *** out of memory #3" );
	if ( allocmem ( 4096, &enviromap ) != -1 )
		fatalerror ( "   *** out of memory #4" );
	if ( allocmem ( 4096, &facetorder ) != -1 )
		fatalerror ( "   *** out of memory #5" );
	if ( allocmem ( 4096, &texturemap ) != -1 )
		fatalerror ( "   *** out of memory #6" );

	putch ( '.' );
	inittexture ();
	putch ( '.' );
	createbasecolors ();
	putch ( '.' );
	createshadetable ();
	putch ( '.' );
	initlookups ();
	putch ( '.' );
	initbumpmap ();
	putch ( '.' );
	initobject ();
	putch ( '.' );
	initgraphics ();
}

real	phongillumination ( real diffusecolor, real specularcolor,
					real lightcolor,  real ambientcolor, real theta )
{
	real	phongcolor;

	phongcolor =  ( K_DIFFUSE * diffusecolor * cos( theta ));
	if ( theta < M_PI / 4 )
	{
		phongcolor += ( K_SPECULAR * specularcolor * pow( cos( theta * 2 ), K_FALLOFF ));
	}
	phongcolor *= ( K_ATTENUATION * lightcolor );
	phongcolor += ( ambientcolor * K_AMBIENT * diffusecolor );

	// clip phong color to between 0.0 and 1.0
	if ( phongcolor > 1.0 )
		phongcolor = 1.0;
	else if ( phongcolor < 0.0 )
		phongcolor = 0.0;

	return phongcolor;
}

void	createbasecolors ( void )
{
	int	palcolor, shade;
	real	incedent;
	colortype	tclr, temp;

	numbasecolors = 0;

	for ( palcolor = 0; palcolor < 32; palcolor ++ ) {
		tclr = texturepal[palcolor];

		if ( tclr.r > 63 ) tclr.r = 63;
		if ( tclr.g > 63 ) tclr.g = 63;
		if ( tclr.b > 63 ) tclr.b = 63;

		for ( shade = 0; shade < 128; shade ++ ) {

			incedent = ( (real)(128 - ( shade + 1 )) / 128 ) * ( M_PI / 2 );

			temp.r = 63 * phongillumination ( tclr.r/63.0, 1.0, LIGHT_R, AMBIENT_R, incedent );
			temp.g = 63 * phongillumination ( tclr.g/63.0, 1.0, LIGHT_G, AMBIENT_G, incedent );
			temp.b = 63 * phongillumination ( tclr.b/63.0, 1.0, LIGHT_B, AMBIENT_B, incedent );

			basecolor[numbasecolors] = temp;
			numbasecolors++;
			}
		}
}

void	initlookups ( void )
{
	int	loop;

	for ( loop = 0; loop < 80; loop ++ )
		bincount[loop] = 0;

	// create arc cosine correction lookup
	for ( loop = -128; loop < 128; loop ++ )
		asinlookup[loop+128] = 255.0 * asin ( loop / 128.0 ) / M_PI + 127;
}

void	loadbackground ( void )
{
	// open the file with read only access (ax=handle on return)
	asm	push	bp
	asm	mov	ax, 0x3d00
	asm	lea	dx, [backgroundfile]
	asm	int	0x21
	asm	mov	bp, ax
	asm	jnc	file_ok
	asm	pop	bp
	fatalerror ( "   *** can't open 'azure.raw'" );
	// read the raw data
	file_ok:
	asm	mov	ah, 0x3f
	asm	mov   bx, bp
	asm	mov	cx, 64000
	asm	push	ds
	asm	mov	ds, [background]
	asm	xor	dx, dx
	asm	int	0x21
	asm	pop	ds

	// close the file
	asm	mov	bx, bp
	asm	mov	ah, 0x3e
	asm	int	0x21
	asm	pop	bp
}

void	initbumpmap ( void )
{
	// open the file with read only access (ax=handle on return)
	asm	push	bp
	asm	mov	ax, 0x3d00
	asm	lea	dx, [bumpmapfile]
	asm	int	0x21
	asm	mov	bp, ax
	asm	jnc	file_ok
	asm	pop	bp
	fatalerror ( "   *** can't open 'bumpmap.raw'" );
	// read the raw data
	file_ok:
	asm	mov	ah, 0x3f
	asm	mov   bx, bp
	asm	mov	cx, 0xffff
	asm	push	ds
	asm	mov	ds, [bumpmap]
	asm	xor	dx, dx
	asm	int	0x21
	asm	pop	ds

	// close the file
	asm	mov	bx, bp
	asm	mov	ah, 0x3e
	asm	int	0x21
	asm	pop	bp
}

void	loadreflectmap ( void )
{
	// open the file with read only access (ax=handle on return)
	asm	push	bp
	asm	mov	ax, 0x3d00
	asm	lea	dx, [reflectfile]
	asm	int	0x21
	asm	mov	bp, ax
	asm	jnc	file_ok
	asm	pop	bp
	fatalerror ( "   *** can't open 'refmap.raw'" );
	// read the raw data
	file_ok:
	asm	mov	ah, 0x3f
	asm	mov   bx, bp
	asm	mov	cx, 0xffff
	asm	push	ds
	asm	mov	ds, [enviromap]
	asm	xor	dx, dx
	asm	int	0x21
	asm	pop	ds

	// close the file
	asm	mov	bx, bp
	asm	mov	ah, 0x3e
	asm	int	0x21
	asm	pop	bp
}

void	inittexture ( void )
{
	FILE	*f;

	// open the file with read only access (ax=handle on return)
	asm	push	bp
	asm	mov	ax, 0x3d00
	asm	lea	dx, [texturefile]
	asm	int	0x21
	asm	mov	bp, ax
	asm	jnc	file_ok
	asm	pop	bp
	fatalerror ( "   *** can't open 'texture.raw'" );
	// read the raw data
	file_ok:
	asm	mov	ah, 0x3f
	asm	mov   bx, bp
	asm	mov	cx, 0xffff
	asm	push	ds
	asm	mov	ds, [texturemap]
	asm	xor	dx, dx
	asm	int	0x21
	asm	pop	ds

	// close the file
	asm	mov	bx, bp
	asm	mov	ah, 0x3e
	asm	int	0x21
	asm	pop	bp

	// post process texture map
	asm	mov	es, [texturemap]
	asm	mov	di, 0
	asm	mov	cx, 0xffff
	pixel_loop:
		asm	mov	al, es:[di]
		asm	cmp	al, 32
		asm	jl		here
		asm	mov	al, 31
		asm	mov	es:[di], al
		here:
		asm	inc	di
		asm	dec	cx
		asm	jnz	pixel_loop

	// read the palette of the texture
	if ( ( f = fopen ( texturepalfile, "rb" ) ) == NULL )
		fatalerror ( "   *** can't open 'texture.pal'" );
	fread ( texturepal, sizeof ( colortype ), 32, f );
	fclose ( f );
}

void	createphongmap ( void )
{
	int	x, y;
	real	xcomp, ycomp, zcomp, incedent, temp;
	byte	paletteindex;

	// clear the map
	asm	mov	es, [enviromap]
	asm	xor	di, di
	asm	mov	cx, 0x4000
	asm	xor	eax, eax
	asm	rep	stosd

	// calculate the top left quadrant of the phong enviroment map
	for ( x = 0; x < 128; x ++ ) {
		for ( y = 0; y < 128; y ++ ) {

			// determine the angle that will have this pixel mapped to
			// it in radian.
			xcomp = (127.5 - x) / 127.5 * (M_PI / 2);
			ycomp = (127.5 - y) / 127.5 * (M_PI / 2);

			// lets find "1 - (x^2 + y^2)" from above.
			temp = 1.0 - ( pow ( sin ( xcomp ), 2 ) + pow ( sin ( ycomp ), 2 ) );

			// we can only that the sqrt if temp is positive.  Also if temp is negative
			//	the pixel in question will never be mapped onto the object so we don't
			// care about it.
			if ( temp >= 0.0 )
			{
				// we now get the z component of the normal vector
				zcomp = sqrt ( temp );

				// find the angle
				incedent = asin ( zcomp );

				// now that we have the angle lets scale down to 0 to 1 since it is
				// an angle between 0 and 90 except it's in radians.
				// Then scale it up the be a palette index
				paletteindex = (byte) ( incedent / ( M_PI / 2 ) * PAL_SIZE + PAL_OFFSET );

				// put the index into the phongmap
				asm	mov	bh, byte ptr [x]
				asm	mov	bl, byte ptr [y]
				asm	mov	es, word ptr [enviromap]
				asm	mov	di, bx
				asm	mov	al, byte ptr [paletteindex]
				asm	mov	es:[di], al
			}
		}
	}

	// copy the quadrant we just calculated to the other three (we flip)
	for ( x = 0; x < 128; x ++ )
		for ( y = 0; y < 128; y ++ ) {
			asm	mov	es, word ptr [enviromap]
			asm	mov	ch, byte ptr [x]
			asm	mov	cl, byte ptr [y]
			asm	mov	dx, cx
			asm	mov	si, cx
			asm	mov	bx, 0xffff
			asm	sub	bx, cx
			asm	mov   al, es:[si]
			asm	mov	di, bx
			asm	mov	es:[di], al
			asm	mov	cl, bl
			asm	mov	di, cx
			asm	mov	es:[di], al
			asm	mov	dh, bh
			asm	mov	di, dx
			asm	mov	es:[di], al
			}
}

void	createminiphongmap ( void )
{
	int	x, y;
	real	xcomp, ycomp, zcomp, incedent, temp;
	byte	paletteindex;

	// clear the map
	asm	mov	es, [enviromap]
	asm	xor	di, di
	asm	mov	cx, 0x4000
	asm	xor	eax, eax
	asm	rep	stosd

	// calculate the top left quadrant of the phong enviroment map
	for ( x = 0; x < 128; x ++ ) {
		for ( y = 0; y < 128; y ++ ) {

			// determine the angle that will have this pixel mapped to
			// it in radian.
			xcomp = (127.5 - x) / 127.5 * (M_PI / 2);
			ycomp = (127.5 - y) / 127.5 * (M_PI / 2);

			// lets find "1 - (x^2 + y^2)" from above.
			temp = 1.0 - ( pow ( sin ( xcomp ), 2 ) + pow ( sin ( ycomp ), 2 ) );

			// we can only that the sqrt if temp is positive.  Also if temp is negative
			//	the pixel in question will never be mapped onto the object so we don't
			// care about it.
			if ( temp >= 0.0 )
			{
				// we now get the z component of the normal vector
				zcomp = sqrt ( temp );

				// find the angle
				incedent = asin ( zcomp );

				// now that we have the angle lets scale down to 0 to 1 since it is
				// an angle between 0 and 90 except it's in radians.
				// Then scale it up the be a palette index
				paletteindex = (byte) ( incedent / ( M_PI / 2 ) * 128 );

				// put the index into the phongmap
				asm	mov	bh, byte ptr [x]
				asm	mov	bl, byte ptr [y]
				asm	mov	es, word ptr [enviromap]
				asm	mov	di, bx
				asm	mov	al, byte ptr [paletteindex]
				asm	mov	es:[di], al
			}
		}
	}

	// copy the quadrant we just calculated to the other three (we flip)
	for ( x = 0; x < 128; x ++ )
		for ( y = 0; y < 128; y ++ ) {
			asm	mov	es, word ptr [enviromap]
			asm	mov	ch, byte ptr [x]
			asm	mov	cl, byte ptr [y]
			asm	mov	dx, cx
			asm	mov	si, cx
			asm	mov	bx, 0xffff
			asm	sub	bx, cx
			asm	mov   al, es:[si]
			asm	mov	di, bx
			asm	mov	es:[di], al
			asm	mov	cl, bl
			asm	mov	di, cx
			asm	mov	es:[di], al
			asm	mov	dh, bh
			asm	mov	di, dx
			asm	mov	es:[di], al
			}
}

void	initobject ( void )
{
	FILE	*f;
	word	vertices, normals, texturecoords, facet, vertex;

	// load vertex file
	if ( ( f = fopen ( vertexfile, "rb" ) ) == NULL )
		fatalerror ( "   *** can't open 'mask.vtx'" );
	fread ( &vertices, 2, 1, f );
	if ( vertices >= MAX_VERTEX )
		fatalerror ( "   *** internal error #2" );
	fread ( objectvertex, sizeof(vectortype)*3, vertices, f );
	fclose ( f );

	totalvertices	= vertices;

	// load texture coord file
	if ( ( f = fopen ( txtrcoordfile, "rb" ) ) == NULL )
		fatalerror ( "   *** can't open 'mask.txr'" );
	fread ( &texturecoords, 2, 1, f );
	if ( texturecoords >= MAX_VERTEX )
		fatalerror ( "   *** internal error #6" );
	fread ( objecttexture, sizeof(texturetype)*3, texturecoords, f );
	fclose ( f );

	// set unchanging texture coords
	for ( vertex = 0; vertex < totalvertices; vertex ++ ) {
		screenvertex[vertex].a = objecttexture[vertex].a;
		screenvertex[vertex].b = objecttexture[vertex].b;
		}

	// load normal file
	if ( ( f = fopen ( normalfile, "rb" ) ) == NULL )
		fatalerror ( "   *** can't open 'mask.nrm'" );
	fread ( &normals, 2, 1, f );
	if ( normals >= MAX_VERTEX )
		fatalerror ( "   *** internal error #3" );
	fread ( objectnormal, sizeof(vectortype)*3, normals, f );
	fclose ( f );

	// check to be sure data if valid
	if ( vertices != normals )
		fatalerror ( "   *** internal error #4" );

	// load face file
	if ( ( f = fopen ( facetfile, "rb" ) ) == NULL )
		fatalerror ( "   *** can't open 'mask.fct'" );
	fread ( &totalfacets, 2, 1, f );
	if ( totalfacets >= MAX_FACET )
		fatalerror ( "   *** internal error #5" );
	fread ( objectfacet, sizeof(facettype), totalfacets, f );
	fclose ( f );

	// change facets from array indices to offset from ds segment
	for ( facet = 0; facet < totalfacets; facet ++ ) {
		objectfacet[facet].a = (word)&screenvertex[objectfacet[facet].a];
		objectfacet[facet].b = (word)&screenvertex[objectfacet[facet].b];
		objectfacet[facet].c = (word)&screenvertex[objectfacet[facet].c];
		}
}

void	initgraphics ( void )
{
	// clear virtual screen
	asm	mov	es, [vscreen]
	asm	xor	di, di
	asm	mov	cx, 16000
	asm	xor	eax, eax
	asm	rep	stosd

	fadepalette ( currentpal, 0 );
	delay ( 300 );

	// set mcga video mode
	asm	mov	ax, 0x13
	asm	int	0x10
}

void	fatalerror ( char *msg )
{
	printf ( "\n%s", msg );
	printf ( "\ninitialization failed. press a key to quit..." );
	getch ();
	exit ( 0 );
}


/* -----------------------------------------------------------
 *
 * 		shutdown functions
 *
 */

void	close ( void )
{
	// set palette to black for a clear mode switch
	fadepalette ( currentpal, 0 );
	delay ( 300 );

	asm	mov	ax, 0x03	// text mode switch
	asm	int	0x10

	delay ( 300 );	// let the display settle down after mode switch
}


