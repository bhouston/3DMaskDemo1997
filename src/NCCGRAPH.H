/*
 *		nccgraph.h	- NCC Simple Graphics Functions
 *
 * 	Nothing Comes Close Library
 * 	Written By Ben Houston [plash / AZuRe]
 *
 *		Started: Friday, February 28, 1997
 *		Updated: n/a
 */

#ifndef	__NCC_GRAPH_H__
#define	__NCC_GRAPH_H__


#include "ncctypes.h"


// simple graphics functions
void	fadepalette ( colortype *pal, byte fade );
void	setpalette ( colortype *pal );
void	vretrace ( void );
void	copyrect ( word source, word dest, recttype rect );
void	copyscreen ( word source, word dest );
void	fillscreen ( word dest, byte color );
void	copypixel ( word x, word y, word u, word v, word source, word dest );
void	putpixel ( word x, word y, byte color, word destscreen );

#endif	// __NCC_GRAPH_H__
