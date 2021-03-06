/****************************************************************/
/*                                                              */
/*                          config.c                            */
/*                            DOS-C                             */
/*                                                              */
/*                config.sys Processing Functions               */
/*                                                              */
/*                      Copyright (c) 1996                      */
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

#include "init-mod.h"

#include "portab.h"
#include "globals.h"

#ifdef VERSION_STRINGS
static BYTE *RcsId = "$Id: config.c 2 2000-05-06 19:34:20Z jhall1 $";
#endif

/*
 * $Log$
 * Revision 1.1  2000/05/06 19:34:56  jhall1
 * Initial revision
 *
 * Revision 1.13  2000/03/09 06:07:10  kernel
 * 2017f updates by James Tabor
 *
 * Revision 1.12  1999/09/23 04:40:46  jprice
 * *** empty log message ***
 *
 * Revision 1.10  1999/08/25 03:18:07  jprice
 * ror4 patches to allow TC 2.01 compile.
 *
 * Revision 1.9  1999/05/03 06:25:45  jprice
 * Patches from ror4 and many changed of signed to unsigned variables.
 *
 * Revision 1.8  1999/04/16 21:43:40  jprice
 * ror4 multi-sector IO
 *
 * Revision 1.7  1999/04/16 12:21:21  jprice
 * Steffen c-break handler changes
 *
 * Revision 1.6  1999/04/16 00:53:32  jprice
 * Optimized FAT handling
 *
 * Revision 1.5  1999/04/12 03:21:17  jprice
 * more ror4 patches.  Changes for multi-block IO
 *
 * Revision 1.4  1999/04/11 04:33:38  jprice
 * ror4 patches
 *
 * Revision 1.2  1999/04/04 22:57:47  jprice
 * no message
 *
 * Revision 1.1.1.1  1999/03/29 15:40:46  jprice
 * New version without IPL.SYS
 *
 * Revision 1.6  1999/03/23 23:38:15  jprice
 * Now checks for a reads fdconfig.sys file, if exists
 *
 * Revision 1.5  1999/02/08 05:55:57  jprice
 * Added Pat's 1937 kernel patches
 *
 * Revision 1.4  1999/02/01 01:48:41  jprice
 * Clean up; Now you can use hex numbers in config.sys. added config.sys screen function to change screen mode (28 or 43/50 lines)
 *
 * Revision 1.3  1999/01/30 08:28:11  jprice
 * Clean up; Fixed bug with set attribute function.
 *
 * Revision 1.2  1999/01/22 04:13:25  jprice
 * Formating
 *
 * Revision 1.1.1.1  1999/01/20 05:51:01  jprice
 * Imported sources
 *
 *
 *    Rev 1.6   22 Jan 1998  4:09:24   patv
 * Fixed pointer problems affecting SDA
 *
 *    Rev 1.5   04 Jan 1998 23:15:18   patv
 * Changed Log for strip utility
 *
 *    Rev 1.4   04 Jan 1998 17:26:14   patv
 * Corrected subdirectory bug
 *
 *    Rev 1.3   16 Jan 1997 12:46:50   patv
 * pre-Release 0.92 feature additions
 *
 *    Rev 1.1   29 May 1996 21:03:44   patv
 * bug fixes for v0.91a
 *
 *    Rev 1.0   19 Feb 1996  3:22:16   patv
 * Added NLS, int2f and config.sys processing
 */

#ifdef __TURBOC__
void __int__(int);              /* TC 2.01 requires this. :( -- ror4 */
#endif

#ifdef KDB
#include <alloc.h>

#define KernelAlloc(x) adjust_far((void far *)malloc((unsigned long)(x)))
#endif

BYTE FAR *lpBase;
static BYTE FAR *lpOldLast;
static COUNT nCfgLine;
static COUNT nPass;
static BYTE szLine[256];
static BYTE szBuf[256];

int singleStep = 0;

