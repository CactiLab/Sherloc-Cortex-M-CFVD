#ifndef CFI_S_H
#define CFI_S_H

#include <inttypes.h>
#include <arm_cmse.h>
#include <stdio.h>
#include <string.h>
#include "IOTKit_CM33_FP.h" /* Device header */
#include "Board_LED.h"		/* ::Board Support:LED */
#include "Board_GLCD.h"		/* ::Board Support:Graphic LCD */
#include "GLCD_Config.h"	/* Keil.SAM4E-EK::Board Support:Graphic LCD */
#include "SMM_MPS2.h"		/* Keil::Board Support:V2M-MPS2:Common */
#include "core_cm33.h"
#include "../inc/Secure_Functions.h"

#define ALWAYS_INLINE __attribute__((always_inline))

/*----------------------------------------------------------------------------
  CONTROLS
 *----------------------------------------------------------------------------*/

// #define TRIGGER
// #define VULFOO
// #define MPU_trigger
// #define LED_trigger
// #define TRIGGER_READ_BUFF
// #define TRIGGER_INS
// #define TRIGGER_IBT
// #define TRIGGER_IBT_BASELINE

//#define DEBUG_MON

#ifndef TRIGGER
//#define TEST_CFI
//#define WATERMARK // If you want to check the MTB buffer in the non-secure state, comment this, or the trace will stop
//#define WATERMARK_VALUE 0xFC2
#endif // TRIGGER
// if target program has interrupts, it is impossible to only detect backward edge without handling interrupts. Since the source address of the interrupt is unknown, which can be a call. If it is not handled by IRQ checking, this address will be considered as a true call and update the call return stack.
// #define LOG_RET    	// print out the calculated return address and whether the return match or not
//#define LOG_IRQ // print out IRQ in and out

/*----------------------------------------------------------------------------
  Eval
 *----------------------------------------------------------------------------*/
#define USE_SYSTICK_NS // this is for systick disabling and enabling
#ifdef TEST_CFI

#define RTOS

// always on when do the sub step evaluation
// #define EVAL_INS_IDENTIFY
// #define EVAL_READ

// #define SS_NO_IRQ
// #define EMU_NO_IRQ
// #define FULL_NO_IRQ
// #define SS_IRQ
// #define EMU_IRQ
// #define FULL_IRQ
#define FULL_EMU

// #define IBT_BASELINE_NO_IRQ
// #define IBT_NO_IRQ
// #define IBT_BASELINE_IRQ
// #define IBT_IRQ
// #define FULL

#define SPLIT

#ifndef TRIGGER
#define IRQ_CHECK
#endif // TRIGGER

#if defined(FULL)

#elif defined(SPLIT)
#define EVAL_SHERLOC_SPLIT

/*
// cpu cycles for different instructions: https://developer.arm.com/documentation/ddi0550/c/programmers-model/instruction-set-summary
// Word, immediate offset	LDR <Rd>, [<Rn>, #<imm>]	2 or 1[f]
// Top	MOVT <Rd>, #<imm>	3
// Wide	MOVW <Rd>, #<imm>	3
// to read from the dwt register
f241 0604 	movw	r6, #4100	; 0x1004 (3)
f2ce 0600 	movt	r6, #57344	; 0xe000 (3)
6830      	ldr	r0, [r6, #0] (1)
PS: when first use this register, run three instructions, then the following only runs one load instruction
// DWT_CYCCNT: 0xE0001004
*/
#define REGISTER_RED_INIT_CYCLES_PIPE 2 // 6 cycles for setting register address to r6, 1 cycle to load from DWT register, 1 cycle for subtraction. However, because of the pipeline, the total extra cycles are 2.
/*
10001a88:	f241 0604 	movw	r6, #4100	; 0x1004
10001a8c:	6028      	str	r0, [r5, #0]
10001a8e:	2000      	movs	r0, #0
10001a90:	f2ce 0600 	movt	r6, #57344	; 0xe000
10001a94:	6068      	str	r0, [r5, #4]
10001a96:	f3bf 8f4f 	dsb	sy
10001a9a:	f3bf 8f6f 	isb	sy
10001a9e:	6831      	ldr	r1, [r6, #0]
*/
#define REGISTER_LOAD_CYCLE 1
#define LOAD_SUB_CYCLE 7
#endif // SPLIT

