/******************************************************************
    Copyright (C) 2020  JPZ4085
    Based on fat32.c Copyright (C) 2009  Henrik Carlqvist

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
******************************************************************/
#ifdef __linux__
#define _XOPEN_SOURCE 700 /* X/Open7 POSIX.1-2008 for fileno() */
#include <alloca.h>
#include <sys/ioctl.h>
#ifndef BLKSSZGET
#include <linux/fs.h>
#endif
#endif

#ifdef __OpenBSD__
#include <sys/dkio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/disklabel.h>
#endif

#if defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/disk.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "exfat.h"

int is_exfat_fs(FILE *fp)
{
   char *szMagic = "EXFAT   ";

   return contains_data(fp, 0x3, szMagic, strlen(szMagic));
} /* is_exfat_fs */

int is_exfat_br(FILE *fp)
{
    /* Check if the device or file contains an EXFAT boot record
       by looking for the magic chars 0x55, 0xAA at the end of the
       first nine reserved sectors in the Volume Boot Record */
    uint64_t blksize = get_sector_size(fp);
    unsigned char aucRef[] = {0x55, 0xAA};
    unsigned char aucMagic[] = {'E','X','F','A','T',' ',' ',' '};
    int i;
    
    for(i=0 ; i<9 ; i++)
      if( ! contains_data(fp, (blksize - 2) + i*blksize, aucRef, sizeof(aucRef)))
            return 0;
    if( ! contains_data(fp, 0x03, aucMagic, sizeof(aucMagic)))
          return 0;
    return 1;
} /* is_exfat_br */

int entire_exfat_nt6_br_matches(FILE *fp)
{
    #include "br_exfatnt6_0x78.h"
    
    uint64_t blksize = get_sector_size(fp);
    uint64_t offset = (blksize * 12) + 120;
    
    return (/* Skip the BIOS Parameter Block and check the main Volume Boot Record */
            contains_data(fp, 0x78, br_exfatnt6_0x78, sizeof(br_exfatnt6_0x78)) &&
            /* Skip the first twelve sectors and the backup BIOS Parameter Block then
               check the backup Volume Boot Record */
            contains_data(fp, offset, br_exfatnt6_0x78, sizeof(br_exfatnt6_0x78))
            );
} /* entire_exfat_nt6_br_matches */

int write_exfat_nt6_br(FILE *fp)
{
    #include "br_exfatnt6_0x78.h"
    
    uint64_t blksize = get_sector_size(fp);
    uint64_t offset = (blksize * 12) + 120;
    
    /* No reason to use (bKeepLabel) since EXFAT doesn't
       keep the partition/volume label in the boot sectors */
    
    return (/* BIOS Parameter Block should not be overwritten while
             writing boot strapping code to the main Volume Boot Record */
            write_data(fp, 0x78, br_exfatnt6_0x78, sizeof(br_exfatnt6_0x78)) &&
            /* Now write boot strapping code to the backup Volume Boot Record */
            write_data(fp, offset, br_exfatnt6_0x78, sizeof(br_exfatnt6_0x78))
            );
} /* write_exfat_nt6_br */

int write_exfat_br_checksum(FILE *fp)
{
    unsigned char *vbrbuffer;
    uint32_t newchecksum = 0;
    uint64_t blksize = 0;
    int i;
    
    blksize = get_sector_size(fp);
    vbrbuffer = alloca(blksize * 11);
    if (!vbrbuffer)
        return 0;
    
    fseek(fp, 0, SEEK_SET);
    if (fread(vbrbuffer, blksize, 11, fp) != 11)
        return 0;

    newchecksum = exfat_boot_checksum(vbrbuffer, blksize);
    /* Write new main VBR checksum */
    fseek(fp, blksize * 11, SEEK_SET);
    for(i=0 ; i<(blksize / sizeof(newchecksum)); i++)
        if(!fwrite(&newchecksum, sizeof(newchecksum), 1, fp))
            return 0;
    /* Write new backup VBR checksum */
    fseek(fp, blksize * 23, SEEK_SET);
    for(i=0 ; i<(blksize / sizeof(newchecksum)); i++)
        if(!fwrite(&newchecksum, sizeof(newchecksum), 1, fp))
            return 0;
    return 1;
} /* write_exfat_br_checksum */

uint32_t exfat_boot_checksum(const void* sector, size_t size)
{
    size_t i;
    uint32_t sum = 0;
    uint32_t block = (uint32_t)size * 11;
    
    for (i = 0; i < block; i++)
    /* skip volume_state and allocated_percent fields */
        if (i != 0x6a && i != 0x6b && i != 0x70)
            sum = ((sum << 31) | (sum >> 1)) + ((const uint8_t*) sector)[i];
    return sum;
} /* exfat_boot_checksum */

uint64_t get_sector_size(FILE *fp)
{
    int iFd = fileno(fp);
#ifdef BLKSSZGET
    uint64_t sector_size = 0;
    int iRes1 = ioctl(iFd, BLKSSZGET, &sector_size);
#endif
#ifdef DIOCGSECTORSIZE
    uint sector_size = 0;
    int iRes1 = ioctl(iFd, DIOCGSECTORSIZE, &sector_size);
#endif
#ifdef DIOCGDINFO
    struct disklabel geometry;
    int iRes1 = ioctl(iFd, DIOCGDINFO, &geometry);
    uint32_t sector_size = geometry.d_secsize;
#endif
#ifdef DKIOCGETBLOCKSIZE
    uint32_t sector_size = 0;
    int iRes1 = ioctl(iFd, DKIOCGETBLOCKSIZE, &sector_size);
#endif
    
    if (!iRes1) {
        return (uint64_t)sector_size;
    } else {
        sector_size = 512;
        return (uint64_t)sector_size;
    }
} /* get_sector_size */