INIT VOID Buffers(BYTE * pLine);
INIT VOID sysScreenMode(BYTE * pLine);
INIT VOID sysVersion(BYTE * pLine);
INIT VOID Break(BYTE * pLine);
INIT VOID Device(BYTE * pLine);
INIT VOID Files(BYTE * pLine);
INIT VOID Fcbs(BYTE * pLine);
INIT VOID Lastdrive(BYTE * pLine);
INIT VOID Country(BYTE * pLine);
INIT VOID InitPgm(BYTE * pLine);
INIT VOID Switchar(BYTE * pLine);
INIT VOID CfgFailure(BYTE * pLine);
INIT VOID Stacks(BYTE * pLine);
INIT BYTE *GetNumArg(BYTE * pLine, COUNT * pnArg);
INIT BYTE *GetStringArg(BYTE * pLine, BYTE * pszString);
INIT struct dhdr FAR *linkdev(struct dhdr FAR * dhp);
INIT UWORD initdev(struct dhdr FAR * dhp, BYTE FAR * cmdTail);
INIT int SkipLine(char *pLine);

INIT static VOID FAR *AlignParagraph(VOID FAR * lpPtr);
#ifndef I86
#define AlignParagraph(x) (x)
#endif

#define EOF 0x1a

INIT struct table *LookUp(struct table *p, BYTE * token);

struct table
{
  BYTE *entry;
  BYTE pass;
    VOID(*func) (BYTE * pLine);
};

static struct table commands[] =
{
  {"break", 1, Break},
  {"buffers", 1, Buffers},
  {"command", 1, InitPgm},
  {"country", 1, Country},
  {"device", 2, Device},
  {"fcbs", 1, Fcbs},
  {"files", 1, Files},
  {"lastdrive", 1, Lastdrive},
        /* rem is never executed by locking out pass                    */
  {"rem", 0, CfgFailure},
  {"shell", 1, InitPgm},
  {"stacks", 1, Stacks},
  {"switchar", 1, Switchar},
  {"screen", 1, sysScreenMode}, /* JPP */
  {"version", 1, sysVersion},   /* JPP */
        /* default action                                               */
  {"", -1, CfgFailure}
};

#ifndef KDB
INIT BYTE FAR *KernelAlloc(WORD nBytes);
INIT BYTE FAR *KernelAllocDma(WORD);
#endif

BYTE *pLineStart;

/* Do first time initialization.  Store last so that we can reset it    */
/* later.                                                               */
INIT void PreConfig(void)
{
  /* Set pass number                                              */
  nPass = 0;

  /* Initialize the base memory pointers                          */
  lpOldLast = lpBase = AlignParagraph((BYTE FAR *) & last);

  /* Begin by initializing our system buffers                     */
  dma_scratch = (BYTE FAR *) KernelAllocDma(BUFFERSIZE);
#ifdef DEBUG
  printf("Preliminary DMA scratchpad allocated at 0x%04x:0x%04x\n",
         FP_SEG(dma_scratch), FP_OFF(dma_scratch));
#endif
  buffers = (struct buffer FAR *)
      KernelAlloc(Config.cfgBuffers * sizeof(struct buffer));
#ifdef DEBUG
  printf("Preliminary %d buffers allocated at 0x%04x:0x%04x\n", Config.cfgBuffers,
         FP_SEG(buffers), FP_OFF(buffers));
#endif

  /* Initialize the file table                                    */
  f_nodes = (struct f_node FAR *)
      KernelAlloc(Config.cfgFiles * sizeof(struct f_node));
  /* sfthead = (sfttbl FAR *)&basesft; */
  /* FCBp = (sfttbl FAR *)&FcbSft; */
  FCBp = (sfttbl FAR *)
      KernelAlloc(sizeof(sftheader)
                  + Config.cfgFiles * sizeof(sft));
  sfthead = (sfttbl FAR *)
      KernelAlloc(sizeof(sftheader)
                  + Config.cfgFiles * sizeof(sft));

  CDSp = (cdstbl FAR *)
      KernelAlloc(0x58 * lastdrive + 1);

#ifdef DEBUG

  printf("Preliminary f_node allocated at 0x%04x:0x%04x\n",
         FP_SEG(f_nodes), FP_OFF(f_nodes));
  printf("Preliminary FCB table allocated at 0x%04x:0x%04x\n",
         FP_SEG(FCBp), FP_OFF(FCBp));
  printf("Preliminary sft table allocated at 0x%04x:0x%04x\n",
         FP_SEG(sfthead), FP_OFF(sfthead));
  printf("Preliminary CDS table allocated at 0x%04x:0x%04x\n",
         FP_SEG(CDSp), FP_OFF(CDSp));
#endif

  /* Done.  Now initialize the MCB structure                      */
  /* This next line is 8086 and 80x86 real mode specific          */
#ifdef DEBUG
  printf("Preliminary  allocation completed: top at 0x%04x:0x%04x\n",
         FP_SEG(lpBase), FP_OFF(lpBase));
#endif

#ifdef KDB
  lpBase = malloc(4096);
  first_mcb = FP_SEG(lpBase) + ((FP_OFF(lpBase) + 0x0f) >> 4);
#else
  first_mcb = FP_SEG(lpBase) + ((FP_OFF(lpBase) + 0x0f) >> 4);
#endif

  /* We expect ram_top as Kbytes, so convert to paragraphs */
  mcb_init((mcb FAR *) (MK_FP(first_mcb, 0)),
           (ram_top << 6) - first_mcb - 1);
  nPass = 1;
}

