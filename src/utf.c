
#include <stdlib.h>

#include "asf.h"
#include "byteio.h"

char *
asf_utf8_from_utf16le(uint8_t *buf, uint16_t buflen)
{
	uint32_t length, pos;
	char *ret;
	int i;

	length = 0;
	for (i=0; i<buflen/2; i++) {
		uint16_t wchar1, wchar2;

		wchar1 = asf_byteio_getWLE(&buf[i*2]);
		if (wchar1 >= 0xD800 && wchar1 < 0xDB00) {
			i++;

			if (i*2 >= buflen) {
				/* unexpected end of buffer */
				return NULL;
			}
			wchar2 = asf_byteio_getWLE(&buf[i*2]);
			if (wchar2 < 0xDB00 || wchar2 > 0xDFFF) {
				/* invalid surrogate pair */
				return NULL;
			}
			length += 4;
		} else if (wchar1 > 0x07FF) {
			length += 3;
		} else if (wchar1 > 0x7F) {
			length += 2;
		} else {
			length++;
		}
	}

	ret = malloc(length + 1);
	if (!ret) {
		return NULL;
	}

	pos = 0;
	for (i=0; i<buflen/2; i++) {
		uint16_t wchar1, wchar2;
		uint32_t codepoint;

		wchar1 = asf_byteio_getWLE(&buf[i*2]);
		if (wchar1 >= 0xD800 && wchar1 < 0xDB00) {
			i++;
			wchar2 = asf_byteio_getWLE(&buf[i*2]);
			codepoint = 0x10000;
			codepoint += ((wchar1 & 0x03FF) << 10);
			codepoint |=  (wchar2 & 0x03FF);
		} else {
			codepoint = wchar1;
		}

		if (codepoint > 0xFFFF) {
			ret[pos++] = 0xF0 | ((codepoint >> 18) & 0x07);
			ret[pos++] = 0x80 | ((codepoint >> 12) & 0x3F);
			ret[pos++] = 0x80 | ((codepoint >> 6)  & 0x3F);
			ret[pos++] = 0x80 |  (codepoint & 0x3F);
		} else if (codepoint > 0x07FF) {
			ret[pos++] = 0xE0 |  (codepoint >> 12);
			ret[pos++] = 0x80 | ((codepoint >> 6) & 0x3F);
			ret[pos++] = 0x80 |  (codepoint & 0x3F);
		} else if (codepoint > 0x7F) {
			ret[pos++] = 0xB0 |  (codepoint >> 6);
			ret[pos++] = 0x80 |  (codepoint & 0x3F);
		} else {
			ret[pos++] = codepoint;
		}
	}

	ret[length] = '\0';
	return ret;
}