#endif // TEST_CFI

/*----------------------------------------------------------------------------
  OTHERS
 *----------------------------------------------------------------------------*/

#define RECORD_32
#define LOG_ERROR // print out precise illegal branches errors

#define DEBUG_LEVEL
#define SHOW_RECORD
// #define SHOW_BT
// #define MTB_ENABLE

#define BINARY_SEARCH

/*----------------------------------------------------------------------------
  Constants
 *----------------------------------------------------------------------------*/
#define NON_SECURE_BASE 0x200000
#define CLEAN_ADDR(addr) ((uint32_t)(addr & ~NON_SECURE_BASE))
#define CONCACT_UINT32(src, dst) (CLEAN_ADDR(src) << 16 | CLEAN_ADDR(dst))

#define MTB_CLEAR_VALUE 12
#define MTB_RESET_VALUE 0x8000000c
#define CLEARED_MASK_VALUE 0x00000010
#define DEBUGMON_DISABLE 0x01100000
#define DEBUGMON_ENABLE 0x01110000
#define DWT_TRACE_ENABLE 0x40000001
#define LAST_BIT_CLEAR 0xFFFFFFFE

#define MTB_ALIAS_MAX 0x10000 // 0x10000: 16kb
#define MTB_BUFF 0x1000		  // 0x4000: 4kb
#define MAXMSG 80
#define FALSE -1
#define TRUE 1
#define TASK_FLAG 0x0FFFFFFF
#define STACK_FULL 0xFFFFFFFF
#define TASK_FULL STACK_FULL
#define STACK_EMPTY 0xFFFFFFFE
#define MULTI_STACK

/*----------------------------------------------------------------------------
  CONSTANTS
 *----------------------------------------------------------------------------*/
#define CODE_NS 0x00400000
#define CODE_S 0x10000000
#define IRQ_EXIT 0xFFFFFF00

/*---------------------------------------------------------------------------
	Hard Fault Handler Functions
 *---------------------------------------------------------------------------*/
void HardFault_Handler(void);
void BusFault_Handler(void);
void MemManage_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);

enum IRQ_states
{
	IRQ_IN,
	PendSV_IN,
	NESTED_IRQ_IN,
	IRQ_OUT,
	PendSV_OUT
};

/*---------------------------------------------------------------------------
	Stack for IRQ checking
 *---------------------------------------------------------------------------*/
#define MAXSTACK 256
typedef struct __attribute__((packed))
{
	uint32_t stk[MAXSTACK];
	uint32_t top;
	int top_pos;
} ST;
extern int STACK_push(ST *stack, uint32_t item);
extern uint32_t STACK_pop(ST *stack);
extern uint32_t STACK_top(ST *stack);
extern uint32_t STACK_check_all(ST *stack);

/*---------------------------------------------------------------------------
	Stack for TASK scheduling
 *---------------------------------------------------------------------------*/
#define MAXTASK 10 // main + tasks
#define MAXTASK_LIST MAXTASK * 50
typedef struct __attribute__((packed))
{
	uint32_t task_list[MAXTASK_LIST];
	int num;
} TASK_ST;
extern int TASK_insertSorted(TASK_ST *task, uint32_t item);
extern int TASK_checkSorted(TASK_ST *task, uint32_t item);
extern int TASK_insert(TASK_ST *task, uint32_t item);
extern int TASK_check(TASK_ST *task, uint32_t item);
extern int TASK_check_all(TASK_ST *task);

extern int binary_search(uint32_t record, uint32_t *list, int list_size);
extern int linear_search(uint32_t record, uint32_t *list, int list_size);

/*---------------------------------------------------------------------------
	Exception stack frame
 *---------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) ContextStateFrame
{
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;  // return_address
	uint32_t psr; // xpsr
} sContextStateFrame;

/*---------------------------------------------------------------------------
	LCD_PRINT macro
 *---------------------------------------------------------------------------*/
extern GLCD_FONT GLCD_Font_16x24;
extern GLCD_FONT GLCD_Font_6x8;

