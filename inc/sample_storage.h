
#include "helpers.h"


typedef struct  {
    uint16_t* start_pointer;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint16_t bits_per_sample;
    uint32_t file_size;
} FileData;

void sample_storage_init();
FileData sample_storage_get();