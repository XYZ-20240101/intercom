#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <opus/opus.h>
#include <stdio.h>

/*The frame size is hardcoded for this sample code but it doesn't have to be*/
#define FRAME_SIZE 2880
#define SAMPLE_RATE 48000
#define APPLICATION OPUS_APPLICATION_AUDIO

#define MAX_FRAME_SIZE 6*960
#define MAX_PACKET_SIZE (3*1276)

int main(int argc, char **argv)
{
   opus_int16 in[FRAME_SIZE];
   opus_int16 out[MAX_FRAME_SIZE];
   unsigned char cbits[MAX_PACKET_SIZE];
   int nbBytes, bitrate;
   /*Holds the state of the encoder and decoder */
   OpusEncoder *encoder;
   int err;

   if (argc != 2)
   {
      fprintf(stderr, "usage: trivial_example bitrate\n");
      return EXIT_FAILURE;
   }

   bitrate = atoi(argv[1]);

   /*Create a new encoder state */
   encoder = opus_encoder_create(SAMPLE_RATE, 1, APPLICATION, &err);
   if (err<0)
   {
      fprintf(stderr, "failed to create an encoder: %s\n", opus_strerror(err));
      return EXIT_FAILURE;
   }
   /* Set the desired bit-rate. You can also set other parameters if needed.
      The Opus library is designed to have good defaults, so only set
      parameters you know you need. Doing otherwise is likely to result
      in worse quality, but better. */
   err = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(bitrate));
   if (err<0)
   {
      fprintf(stderr, "failed to set bitrate: %s\n", opus_strerror(err));
      return EXIT_FAILURE;
   }

   while (1)
   {
      int i;
      unsigned char pcm_bytes[MAX_FRAME_SIZE*2];
      int frame_size;

      /* Read a 16 bits/sample audio frame (big endian). */
      fread(pcm_bytes, sizeof(short), FRAME_SIZE, stdin);
      if (feof(stdin))
         break;

      for (i=0; i < FRAME_SIZE; i++)
         in[i] = pcm_bytes[2*i]<<8|pcm_bytes[2*i+1];

      /* Encode the frame. */
      nbBytes = opus_encode(encoder, in, FRAME_SIZE, cbits, MAX_PACKET_SIZE);
      if (nbBytes<0)
      {
         fprintf(stderr, "encode failed: %s\n", opus_strerror(nbBytes));
         return EXIT_FAILURE;
      }
      fprintf(stderr, "nbBytes: %d\n", nbBytes);
      fprintf(stderr, "[%d, %d, ... %d, %d]\n", cbits[0], cbits[1], cbits[nbBytes-2], cbits[nbBytes-1]);

      fwrite(&nbBytes, sizeof(nbBytes), 1, stdout);

      /* Write the decoded audio to file. */
      fwrite(cbits, sizeof(char), nbBytes, stdout);
   }
   /*Destroy the encoder state*/
   opus_encoder_destroy(encoder);
   return EXIT_SUCCESS;
}