/* Do second pass initialization.                                       */
/* Also, run config.sys to load drivers.                                */
INIT void PostConfig(void)
{
  /* Set pass number                                              */
  nPass = 2;

  /* Initialize the base memory pointers from last time.          */
  lpBase = AlignParagraph(lpOldLast);

  /* Begin by initializing our system buffers                     */
  dma_scratch = (BYTE FAR *) KernelAllocDma(BUFFERSIZE);
#ifdef DEBUG
  printf("DMA scratchpad allocated at 0x%04x:0x%04x\n", FP_SEG(dma_scratch),
         FP_OFF(dma_scratch));
#endif
  buffers = (struct buffer FAR *)
      KernelAlloc(Config.cfgBuffers * sizeof(struct buffer));
#ifdef DEBUG
  printf("%d buffers allocated at 0x%04x:0x%04x\n", Config.cfgBuffers,
         FP_SEG(buffers), FP_OFF(buffers));
#endif

  /* Initialize the file table                                    */
  f_nodes = (struct f_node FAR *)
      KernelAlloc(Config.cfgFiles * sizeof(struct f_node));
  /* sfthead = (sfttbl FAR *)&basesft; */
  /* FCBp = (sfttbl FAR *)&FcbSft; */
  FCBp = (sfttbl FAR *)
      KernelAlloc(sizeof(sftheader)
                  + Config.cfgFiles * sizeof(sft));
  sfthead = (sfttbl FAR *)
      KernelAlloc(sizeof(sftheader)
                  + Config.cfgFiles * sizeof(sft));

  CDSp = (cdstbl FAR *)
      KernelAlloc(0x58 * lastdrive + 1);

#ifdef DEBUG

  printf("f_node allocated at 0x%04x:0x%04x\n",
         FP_SEG(f_nodes), FP_OFF(f_nodes));
  printf("FCB table allocated at 0x%04x:0x%04x\n",
         FP_SEG(FCBp), FP_OFF(FCBp));
  printf("sft table allocated at 0x%04x:0x%04x\n",
         FP_SEG(sfthead), FP_OFF(sfthead));
  printf("CDS table allocated at 0x%04x:0x%04x\n",
         FP_SEG(CDSp), FP_OFF(CDSp));
#endif
  if (Config.cfgStacks)
  {
    VOID FAR *stackBase = KernelAlloc(Config.cfgStacks * Config.cfgStackSize);
    init_stacks(stackBase, Config.cfgStacks, Config.cfgStackSize);

#ifdef  DEBUG
    printf("Stacks allocated at %04x:%04x\n",
           FP_SEG(stackBase), FP_OFF(stackBase));
#endif
  }
#ifdef DEBUG
  printf("Allocation completed: top at 0x%04x:0x%04x\n",
         FP_SEG(lpBase), FP_OFF(lpBase));
#endif
}

