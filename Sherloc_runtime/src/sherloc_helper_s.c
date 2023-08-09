#include "../inc/sherloc_s.h"

static char msg[MAXMSG];
static int count = 0;

/*---------------------------------------------------------------------------
   Implementation for systick settings
 *---------------------------------------------------------------------------*/
// disable non-secure systick
ALWAYS_INLINE void disable_SysTick_ns(void)
{
   // disable systick
   SysTick_NS->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

// enable non-secure systick with frequencies
ALWAYS_INLINE void enable_SysTick_ns()
{
   SysTick_NS->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                      SysTick_CTRL_TICKINT_Msk |
                      SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
}

/*---------------------------------------------------------------------------
   Implementation for task list save and resume
 *---------------------------------------------------------------------------*/

ALWAYS_INLINE int TASK_insertSorted(TASK_ST *task, uint32_t item)
{
   int i = 0;
   if (task->num > MAXTASK_LIST - 1)
   {
      return -1;
   }
   if (item == STACK_FULL)
   {
      return task->num;
   }
   // insert the first item
   if (task->num == 0)
   {
#ifdef LOG_RET
      printf("Insert first item: 0x%08lx.\r\n", item);
#endif // LOG_RET
      task->task_list[task->num] = item;
      return task->num++;
   }

#ifdef LOG_RET
   printf("Insert item: 0x%08lx.\r\n", item);
#endif // LOG_RET
   for (i = task->num - 1; (i >= 0 && item <= task->task_list[i]); i--)
   {
      task->task_list[i + 1] = task->task_list[i];
   }
   task->task_list[i + 1] = item;
   return task->num++;
}

ALWAYS_INLINE int TASK_insert(TASK_ST *task, uint32_t item)
{
   int i = 0;
   if (task->num > MAXTASK_LIST - 1)
   {
      return -1;
   }
   // we don't insert empty stack or empty value
   if ((item == STACK_FULL) || (item == 0x0))
   {
      return task->num;
   }

#ifdef LOG_RET
   printf("Insert item: 0x%08lx.\r\n", item);
#endif // LOG_RET

   task->task_list[task->num] = item;
   return task->num++;
}

ALWAYS_INLINE int TASK_checkSorted(TASK_ST *task, uint32_t item)
{
   int idx = binary_search(item, task->task_list, task->num);
   int i = 0;

   if (idx < 0)
   {
      return idx;
   }

   for (i = idx; i < task->num; i++)
   {
      task->task_list[i] = task->task_list[i + 1];
   }
   task->num--;

   return idx;
}

ALWAYS_INLINE int TASK_check(TASK_ST *task, uint32_t item)
{
   int idx = linear_search(item, task->task_list, task->num);

   if (idx < 0)
   {
      return idx;
   } 

   for (size_t i = idx; i < task->num; i++)
   {
      task->task_list[i] = task->task_list[i + 1];
   }
   task->task_list[task->num] = 0;
   task->num--;

   return task->num;
}

ALWAYS_INLINE int TASK_check_all(TASK_ST *task)
{
   printf("Check task list. num: %d.\r\n", task->num);
   task->num = task->num < (MAXTASK_LIST - 1) ? task->num : (MAXTASK_LIST - 1);

   if (task->num == -1)
   {
      return task->num;
   }
   for (size_t i = 0; i < task->num; i++)
   {
      printf("0x%08x\r\n", task->task_list[i]);
   }
   return task->num;
}

/*---------------------------------------------------------------------------
   Implementation for stack push and pop
 *---------------------------------------------------------------------------*/
ALWAYS_INLINE int STACK_push(ST *stack, uint32_t item)
{
   if (stack->top_pos > MAXSTACK - 1)
   {
      return -1;
   }

   stack->top = item;
   stack->stk[++stack->top_pos] = item;
   return stack->top_pos;
}

ALWAYS_INLINE uint32_t STACK_pop(ST *stack)
{
   if (stack->top_pos == -1)
   {
      return -1;
   }

   stack->top_pos--;
   stack->top = STACK_EMPTY ? stack->stk[stack->top_pos] : stack->top_pos == -1;
   return stack->stk[stack->top_pos + 1];
}

ALWAYS_INLINE uint32_t STACK_top(ST *stack)
{
   if (stack->top_pos == -1)
   {
      return -1;
   }

   return stack->stk[stack->top_pos];
}

ALWAYS_INLINE uint32_t STACK_check_all(ST *stack)
{
   if (stack->top_pos == -1)
   {
      printf("Stack empty.\r\n");
      return -1;
   }

   printf("Check stack. top: %d. \r\n", stack->top_pos);
   for (int i = stack->top_pos; i >= 0; i--)
   {
      printf("0x%08x\r\n", stack->stk[i]);
   }

   return stack->stk[stack->top_pos];
}

// binary search
ALWAYS_INLINE int binary_search(uint32_t record, uint32_t *list, int task_num)
{
   int mid = 0;
   int l = 0;
   int r = task_num;

   while (l <= r)
   {
      mid = l + (r - l) / 2;
      if (record == list[mid])
      {
         return mid;
      }
      else if (record < list[mid])
         r = mid - 1;
      else
         l = mid + 1;
   }
   // We reach here when element is not present in array
   return -1;
}

ALWAYS_INLINE int linear_search(uint32_t record, uint32_t *list, int task_num)
{
   if (task_num > 0)
   {
#ifdef TRIGGER
      for (int i = 0; i < task_num; i++)

#else
      // starts from the third element, because the first ans second store the return address of vStartFirstTask and prvTimerTask, which will never return 
      for (int i = 2; i < task_num; i++)
#endif // TRIGGER

      {
         if (record == list[i])
         {
            return i;
         }
      }
   }
   return -1;
}

/* Sleep function to delay n*mS
 * Uses FPGA counter. Prescale is set to divide 25MHZ
 * to 1MHz
 */
void apSleep(unsigned int msec)
{
   unsigned int end;
   unsigned int start;

   start = MPS2_FPGAIO->COUNTER;
   end = start + (25 * msec * 1000);

   if (end >= start)
   {
      while (MPS2_FPGAIO->COUNTER >= start && MPS2_FPGAIO->COUNTER < end)
         ;
   }
   else
   {
      while (MPS2_FPGAIO->COUNTER >= start)
         ;
      while (MPS2_FPGAIO->COUNTER < end)
         ;
   }
}

void Secure_printf(char *pString) __attribute__((cmse_nonsecure_entry));
void Secure_printf(char *pString)
{
   printf("%s\r\n", pString);
}

void Secure_printf_int(int num) __attribute__((cmse_nonsecure_entry));
void Secure_printf_int(int num)
{
   printf("secure_printf_int: %d\r\n", num);
}

/* configure the SAU region: starts from 4
input: SAU region number, start_address, end_address,
            security attributs, the NSC bit, <0=>Non-Secure, <1=>Secure, Non-Secure Callable
*/
void config_sau_s(uint32_t region_id, uint32_t start, uint32_t end, uint32_t nsc_attr)
{
   /* region number */
   SAU->RNR = region_id & SAU_RNR_REGION_Msk;
   /* region start address*/
   SAU->RBAR = start & SAU_RBAR_BADDR_Msk;
   /* region end address */
   SAU->RLAR = (end & SAU_RLAR_LADDR_Msk) |
               /* Region memory attribute index*/
               ((nsc_attr << SAU_RLAR_NSC_Pos) & SAU_RLAR_NSC_Msk) |
               /* Enable region */
               ((1U << SAU_RLAR_ENABLE_Pos) & SAU_RLAR_ENABLE_Msk);
}

void dummy_handler()
{
#ifdef EMU
   mtb_disable();
   while (1)
      ;
#else
   while (1)
      ;
#endif
}

/*---------------------------------------------------------------------------
Recover the CPU state only when usage fault, others will reboot the system
 *---------------------------------------------------------------------------*/
void sys_recover(sContextStateFrame *stack)
{
   __ASM volatile("BKPT #02");

   // clear any logged faults from CFSR
   SCB->CFSR |= SCB->CFSR;
   // return to where exit from the exception
   stack->pc = stack->lr;
   // set the lr a pattern
   stack->lr = 0xdeadbeef;

   // reset the psr state and only leave the "thumb instruction interworking" bit set
   stack->psr = (1 << 24);
}

/*---------------------------------------------------------------------------
Reboot the system for unrecoverable fault: Application interrupt and reset control register (AIRCR-0xE000ED0C)
 *---------------------------------------------------------------------------*/
void sys_reset()
{
   SCB->AIRCR = (0x05FA << 16) | 0x1 << 2;
   while (1)
      ;
}

void clear_stack(sContextStateFrame *stack)
{
   __asm volatile(
       "MOV    R0, #0;      "
       "MOV    R1, R0;       "
       "MOV    R2, R0;       "
       "MOV    R3, R0;       "
       "MOV    R4, R0;       "
       "MOV    R5, R0;       "
       "MOV    R6, R0;       "
       "MOV    R7, R0;       "
       "MOV    R12, R0;       ");
}

/*
 * Function for MTB
 */
int mtb_enable() __attribute__((cmse_nonsecure_entry));
int mtb_enable(size_t mtb_size)
{
   if ((mtb_size < 16) || (__builtin_popcount(mtb_size) != 1))
   {
      // MTB must be at least 16 bytes and be a power of 2
      return -1;
   }
   
   // configure the MTB buffer as S
   config_sau_s(4, 0x24000000, 0x24003FFF, 1); // already configured 0-3
                                               // then separate the buffer into equally secure and non-secure: 00x2400_1FFFF
                                               // 100100000000000001111111111111
   //  scrub MTB SRAM so it's easy to see what has gotten written to
   memset((void *)MTB->BASE, 0x0, mtb_size);
   const int mask = __builtin_ctz(mtb_size) - 4;

   // we are about to reconfigure so turn off MTB
   mtb_disable();

   // reset position
   MTB->POSITION = 0;
#ifdef WATERMARK
   MTB->FLOW = WATERMARK_VALUE;
#endif

   // Start tracing!
   MTB->MASTER = (1 << 31) | (mask << 0);

   return 0;
}

void __inline mtb_disable(void)
{
   MTB->MASTER &= ~(1 << 31);
}

/*---------------------------------------------------------------------------
 * functions used to configure the DWT
   Cortex-M DWT TIMER
   Refer to https://www.embedded-computing.com/articles/measuring-code-execution-time-on-arm-cortex-m-mcus
 *---------------------------------------------------------------------------*/
/* init function must be called at the very beginning of measurement */

void measurement_init(void)
{
   DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk; // enable DWT trace, set DEMCR bit 24
   // See if DWT is available
   if (DWT->CTRL != 0)
   {
      DWT->CYCCNT = 0; // reset cycle counting to 0
      DWT->EXCCNT = 0; // reset exception counting to 0
      DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
   }
}

// install a watchpoint based on user's input: a comparator id, an address, and a function
void dwt_install_watchpoint(uint32_t comp_id, uint32_t address, uint32_t function)
{
   uint32_t number_of_comparators = (DWT->CTRL >> 28) & 0xF;
   // if the com_id is larger than the number of comparators, return
   if (comp_id >= number_of_comparators)
   {
      return;
   }
   switch (comp_id)
   {
   case 0:
      DWT->COMP0 = address;
      DWT->FUNCTION0 = function;
      break;
   case 1:
      DWT->COMP1 = address;
      DWT->FUNCTION1 = function;
      break;
   case 2:
      DWT->COMP2 = address;
      DWT->FUNCTION2 = function;
      break;
   case 3:
      DWT->COMP3 = address;
      DWT->FUNCTION3 = function;
      break;
   default:
      break;
   }
}

// disable dwt watchpoint
void dwt_disable_watchpoint(uint32_t comp_id)
{
   uint32_t number_of_comparators = (DWT->CTRL >> 28) & 0xF;
   // if the com_id is larger than the number of comparators, return
   if (comp_id >= number_of_comparators)
   {
      return;
   }
   switch (comp_id)
   {
   case 0:
      DWT->FUNCTION0 = 0;
      break;
   case 1:
      DWT->FUNCTION1 = 0;
      break;
   case 2:
      DWT->FUNCTION2 = 0;
      break;
   case 3:
      DWT->FUNCTION3 = 0;
      break;
   default:
      break;
   }
}

/*---------------------------------------------------------------------------
 * functions used to configure the debugmonitor exception
 *---------------------------------------------------------------------------*/

bool debug_monitor_enable(void)
{
   if ((DCB->DHCSR & 0x1) != 0)
   {
      printf("Halting debug enabled-\r\n");
      return FALSE;
   }

   DCB->DEMCR |= DCB_DEMCR_MON_EN_Msk | DCB_DEMCR_TRCENA_Msk;

   // Priority for DebugMonitor Exception is bits[7:0].
   // We will use the lowest priority so other ISRs can
   // fire while in the DebugMonitor Interrupt
   // __NVIC_PRIO_BITS is the loweast priority
   uint32_t priority = NVIC_GetPriority(UART0_IRQn);
   NVIC_SetPriority(DebugMonitor_IRQn, priority + 1);
   return true;
}

void debug_monitor_disable(void)
{
   DCB->DEMCR &= ~DCB_DEMCR_MON_EN_Msk;
}

/*---------------------------------------------------------------------------
   Fault_Handler configuration
 *---------------------------------------------------------------------------*/

void stackDump(sContextStateFrame *stack)
{
   uint32_t r0 = stack->r0;
   uint32_t r1 = stack->r1;
   uint32_t r2 = stack->r2;
   uint32_t r3 = stack->r3;
   uint32_t r12 = stack->r12;
   uint32_t lr = stack->lr;
   uint32_t pc = stack->pc;
   uint32_t psr = stack->psr;

}

void DebugMon_Handler(void)
{
   __asm volatile(
       "TST    LR, #0b0100;      "
       "ITE    EQ;               "
       "MRSEQ  R0, MSP;          "
       "MRSNE  R0, PSP;          "
       "B      DebugMon_Handler_Main;  ");
}

void HardFault_Handler(void)
{
   __asm volatile(
       "TST    LR, #0b0100;      "
       "ITE    EQ;               "
       "MRSEQ  R0, MSP;          "
       "MRSNE  R0, PSP;          "
       "MOV    R1, LR;           "
       "B      .;  ");
}

void BusFault_Handler(void)
{
   __asm volatile(
       "TST    LR, #0b0100;      "
       "ITE    EQ;               "
       "MRSEQ  R0, MSP;          "
       "MRSNE  R0, PSP;          "
       "MOV    R1, LR;           "
       "B      .;  ");
}

void MemManage_Handler(void)
{
   __asm volatile(
       "TST    LR, #0b0100;      "
       "ITE    EQ;               "
       "MRSEQ  R0, MSP;          "
       "MRSNE  R0, PSP;          "
       "MOV    R1, LR;           "
       "B      .;  ");
}

void UsageFault_Handler(void)
{
   __asm volatile(
       "TST    LR, #0b0100;      "
       "ITE    EQ;               "
       "MRSEQ  R0, MSP;          "
       "MRSNE  R0, PSP;          "
       "MOV    R1, LR;           "
       "B      .;  ");
}

void SecureFault_Handler(void)
{
   __asm volatile(
       "TST    LR, #0b0100;      "
       "ITE    EQ;               "
       "MRSEQ  R0, MSP;          "
       "MRSNE  R0, PSP;          "
       "MOV    R1, LR;           "
       "B      .;  ");
}