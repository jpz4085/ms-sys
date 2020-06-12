#ifndef exfat_h
#define exfat_h

#ifdef __linux__
#include <stdint.h>
#endif

#ifdef __FreeBSD__
#include <sys/types.h>
#endif

#include <stdlib.h>

/* returns TRUE if the file has a EXFAT file system, otherwise FALSE.
 The file position will change when this function is called! */
int is_exfat_fs(FILE *fp);

/* returns TRUE if the file has a EXFAT boot record, otherwise FALSE.
 The file position will change when this function is called! */
int is_exfat_br(FILE *fp);

/* returns TRUE if the file has an exact match of the EXFAT boot record this
 program would create for NT6.0, otherwise FALSE.
 The file position will change when this function is called! */
int entire_exfat_nt6_br_matches(FILE *fp);

/* Writes an EXFAT NT6.0 boot record to a file, returns TRUE on success,
 otherwise FALSE */
int write_exfat_nt6_br(FILE *fp);

/* Calculate the new EXFAT Volume Boot Record checksum and fill the
 twelfth sector of the main and backup VBR regions with this value. */
int write_exfat_br_checksum(FILE *fp);

/* Function to calculate the new VBR checksum value required above. */
uint32_t exfat_boot_checksum(const void* sector, size_t size);

/* Function to get logical sector size of media, returns reported
 value or 512 if unable, then is used to determine VBR byte offsets. */
uint64_t get_sector_size(FILE *fp);

#endif /* exfat_h */
