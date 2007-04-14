
#ifndef ASF_H
#define ASF_H


/* used int types for different platforms */
#if defined (_WIN32) && !defined (__MINGW_H)
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;

typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif


struct asf_stream_s {
	int (*read)(void *buffer, int size, void *opaque);
	int (*write)(void *buffer, int size, void *opaque);
	int64_t (*seek)(void *opaque, int64_t offset, int whence);
	void *opaque;
};
typedef struct asf_stream_s asf_stream_t;

struct asf_payload_s {
	uint8_t stream_number;
	uint32_t media_object_number;
	uint32_t media_object_offset;

	uint32_t replicated_length;
	uint8_t *replicated_data;

	uint32_t datalen;
	uint8_t *data;

	uint32_t pts;
};
typedef struct asf_payload_s asf_payload_t;

struct asf_metadata_entry_s {
	char *key;
	char *value;
};
typedef struct asf_metadata_entry_s asf_metadata_entry_t;

struct asf_metadata_s {
	char *title;
	char *artist;
	char *copyright;
	char *description;
	char *rating;
	uint16_t extended_count;
	asf_metadata_entry_t *extended;
};
typedef struct asf_metadata_s asf_metadata_t;

struct asf_packet_s {
	uint8_t ec_length;
	uint8_t *ec_data;

	uint32_t length;
	uint32_t padding_length;
	uint32_t send_time;
	uint16_t duration;

	uint16_t payload_count;
	asf_payload_t *payloads;

	uint32_t datalen;
	uint8_t *payload_data;
};
typedef struct asf_packet_s asf_packet_t;

struct asf_stream_type_s {
	uint8_t type;
	uint32_t datalen;
	uint8_t *data;
};
typedef struct asf_stream_type_s asf_stream_type_t;

typedef struct asf_file_s asf_file_t;

enum {
	ASF_SEEK_SET,
	ASF_SEEK_CUR,
	ASF_SEEK_END
};

#define ASF_STREAM_TYPE_NONE     0x00
#define ASF_STREAM_TYPE_AUDIO    0x01
#define ASF_STREAM_TYPE_VIDEO    0x02
#define ASF_STREAM_TYPE_COMMAND  0x04
#define ASF_STREAM_TYPE_UNKNOWN  0xff

enum {
	ASF_ERROR_INTERNAL       = -1,  /* incorrect input to API calls */
	ASF_ERROR_OUTOFMEM       = -2,  /* some malloc inside program failed */
	ASF_ERROR_EOF            = -3,  /* unexpected end of file */
	ASF_ERROR_IO             = -4,  /* error reading or writing to file */
	ASF_ERROR_INVALID_LENGTH = -5,  /* length value conflict in input data */
	ASF_ERROR_INVALID_VALUE  = -6,  /* other value conflict in input data */
	ASF_ERROR_INVALID_OBJECT = -7,  /* ASF object missing or in wrong place */
	ASF_ERROR_OBJECT_SIZE    = -8,  /* invalid ASF object size (too small) */
	ASF_ERROR_SEEKABLE       = -9,  /* file not seekable */
	ASF_ERROR_SEEK           = -10  /* file is seekable but seeking failed */
};

asf_file_t *asf_open_file(const char *filename);
asf_file_t *asf_open_cb(asf_stream_t *stream);
void asf_close(asf_file_t *file);

int asf_init(asf_file_t *file);
void asf_init_packet(asf_packet_t *packet);
int asf_get_packet(asf_file_t *file, asf_packet_t *packet);
int64_t asf_seek_to_msec(asf_file_t *file, int64_t msec);
void asf_free_packet(asf_packet_t *packet);

asf_metadata_t *asf_get_metadata(asf_file_t *file);
void asf_free_metadata(asf_metadata_t *metadata);

asf_stream_type_t *asf_get_stream_type(asf_file_t *file, uint8_t track);

uint64_t asf_file_get_file_size(asf_file_t *file);
uint64_t asf_file_get_creation_date(asf_file_t *file);
uint64_t asf_file_get_data_packets(asf_file_t *file);
uint64_t asf_file_get_duration(asf_file_t *file);
uint32_t asf_file_get_max_bitrate(asf_file_t *file);

#endif
