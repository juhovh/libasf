/*  libasf - An Advanced Systems Format media file parser
 *  Copyright (C) 2006-2010 Juho Vähä-Herttua
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <string.h>

#include "asfint.h"
#include "byteio.h"

int
asf_byteio_read(asf_iostream_t *iostream, uint8_t *data, int size)
{
	int read = 0, tmp;

	if (!iostream->read) {
		return ASF_ERROR_INTERNAL;
	}

	while ((tmp = iostream->read(iostream->opaque, data+read, size-read)) > 0) {
		read += tmp;
		if (read == size) {
			return read;
		}
	}

	/* FIXME: should return tmp if any bytes were read, but the
	          rest of the code needs to be fixed before that */

	return (tmp == 0) ? ASF_ERROR_EOF : ASF_ERROR_IO;
}

