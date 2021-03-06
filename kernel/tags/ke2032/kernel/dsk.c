/****************************************************************/
/*                                                              */
/*                            dsk.c                             */
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

#include "portab.h"
#include "globals.h"
#include "dyndata.h"

#ifdef VERSION_STRINGS
static BYTE *dskRcsId =
    "$Id: dsk.c 698 2003-09-18 21:00:09Z bartoldeman $";
#endif

#if defined(DEBUG)
#define DebugPrintf(x) printf x
#else
#define DebugPrintf(x)
#endif

/* #define STATIC  */

#ifdef PROTO
BOOL ASMCFUNC fl_reset(WORD);
COUNT ASMCFUNC fl_readdasd(WORD);
COUNT ASMCFUNC fl_diskchanged(WORD);
COUNT ASMCFUNC fl_rd_status(WORD);

COUNT ASMCFUNC fl_format(WORD, WORD, WORD, WORD, WORD, UBYTE FAR *);
COUNT ASMCFUNC fl_read(WORD, WORD, WORD, WORD, WORD, UBYTE FAR *);
COUNT ASMCFUNC fl_write(WORD, WORD, WORD, WORD, WORD, UBYTE FAR *);
COUNT ASMCFUNC fl_verify(WORD, WORD, WORD, WORD, WORD, UBYTE FAR *);
COUNT ASMCFUNC fl_setdisktype(WORD, WORD);
COUNT ASMCFUNC fl_setmediatype(WORD, WORD, WORD);
VOID ASMCFUNC fl_readkey(VOID);

extern COUNT ASMCFUNC fl_lba_ReadWrite(BYTE drive, WORD mode,
                                       struct _bios_LBA_address_packet FAR
                                       * dap_p);

STATIC int LBA_Transfer(ddt * pddt, UWORD mode, VOID FAR * buffer,
                 ULONG LBA_address, unsigned total, UWORD * transferred);
#else
BOOL ASMCFUNC fl_reset();
COUNT ASMCFUNC fl_readdasd();
COUNT ASMCFUNC fl_diskchanged();
COUNT ASMCFUNC fl_rd_status();
COUNT ASMCFUNC fl_format();
COUNT ASMCFUNC fl_read();
COUNT ASMCFUNC fl_write();
COUNT ASMCFUNC fl_verify();
VOID ASMCFUNC fl_readkey();
COUNT ASMCFUNC fl_setmediatype();
COUNT ASMCFUNC fl_setdisktype();
#endif

#define NENTRY		26      /* total size of dispatch table */

#define LBA_READ         0x4200
#define LBA_WRITE        0x4300
UWORD LBA_WRITE_VERIFY = 0x4302;
#define LBA_VERIFY       0x4400
#define LBA_FORMAT       0xffff /* fake number for FORMAT track
                                   (only for NON-LBA floppies now!) */

                /* this buffer must not overlap a 64K boundary
                   due to DMA transfers
                   this is certainly true, if located somewhere
                   at 0xf+1000 and must hold already during BOOT time
                 */
UBYTE DiskTransferBuffer[1 * SEC_SIZE];

struct Access_info {
  BYTE AI_spec;
  BYTE AI_Flag;
};

struct FS_info {
  ULONG serialno;
  BYTE volume[11];
  BYTE fstype[8];
};

extern struct DynS ASM Dyn;

/*TE - array access functions */
ddt *getddt(int dev)
{
  return &(((ddt *) Dyn.Buffer)[dev]);
}

STATIC VOID tmark(ddt *pddt)
{
  pddt->ddt_fh.ddt_lasttime = ReadPCClock();
}

STATIC BOOL tdelay(ddt *pddt, ULONG ticks)
{
  return ReadPCClock() - pddt->ddt_fh.ddt_lasttime >= ticks;
}

#define N_PART 4                /* number of partitions per
                                   table partition              */

#define PARTOFF 0x1be

#ifdef PROTO
typedef WORD dsk_proc(rqptr rq, ddt * pddt);
#else
typedef WORD dsk_proc();
#endif

STATIC dsk_proc mediachk, bldbpb, blockio, IoctlQueblk,
    Genblkdev, Getlogdev, Setlogdev, blk_Open, blk_Close,
    blk_Media, blk_noerr, blk_nondr, blk_error;

STATIC WORD getbpb(ddt * pddt);
#ifdef PROTO
STATIC WORD dskerr(COUNT);
#else
STATIC WORD dskerr();
#endif

/*                                                                      */
/* the function dispatch table                                          */
/*                                                                      */

