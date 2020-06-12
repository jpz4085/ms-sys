#ifndef FAT32_H
#define FAT32_H

#include <stdio.h>

/* returns TRUE if the file has a FAT32 file system, otherwise FALSE.
   The file position will change when this function is called! */
int is_fat_32_fs(FILE *fp);

/* returns TRUE if the file has a FAT32 DOS/NT boot record, otherwise FALSE.
   The file position will change when this function is called! */
int is_fat_32_br(FILE *fp);

/* returns TRUE if the file has an exact match of the FAT32 DOS boot record
   this program would create, otherwise FALSE.
   The file position will change when this function is called! */
int entire_fat_32_br_matches(FILE *fp);

/* Writes a FAT32 DOS boot record to a file, returns TRUE on success, otherwise
   FALSE */
int write_fat_32_br(FILE *fp, int bKeepLabel);

/* returns TRUE if the file has an exact match of the FAT32 boot record this
   program would create for FreeDOS, otherwise FALSE.
   The file position will change when this function is called! */
int entire_fat_32_fd_br_matches(FILE *fp);

/* Writes a FAT32 FreeDOS boot record to a file, returns TRUE on success,
   otherwise FALSE */
int write_fat_32_fd_br(FILE *fp, int bKeepLabel);

/* returns TRUE if the file has an exact match of the FAT32 boot record this
   program would create for NT5.0, otherwise FALSE.
   The file position will change when this function is called! */
int entire_fat_32_nt5_br_matches(FILE *fp);

/* Writes a FAT32 NT5.0 boot record to a file, returns TRUE on success, otherwise
   FALSE */
int write_fat_32_nt5_br(FILE *fp, int bKeepLabel);

/* returns TRUE if the file has an exact match of the FAT32 boot record this
   program would create for NT6.0, otherwise FALSE.
   The file position will change when this function is called! */
int entire_fat_32_nt6_br_matches(FILE *fp);

/* Writes a FAT32 NT6.0 boot record to a file, returns TRUE on success, otherwise
   FALSE */
int write_fat_32_nt6_br(FILE *fp, int bKeepLabel);

/* returns TRUE if the file has an exact match of the FAT32 boot record this
   program would create for NT, otherwise FALSE.
   The file position will change when this function is called! */
int entire_fat_32_pe_br_matches(FILE *fp);

/* Writes a FAT32 NT boot record to a file, returns TRUE on success, otherwise
   FALSE */
int write_fat_32_pe_br(FILE *fp, int bKeepLabel);

/* returns TRUE if the file has an exact match of the FAT32 boot record this
   program would create for ReactOS, otherwise FALSE.
   The file position will change when this function is called! */
int entire_fat_32_ros_br_matches(FILE *fp);

/* Writes a FAT32 ReactOS boot record to a file, returns TRUE on success, otherwise
   FALSE */
int write_fat_32_ros_br(FILE *fp, int bKeepLabel);

/* returns TRUE if the file has an exact match of the FAT32 boot record this
   program would create for KolibriOS, otherwise FALSE.
   The file position will change when this function is called! */
int entire_fat_32_kos_br_matches(FILE *fp);

/* Writes a FAT32 KolibriOS boot record to a file, returns TRUE on success, otherwise
   FALSE */
int write_fat_32_kos_br(FILE *fp, int bKeepLabel);

#endif
