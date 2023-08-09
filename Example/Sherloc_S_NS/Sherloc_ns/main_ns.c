/*----------------------------------------------------------------------------
 * Name:    main_ns.c
 * Purpose: Main function non-secure mode
 *----------------------------------------------------------------------------*/

#include <arm_cmse.h>
#include "IOTKit_CM33_FP.h"                           /* Device header */
#include "../../../Sherloc_runtime/inc/Secure_Functions.h" /* Secure Code Entry Points */
#include "../../../Sherloc_runtime/evaluation/BEEBS.h"
#include "string.h"

#define ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004) // DWT->CYCCNT
#define MTB_ALIAS_MAX 0x10000                       // 0x10000: 16kb

/*----------------------------------------------------------------0------------
  Main function.
 *----------------------------------------------------------------------------*/
int main(void)
{
  volatile int result = 0;
  mtb_enable(MTB_ALIAS_MAX);

  for (size_t count = 0; count < 5; count++)
  {
    ARM_CM_DWT_CYCCNT = 0;
    for (size_t i = 0; i < REPEAT_FACTOR; i++)
    {
      initialise_benchmark();
      __asm volatile("" ::"r"(result)
                     : "memory");
      result = benchmark();
      __asm volatile("" ::"r"(result)
                     : "memory");
      /* register NonSecure callbacks in Secure application */
    }
    // end trigger
    time_measurement_s(ARM_CM_DWT_CYCCNT);
  }

  while (1)
  {
  }
}
