/******************************************************************
    Copyright (C) 2009  Henrik Carlqvist

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
#include <stdio.h>

#include "file.h"
#include "fat16.h"
#include "fat32.h"
#include "exfat.h"
#include "ntfs.h"
#include "identify.h"
#include "partition_info.h"

int write_partition_number_of_heads(FILE *fp, int iHeads)
{
   unsigned char aucBuf[2];
   unsigned short s = (iHeads < 0) ? partition_number_of_heads(fp) : (unsigned short)iHeads;

   if(!s)
      return 0;
   /* Converting a number like this is not necessary as long as we are on
      i386 compatible CPUs, however, the following code might make the program
      more portable... */
   aucBuf[0] = (unsigned char)(s & 0xff);
   aucBuf[1] = (unsigned char)((s & 0xff00) >> 8);
   return write_data(fp, 0x1a, aucBuf, 2);
} /* write_partition_number_of_heads */

#ifdef __OpenBSD__
int write_partition_start_sector_number(FILE *fp, const char *szPath)
#else
int write_partition_start_sector_number(FILE *fp)
#endif
{
#ifdef __OpenBSD__
   unsigned long l = partition_start_sector(fp, szPath);
#else
   unsigned long l = partition_start_sector(fp);
#endif

   if(!l)
      return 0;
   /* Converting a number like this is not necessary as long as we are on
      i386 compatible CPUs, however, the following code might make the program
      more portable... */
    if((is_fat_16_fs(fp)) || (is_fat_32_fs(fp)) || is_ntfs_fs(fp)) {
        unsigned char aucBuf[4];
        aucBuf[0] = (unsigned char)(l & 0xff);
        aucBuf[1] = (unsigned char)((l & 0xff00) >> 8);
        aucBuf[2] = (unsigned char)((l & 0xff0000) >> 16);
        aucBuf[3] = (unsigned char)((l & 0xff000000) >> 24);
        return write_data(fp, 0x1c, aucBuf, 4);
    }
    if(is_exfat_fs(fp)) {
        unsigned char aucBuf[8];
        aucBuf[0] = (unsigned char)(l & 0xff);
        aucBuf[1] = (unsigned char)((l & 0xff00) >> 8);
        aucBuf[2] = (unsigned char)((l & 0xff0000) >> 16);
        aucBuf[3] = (unsigned char)((l & 0xff000000) >> 24);
        aucBuf[4] = (unsigned char)((l & 0xff00000000) >> 32);
        aucBuf[5] = (unsigned char)((l & 0xff0000000000) >> 40);
        aucBuf[6] = (unsigned char)((l & 0xff000000000000) >> 48);
        aucBuf[7] = (unsigned char)((l & 0xff00000000000000) >> 56);
        return write_data(fp, 0x40, aucBuf, 8);
    } else {
        return 0;
    }
} /* write_partition_start_sector_number */

int write_partition_physical_disk_drive_id_fat16(FILE *fp)
{
    unsigned char ucId = 0x80; /* C: */
    
    return write_data(fp, 0x24, &ucId, 1);
} /* write_partition_physical_disk_drive_id_fat16 */

int write_partition_physical_disk_drive_id_fat32(FILE *fp)
{
   unsigned char ucId = 0x80; /* C: */
   
   return write_data(fp, 0x40, &ucId, 1);
} /* write_partition_physical_disk_drive_id_fat32 */

int write_partition_physical_disk_drive_id_exfat(FILE *fp)
{
   unsigned char ucId = 0x80; /* C: */
   
   return write_data(fp, 0x6f, &ucId, 1);
} /* write_partition_physical_disk_drive_id_exfat */
