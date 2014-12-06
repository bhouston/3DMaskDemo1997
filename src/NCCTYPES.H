/*
 *		ncctypes.h	- NCC Type Definitions Header
 *
 * 	Nothing Comes Close Library
 * 	Written By Ben Houston [plash / AZuRe]
 *
 *		Started: Friday, February 28, 1997
 *		Updated: n/a
 */

#ifndef	__NCC_TYPES_H__
#define	__NCC_TYPES_H__

#pragma	inline	// get TASM to compile the 32bit asm
asm	.386p

// headers that are always used
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

// my own types, I use them just to be annoying =)
typedef	unsigned int	boolean;
typedef	unsigned char	byte;
typedef	unsigned int	word;
typedef	unsigned long	dword;
typedef	float				real;
enum		boolean_values { false = 0, true = 1 };

typedef struct { int a, b; } texturetype;
typedef struct { int x, y, z; } vectortype;
typedef struct { word a, b, c; } facettype;
typedef struct { int x, y, z, a, b, u, v; } screentype;
typedef struct { int l, t, b, r; } recttype;
typedef struct { byte r, g, b; }	colortype;

// user defined error handler
extern	void	fatalerror ( char *errorstring );

#endif	//	__NCC_TYPES_H__