static dsk_proc * const dispatch[NENTRY] =
{
      /* disk init is done in diskinit.c, so this should never be called */
      blk_error,                /* Initialize                   */
      mediachk,                 /* Media Check                  */
      bldbpb,                   /* Build BPB                    */
      blk_error,                /* Ioctl In                     */
      blockio,                  /* Input (Read)                 */
      blk_nondr,                /* Non-destructive Read         */
      blk_noerr,                /* Input Status                 */
      blk_noerr,                /* Input Flush                  */
      blockio,                  /* Output (Write)               */
      blockio,                  /* Output with verify           */
      blk_noerr,                /* Output Status                */
      blk_noerr,                /* Output Flush                 */
      blk_error,                /* Ioctl Out                    */
      blk_Open,                 /* Device Open                  */
      blk_Close,                /* Device Close                 */
      blk_Media,                /* Removable Media              */
      blk_noerr,                /* Output till busy             */
      blk_error,                /* undefined                    */
      blk_error,                /* undefined                    */
      Genblkdev,                /* Generic Ioctl Call           */
      blk_error,                /* undefined                    */
      blk_error,                /* undefined                    */
      blk_error,                /* undefined                    */
      Getlogdev,                /* Get Logical Device           */
      Setlogdev,                /* Set Logical Device           */
      IoctlQueblk               /* Ioctl Query                  */
};

#define hd(x)   ((x) & DF_FIXED)

/* ----------------------------------------------------------------------- */
/*  F U N C T I O N S  --------------------------------------------------- */
/* ----------------------------------------------------------------------- */

COUNT ASMCFUNC FAR blk_driver(rqptr rp)
{
  if (rp->r_unit >= blk_dev.dh_name[0] && rp->r_command != C_INIT)
    return failure(E_UNIT);
  if (rp->r_command > NENTRY)
  {
    return failure(E_FAILURE);  /* general failure */
  }
  else
    return ((*dispatch[rp->r_command]) (rp, getddt(rp->r_unit)));
}

STATIC char template_string[] = "XXXXXX diskette in drive X:\n";
#define DRIVE_POS (sizeof(template_string) - 4)

STATIC WORD play_dj(ddt * pddt)
{
  /* play the DJ ... */
  if ((pddt->ddt_descflags & (DF_MULTLOG | DF_CURLOG)) == DF_MULTLOG)
  {
    int i;
    ddt *pddt2 = getddt(0);
    for (i = 0; i < blk_dev.dh_name[0]; i++, pddt2++)
    {
      if (pddt->ddt_driveno == pddt2->ddt_driveno &&
          (pddt2->ddt_descflags & (DF_MULTLOG | DF_CURLOG)) ==
          (DF_MULTLOG | DF_CURLOG))
        break;
    }
    if (i == blk_dev.dh_name[0])
    {
      put_string("Error in the DJ mechanism!\n");   /* should not happen! */
      return M_CHANGED;
    }
    memcpy(template_string, "Remove", 6);
    template_string[DRIVE_POS] = 'A' + pddt2->ddt_logdriveno;
    put_string(template_string);
    memcpy(template_string, "Insert", 6);
    template_string[DRIVE_POS] = 'A' + pddt->ddt_logdriveno;
    put_string(template_string);
    put_string("Press the any key to continue ... \n");
    fl_readkey();
    pddt2->ddt_descflags &= ~DF_CURLOG;
    pddt->ddt_descflags |= DF_CURLOG;
    return M_CHANGED;
  }
  return M_NOT_CHANGED;
}

STATIC WORD diskchange(ddt * pddt)
{
  COUNT result;

  /* if it's a hard drive, media never changes */
  if (hd(pddt->ddt_descflags))
    return M_NOT_CHANGED;

  if (play_dj(pddt) == M_CHANGED)
    return M_CHANGED;

  if (pddt->ddt_descflags & DF_CHANGELINE)      /* if we can detect a change ... */
  {
    if ((result = fl_diskchanged(pddt->ddt_driveno)) == 1)
      /* check if it has changed... */
      return M_CHANGED;
    else if (result == 0)
      return M_NOT_CHANGED;
  }

  /* can not detect or error... */
  return tdelay(pddt, 37ul) ? M_DONT_KNOW : M_NOT_CHANGED;
}

STATIC WORD mediachk(rqptr rp, ddt * pddt)
{
  /* check floppy status */
  if (pddt->ddt_descflags & DF_REFORMAT)
  {
    pddt->ddt_descflags &= ~DF_REFORMAT;
    rp->r_mcretcode = M_CHANGED;
  }
  else if (pddt->ddt_descflags & DF_DISKCHANGE)
  {
    pddt->ddt_descflags &= ~DF_DISKCHANGE;
    rp->r_mcretcode = M_DONT_KNOW;
  }
  else
  {
    rp->r_mcretcode = diskchange(pddt);
    if (rp->r_mcretcode == M_DONT_KNOW)
    {
      /* don't know but can check serial number ... */
      ULONG serialno = pddt->ddt_serialno;
      COUNT result = getbpb(pddt);
      if (result != 0)
        return (result);
      if (serialno != pddt->ddt_serialno)
        rp->r_mcretcode = M_CHANGED;
    }
  }
  return S_DONE;
}

