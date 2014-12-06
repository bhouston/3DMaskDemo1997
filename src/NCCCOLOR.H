/*
 *		NCCColor.H
 *			- An Quantization Routine Header
 *
 *		Coded by Ben Houston [plash | AZuRe]
 *
 *		Started :  Wednesday, March 5, 1997
 *		Updated :  Thursday, March 6, 1997
 */


#ifndef	__NCC_COLOR_H__
#define	__NCC_COLOR_H__


#include "ncctypes.h"


#define	NUM_BASECOLORS	4096


extern	byte			shadetable[NUM_BASECOLORS];
extern	colortype	basecolor[NUM_BASECOLORS];
extern colortype	optimalpal[256];

void	createshadetable ( void );


#endif	// __NCC_COLOR_H__
