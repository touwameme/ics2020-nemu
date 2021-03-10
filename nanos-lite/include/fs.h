#ifndef __FS_H__
#define __FS_H__

#include <common.h>
size_t fs_read(int fd, void* buf,size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
void init_fs(); 
#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

#endif