/*
 *  Read Write Sector Zero or Hard Drive Dos Bpb
 */
STATIC WORD RWzero(ddt * pddt, UWORD mode)
{
  UWORD done;

  return LBA_Transfer(pddt, mode,
                      (UBYTE FAR *) & DiskTransferBuffer,
                      pddt->ddt_offset, 1, &done);
}

/*
   0 if not set, 1 = a, 2 = b, etc, assume set.
   page 424 MS Programmer's Ref.
 */
STATIC WORD Getlogdev(rqptr rp, ddt * pddt)
{
  int i;
  ddt *pddt2 = getddt(0);

  if (!(pddt->ddt_descflags & DF_MULTLOG)) {
    rp->r_unit = 0;
    return S_DONE;
  }

  for (i = 0; i < blk_dev.dh_name[0]; i++, pddt2++)
  {
    if (pddt->ddt_driveno == pddt2->ddt_driveno &&
        (pddt2->ddt_descflags & (DF_MULTLOG | DF_CURLOG)) ==
        (DF_MULTLOG | DF_CURLOG))
        break;
  }

  rp->r_unit = i+1;
  return S_DONE;
}

STATIC WORD Setlogdev(rqptr rp, ddt * pddt)
{
  int i;
  ddt *pddt2 = getddt(0);

  if (!(pddt->ddt_descflags & DF_MULTLOG)) {
    rp->r_unit = 0;
    return S_DONE;
  }

  for (i = 0; i < blk_dev.dh_name[0]; i++, pddt2++)
  {
    if (pddt->ddt_driveno == pddt2->ddt_driveno &&
        (pddt2->ddt_descflags & (DF_MULTLOG | DF_CURLOG)) ==
        (DF_MULTLOG | DF_CURLOG))
        break;
  }

  pddt2->ddt_descflags &= ~DF_CURLOG;
  pddt->ddt_descflags |= DF_CURLOG;
  rp->r_unit++;
  return S_DONE;
}

STATIC WORD blk_Open(rqptr rp, ddt * pddt)
{
  UNREFERENCED_PARAMETER(rp);

  pddt->ddt_FileOC++;
  return S_DONE;
}

STATIC WORD blk_Close(rqptr rp, ddt * pddt)
{
  UNREFERENCED_PARAMETER(rp);

  pddt->ddt_FileOC--;
  return S_DONE;
}

STATIC WORD blk_nondr(rqptr rp, ddt * pddt)
{
  UNREFERENCED_PARAMETER(rp);
  UNREFERENCED_PARAMETER(pddt);

  return S_BUSY | S_DONE;
}

STATIC WORD blk_Media(rqptr rp, ddt * pddt)
{
  UNREFERENCED_PARAMETER(rp);

  if (hd(pddt->ddt_descflags))
    return S_BUSY | S_DONE;     /* Hard Drive */
  else
    return S_DONE;              /* Floppy */
}

