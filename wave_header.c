#include "wave_header.h"
#include <unistd.h>
#include <stdint.h>

struct hstr{
  char rifftag[4];
  uint32_t chunksize;
  char wavetag[4];
  char fmttag[4];
  uint32_t subchunk1size;
  uint16_t audioformat;
  uint16_t numchannels;
  uint32_t samplerate;
  uint32_t byterate;
  uint16_t blockalign;
  uint16_t bitspersample;
  char datatag[4];
  uint32_t subchunk2size;

};

void write_wave_header(int fd, int nsamples)
{
  static struct hstr header= {
    "RIFF",
    sizeof(struct hstr),   
    "WAVE",
    "fmt ",
    16,    // subchunk1size
    1,     // audio format
    1,     // num channels
    44100, // sample rate
    88200, // byte rate
    2,     // block alignment
    16,    // bits per sample
    "data",
    0      // sub-chunk 2 size is set at run-time
  };
  header.subchunk2size = nsamples * header.numchannels * sizeof(int16_t);
  write(fd,&header,sizeof(header));
}
 
