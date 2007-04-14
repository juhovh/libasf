
#ifndef FILEIO_H
#define FILEIO_H

#include "asf.h"

int asf_fileio_read_cb(void *stream, void *buffer, int size);
int asf_fileio_write_cb(void *stream, void *buffer, int size);
int64_t asf_fileio_seek_cb(void *stream, int64_t offset);

#endif
