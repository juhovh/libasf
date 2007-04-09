
#ifndef FILEIO_H
#define FILEIO_H

#include "asf.h"

int asf_fileio_read_cb(void *buffer, int size, void *stream);
int asf_fileio_write_cb(void *buffer, int size, void *stream);
int64_t asf_fileio_seek_cb(void *stream, int64_t offset, int whence);

#endif