STATIC WORD getbpb(ddt * pddt)
{
  ULONG count;
  bpb *pbpbarray = &pddt->ddt_bpb;
  WORD head, /*track, */ sector, ret;

  /* pddt->ddt_descflags |= DF_NOACCESS; 
   * disabled for now - problems with FORMAT ?? */

  /* set drive to not accessible and changed */
  if (diskchange(pddt) != M_NOT_CHANGED)
    pddt->ddt_descflags |= DF_DISKCHANGE;

  ret = RWzero(pddt, LBA_READ);
  if (ret != 0)
    return (dskerr(ret));

  pbpbarray->bpb_nbyte = getword(&DiskTransferBuffer[BT_BPB]);

  if (DiskTransferBuffer[0x1fe] != 0x55
      || DiskTransferBuffer[0x1ff] != 0xaa || pbpbarray->bpb_nbyte != 512)
  {
    /* copy default bpb to be sure that there is no bogus data */
    memcpy(pbpbarray, &pddt->ddt_defbpb, sizeof(bpb));
    return S_DONE;
  }

  pddt->ddt_descflags &= ~DF_NOACCESS;  /* set drive to accessible */

/*TE ~ 200 bytes*/

  memcpy(pbpbarray, &DiskTransferBuffer[BT_BPB], sizeof(bpb));

  /*?? */
  /*  2b is fat16 volume label. if memcmp, then offset 0x36.
     if (fstrncmp((BYTE *) & DiskTransferBuffer[0x36], "FAT16",5) == 0  ||
     fstrncmp((BYTE *) & DiskTransferBuffer[0x36], "FAT12",5) == 0) {
     TE: I'm not sure, what the _real_ decision point is, however MSDN
     'A_BF_BPB_SectorsPerFAT
     The number of sectors per FAT.
     Note: This member will always be zero in a FAT32 BPB.
     Use the values from A_BF_BPB_BigSectorsPerFat...
  */
  {
    struct FS_info *fs = (struct FS_info *)&DiskTransferBuffer[0x27];
#ifdef WITHFAT32
    if (pbpbarray->bpb_nfsect == 0)
    {
      /* FAT32 boot sector */
      fs = (struct FS_info *)&DiskTransferBuffer[0x43];
      pbpbarray->bpb_ndirent = 512;
    }
#endif    
    pddt->ddt_serialno = getlong(&fs->serialno);
    memcpy(pddt->ddt_volume, fs->volume, sizeof fs->volume);
    memcpy(pddt->ddt_fstype, fs->fstype, sizeof fs->fstype);
  }

#ifdef DSK_DEBUG
  printf("BPB_NBYTE     = %04x\n", pbpbarray->bpb_nbyte);
  printf("BPB_NSECTOR   = %02x\n", pbpbarray->bpb_nsector);
  printf("BPB_NRESERVED = %04x\n", pbpbarray->bpb_nreserved);
  printf("BPB_NFAT      = %02x\n", pbpbarray->bpb_nfat);
  printf("BPB_NDIRENT   = %04x\n", pbpbarray->bpb_ndirent);
  printf("BPB_NSIZE     = %04x\n", pbpbarray->bpb_nsize);
  printf("BPB_MDESC     = %02x\n", pbpbarray->bpb_mdesc);
  printf("BPB_NFSECT    = %04x\n", pbpbarray->bpb_nfsect);
#endif

  count =
      pbpbarray->bpb_nsize == 0 ?
      pbpbarray->bpb_huge : pbpbarray->bpb_nsize;
  head = pbpbarray->bpb_nheads;
  sector = pbpbarray->bpb_nsecs;

  if (head == 0 || sector == 0)
  {
    tmark(pddt);
    return failure(E_FAILURE);
  }
  pddt->ddt_ncyl = (count + head * sector - 1) / (head * sector);

  tmark(pddt);

#ifdef DSK_DEBUG
  printf("BPB_NSECS     = %04x\n", sector);
  printf("BPB_NHEADS    = %04x\n", head);
  printf("BPB_HIDDEN    = %08lx\n", pbpbarray->bpb_hidden);
  printf("BPB_HUGE      = %08lx\n", pbpbarray->bpb_huge);
#endif

  return 0;
}

STATIC WORD bldbpb(rqptr rp, ddt * pddt)
{
  WORD result;

  if ((result = getbpb(pddt)) != 0)
    return result;

  rp->r_bpptr = &pddt->ddt_bpb;
  return S_DONE;
}

STATIC WORD IoctlQueblk(rqptr rp, ddt * pddt)
{
  UNREFERENCED_PARAMETER(pddt);

  switch (rp->r_count)
  {
    case 0x0846:
    case 0x0847:
    case 0x0860:
    case 0x0866:
    case 0x0867:
      break;
    default:
      return failure(E_CMD);
  }
  return S_DONE;

}

STATIC COUNT Genblockio(ddt * pddt, UWORD mode, WORD head, WORD track,
                 WORD sector, WORD count, VOID FAR * buffer)
{
  UWORD transferred;

  /* apparently sector is ZERO, not ONE based !!! */
  return LBA_Transfer(pddt, mode, buffer,
                      ((ULONG) track * pddt->ddt_bpb.bpb_nheads + head) *
                      (ULONG) pddt->ddt_bpb.bpb_nsecs +
                      pddt->ddt_offset + sector, count, &transferred);
}

