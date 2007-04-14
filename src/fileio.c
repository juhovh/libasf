
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
asf_fileio_seek_cb(void *stream, int64_t offset, int whence)
{
	int fwhence;
	int ret;

	switch (whence) {
	case ASF_SEEK_SET:
		fwhence = SEEK_SET;
		break;
	case ASF_SEEK_CUR:
		fwhence = SEEK_CUR;
		break;
	case ASF_SEEK_END:
		fwhence = SEEK_END;
		break;
	default:
		return ASF_ERROR_INTERNAL;
	}

	ret = fseek(stream, offset, fwhence);
	if (ret < 0)
		return -1;

	return offset;
}
