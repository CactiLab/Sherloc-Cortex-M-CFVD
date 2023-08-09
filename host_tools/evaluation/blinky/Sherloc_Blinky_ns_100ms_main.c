/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher for MPS2
 * Note(s): possible defines set in "options for target - C/C++ - Define"
 *            __USE_LCD    - enable Output on GLCD
 *            __USE_TIMER0 - use Timer0  to generate timer interrupt
 *                         - use SysTick to generate timer interrupt (default)
 *-----------------------------------------------------------------------------*/

#include <stdio.h>
#include "Device.h"         /* Keil::Board Support:V2M-MPS2:Common */
#include "RTE_Components.h" /* Component selection */
#include "Board_LED.h"      /* ::Board Support:LED */
#include "Board_Buttons.h"  /* ::Board Support:Buttons */
#include "Board_Touch.h"    /* ::Board Support:Touchscreen */
#include "Board_GLCD.h"     /* ::Board Support:Graphic LCD */
#include "GLCD_Config.h"    /* Keil.V2M-MPS2 IOT-Kit::Board Support:Graphic LCD */
#include "..\Sherloc_Blinky_s\Secure_Functions.h"
// #include "cfi_helper_ns.h"

#if defined SECURE
#define TIMER_PORT IOTKIT_SECURE_TIMER0
#else
#define TIMER_PORT IOTKIT_TIMER0
#endif

extern GLCD_FONT GLCD_Font_16x24;

extern int stdout_init(void);

static uint32_t LEDOn, LEDOff;
static uint32_t ticks_1s; /* Flag activated each second */
static uint64_t test_count, t_start, t_end, t_total;

#define ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004) // DWT->CYCCNT
#define MTB_ALIAS_MAX 0x10000                       // 0x10000: 16kb

/*----------------------------------------------------------------------------
  SysTick / Timer0 IRQ Handler
 *----------------------------------------------------------------------------*/
