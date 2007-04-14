
#include <stdlib.h>
#include <stdio.h>

#include "asf.h"
#include "asfint.h"
#include "fileio.h"
#include "byteio.h"
#include "header.h"
#include "parse.h"
#include "data.h"

asf_file_t *
asf_open_file(const char *filename)
{
	asf_file_t *file;
	asf_stream_t stream;
	FILE *fstream;

	fstream = fopen(filename, "r");
	if (!fstream)
		return NULL;

	stream.read = asf_fileio_read_cb;
	stream.write = asf_fileio_write_cb;
	stream.seek = asf_fileio_seek_cb;
	stream.opaque = fstream;

	file = asf_open_cb(&stream);
	if (!file)
		return NULL;

	file->filename = filename;

	return file;
}

asf_file_t *
asf_open_cb(asf_stream_t *stream)
{
	asf_file_t *file;
	int i;

	file = calloc(1, sizeof(asf_file_t));
	if (!file)
		return NULL;

	file->filename = NULL;
	file->stream.read = stream->read;
	file->stream.write = stream->write;
	file->stream.seek = stream->seek;
	file->stream.opaque = stream->opaque;

	file->header = NULL;
	file->data = NULL;
	file->index = NULL;

	for (i = 0; i < ASF_MAX_STREAMS; i++)
		file->streams[i].type = ASF_STREAM_TYPE_NONE;

	return file;
}

int
asf_init(asf_file_t *file)
{
	int tmp;

	if (!file)
		return ASF_ERROR_INTERNAL;

	tmp = asf_parse_header(file);
	if (tmp < 0) {
#ifdef DEBUG
		printf("error parsing header: %d\n", tmp);
#endif
		return tmp;
	}
	file->position += tmp;
	file->data_position = file->position;

	tmp = asf_parse_data(file);
	if (tmp < 0) {
#ifdef DEBUG
		printf("error parsing data object: %d\n", tmp);
#endif
		return tmp;
	}
	file->position += tmp;

	if (file->flags & ASF_FLAG_SEEKABLE && file->stream.seek) {
		int64_t seek_position;

		file->index_position = file->data_position +
		                       file->data->size;

		while (!file->index && file->index_position < file->file_size) {
			seek_position = file->stream.seek(file->stream.opaque,
							  file->index_position);
			if (seek_position != file->index_position) {
				/* Error in seeking */
				break;
			}

			tmp = asf_parse_index(file);
			if (tmp < 0) {
#ifdef DEBUG
				printf("Error finding index object! %d\n", tmp);
#endif
				break;
			}

			/* The object read was something else than index */
			if (!file->index)
				file->index_position += tmp;
		}

		if (!file->index) {
#ifdef DEBUG
			printf("Couldn't find an index object\n");
#endif
			file->index_position = 0;
		}

		seek_position = file->stream.seek(file->stream.opaque,
		                                  file->data->packets_position);
		if (seek_position != file->data->packets_position) {
			/* Couldn't seek back to packets position, this is fatal! */
			return ASF_ERROR_SEEK;
		}
	}

	for (tmp = 0; tmp < ASF_MAX_STREAMS; tmp++) {
		if (file->streams[tmp].type != ASF_STREAM_TYPE_NONE) {
#ifdef DEBUG
			printf("stream %d of type %d found!\n", tmp, file->streams[tmp].type);
#endif
		}
	}

	return 0;
}

void
asf_init_packet(asf_packet_t *packet)
{
	asf_data_init_packet((asf_packet_int_t *) packet);
}

int
asf_get_packet(asf_file_t *file, asf_packet_t *packet)
{
	int tmp;

	if (!file || !packet)
		return ASF_ERROR_INTERNAL;

	if (file->packet >= file->data_packets_count) {
		return 0;
	}

	tmp = asf_data_get_packet((asf_packet_int_t *) packet, file);
	if (tmp < 0) {
		return tmp;
	}

	file->position += tmp;
	file->packet++;

	return tmp;
}

void
asf_free_packet(asf_packet_t *packet)
{
	asf_data_free_packet((asf_packet_int_t *) packet);
}

int64_t
asf_seek_to_msec(asf_file_t *file, int64_t msec)
{
	uint64_t packet;
	uint64_t new_position;
	uint64_t new_msec;
	int64_t seek_position;

	if (!file)
		return ASF_ERROR_INTERNAL;

	if (!(file->flags & ASF_FLAG_SEEKABLE) || !file->stream.seek) {
		return ASF_ERROR_SEEKABLE;
	}

	/* Index structure is missing, check if we can still seek */
	if (file->index == NULL) {
		int i, audiocount;

		audiocount = 0;
		for (i=0; i<ASF_MAX_STREAMS; i++) {
			/* Non-audio files are not seekable without index */
			if (file->streams[i].type != ASF_STREAM_TYPE_AUDIO)
				return ASF_ERROR_SEEKABLE;
			else
				audiocount++;
		}

		/* Audio files with more than one audio track are not seekable
		 * without index */
		if (audiocount != 1)
			return ASF_ERROR_SEEKABLE;
	}

	if (!file->play_duration || msec > (file->play_duration / 10000)) {
		return ASF_ERROR_SEEK;
	}

	if (file->index) {
		uint32_t index_entry;

		/* Fetch current packet from index entry structure */
		index_entry = msec * 10000 / file->index->entry_time_interval;
		if (index_entry >= file->index->entry_count) {
			return ASF_ERROR_SEEK;
		}
		packet = file->index->entries[index_entry].packet_index;
	} else {
		/* convert msec into bytes per second and divide with packet_size */
		packet = msec * file->max_bitrate / 8000 / file->packet_size;
	}

	/* calculate new position to be in the beginning of the current frame */
	new_position = file->data->packets_position + packet * file->packet_size;
	new_msec = packet * file->packet_size * 8000 / file->max_bitrate;

	seek_position = file->stream.seek(file->stream.opaque,
	                                  new_position);

	if (seek_position < 0 || seek_position != new_position) {
		return ASF_ERROR_SEEK;
	}

	/* update current file position information */
	file->position = new_position;
	file->packet = packet;

	return new_msec;
}

void
asf_close(asf_file_t *file)
{
	if (file) {
		asf_header_destroy(file->header);
		free(file->data);
		if (file->index)
			free(file->index->entries);
		free(file->index);

		if (file->filename)
			fclose(file->stream.opaque);

		free(file);
	}
}


asf_metadata_t *
asf_get_metadata(asf_file_t *file)
{
	if (!file)
		return NULL;

	return asf_header_get_metadata(file->header);
}

void
asf_free_metadata(asf_metadata_t *metadata)
{
	asf_header_metadata_destroy(metadata);
}

asf_stream_type_t
*asf_get_stream_type(asf_file_t *file, uint8_t track) {
	if (!file || track >= ASF_MAX_STREAMS)
		return NULL;

	return &file->streams[track];
}

