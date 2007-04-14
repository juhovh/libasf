
#include <stdio.h>

#include "asf.h"

int
asf_fileio_read_cb(void *stream, void *buffer, int size)
{
	int ret;

	ret = fread(buffer, 1, size, stream);
	if (!ret && !feof(stream))
		ret = -1;

	return ret;
}

int
asf_fileio_write_cb(void *stream, void *buffer, int size)
{
	int ret;

	ret = fwrite(buffer, 1, size, stream);
	if (!ret && !feof(stream))
		ret = -1;

	return ret;
}

int64_t
asf_fileio_seek_cb(void *stream, int64_t offset)
{
	int ret;

	ret = fseek(stream, offset, SEEK_SET);
	if (ret < 0)
		return -1;

	return offset;
}
