
#ifndef HEADER_H
#define HEADER_H

#include "asf.h"

int asf_parse_header_validate(asf_file_t *file, asf_object_header_t *header);
void asf_header_destroy(asf_object_header_t *header);
asf_metadata_t *asf_header_get_metadata(asf_object_header_t *header);
void asf_header_metadata_destroy(asf_metadata_t *metadata);

#endif
