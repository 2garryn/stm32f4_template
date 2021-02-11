#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>


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

int main(int argc, char *argv[]){
    volatile int16_t v = -123;
    volatile uint32_t temp = 0;
    int16_t *hah = (int16_t*) &temp;
    *hah = v;
    printf("Conv res: %" PRIu32 "\n", temp);

    if(argc < 2){
        printf("File path not mentioned\n");
        exit(0);
    }

    const char *filepath = argv[1];
    int fd = open(filepath, O_RDONLY);
    if(fd < 0){
        printf("\n\"%s \" could not open\n", filepath);
        exit(1);
    }

    struct stat statbuf;
    int err = fstat(fd, &statbuf);
    if(err < 0){
        printf("\n\"%s \" could not open\n", filepath);
        exit(2);
    }
    char *ptr = mmap(NULL, sizeof(WavFile), PROT_READ, MAP_PRIVATE, fd, 0);
    //char *ptr = mmap(wav, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(ptr == MAP_FAILED){
        printf("Mapping Failed\n");
        return 1;
    }
    close(fd);

    WavFile *wav = (WavFile *) ptr;

    printf("subchunk2Id: %.4s\n", wav->chunkId);
    printf("format: %.4s\n", wav->format);
    printf("subchunk1Size: %" PRIu32 "\n", wav->subchunk1Size);
    printf("Channels: %" PRIu16 "\n", wav->numChannels);
    printf("SampleRate: %" PRIu32 "\n", wav->sampleRate);
    printf("BitsPerSample: %" PRIu16 "\n", wav->bitsPerSample);
    printf("subchunk2Id: %.4s\n", wav->subchunk2Id);
    printf("Subchunk2Size: %" PRIu32 "\n", wav->subchunk2Size);
    int16_t* start_data = (uint16_t*) (ptr + sizeof(WavFile));

    int s = wav->subchunk2Size / 2;
    for(int i = 0; i < s; i++) {
        if (i % 2 == 0) {
            printf(" %d--%" PRIu16, i, *(start_data + i));
        }
    } 
    
}