/****************************************************************/
/*                                                              */
/*                          syspack.c                           */
/*                                                              */
/*            System Disk Byte Order Packing Functions          */
/*                                                              */
/*                      Copyright (c) 1995                      */
/*                      Pasquale J. Villani                     */
/*                      All Rights Reserved                     */
/*                                                              */
/* This file is part of DOS-C.                                  */
/*                                                              */
/* DOS-C is free software; you can redistribute it and/or       */
/* modify it under the terms of the GNU General Public License  */
/* as published by the Free Software Foundation; either version */
/* 2, or (at your option) any later version.                    */
/*                                                              */
/* DOS-C is distributed in the hope that it will be useful, but */
/* WITHOUT ANY WARRANTY; without even the implied warranty of   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See    */
/* the GNU General Public License for more details.             */
/*                                                              */
/* You should have received a copy of the GNU General Public    */
/* License along with DOS-C; see the file COPYING.  If not,     */
/* write to the Free Software Foundation, 675 Mass Ave,         */
/* Cambridge, MA 02139, USA.                                    */
/*                                                              */
/****************************************************************/

#include "portab.h"
#include "globals.h"

#ifdef VERSION_STRINGS
static BYTE *syspackRcsId =
    "$Id: syspack.c 382 2002-05-08 22:49:35Z bartoldeman $";
#endif

#ifdef NONNATIVE
UDWORD getlong(REG VOID * vp)
{
  return (((UBYTE *) vp)[0] & 0xff) +
      ((((UBYTE *) vp)[1] & 0xff) << 8) +
      ((((UBYTE *) vp)[2] & 0xff) << 16) +
      ((((UBYTE *) vp)[3] & 0xff) << 24);
}

UWORD getword(REG VOID * vp)
{
  return (((UBYTE *) vp)[0] & 0xff) + ((((UBYTE *) vp)[1] & 0xff) << 8);
}

UBYTE getbyte(VOID * vp)
{
  return *((BYTE *) vp);
}

UWORD fgetword(REG VOID FAR * vp)
{
  return (((UBYTE FAR *) vp)[0] & 0xff) + ((((UBYTE FAR *) vp)[1] & 0xff) << 8);
}

UDWORD fgetlong(REG VOID FAR * vp)
{
  return (((UBYTE *) vp)[0] & 0xff) +
      ((((UBYTE *) vp)[1] & 0xff) << 8) +
      ((((UBYTE *) vp)[2] & 0xff) << 16) +
      ((((UBYTE *) vp)[3] & 0xff) << 24);
}

UBYTE fgetbyte(VOID FAR * vp)
{
  return *((UBYTE FAR *) vp);
}

VOID fputlong(VOID FAR * vp, UDWORD l)
{
  REG UBYTE FAR *bp = (UBYTE FAR *) vp;

  bp[0] = l & 0xff;
  bp[1] = (l >> 8) & 0xff;
  bp[2] = (l >> 16) & 0xff;
  bp[3] = (l >> 24) & 0xff;
}

VOID fputword(VOID FAR * vp, UWORD w)
{
  REG UBYTE FAR *bp = (UBYTE FAR *) vp;

  bp[0] = w & 0xff;
  bp[1] = (w >> 8) & 0xff;
}

VOID fputbyte(VOID FAR * vp, UBYTE b)
{
  *(UBYTE FAR *) vp = b;
}

VOID getdirent(UBYTE FAR * vp, struct dirent FAR * dp)
{
  fmemcpy(dp->dir_name, &vp[DIR_NAME], FNAME_SIZE);
  fmemcpy(dp->dir_ext, &vp[DIR_EXT], FEXT_SIZE);
  dp->dir_attrib = fgetbyte(&vp[DIR_ATTRIB]);
  dp->dir_time = fgetword(&vp[DIR_TIME]);
  dp->dir_date = fgetword(&vp[DIR_DATE]);
  dp->dir_start = fgetword(&vp[DIR_START]);
  dp->dir_size = fgetlong(&vp[DIR_SIZE]);
}

VOID putdirent(struct dirent FAR * dp, UBYTE FAR * vp)
{
  REG COUNT i;
  REG BYTE FAR *p;

  fmemcpy(&vp[DIR_NAME], dp->dir_name, FNAME_SIZE);
  fmemcpy(&vp[DIR_EXT], dp->dir_ext, FEXT_SIZE);
  fputbyte(&vp[DIR_ATTRIB], dp->dir_attrib);
  fputword(&vp[DIR_TIME], dp->dir_time);
  fputword(&vp[DIR_DATE], dp->dir_date);
  fputword(&vp[DIR_START], dp->dir_start);
  fputlong(&vp[DIR_SIZE], dp->dir_size);
  for (i = 0, p = (UBYTE FAR *) & vp[DIR_RESERVED]; i < 10; i++)
    *p++ = NULL;
}
#endif

/*
 * Log: syspack.c,v - for newer entries see "cvs log syspack.c"
 *
 * Revision 1.1.1.1  1999/03/29 15:42:21  jprice
 * New version without IPL.SYS
 *
 * Revision 1.3  1999/02/01 01:43:28  jprice
 * Fixed findfirst function to find volume label with Windows long filenames
 *
 * Revision 1.2  1999/01/22 04:15:28  jprice
 * Formating
 *
 * Revision 1.1.1.1  1999/01/20 05:51:00  jprice
 * Imported sources
 *
 *
 *    Rev 1.4   04 Jan 1998 23:14:38   patv
 * Changed Log for strip utility
 *
 *    Rev 1.3   29 May 1996 21:15:12   patv
 * bug fixes for v0.91a
 *
 *    Rev 1.2   01 Sep 1995 17:48:42   patv
 * First GPL release.
 *
 *    Rev 1.1   30 Jul 1995 20:50:26   patv
 * Eliminated version strings in ipl
 *
 *    Rev 1.0   02 Jul 1995  8:05:34   patv
 * Initial revision.
 */
