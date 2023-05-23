/* Generate N bytes of random output.  */

/* When generating output this program uses the x86-64 RDRAND
   instruction if available to generate random numbers, falling back
   on /dev/random and stdio otherwise.

   This program is not portable.  Compile it with gcc -mrdrnd for a
   x86-64 machine.

   Copyright 2015, 2017, 2020 Paul Eggert

   This program is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "output.h"
#include "rand64-sw.h"
#include "rand64-hw.h"
#include "options.h"

/* Main program, which outputs N bytes of random data.  */
int
main (int argc, char **argv)
{
  /* Check arguments.  */
  bool valid = false;
  long long nbytes;
  int input=0;
  int output=0;
  unsigned int blksz=0;
  char* input_file;
 
  valid = readopt(argc, argv, &input, &output, &nbytes, &blksz, &input_file);
  /*printf ("input = %d, output = %d, nbytes=%lld, blksz=%u, input_file=%s", 
    input, output, nbytes, blksz, input_file);*/
  if (!valid)
    {
      fprintf (stderr, "%s: usage: %s [OPTION] NBYTES\n", argv[0], argv[0]);
      return 1;
    }

  /* If there's no work to do, don't worry about which library to use.  */
  if (nbytes == 0)
    return 0;

  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */
  void (*initialize) (void);
  unsigned long long (*rand64) (void);
  void (*finalize) (void);
 /* if (input == NONE){
      initialize = software_rand64_init;
      rand64 = software_rand64;
      finalize = software_rand64_fini;

  }else */
  if (input == RDRAND || input == NONE) {
    if ( rdrand_supported ())
    {
   /*   printf ("\nRDRAND\n");*/
      initialize = hardware_rand64_init;
      rand64 = hardware_rand64;
      finalize = hardware_rand64_fini;
    } else {
      fprintf (stderr, "rdrand not available\n");
      return 1;
    }
 } else if (input == MRAND48_R) {
   /*   printf ("\nMRAND48_R\n");*/
      initialize = hardware_rand64_init;
      rand64 = hardware_mrand48r;
      finalize = hardware_rand64_fini;
 } else if (input == INPFILE) {
  /*    printf ("\nSOFTWARE\n");*/
      openfile(input_file);
      initialize = software_rand64_init;
      rand64 = software_rand64;
      finalize = software_rand64_fini;
 } else {
    return 1;
 }

  initialize ();
  int wordsize = sizeof rand64 ();
  int output_errno = 0;

  if (output == STDIO || output == NONE) {
  do
    {
      unsigned long long x = rand64 ();
      int outbytes = nbytes < wordsize ? nbytes : wordsize;
      if (!writebytes (x, outbytes))
	{
	  output_errno = errno;
	  break;
	}
      nbytes -= outbytes;
    }
  while (0 < nbytes);
  if (fclose (stdout) != 0)
    output_errno = errno;

  if (output_errno)
    {
      errno = output_errno;
      perror ("output");
    }
 } else if ( output == N) {
    unsigned int bufbytes = blksz;
        char *buffer = malloc(bufbytes);
        int i;
        size_t j;

        do {
            int outbytes = nbytes < bufbytes ? nbytes : bufbytes;
            unsigned long long x;
            for (i = 0; i < outbytes; i += sizeof(x)) {
                x = rand64 ();
                for (j = 0; j < sizeof(x); j++) {
                    unsigned char b = *((unsigned char *)&x + j);
                    buffer[i + j] = b;
                }
            }
            write(1, buffer, outbytes);
            nbytes -= outbytes;
        }
        while (0 < nbytes);
       if (buffer != NULL){ free(buffer);}
   
 }
  finalize ();
  return !!output_errno;
}