/* This code must be executed after device drivers has been loaded */
INIT VOID configDone(VOID)
{
  COUNT i;

  first_mcb = FP_SEG(lpBase) + ((FP_OFF(lpBase) + 0x0f) >> 4);

  /* We expect ram_top as Kbytes, so convert to paragraphs */
  mcb_init((mcb FAR *) (MK_FP(first_mcb, 0)),
           (ram_top << 6) - first_mcb - 1);

  /* The standard handles should be reopened here, because
     we may have loaded new console or printer drivers in CONFIG.SYS */
}

INIT VOID DoConfig(VOID)
{
  COUNT nFileDesc;
  COUNT nRetCode;
  BYTE *pLine,
   *pTmp;
  BOOL bEof;

  /* Check to see if we have a config.sys file.  If not, just     */
  /* exit since we don't force the user to have one.              */
  if ((nFileDesc = dos_open((BYTE FAR *) "fdconfig.sys", 0)) < 0)
  {
#ifdef DEBUG
    printf("FDCONFIG.SYS not found\n");
#endif
    if ((nFileDesc = dos_open((BYTE FAR *) "config.sys", 0)) < 0)
    {
#ifdef DEBUG
      printf("CONFIG.SYS not found\n");
#endif
      return;
    }
#ifdef DEBUG
 		else
			printf("Reading CONFIG.SYS...\n");
#endif
  }
#ifdef DEBUG
	else
		printf("Reading FDCONFIG.SYS...\n");
#endif

  /* Have one -- initialize.                                      */
  nCfgLine = 0;
  bEof = 0;
  pLine = szLine;

  /* Read each line into the buffer and then parse the line,      */
  /* do the table lookup and execute the handler for that         */
  /* function.                                                    */
  while (!bEof)
  {
    struct table *pEntry;
    UWORD bytesLeft = 0;

    if (pLine > szLine)
      bytesLeft = LINESIZE - (pLine - szLine);

    if (bytesLeft)
    {
      fbcopy(pLine, szLine, LINESIZE - bytesLeft);
      pLine = szLine + bytesLeft;
    }

    /* Read a line from config                              */
    /* Interrupt processing if read error or no bytes read  */
    if ((nRetCode = dos_read(nFileDesc, pLine, LINESIZE - bytesLeft)) <= 0)
      break;

    /* If the buffer was not filled completely, append a
       CTRL-Z character to mark where the file ends */

    if (nRetCode + bytesLeft < LINESIZE)
      szLine[nRetCode + bytesLeft] = EOF;

    /* Process the buffer, line by line */
    pLine = szLine;

    while (!bEof && *pLine != EOF)
    {
      for (pTmp = pLine; pTmp - szLine < LINESIZE; pTmp++)
      {
        if (*pTmp == '\r' || *pTmp == EOF)
          break;
      }

      if (pTmp - szLine >= LINESIZE)
        break;

      if (*pTmp == EOF)
        bEof = TRUE;

      *pTmp = '\0';
      pLineStart = pLine;

      /* Skip leading white space and get verb.               */
      pLine = scan(pLine, szBuf);

      /* Translate the verb to lower case ...                 */
      for (pTmp = szBuf; *pTmp != '\0'; pTmp++)
        *pTmp = tolower(*pTmp);

      /* If the line was blank, skip it.  Otherwise, look up  */
      /* the verb and execute the appropriate function.       */
      if (*szBuf != '\0')
      {
        pEntry = LookUp(commands, szBuf);

        if (pEntry->pass < 0 || pEntry->pass == nPass)
        {
          if (!singleStep || !SkipLine(pLineStart))
          {
            skipwh(pLine);

            if ('=' != *pLine)
              CfgFailure(pLine);
            else
              (*(pEntry->func)) (++pLine);
          }
        }
      }
    skipLine:nCfgLine++;
      pLine += strlen(pLine) + 1;
    }
  }
  dos_close(nFileDesc);
}

INIT struct table *LookUp(struct table *p, BYTE * token)
{
  while (*(p->entry) != '\0')
  {
    if (strcmp(p->entry, token) == 0)
      break;
    else
      ++p;
  }
  return p;
}