/*---------------------------------------------------------------------------
	Hard Fault Handler Functions
 *---------------------------------------------------------------------------*/
void DebugMon_Handler(void);
#define ENABLE_DEBUG_MON DCB->DEMCR |= DCB_DEMCR_MON_EN_Msk | DCB_DEMCR_TRCENA_Msk; /* Enable Trace and Debug Monitor exception: dcb 0xE000EDF0UL */

/*---------------------------------------------------------------------------
	Cortex-M DWT TIMER
	Refer to https://www.embedded-computing.com/articles/measuring-code-execution-time-on-arm-cortex-m-mcus
 *---------------------------------------------------------------------------*/
#define ARM_CM_DEMCR (*(uint32_t *)0xE000EDFC)
#define ARM_CM_DWT_CTRL (*(uint32_t *)0xE0001000)	// DWT->CTRL (core_cm33.h, DWT_Type)
#define ARM_CM_DWT_CYCCNT (*(uint32_t *)0xE0001004) // DWT->CYCCNT

/*----------------------------------------------------------------------------
	DWT FUNCTION filed setting
	0b0000: disable, 0b0100: read, 0b0101: write, 0b0110: read/write
 *----------------------------------------------------------------------------*/
#define DWT_FUNCTION_R 0x40000814  // 0b01000000000000000000100000010100, trigger a debugmon event when the comparator matches
#define DWT_FUNCTION_W 0x40000815  // 0b01000000000000000000100000010101, trigger a debugmon event when the comparator matches
#define DWT_FUNCTION_RW 0x00000816 // 0b00000000000000000000100000010110, trigger a debugmon event when the comparator matches

void measurement_init(void);
void config_dwt();
void dwt_install_watchpoint(uint32_t comp_id, uint32_t address, uint32_t function);
void dwt_disable_watchpoint(uint32_t comp_id);

/*---------------------------------------------------------------------------
	MTB refer to https://interrupt.memfault.com/blog/instruction-tracing-mtb-m33
 *---------------------------------------------------------------------------*/
typedef struct __attribute__((packed)) MtbM33
{ // 0xE0043000
	__IOM uint32_t POSITION;
	__IOM uint32_t MASTER; // [6]: TSTOPEN, trace stop input enable. [5]: TSTARTEN, trace start input enable.
	__IOM uint32_t FLOW;   // 0: AUTOSTOP, 1: AUTOHALT, [31:3]: WATERMARK
	__IM uint32_t BASE;	   // 0x24000000
	__IOM uint32_t TSTART;
	__IOM uint32_t TSTOP;
	__IOM uint32_t SECURE;
} sMtbM33;

// 0xE00FF01C MTB 0xFFF44003. Reads as 0xFFF44002 if the MTB is not implemented..
// 0xE00FF014 ETM 0xFFF42003. Reads as 0xFFF42003 if the ETM is not implemented.

static sMtbM33 *const MTB = ((sMtbM33 *)0xE0043000);
void mtb_disable(void);

// Transition Table structure
#define IBT_OFFSET 12

typedef struct __attribute__((__packed__))
{
	uint32_t dwMagicCode; // header, __MGAIC_CODE__
	int32_t IBT_size;
	int32_t IRQ_size;
	int32_t TASK_size;
	int32_t LOOP_size;
	int32_t INS_tag_size;
	uint32_t PendSV;
	uint32_t *IBT_entry;
	uint32_t *IRQ_entry;
	uint32_t *TASK_entry;
	uint32_t *LOOP_entry;
} BtHeader;

/*---------------------------------------------------------------------------
	Functions declaration for sherloc_s.c
 *---------------------------------------------------------------------------*/
void apSleep(unsigned int msec);
void disable_SysTick_ns(void);
void enable_SysTick_ns(void);
void sys_reset();
void dummy_handler();
void cfi_test_s();
bool debug_monitor_enable(void);

extern int IBT_QUERY(uint32_t src, uint32_t dst);

extern int binary_search_branch(uint32_t record, int l, int r);
extern int linear_search_branch(uint32_t record);

#ifdef TRIGGER
int cfi_validation_trigger(uint32_t start_pos, uint32_t end_pos);
#else
void cfi_validation();
#endif // TRIGGER

#endif