STATIC WORD Genblkdev(rqptr rp, ddt * pddt)
{
  int ret;
  bpb *pbpb;
#ifdef WITHFAT32
  int extended = 0;

  if ((rp->r_count >> 8) == 0x48)
    extended = 1;
  else
#endif
  if ((rp->r_count >> 8) != 8)
    return failure(E_CMD);

  switch (rp->r_count & 0xff)
  {
    case 0x40:                 /* set device parameters */
      {
        struct gblkio FAR *gblp = (struct gblkio FAR *)rp->r_trans;

        pddt->ddt_type = gblp->gbio_devtype;
        pddt->ddt_descflags &= ~3;
        pddt->ddt_descflags |= (gblp->gbio_devattrib & 3)
            | (DF_DPCHANGED | DF_REFORMAT);
        pddt->ddt_ncyl = gblp->gbio_ncyl;
        /* use default dpb or current bpb? */
        pbpb =
            (gblp->gbio_spcfunbit & 0x01) ==
            0 ? &pddt->ddt_defbpb : &pddt->ddt_bpb;
#ifdef WITHFAT32
        if (!extended)
          fmemcpy(pbpb, &gblp->gbio_bpb, BPB_SIZEOF);
        else
#endif
          fmemcpy(pbpb, &gblp->gbio_bpb, sizeof(gblp->gbio_bpb));
        /*pbpb->bpb_nsector = gblp->gbio_nsecs; */
        break;
      }
    case 0x41:                 /* write track */
      {
        struct gblkrw FAR *rw = (struct gblkrw FAR *)rp->r_trans;
        ret = Genblockio(pddt, LBA_WRITE, rw->gbrw_head, rw->gbrw_cyl,
                         rw->gbrw_sector, rw->gbrw_nsecs, rw->gbrw_buffer);
        if (ret != 0)
          return dskerr(ret);
      }
      break;
    case 0x42:                 /* format/verify track */
      {
        struct gblkfv FAR *fv = (struct gblkfv FAR *)rp->r_trans;
        COUNT tracks;
        struct thst {
          UBYTE track, head, sector, type;
        } *addrfield, afentry;

        if (hd(pddt->ddt_descflags))
        {
          /* XXX no low-level formatting for hard disks implemented */
          fv->gbfv_spcfunbit = 1;       /* "not supported by bios" */
          pddt->ddt_descflags &= ~DF_DPCHANGED;
          return S_DONE;
        }
        if (pddt->ddt_descflags & DF_DPCHANGED)
        {
          pddt->ddt_descflags &= ~DF_DPCHANGED;

          /* first try newer setmediatype function */
          ret = fl_setmediatype(pddt->ddt_driveno, pddt->ddt_ncyl,
                                pddt->ddt_bpb.bpb_nsecs);
          if (ret == 0xc)
          {
            /* specified tracks, sectors/track not allowed for drive */
            fv->gbfv_spcfunbit = 2;
            return dskerr(ret);
          }
          else if (ret == 0x80)
          {
            fv->gbfv_spcfunbit = 3;     /* no disk in drive */
            return dskerr(ret);
          }
          else if (ret != 0)
            /* otherwise, setdisktype */
          {
            COUNT type = 0;
            if ((fv->gbfv_spcfunbit & 1) &&
                (ret =
                 fl_read(pddt->ddt_driveno, 0, 0, 1, 1,
                         DiskTransferBuffer)) != 0)
            {
              fv->gbfv_spcfunbit = 3;   /* no disk in drive */
              return dskerr(ret);
            }
            if (pddt->ddt_ncyl == 40 &&
                (pddt->ddt_bpb.bpb_nsecs == 9
                 || pddt->ddt_bpb.bpb_nsecs == 8))
            {
              if (pddt->ddt_type == 0)
                type = 1;       /* 320/360K disk in 360K drive */
              else if (pddt->ddt_type == 1)
                type = 2;       /* 320/360K disk in 1.2M drive */
            }
            else if (pddt->ddt_type == 1 && pddt->ddt_ncyl == 80 &&
                     pddt->ddt_bpb.bpb_nsecs == 15)
              type = 3;         /* 1.2M disk in 1.2M drive */
            else if ((pddt->ddt_type == 2 || pddt->ddt_type == 7) &&
                     pddt->ddt_ncyl == 80 && pddt->ddt_bpb.bpb_nsecs == 15)
              type = 4;         /* 720kb disk in 1.44M or 720kb drive */

            if (type == 0)
            {
              /* specified tracks, sectors/track not allowed for drive */
              fv->gbfv_spcfunbit = 2;
              return dskerr(0xc);
            }
            fl_setdisktype(pddt->ddt_driveno, type);
          }
        }
        if (fv->gbfv_spcfunbit & 1)
          return S_DONE;

        afentry.type = 2;       /* 512 byte sectors */
        afentry.track = fv->gbfv_cyl;
        afentry.head = fv->gbfv_head;

        for (tracks = fv->gbfv_spcfunbit & 2 ? fv->gbfv_ntracks : 1;
             tracks > 0; tracks--)
        {
          addrfield = (struct thst *)DiskTransferBuffer;

          if (afentry.track > pddt->ddt_ncyl)
            return failure(E_FAILURE);

          for (afentry.sector = 1;
               afentry.sector <= pddt->ddt_bpb.bpb_nsecs; afentry.sector++)
            memcpy(addrfield++, &afentry, sizeof(afentry));

          ret =
              Genblockio(pddt, LBA_FORMAT, afentry.head, afentry.track, 0,
                         pddt->ddt_bpb.bpb_nsecs, DiskTransferBuffer);
          if (ret != 0)
            return dskerr(ret);
        }
        afentry.head++;
        if (afentry.head >= pddt->ddt_bpb.bpb_nheads)
        {
          afentry.head = 0;
          afentry.track++;
        }
      }

      /* fall through to verify */

    case 0x62:                 /* verify track */
      {
        struct gblkfv FAR *fv = (struct gblkfv FAR *)rp->r_trans;

        ret = Genblockio(pddt, LBA_VERIFY, fv->gbfv_head, fv->gbfv_cyl, 0,
                         (fv->gbfv_spcfunbit ?
                          fv->gbfv_ntracks * pddt->ddt_defbpb.bpb_nsecs :
                          pddt->ddt_defbpb.bpb_nsecs), DiskTransferBuffer);
        if (ret != 0)
          return dskerr(ret);
        fv->gbfv_spcfunbit = 0; /* success */
      }
      break;
    case 0x46:                 /* set volume serial number */
      {
        struct Gioc_media FAR *gioc = (struct Gioc_media FAR *)rp->r_trans;
        struct FS_info *fs;

        ret = getbpb(pddt);
        if (ret != 0)
          return (ret);

        fs = (struct FS_info *)&DiskTransferBuffer
            [(pddt->ddt_bpb.bpb_nfsect != 0 ? 0x27 : 0x43)];
        fs->serialno = gioc->ioc_serialno;
        pddt->ddt_serialno = fs->serialno;

        ret = RWzero(pddt, LBA_WRITE);
        if (ret != 0)
          return (dskerr(ret));
      }
      break;
    case 0x47:                 /* set access flag */
      {
        struct Access_info FAR *ai = (struct Access_info FAR *)rp->r_trans;
        pddt->ddt_descflags &= ~DF_NOACCESS;
        pddt->ddt_descflags |= (ai->AI_Flag ? 0 : DF_NOACCESS);
      }
      break;
    case 0x60:                 /* get device parameters */
      {
        struct gblkio FAR *gblp = (struct gblkio FAR *)rp->r_trans;

        gblp->gbio_devtype = pddt->ddt_type;
        gblp->gbio_devattrib = pddt->ddt_descflags & 3;
        /* 360 kb disk in 1.2 MB drive */
        gblp->gbio_media = (pddt->ddt_type == 1) && (pddt->ddt_ncyl == 40);
        gblp->gbio_ncyl = pddt->ddt_ncyl;
        /* use default dpb or current bpb? */
        pbpb =
            (gblp->gbio_spcfunbit & 0x01) ==
            0 ? &pddt->ddt_defbpb : &pddt->ddt_bpb;
#ifdef WITHFAT32
        if (!extended)
          fmemcpy(&gblp->gbio_bpb, pbpb, BPB_SIZEOF);
        else
#endif
          fmemcpy(&gblp->gbio_bpb, pbpb, sizeof(gblp->gbio_bpb));
        /*gblp->gbio_nsecs = pbpb->bpb_nsector; */
        break;
      }
    case 0x61:                 /* read track */
      {
        struct gblkrw FAR *rw = (struct gblkrw FAR *)rp->r_trans;
        ret = Genblockio(pddt, LBA_READ, rw->gbrw_head, rw->gbrw_cyl,
                         rw->gbrw_sector, rw->gbrw_nsecs, rw->gbrw_buffer);
        if (ret != 0)
          return dskerr(ret);
      }
      break;
    case 0x66:                 /* get volume serial number */
      {
        struct Gioc_media FAR *gioc = (struct Gioc_media FAR *)rp->r_trans;

        ret = getbpb(pddt);
        if (ret != 0)
          return (ret);

        gioc->ioc_serialno = pddt->ddt_serialno;
        fmemcpy(gioc->ioc_volume, pddt->ddt_volume, 11);
        fmemcpy(gioc->ioc_fstype, pddt->ddt_fstype, 8);
      }
      break;
    case 0x67:                 /* get access flag */
      {
        struct Access_info FAR *ai = (struct Access_info FAR *)rp->r_trans;
        ai->AI_Flag = pddt->ddt_descflags & DF_NOACCESS ? 0 : 1;        /* bit 9 */
      }
      break;
    default:
      return failure(E_CMD);
  }
  return S_DONE;
}