INIT BOOL SkipLine(char *pLine)
{
  char kbdbuf[16];
  keyboard *kp = (keyboard *) kbdbuf;
  char *pKbd = &kp->kb_buf[0];

  kp->kb_size = 12;
  kp->kb_count = 0;

  printf("%s [Y,N]?", pLine);
  sti(kp);

  pKbd = skipwh(pKbd);

  if (*pKbd == 'n' || *pKbd == 'N')
    return TRUE;

  return FALSE;
}

INIT BYTE *GetNumArg(BYTE * pLine, COUNT * pnArg)
{
  /* look for NUMBER                               */
  pLine = skipwh(pLine);
  if (!isnum(pLine))
  {
    CfgFailure(pLine);
    return (BYTE *) 0;
  }
  return GetNumber(pLine, pnArg);
}

INIT BYTE *GetStringArg(BYTE * pLine, BYTE * pszString)
{
  /* look for STRING                               */
  pLine = skipwh(pLine);

  /* just return whatever string is there, including null         */
  return scan(pLine, pszString);
}

INIT static VOID Buffers(BYTE * pLine)
{
  COUNT nBuffers;

  /* Get the argument                                             */
  if (GetNumArg(pLine, &nBuffers) == (BYTE *) 0)
    return;

  /* Got the value, assign either default or new value            */
  Config.cfgBuffers = max(Config.cfgBuffers, nBuffers);
}

INIT static VOID sysScreenMode(BYTE * pLine)
{
  COUNT nMode;

  /* Get the argument                                             */
  if (GetNumArg(pLine, &nMode) == (BYTE *) 0)
    return;

  if ((nMode != 0x11) && (nMode != 0x12) && (nMode != 0x14))
    return;

/* Modes
   0x11 (17)   28 lines
   0x12 (18)   43/50 lines
   0x14 (20)   25 lines
 */
  _AX = (0x11 << 8) + nMode;
  _BL = 0;
  __int__(0x10);
}

INIT static VOID sysVersion(BYTE * pLine)
{
  COUNT major,
    minor;
  char *p;

  p = pLine;
  while (*p && *p != '.')
    p++;

  if (*p++ == '\0')
    return;

  /* Get major number */
  if (GetNumArg(pLine, &major) == (BYTE *) 0)
    return;

  /* Get minor number */
  if (GetNumArg(p, &minor) == (BYTE *) 0)
    return;

  printf("Changing reported version to %d.%d\n", major, minor);

  os_major = major;
  os_minor = minor;
}

INIT static VOID Files(BYTE * pLine)
{
  COUNT nFiles;

  /* Get the argument                                             */
  if (GetNumArg(pLine, &nFiles) == (BYTE *) 0)
    return;

  /* Got the value, assign either default or new value            */
  Config.cfgFiles = max(Config.cfgFiles, nFiles);
}

INIT static VOID Lastdrive(BYTE * pLine)
{
  /* Format:   LASTDRIVE = letter         */
  COUNT nFiles;
  BYTE drv;

  pLine = skipwh(pLine);
  drv = *pLine & ~0x20;

  if (drv < 'A' || drv > 'Z')
  {
    CfgFailure(pLine);
    return;
  }
  drv -= 'A';
  Config.cfgLastdrive = max(Config.cfgLastdrive, drv);
}

INIT static VOID Switchar(BYTE * pLine)
{
  /* Format: SWITCHAR = character         */

  GetStringArg(pLine, szBuf);
  switchar = *szBuf;
}

INIT static VOID Fcbs(BYTE * pLine)
{
  /*  Format:     FCBS = totalFcbs [,protectedFcbs]    */
  COUNT fcbs;

  if ((pLine = GetNumArg(pLine, &fcbs)) == 0)
    return;
  Config.cfgFcbs = fcbs;

  pLine = skipwh(pLine);

  if (*pLine == ',')
  {
    GetNumArg(++pLine, &fcbs);
    Config.cfgProtFcbs = fcbs;
  }

  if (Config.cfgProtFcbs > Config.cfgFcbs)
    Config.cfgProtFcbs = Config.cfgFcbs;
}

