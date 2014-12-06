/*
 *		ncctri.h	- NCC Triangle Fillers Header
 *
 * 	Nothing Comes Close Library
 * 	Written By Ben Houston [plash / AZuRe]
 *
 *		Started: Friday, February 28, 1997
 *		Updated: n/a
 */

#ifndef	__NCC_TRI_H__
#define	__NCC_TRI_H__


#include "ncctypes.h"


void	envirotexturetriangle ( screentype *v1, screentype *v2, screentype *v3, word enviromap, word texturemap, word destscreen );
void	envirobumptexturetriangle ( screentype *v1, screentype *v2, screentype *v3,
					 word enviromap, word texturemap, word bumpmap, word destscreen );
void	texturetriangle ( screentype *v1, screentype *v2, screentype *v3, word texturemap, word destscreen );
void	envirotriangle ( screentype *v1, screentype *v2, screentype *v3, word enviromap, word destscreen );
void	envirobumptriangle ( screentype *v1, screentype *v2, screentype *v3, word enviromap, word bumpmap, word destscreen );


#endif	// __NCC_TRI_H__

