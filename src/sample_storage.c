#include "sample_storage.h"

// we start from SECTOR 6 (RM0090 page 75)
#define INTERNAL_DATA_OFFSET  0x08040000

typedef struct {
    char chunkId[4];
    uint32_t chunkSize;
    char format[4]; // "WAVE"

    char subchunk1Id[4]; // "fmt"
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchunk2Id[4]; // "data"
    uint32_t subchunk2Size;
} WavFile;


volatile FileData data;


void sample_storage_init() {
    WavFile *wav = (WavFile *) INTERNAL_DATA_OFFSET;
    data.num_channels = wav->numChannels;
    data.sample_rate = wav->sampleRate;
    data.bits_per_sample = wav->bitsPerSample;
    data.start_pointer = (uint16_t*) (INTERNAL_DATA_OFFSET + sizeof(WavFile));
    data.file_size = wav->subchunk2Size / 2;

}

FileData sample_storage_get() {
    return data;
}