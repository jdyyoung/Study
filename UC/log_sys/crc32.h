#ifndef _CRC32_H_
#define _CRC32_H_

#include "typedef.h"

extern u32 crc32(u32 crc, const u8 *buf, int len);
extern int crc32_file(char* filename, u32* crc_result, size_t max_size);
extern int crc32_getcrc(char* filename, u32* crc_result);
extern int crc32_setcrc(char* filename, u32 crc);

#endif
