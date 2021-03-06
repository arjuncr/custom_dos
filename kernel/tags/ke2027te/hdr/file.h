/****************************************************************/
/*                                                              */
/*                            file.h                            */
/*                                                              */
/*                      DOS File mode flags                     */
/*                                                              */
/*                       December 1, 1991                       */
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
/****************************************************************/

#ifdef MAIN
#ifdef VERSION_STRINGS
static BYTE *file_hRcsId =
    "$Id: file.h 412 2002-08-03 04:08:58Z bartoldeman $";
#endif
#endif

/* 0 = CON, standard input, can be redirected                           */
/* 1 = CON, standard output, can be redirected                          */
/* 2 = CON, standard error                                              */
/* 3 = AUX, auxiliary                                                   */
/* 4 = PRN, list device                                                 */
/* 5 = 1st user file ...                                                */
#define STDIN           0
#define STDOUT          1
#define STDERR          2
#define STDAUX          3
#define STDPRN          4

#define O_RDONLY        SFT_MREAD
#define O_WRONLY        SFT_MWRITE
#define O_RDWR          SFT_MRDWR

/* bits 2, 3 reserved */
/* bits 4, 5, 6 sharing modes */
#define O_NOINHERIT     0x0080
#define O_OPEN          0x0100 /* not     */
#define O_TRUNC         0x0200 /*    both */
#define O_CREAT         0x0400
#define O_LEGACY        0x0800
#define O_LARGEFILE     0x1000
#define O_NOCRIT        0x2000
#define O_SYNC          0x4000
#define O_FCB           0x8000

/* status for extended open */
enum {S_OPENED = 1, S_CREATED = 2, S_REPLACED = 3};

/*
 * Log: file.h,v 
 *
 * Revision 1.1.1.1  1999/03/29 15:39:30  jprice
 * New version without IPL.SYS
 *
 * Revision 1.3  1999/02/01 01:40:06  jprice
 * Clean up
 *
 * Revision 1.2  1999/01/22 04:17:40  jprice
 * Formating
 *
 * Revision 1.1.1.1  1999/01/20 05:51:01  jprice
 * Imported sources
 *
 *
 *         Rev 1.5   04 Jan 1998 23:14:18   patv
 *      Changed Log for strip utility
 *
 *         Rev 1.4   29 May 1996 21:25:14   patv
 *      bug fixes for v0.91a
 *
 *         Rev 1.3   19 Feb 1996  3:15:32   patv
 *      Added NLS, int2f and config.sys processing
 *
 *         Rev 1.2   01 Sep 1995 17:35:42   patv
 *      First GPL release.
 *
 *         Rev 1.1   30 Jul 1995 20:43:48   patv
 *      Eliminated version strings in ipl
 *
 *         Rev 1.0   02 Jul 1995 10:39:42   patv
 *      Initial revision.
 */