INIT static VOID Country(BYTE * pLine)
{
  /* Format: COUNTRY = countryCode, [codePage], filename  */
  COUNT ctryCode;
  COUNT codePage;

  if ((pLine = GetNumArg(pLine, &ctryCode)) == 0)
    return;

  pLine = skipwh(pLine);
  if (*pLine == ',')
  {
    pLine = skipwh(pLine);

    if (*pLine == ',')
    {
      codePage = 0;
      ++pLine;
    }
    else
    {
      if ((pLine = GetNumArg(pLine, &codePage)) == 0)
        return;
    }

    pLine = skipwh(pLine);
    if (*pLine == ',')
    {
      GetStringArg(++pLine, szBuf);

      if (LoadCountryInfo(szBuf, ctryCode, codePage))
        return;
    }
  }
  CfgFailure(pLine);
}

INIT static VOID Stacks(BYTE * pLine)
{
  COUNT stacks;

  /* Format:  STACKS = stacks [, stackSize]       */
  pLine = GetNumArg(pLine, &stacks);
  Config.cfgStacks = stacks;

  pLine = skipwh(pLine);

  if (*pLine == ',')
  {
    GetNumArg(++pLine, &stacks);
    Config.cfgStackSize = stacks;
  }

  if (Config.cfgStacks)
  {
    if (Config.cfgStackSize < 32)
      Config.cfgStackSize = 32;
    if (Config.cfgStackSize > 512)
      Config.cfgStackSize = 512;
    if (Config.cfgStacks > 64)
      Config.cfgStacks = 64;
  }
}

INIT static VOID InitPgm(BYTE * pLine)
{
  /* Get the string argument that represents the new init pgm     */
  pLine = GetStringArg(pLine, Config.cfgInit);

  /* Now take whatever tail is left and add it on as a single     */
  /* string.                                                      */
  strcpy(Config.cfgInitTail, pLine);

  /* and add a DOS new line just to be safe                       */
  strcat(Config.cfgInitTail, "\r\n");
}

INIT static VOID Break(BYTE * pLine)
{
  /* Format:      BREAK = (ON | OFF)      */
  BYTE *pTmp;

  GetStringArg(pLine, szBuf);
  break_ena = strcmp(szBuf, "OFF") ? 1 : 0;
}

INIT static VOID Device(BYTE * pLine)
{
  VOID FAR *driver_ptr;
  BYTE *pTmp;
  exec_blk eb;
  struct dhdr FAR *dhp;
  struct dhdr FAR *next_dhp;
  UWORD dev_seg = (((ULONG) FP_SEG(lpBase) << 4) + FP_OFF(lpBase) + 0xf) >> 4;

  /* Get the device driver name                                   */
  GetStringArg(pLine, szBuf);

  /* The driver is loaded at the top of allocated memory.         */
  /* The device driver is paragraph aligned.                      */
  eb.load.reloc = eb.load.load_seg = dev_seg;
  dhp = MK_FP(dev_seg, 0);

#ifdef DEBUG
  printf("Loading device driver %s at segment %04x\n",
         szBuf, dev_seg);
#endif

  if (DosExec(3, &eb, szBuf) == SUCCESS)
  {
    while (FP_OFF(dhp) != 0xFFFF)
    {
      next_dhp = MK_FP(FP_SEG(dhp), FP_OFF(dhp->dh_next));
      dhp->dh_next = nul_dev.dh_next;
      link_dhdr(&nul_dev, dhp, pLine);
      dhp = next_dhp;
    }
  }
  else
    CfgFailure(pLine);
}

INIT static VOID CfgFailure(BYTE * pLine)
{
  BYTE *pTmp = pLineStart;

  printf("CONFIG.SYS error in line %d\n", nCfgLine);
  printf(">>>%s\n", pTmp);
  while (++pTmp != pLine)
    printf(" ");
  printf("^\n");
}

