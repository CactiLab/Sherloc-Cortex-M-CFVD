/*----------------------------------------------------------------------------
 * Name:    main_s.c
 * Purpose: Main function secure mode
 *----------------------------------------------------------------------------*/

#include <arm_cmse.h>
#include <stdio.h>
//#include "Driver_USART.h"
#include "IOTKit_CM33_FP.h" /* Device header */
#include "Board_LED.h"      /* ::Board Support:LED */
#include "Board_GLCD.h"     /* ::Board Support:Graphic LCD */
#include "GLCD_Config.h"    /* Keil.SAM4E-EK::Board Support:Graphic LCD */
#include "../../../Sherloc_runtime/inc/sherloc_s.h"

/* Start address of non-secure application */
#define NONSECURE_START (0x00200000u)

extern GLCD_FONT GLCD_Font_16x24;

extern int stdout_init(void);

/* typedef for NonSecure callback functions */
typedef int32_t (*NonSecure_fpParam)(uint32_t) __attribute__((cmse_nonsecure_call));
typedef void (*NonSecure_fpVoid)(void) __attribute__((cmse_nonsecure_call));

char text[] = "Hello World (secure)\r\n";

static uint32_t x;
/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
  uint32_t NonSecure_StackPointer = (*((uint32_t *)(NONSECURE_START + 0u)));
  NonSecure_fpVoid NonSecure_ResetHandler = (NonSecure_fpVoid)(*((uint32_t *)(NONSECURE_START + 4u)));

  SystemCoreClockUpdate();

  stdout_init(); /* Initialize Serial interface */

  cfi_test_s();

#ifdef MTB_ENABLE
  // reset the position to 0
  MTB->POSITION = 0;
#endif

  printf("Hello World (Secure)\r\n");

  // call non-secure reset handler
  NonSecure_ResetHandler();
}