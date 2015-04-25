/* Stringed Instrument Heroine (TM)
   Author: Larry Pyeatt
   Date: 11-1-2012
  (C) All rights reserved

   This program uses to Karplus-Strong algorithm to simulate a
   stringed instrument.  It sounds similar to a harp.  The sample
   rate for the audio samples can be adjusted using the SAMPLE_RATE
   constant.  The audio is first written to a file, then the file is
   converted to Microsoft (TM) Wave (TM) format.

   The input file is made of triplets of values.  The first value is
   an integer specifying the time at which a note is to be played, in
   milliseconds since the start of the song.  The second value is the
   MIDI number of the note to be played, and the third number is the
   relative volume of the note.  The volume is given as a floating
   point number between 0.0 and 1.0 inclusive.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include "wave_header.h"
#include <unistd.h>

#define SAMPLE_RATE 44100
#define NUM_NOTES (10*12)
#define BASE_SIZE (44100/16)
#define SMPLS_PER_MS (SAMPLE_RATE/100)
#define MAX_SECONDS 600

#define INT16_T_MAX (0x7FFF)


void check_write(int w, int n)
{
  if(w!=n)
    {
      perror("Error writing to wave file\n");
      exit(1);
    }
}


/* if note is a midi number, then the frequency is:
   hz = pow(2,(note-69.0)/12.0) * 440;
   and the array size needed is SAMPLE_RATE/Hz        
*/
int array_size(int note)
{
  double hz;
  hz = pow(2,(note-69.0)/12.0) * 440;
  return (int)(SAMPLE_RATE/hz + 0.5);
}

/* Plucking a string is simulated by filling the array with
   random numbers between -0.5 and +0.5 
*/
void pluck(double buffer[], int size, double volume)
{
  int i;
  for(i=0;i<size;i++)
    buffer[i] = volume * ((double)rand()/RAND_MAX - 0.5);
}

/* The update function treats the array as a queue.  The
   first item is taken from the queue, then averaged with
   the next item.  The result of the average is added to
   the end of the queue and returned by the function as the
   next audio sample.
*/
double update(double buffer[], int size, int *position)
{
  int nextpos;
  double value;
  nextpos = (*position+1)%size;
  buffer[*position] = value = 0.996*(buffer[*position]+buffer[nextpos])*0.5;
  *position = nextpos;
  return value;
}

void scream_and_die(char progname[])
{
  fprintf(stderr,"Usage: %s input_file <tempo>\n",progname);;
  fprintf(stderr,"The input file is in Dr. Pyeatt's .notes format.\n");
  fprintf(stderr,"Tempo can be any number between 0.25 and 4.0\n");
  exit(1);
}

struct filedat{
  int32_t time;
  int16_t note;
  int16_t vol;
};


/*  
  main opens the input and temporary files, generates the audio data
  as a stream of numbers in ASCII format, then calls convert_to_wave()
  to convert the temporary file into the output file in .wav format.
*/    
int main(int argc, char **argv)
{
  static double notes[NUM_NOTES][BASE_SIZE] = {0};
  static int position[NUM_NOTES] = {0};
  double temp;
  int i,j;
  FILE *input;
  FILE *output = stdout;
  int current_time = 0;
  int current_sample = 0;
  double next_volume;
  double tempo = 1.0;
  char *endptr;
  int num_samples;
  static uint16_t data[2048];
  int16_t sample;
  double MAX = 1.0;
  struct filedat next_note;
  
  if(argc < 2)
    scream_and_die(argv[0]);
  
  if(argc == 3)
    {
      // try to interpret argv[2] as a float
      tempo = strtof(argv[2],&endptr);
      if(endptr==argv[2] || tempo<0.25 || tempo > 4.0)
	{
	  fprintf(stderr,"Illegal tempo value.\n");
	  scream_and_die(argv[0]);
	}
      tempo = 1.0/tempo;
    }

  if((input = fopen(argv[1],"r"))==NULL)
    {
      perror("Unable to open input file");
      scream_and_die(argv[0]);
    }

  /* find time of for end of song */
  while((fread(&next_note,sizeof(next_note),1,input)==1)&&
	(next_note.note != 1));
  fseek(input,0,SEEK_SET);
  
  num_samples = tempo * next_note.time * SAMPLE_RATE / 100;

  write_wave_header(STDOUT_FILENO,num_samples);

  srand(time(NULL));
  
  do{
    // read the next note
    if(fread(&next_note,sizeof(next_note),1,input)==1)
      {
	next_note.time = (int)(next_note.time * tempo);
	// generate sound, one ms at a time, until we need to start
	// the next note
	while(current_time < next_note.time)
	  {
	    // generate another millsecond of sound 
	    for(i=current_sample; i < current_sample + SMPLS_PER_MS; i++)
	      {
		temp = 0.0;
		// update each active string and add its output to the sum
		for(j = 0; j< NUM_NOTES; j++)
		  temp += update(notes[j],array_size(j),&position[j]);
		// write a sample to the wave file 
		sample = (int16_t)((temp/MAX) * (INT16_T_MAX-1));
		fwrite(&sample,sizeof(int16_t),1,output);
	      }
	    current_sample += SMPLS_PER_MS;
	    current_time++;
	  }
	if(next_note.note >= 0) // pluck the next note
	  pluck(notes[next_note.note],array_size(next_note.note),
		next_note.vol/32767.0);
      }
  }while(!feof(input) && (next_note.note > 0));  
  fclose(input);
  fclose(output);
  return 0;
}
