
#ifndef DATA_H
#define DATA_H

#include "asf.h"

void asf_data_init_packet(asf_packet_t *packet);
int asf_data_get_packet(asf_packet_t *packet, asf_file_t *file);
void asf_data_free_packet(asf_packet_t *packet);

#endif