STATIC WORD blockio(rqptr rp, ddt * pddt)
{
  ULONG start, size;
  WORD ret;
  UWORD done;

  int action;
  bpb *pbpb;

  switch (rp->r_command)
  {
    case C_INPUT:
      action = LBA_READ;
      break;
    case C_OUTPUT:
      action = LBA_WRITE;
      break;
    case C_OUTVFY:
      action = LBA_WRITE_VERIFY;
      break;
    default:
      return failure(E_FAILURE);
  }

  if (pddt->ddt_descflags & DF_NOACCESS)      /* drive inaccessible */
    return failure(E_FAILURE);

  tmark(pddt);
  start = (rp->r_start != HUGECOUNT ? rp->r_start : rp->r_huge);
  pbpb = hd(pddt->ddt_descflags) ? &pddt->ddt_defbpb : &pddt->ddt_bpb;
  size = (pbpb->bpb_nsize ? pbpb->bpb_nsize : pbpb->bpb_huge);

  if (start >= size || start + rp->r_count > size)
  {
    return 0x0408;
  }
  start += pddt->ddt_offset;

  ret = LBA_Transfer(pddt, action,
                     rp->r_trans,
                     start, rp->r_count, &done);
  rp->r_count = done;

  if (ret != 0)
  {
    return dskerr(ret);
  }
  return S_DONE;
}

