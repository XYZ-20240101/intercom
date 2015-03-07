#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <opus/opus.h>
#include <stdio.h>

/*The frame size is hardcoded for this sample code but it doesn't have to be*/
#define FRAME_SIZE 2880
#define SAMPLE_RATE 48000
#define APPLICATION OPUS_APPLICATION_AUDIO

#define MAX_FRAME_SIZE 6*9600
#define MAX_PACKET_SIZE (3*1276)

int main(int argc, char **argv)
{
   opus_int16 in[FRAME_SIZE];
   opus_int16 out[MAX_FRAME_SIZE];
   unsigned char cbits[MAX_PACKET_SIZE];
   int nbBytes;
   /*Holds the state of the encoder and decoder */
   OpusDecoder *decoder;
   int err;

   if (argc != 1)
   {
      fprintf(stderr, "usage: %s\n", argv[0]);
      return EXIT_FAILURE;
   }

   /* Create a new decoder state. */
   decoder = opus_decoder_create(SAMPLE_RATE, 1, &err);
   if (err<0)
   {
      fprintf(stderr, "failed to create decoder: %s\n", opus_strerror(err));
      return EXIT_FAILURE;
   }

   while (1)
   {
      int i;
      unsigned char opus_bytes[MAX_FRAME_SIZE*2];
      unsigned char pcm_bytes[MAX_FRAME_SIZE*2];
      int frame_size;

      fread(&nbBytes, sizeof(nbBytes), 1, stdin);

      fprintf(stderr, "reading %d\n", nbBytes);

      /* Read a 16 bits/sample audio frame (big endian). */
      fread(opus_bytes, sizeof(char), nbBytes, stdin);
      if (feof(stdin))
         break;

      fprintf(stderr, "[%d, %d, ... %d, %d]\n", opus_bytes[0], opus_bytes[1], opus_bytes[nbBytes-2], opus_bytes[nbBytes-1]);

      /* Decode the data. In this example, frame_size will be constant because
         the encoder is using a constant frame size. However, that may not
         be the case for all encoders, so the decoder must always check
         the frame size returned. */
      frame_size = opus_decode(decoder, opus_bytes, nbBytes, out, MAX_FRAME_SIZE, 0);
      if (frame_size<0)
      {
         fprintf(stderr, "decoder failed: %s\n", opus_strerror(err));
         return EXIT_FAILURE;
      }

      for(i=0; i < frame_size; i++) {
         pcm_bytes[2*i+1] = out[i] & 0xFF;
         pcm_bytes[2*i] = (out[i]>>8)&0xFF;
      }

      /* Write the decoded audio to file. */
      fwrite(pcm_bytes, sizeof(short), frame_size, stdout);
      fprintf(stderr, "frame size: %d\n", frame_size);
   }
   opus_decoder_destroy(decoder);
   return EXIT_SUCCESS;
}
