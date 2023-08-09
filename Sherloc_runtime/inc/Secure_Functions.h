/*----------------------------------------------------------------------------
 * Name:    Secure_Functions.h
 * Purpose: Function and Typedef Declarations to include into NonSecure Application.
 *----------------------------------------------------------------------------*/

/* Define DWT and MTB buffer size */
#define ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004) // DWT->CYCCNT
#define ARM_CM_DWT_EXCCNT (*(uint32_t *)0xE000100C) // DWT->EXCCNT
#define MTB_ALIAS_MAX 0x10000                       // 0x10000: 16kb

/* Define typedef for NonSecure callback function */ 
typedef int32_t (*NonSecure_funcptr)(uint32_t);

/* Function declarations for Secure functions called from NonSecure application */
extern int32_t Secure_LED_On (uint32_t);
extern int32_t Secure_LED_Off(uint32_t);
extern int32_t Secure_LED_On_callback (NonSecure_funcptr);
extern int32_t Secure_LED_Off_callback(NonSecure_funcptr);
extern void    Secure_printf (char*);
extern void    printf_int(int num);

extern int32_t time_measurement_s(uint32_t time);
extern int mtb_enable(size_t mtb_size);