STATIC WORD blk_error(rqptr rp, ddt * pddt)
{
  UNREFERENCED_PARAMETER(pddt);

  rp->r_count = 0;
  return failure(E_FAILURE);    /* general failure */
}

STATIC WORD blk_noerr(rqptr rp, ddt * pddt)
{
  UNREFERENCED_PARAMETER(rp);
  UNREFERENCED_PARAMETER(pddt);

  return S_DONE;
}

STATIC WORD dskerr(COUNT code)
{
/*      printf("diskette error:\nhead = %d\ntrack = %d\nsector = %d\ncount = %d\n",
   head, track, sector, count); */
  switch (code & 0x03)
  {
    case 1:                    /* invalid command - general failure */
      if (code & 0x08)
        return S_ERROR | E_NOTRDY;      /* failure(E_NOTRDY); at least on yhe INT25 route,
                                           0x8002 is returned */
      else
        return failure(E_CMD);

    case 2:                    /* address mark not found - general  failure */
      return failure(E_FAILURE);

    case 3:                    /* write protect */
      return failure(E_WRPRT);

    default:
      if (code & 0x80)          /* time-out */
        return failure(E_NOTRDY);
      else if (code & 0x40)     /* seek error */
        return failure(E_SEEK);
      else if (code & 0x10)     /* CRC error */
        return failure(E_CRC);
      else if (code & 0x04)
        return failure(E_NOTFND);
      else
        return failure(E_FAILURE);
  }
}

/*
    translate LBA sectors into CHS addressing
*/

STATIC int LBA_to_CHS(ULONG LBA_address, struct CHS *chs, const ddt * pddt)
{
  /* we need the defbpb values since those are taken from the
     BIOS, not from some random boot sector, except when
     we're dealing with a floppy */

  const bpb *pbpb = hd(pddt->ddt_descflags) ? &pddt->ddt_defbpb : &pddt->ddt_bpb;
  unsigned hs = pbpb->bpb_nsecs * pbpb->bpb_nheads;
  unsigned hsrem = (unsigned)(LBA_address % hs);

  LBA_address /= hs;

  if (LBA_address > 1023ul)
  {
#ifdef DEBUG
    printf("LBA-Transfer error : cylinder %lu > 1023\n", LBA_address);
#else
    put_string("LBA-Transfer error : cylinder > 1023\n");
#endif
    return 1;
  }

  chs->Cylinder = LBA_address;
  chs->Head = hsrem / pbpb->bpb_nsecs;
  chs->Sector =  hsrem % pbpb->bpb_nsecs + 1;
  return 0;
}

  /* Test for 64K boundary crossing and return count small        */
  /* enough not to exceed the threshold.                          */

STATIC unsigned DMA_max_transfer(void FAR * buffer, unsigned count)
{
  unsigned dma_off = (UWORD)((FP_SEG(buffer) << 4) + FP_OFF(buffer));
  unsigned sectors_to_dma_boundary = (dma_off == 0 ?
    0xffff / SEC_SIZE :
    (UWORD)(-dma_off) / SEC_SIZE);

  return min(count, sectors_to_dma_boundary);
}

/*
    int LBA_Transfer(
        ddt *pddt,                          physical characteristics of drive
        UWORD mode,                         LBA_READ/WRITE/WRITE_VERIFY/VERIFY
        VOID FAR *buffer,                   user buffer
        ULONG LBA_address,                  absolute sector address
        unsigned totaltodo,                 number of sectors to transfer
        UWORD *transferred                  sectors actually transferred

    Read/Write/Write+verify some sectors, using LBA addressing.
    
    
    This function handles all the minor details, including:
    
        retry in case of errors
        
        crossing the 64K DMA boundary
        
        translation to CHS addressing if necessary
        
        crossing track boundaries (necessary for some BIOS's
    
        High memory doesn't work very well, use internal buffer
        
        write with verify details for LBA
    
*/

