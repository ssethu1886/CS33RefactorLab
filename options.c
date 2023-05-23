#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "options.h"

bool readopt(int argc, char **argv, int* input, int* output, long long *nbytes,
 unsigned int* blksz, char** input_file) {
    int c;
    while ((c = getopt(argc, argv, ":i:o:")) != -1) {
        switch(c) {
            case 'i':
                if (strcmp("rdrand", optarg) == 0)
                    *input = RDRAND;
                else if (strcmp("mrand48_r", optarg) == 0)
                    *input = MRAND48_R;
                else if ('/' == optarg[0]) {
                    *input = INPFILE;
                    *input_file = optarg;
                }
                else
                    return false;
                break;
            case 'o':
                if (strcmp("stdio", optarg) == 0)
                    *output = STDIO;
                else {
                    *output = N;
                    *blksz = atoi(optarg);
                    if (*blksz == 0) {
                        fprintf (stderr,
                             "Option -o N requires a non-zero block size\n");
                        return false;
                    }
                }
                break;
            case '?':
             if (optopt == 'o' || optopt == 'i')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else
                fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
             return false;
      default:
        abort ();
        }
    }

    if (optind >= argc)
        return false;

    *nbytes = atol(argv[optind]);
    if (*nbytes >= 0)
      return true;
    else
      return false;
}
