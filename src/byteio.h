
#ifndef BYTEIO_H
#define BYTEIO_H

#include "asf.h"
#include "guid.h"

uint16_t asf_byteio_getWLE(uint8_t *data);
uint32_t asf_byteio_getDWLE(uint8_t *data);
uint64_t asf_byteio_getQWLE(uint8_t *data);
void asf_byteio_getGUID(guid_t *guid, uint8_t *data);
void asf_byteio_get_string(uint16_t *string, uint16_t strlen, uint8_t *data);

int asf_byteio_readbyte(asf_stream_t *stream);
int asf_byteio_read(uint8_t *data, int size, asf_stream_t *stream);

#endif