#ifndef KDB
INIT static BYTE FAR *KernelAlloc(WORD nBytes)
{
  BYTE FAR *lpAllocated;

  lpBase = AlignParagraph(lpBase);
  lpAllocated = lpBase;

  if (0x10000 - FP_OFF(lpBase) <= nBytes)
  {
    UWORD newOffs = (FP_OFF(lpBase) + nBytes) & 0xFFFF;
    UWORD newSeg = FP_SEG(lpBase) + 0x1000;

    lpBase = MK_FP(newSeg, newOffs);
  }
  else
    lpBase += nBytes;

  return lpAllocated;
}
#endif

#ifdef I86
INIT static BYTE FAR *KernelAllocDma(WORD bytes)
{
  BYTE FAR *allocated;

  lpBase = AlignParagraph(lpBase);
  if ((FP_SEG(lpBase) & 0x0fff) + (bytes >> 4) > 0x1000)
    lpBase = MK_FP((FP_SEG(lpBase) + 0x0fff) & 0xf000, 0);
  allocated = lpBase;
  lpBase += bytes;
  return allocated;
}

INIT static VOID FAR *AlignParagraph(VOID FAR * lpPtr)
{
  ULONG lTemp;
  UWORD uSegVal;

  /* First, convert the segmented pointer to linear address       */
  lTemp = FP_SEG(lpPtr);
  lTemp = (lTemp << 4) + FP_OFF(lpPtr);

  /* Next, round up the linear address to a paragraph boundary.   */
  lTemp += 0x0f;
  lTemp &= 0xfffffff0l;

  /* Break it into segments.                                      */
  uSegVal = (UWORD) (lTemp >> 4);

  /* and return an adddress adjusted to the nearest paragraph     */
  /* boundary.                                                    */
  return MK_FP(uSegVal, 0);
}
#endif

INIT BYTE *
  skipwh(BYTE * s)
{
  while (*s && (*s == 0x0d || *s == 0x0a || *s == ' ' || *s == '\t'))
    ++s;
  return s;
}

INIT BYTE *
  scan(BYTE * s, BYTE * d)
{
  s = skipwh(s);
  while (*s &&
         !(*s == 0x0d
           || *s == 0x0a
           || *s == ' '
           || *s == '\t'
           || *s == '='))
    *d++ = *s++;
  *d = '\0';
  return s;
}

INIT BYTE *scan_seperator(BYTE * s, BYTE * d)
{
  s = skipwh(s);
  if (*s)
    *d++ = *s++;
  *d = '\0';
  return s;
}

INIT BOOL isnum(BYTE * pLine)
{
  return (*pLine >= '0' && *pLine <= '9');
}

/* JPP - changed so will accept hex number. */
INIT BYTE *GetNumber(REG BYTE * pszString, REG COUNT * pnNum)
{
  BYTE Base = 10;

  *pnNum = 0;
  while (isnum(pszString) || toupper(*pszString) == 'X')
  {
    if (toupper(*pszString) == 'X')
    {
      Base = 16;
      pszString++;
    }
    else
      *pnNum = *pnNum * Base + (*pszString++ - '0');
  }
  return pszString;
}

/* Yet another change for true portability (WDL)                        */
INIT COUNT tolower(COUNT c)
{
  if (c >= 'A' && c <= 'Z')
    return (c + ('a' - 'A'));
  else
    return c;
}

/* Yet another change for true portability (PJV) */
INIT COUNT toupper(COUNT c)
{
  if (c >= 'a' && c <= 'z')
    return (c - ('a' - 'A'));
  else
    return c;
}

/* The following code is 8086 dependant                         */

#ifdef KERNEL
INIT VOID
  mcb_init(mcb FAR * mcbp, UWORD size)
{
  COUNT i;

  mcbp->m_type = MCB_LAST;
  mcbp->m_psp = FREE_PSP;
  mcbp->m_size = size;
  for (i = 0; i < 8; i++)
    mcbp->m_name[i] = '\0';
  mem_access_mode = FIRST_FIT;
}
#endif

INIT VOID
  strcat(REG BYTE * d, REG BYTE * s)
{
  while (*d != 0)
    ++d;
  strcpy(d, s);
}

