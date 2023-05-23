#include <cpuid.h>
#include <stdbool.h>
#include <immintrin.h>
#include <time.h>

#include "rand64-hw.h"

/* Hardware implementation.  */

/* Description of the current CPU.  */
struct cpuid { unsigned eax, ebx, ecx, edx; };

/* Return information about the CPU.  See <http://wiki.osdev.org/CPUID>.  */
 struct cpuid
cpuid (unsigned int leaf, unsigned int subleaf)
{
  struct cpuid result;
  asm ("cpuid"
       : "=a" (result.eax), "=b" (result.ebx),
	 "=c" (result.ecx), "=d" (result.edx)
       : "a" (leaf), "c" (subleaf));
  return result;
}

/* Return true if the CPU supports the RDRAND instruction.  */
_Bool rdrand_supported (void)
{
  struct cpuid extended = cpuid (1, 0);
  return (extended.ecx & bit_RDRND) != 0;
}

struct drand48_data randBuffer;
/* Initialize the hardware rand64 implementation.  */
 void hardware_rand64_init (void)
{
srand48_r(time(NULL), &randBuffer);
}

/* Return a random value, using hardware operations.  */
 unsigned long long
hardware_rand64 (void)
{
  unsigned long long int x;

  /* Work around GCC bug 107565
     <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=107565>.  */
  x = 0;

  while (! _rdrand64_step (&x))
    continue;
  return x;
}


unsigned long long hardware_mrand48r (void)
{
   long int x;
mrand48_r(&randBuffer, &x);

  return  (unsigned long long) x;
}
/* Finalize the hardware rand64 implementation.  */
 void
hardware_rand64_fini (void)
{
}