#ifdef __USE_TIMER0
void TIMER0_Handler(void);
void TIMER0_Handler(void)
{
#else
void SysTick_Handler(void);
void SysTick_Handler(void)
{
#endif
  static uint32_t ticks;
  static uint32_t ticks_10ms;

  if (ticks_10ms++ >= 99)
  { /* Set Clock1s to 1 every second */
    ticks_10ms = 0;
    ticks_1s = 1;
  }

  switch (ticks++)
  {
  case 0:
    LEDOn = 1;
    break;
  case 40:
    LEDOff = 1;
    break;
  case 45:
    ticks = 0;
    break;

  default:
    if (ticks > 50)
    {
      ticks = 0;
    }
  }

#ifdef __USE_TIMER0
  TIMER_PORT->INTCLEAR = (1ul << 0); /* clear interrupt */
#endif
}

#ifdef __USE_TIMER0
/*----------------------------------------------------------------------------
  Timer0 Configuration
 *----------------------------------------------------------------------------*/
void Timer0_Config(uint32_t ticks);
void Timer0_Config(uint32_t ticks)
{

  TIMER_PORT->INTCLEAR = (1u << 0); /* clear interrupt */
  TIMER_PORT->RELOAD = (ticks - 1); /* set reload value */
  TIMER_PORT->CTRL = ((1u << 3) |   /* enable Timer interrupt */
                      (1u << 0));   /* enable Timer */

  NVIC_EnableIRQ(TIMER0_IRQn); /* Enable interrupt in NVIC */
}
#endif

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main(void)
{
  uint32_t ledMax = LED_GetCount() - 1;
  uint32_t ledNum = 0;
  uint32_t keyMax = Buttons_GetCount();
  uint32_t keyNum = 0;
  uint32_t keyboard = 0, keyboard_ = 0xFFFFFFFFu;
  int32_t dir = 1;
  char text[] = "n\r\n";
#ifdef __USE_LCD
  TOUCH_STATE tsc_state;
  int32_t tsc = -1;
  uint32_t i;
#endif

  SystemCoreClockUpdate();

  LED_Initialize();     /* Initializ LEDs */
  Buttons_Initialize(); /* Initializ Push Buttons */
#ifdef RTE_Compiler_IO_STDOUT_User
  // stdout_init();                                      /* Initializ Serial interface */
#endif
#ifdef __USE_LCD
  Touch_Initialize(); /* Initialize touchscreen */
  GLCD_Initialize();  /* Initialize the GLCD */

  /* display initial screen */
  GLCD_SetFont(&GLCD_Font_16x24);
  GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
  GLCD_ClearScreen();
  GLCD_SetBackgroundColor(GLCD_COLOR_BLUE);
  GLCD_SetForegroundColor(GLCD_COLOR_RED);
  GLCD_DrawString(0 * 16, 0 * 24, "   V2M-MPS2 Demo    ");
  GLCD_DrawString(0 * 16, 1 * 24, "      Blinky        ");
  GLCD_DrawString(0 * 16, 2 * 24, "   www.keil.com     ");

  GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
  GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
  switch ((SCB->CPUID >> 4) & 0xFFF)
  {
  case 0xD20:
    GLCD_DrawString(0 * 16, 4 * 24, "  Cortex-M23        ");
    break;
  case 0xD21:
    GLCD_DrawString(0 * 16, 4 * 24, "  Cortex-M33        ");
    break;
  default:
    GLCD_DrawString(0 * 16, 4 * 24, "  unknown Cortex-M  ");
    break;
  }
  GLCD_DrawString(2 * 16, 6 * 24, "LED:");
  GLCD_DrawString(2 * 16, 7 * 24, "KEY:");
  GLCD_DrawString(2 * 16, 8 * 24, "Touch:");

  GLCD_SetForegroundColor(GLCD_COLOR_LIGHT_GREY);
  for (i = 0; i <= ledMax; i++)
  {
    GLCD_DrawChar((9 + i) * 16, 6 * 24, 0x80 + 0); /* draw empty circle */
  }
  for (i = 0; i < keyMax; i++)
  {
    GLCD_DrawChar((9 + i) * 16, 7 * 24, 0x80 + 0); /* draw empty circle */
  }
  GLCD_DrawChar(9 * 16, 8 * 24, 0x80 + 0); /* draw empty circle */
#endif

#ifdef __USE_TIMER0
  Timer0_Config(SystemCoreClock / 1000 * 100); /* Generate interrupt each 100 ms */
#else
  SysTick_Config(SystemCoreClock / 1000 * 100); /* Generate interrupt each 100 ms */
#endif

  // mtb_enable(MTB_ALIAS_MAX);
  test_count = 10;
  // t_start = ARM_CM_DWT_CYCCNT;
  //  elapsed_time_start(0);
  while (test_count > 0)
  {
    if (LEDOn)
    {
      LEDOn = 0;
      LED_On(ledNum); /* Turn specified LED on */
#ifdef __USE_LCD
      GLCD_SetForegroundColor(GLCD_COLOR_GREEN);
      GLCD_DrawChar((9 + ledNum) * 16, 6 * 24, 0x80 + 1); /* Circle Full */
#endif
			test_count--;
    }

    if (LEDOff)
    {
      LEDOff = 0;
      LED_Off(ledNum); /* Turn specified LED off */
#ifdef __USE_LCD
      GLCD_SetForegroundColor(GLCD_COLOR_LIGHT_GREY);
      GLCD_DrawChar((9 + ledNum) * 16, 6 * 24, 0x80 + 0); /* Circle Full */
#endif

      ledNum = (uint32_t)((int32_t)ledNum + dir); /* Change LED number */

      if (dir == 1 && ledNum == ledMax)
      {
        dir = -1; /* Change direction to down */
      }
      else if (ledNum == 0)
      {
        dir = 1; /* Change direction to up */
      }
			test_count--;
    }

    keyboard = Buttons_GetState();

    if (keyboard ^ keyboard_)
    {
      keyboard_ = keyboard;

      for (keyNum = 0; keyNum < keyMax; keyNum++)
      {
        if (keyboard & (1ul << keyNum))
        {
#ifdef __USE_LCD
          GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
          GLCD_DrawChar((9 + keyNum) * 16, 7 * 24, 0x80 + 1); /* Circle Full */
#endif
        }
        else
        {
#ifdef __USE_LCD
          GLCD_SetForegroundColor(GLCD_COLOR_LIGHT_GREY);
          GLCD_DrawChar((9 + keyNum) * 16, 7 * 24, 0x80 + 0); /* Circle Full */
#endif
        }
      }
    }

#ifdef __USE_LCD
    if (Touch_GetState(&tsc_state) == 0)
    { /* Get touch screen state */
      if (tsc ^ tsc_state.pressed)
      {
        tsc = tsc_state.pressed;
        if ((tsc_state.pressed) &&
            (tsc_state.x >= 500 && tsc_state.x <= 1000 &&
             tsc_state.y >= 550 && tsc_state.y <= 1550))
        {
          GLCD_SetForegroundColor(GLCD_COLOR_LIGHT_GREY);
          GLCD_DrawString(2 * 16, 8 * 24, "Touch:");
          GLCD_SetForegroundColor(GLCD_COLOR_YELLOW);
          GLCD_DrawChar(9 * 16, 8 * 24, 0x80 + 1); /* Circle Full */
        }
        else
        {
          GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
          GLCD_DrawString(2 * 16, 8 * 24, "Touch:");
          GLCD_SetForegroundColor(GLCD_COLOR_LIGHT_GREY);
          GLCD_DrawChar(9 * 16, 8 * 24, 0x80 + 0); /* Circle Full */
        }
      }
    }
#endif

    /* Print message with AD value every second */
    if (ticks_1s)
    {
      ticks_1s = 0;
      // printf("%s\r\n", text);
    }
  }
  // elapsed_time_stop(0);
  /* register NonSecure callbacks in Secure application */
  time_measurement_s(ARM_CM_DWT_CYCCNT);

  while (1)
  {
    ;
  }
}
