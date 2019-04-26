/*
    This file is part of SUPPL - the supplemental library for DOS
    Copyright (C) 1996-2000 Steffen Kaiser

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* $Id: fmemory.h 53 2000-07-24 22:38:11Z skaus $
   $Locker$	$Name$	$State$

	Far memory handling function.

*/

#ifndef __FMEMORY_H
#define __FMEMORY_H

#include <SUPL_DEF.H>

#ifdef _MICROC_				/* for use for Micro-C only */

#define MK_FP(seg,ofs) (seg), (ofs)
#define TO_FP(var) get_ds(), (var)
#define _fnormalize(seg,ofs) _fnormalize_(&(seg), &(ofs))

#else	/* !_MICROC_ */

#ifdef _TC_LATER_
#include <string.h>

#else
#if defined(_PAC_NOCLIB_) || defined(_TC_EARLY_)

#ifdef _PAC_NOCLIB_
#include <stdlib.h>
#define _fmemcpy farmemcpy
#define _fmemset farmemset

#else
void _fmemcpy(void far * const dst, const void far * const src, unsigned length);
void _fmemset(void far * const dst, int ch, unsigned length);
#endif

unsigned _fstrlen(const char far * const s);
char far *_fstrchr(const char far * const s, int ch);
void _fmemmove(void far * const dst, const void far * const src, unsigned length);
int _fmemcmp(const void far * const dst, const void far * const src, unsigned length);
int _fmemicmp(const void far * const dst, const void far * const src, unsigned length);
int _fstrcmp(const char far * const dst, const char far * const src);
int _fstricmp(const char far * const dst, const char far * const src);
void _fstrcpy(char far * const dst, const char far * const src);

#endif
#endif		/* _TC_LATER_ */

#if defined(HI_TECH_C) || defined(_TC_EARLY_)
	/* HiTech C has no huge pointers
	 & early Turbo C does ignore the cast */
void far *_fnormalize(void far *poi);
#else
#define _fnormalize(poi) ((void huge *)(poi))
#endif

char *_fdupstr(const char far * const s);
#endif

#endif
