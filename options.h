
#define NONE 0 
#define RDRAND 1 
#define MRAND48_R 2 
#define INPFILE 3 

#define STDIO 4
#define N 5

bool readopt(int argc, char **argv, int* input, int* output, long long *nbytes, 
unsigned int* blksz, char** input_file);