STATIC int LBA_Transfer(ddt * pddt, UWORD mode, VOID FAR * buffer,
                 ULONG LBA_address, unsigned totaltodo,
                 UWORD * transferred)
{
  static struct _bios_LBA_address_packet dap = {
    16, 0, 0, 0, 0, 0, 0
  };

  unsigned count;
  unsigned error_code = 0;
  struct CHS chs;
  void FAR *transfer_address;

  int num_retries;

  *transferred = 0;
  
  /* only low-level format floppies for now ! */
  if (mode == LBA_FORMAT && hd(pddt->ddt_descflags))
    return 0;

  /* optionally change from A: to B: or back */
  play_dj(pddt);

  if (!hd(pddt->ddt_descflags))
  {
    UBYTE FAR  *int1e_ptr = (UBYTE FAR *)getvec(0x1e);

    if (int1e_ptr[4] != pddt->ddt_bpb.bpb_nsecs)
    {
      int1e_ptr[4] = pddt->ddt_bpb.bpb_nsecs;
      fl_reset(pddt->ddt_driveno);
    }
  }
        
/*    
    if (LBA_address+totaltodo > pddt->total_sectors)
        {
        printf("LBA-Transfer error : address overflow = %lu > %lu max\n",LBA_address+totaltodo,driveParam->total_sectors);
        return 1;
        }
*/

  for (; totaltodo != 0;)
  {
    /* avoid overflowing 64K DMA boundary */
    count = DMA_max_transfer(buffer, totaltodo);

    if (FP_SEG(buffer) >= 0xa000 || count == 0)
    {
      transfer_address = DiskTransferBuffer;
      count = 1;

      if ((mode & 0xff00) == (LBA_WRITE & 0xff00))
      {
        fmemcpy(DiskTransferBuffer, buffer, 512);
      }
    }
    else
    {
      transfer_address = buffer;
    }

    for (num_retries = 0; num_retries < N_RETRY; num_retries++)
    {
      if (pddt->ddt_LBASupported && mode != LBA_FORMAT)
      {
        dap.number_of_blocks = count;

        dap.buffer_address = transfer_address;

        dap.block_address_high = 0;     /* clear high part */
        dap.block_address = LBA_address;        /* clear high part */

        /* Load the registers and call the interrupt. */

        if (pddt->ddt_WriteVerifySupported || mode != LBA_WRITE_VERIFY)
        {
          error_code = fl_lba_ReadWrite(pddt->ddt_driveno, mode, &dap);
        }
        else
        {
          /* verify requested, but not supported */
          error_code =
              fl_lba_ReadWrite(pddt->ddt_driveno, LBA_WRITE, &dap);

          if (error_code == 0)
          {
            error_code =
                fl_lba_ReadWrite(pddt->ddt_driveno, LBA_VERIFY, &dap);
          }
        }
      }
      else
      {                         /* transfer data, using old bios functions */

        if (LBA_to_CHS(LBA_address, &chs, pddt))
          return 1;

        /* avoid overflow at end of track */

        if (chs.Sector + count > (unsigned)pddt->ddt_bpb.bpb_nsecs + 1)
        {
          count = pddt->ddt_bpb.bpb_nsecs + 1 - chs.Sector;
        }

        error_code = (mode == LBA_READ ? fl_read :
                      mode == LBA_VERIFY ? fl_verify :
                      mode ==
                      LBA_FORMAT ? fl_format : fl_write) (pddt->
                                                          ddt_driveno,
                                                          chs.Head,
                                                          chs.Cylinder,
                                                          chs.Sector,
                                                          count,
                                                          transfer_address);

        if (error_code == 0 && mode == LBA_WRITE_VERIFY)
        {
          error_code = fl_verify(pddt->ddt_driveno,
                                 chs.Head, chs.Cylinder,
                                 chs.Sector, count, transfer_address);
        }
      }
      if (error_code == 0)
        break;

      fl_reset(pddt->ddt_driveno);

    }                           /* end of retries */

    if (error_code)
    {
      return error_code;
    }

    /* copy to user buffer if nesessary */
    if (transfer_address == DiskTransferBuffer &&
        (mode & 0xff00) == (LBA_READ & 0xff00))
    {
      fmemcpy(buffer, DiskTransferBuffer, 512);
    }

    *transferred += count;
    LBA_address += count;
    totaltodo -= count;

    buffer = add_far(buffer, count * 512);
  }

  return (error_code);
}

/*
 * Revision 1.17  2001/05/13           tomehlert
 * Added full support for LBA hard drives
 * initcode moved (mostly) to initdisk.c
 * lower interface partly redesigned
 */
