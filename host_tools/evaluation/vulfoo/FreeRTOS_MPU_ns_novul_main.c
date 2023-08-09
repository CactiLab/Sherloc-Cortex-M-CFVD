/*----------------------------------------------------------------------------
 * Name:    main_ns.c
 * Purpose: Main function non-secure mode
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include "IOTKit_CM33_FP.h" /* Device header */
#include "Board_LED.h"      /* ::Board Support:LED */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "../../../Sherloc_runtime/inc/Secure_Functions.h"
#include "../../../Sherloc_runtime/evaluation/BEEBS.h"

/* Externs needed by the MPU setup code. These are defined in Scatter-Loading
 * description file (FreeRTOSDemo_ns.sct). */
extern uint32_t Image$$ER_IROM_NS_PRIVILEGED$$Base;
extern uint32_t Image$$ER_IROM_NS_PRIVILEGED_ALIGN$$Limit;
extern uint32_t Image$$ER_IROM_NS_FREERTOS_SYSCALL$$Base;
extern uint32_t Image$$ER_IROM_NS_FREERTOS_SYSCALL_ALIGN$$Limit;
extern uint32_t Image$$ER_IROM_NS_UNPRIVILEGED$$Base;
extern uint32_t Image$$ER_IROM_NS_UNPRIVILEGED_ALIGN$$Limit;

extern uint32_t Image$$RW_IRAM_NS_PRIVILEGED$$Base;
extern uint32_t Image$$RW_IRAM_NS_PRIVILEGED_ALIGN$$Limit;
extern uint32_t Image$$RW_IRAM_NS_UNPRIVILEGED$$Base;
extern uint32_t Image$$RW_IRAM_NS_UNPRIVILEGED_ALIGN$$Limit;

/* Privileged flash. */
const uint32_t *__privileged_functions_start__ = (uint32_t *)&(Image$$ER_IROM_NS_PRIVILEGED$$Base);
const uint32_t *__privileged_functions_end__ = (uint32_t *)((uint32_t) & (Image$$ER_IROM_NS_PRIVILEGED_ALIGN$$Limit)-0x1); /* Last address in privileged Flash region. */

/* Flash containing system calls. */
const uint32_t *__syscalls_flash_start__ = (uint32_t *)&(Image$$ER_IROM_NS_FREERTOS_SYSCALL$$Base);
const uint32_t *__syscalls_flash_end__ = (uint32_t *)((uint32_t) & (Image$$ER_IROM_NS_FREERTOS_SYSCALL_ALIGN$$Limit)-0x1); /* Last address in Flash region containing system calls. */

/* Unprivileged flash. Note that the section containing system calls is
 * unprivileged so that unprivileged tasks can make system calls. */
const uint32_t *__unprivileged_flash_start__ = (uint32_t *)&(Image$$ER_IROM_NS_UNPRIVILEGED$$Base);
const uint32_t *__unprivileged_flash_end__ = (uint32_t *)((uint32_t) & (Image$$ER_IROM_NS_UNPRIVILEGED_ALIGN$$Limit)-0x1); /* Last address in un-privileged Flash region. */

/* RAM with priviledged access only. This contains kernel data. */
const uint32_t *__privileged_sram_start__ = (uint32_t *)&(Image$$RW_IRAM_NS_PRIVILEGED$$Base);
const uint32_t *__privileged_sram_end__ = (uint32_t *)((uint32_t) & (Image$$RW_IRAM_NS_PRIVILEGED_ALIGN$$Limit)-0x1); /* Last address in privileged RAM. */

/* Unprivileged RAM. */
const uint32_t *__unprivileged_sram_start__ = (uint32_t *)&(Image$$RW_IRAM_NS_UNPRIVILEGED$$Base);
const uint32_t *__unprivileged_sram_end__ = (uint32_t *)((uint32_t) & (Image$$RW_IRAM_NS_UNPRIVILEGED_ALIGN$$Limit)-0x1); /* Last address in un-privileged RAM. */
/*-----------------------------------------------------------*/

/**
 * @brief Control evaluation.
 */
#define VULFOO

/**
 * @brief Application task startup hook.
 */
void vApplicationDaemonTaskStartupHook(void);

/**
 * @brief Size of the shared memory region.
 */
#define SHARED_MEMORY_SIZE 32

/**
 * @brief Memory region shared between two tasks.
 */
static uint8_t ucSharedMemory[SHARED_MEMORY_SIZE] __attribute__((aligned(32)));

/**
 * @brief Memory region used to track Memory Fault intentionally caused by the
 * RO Access task.
 *
 * RO Access task sets ucROTaskFaultTracker[ 0 ] to 1 before accessing illegal
 * memory. Illegal memory access causes Memory Fault and the fault handler
 * checks ucROTaskFaultTracker[ 0 ] to see if this is an expected fault. We
 * recover gracefully from an expected fault by jumping to the next instruction.
 *
 * @note We are declaring a region of 32 bytes even though we need only one. The
 * reason is that the size of an MPU region must be a multiple of 32 bytes.
 */
static uint8_t ucROTaskFaultTracker[SHARED_MEMORY_SIZE] __attribute__((aligned(32))) = {0};
/*-----------------------------------------------------------*/

/**
 * @brief Implements the task which has Read Only access to the memory region
 * ucSharedMemory.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void sherlocMicroTask(void *pvParameters);

/**
 * @brief Implements the task which has Read Write access to the memory region
 * ucSharedMemory.
 *
 * @param pvParameters[in] Parameters as passed during task creation.
 */
static void sherlocBeebsTask(void *pvParameters);

/*-----------------------------------------------------------*/

/**
 * @brief The hard fault handler.
 *
 * It calls a function called vHandleMemoryFault.
 */
void MemManage_Handler(void) __attribute__((naked));
/*-----------------------------------------------------------*/

/**
 * @brief Create all demo tasks.
 */
// void sherlocSyscall() FREERTOS_SYSTEM_CALL;
static void sherlocMicroTask(void *pvParameters);
static void sherlocBeebsTask(void *pvParameters);
static void sherlocEmptyTask(void *pvParameters);

#define BufferSize configMINIMAL_STACK_SIZE * 2
#define StackSize configMINIMAL_STACK_SIZE * 4
static StackType_t beebsStackBuffer1[StackSize];
static StackType_t beebsStackBuffer2[BufferSize];
static StackType_t vulfooStackBuffer[BufferSize];

void vApplicationDaemonTaskStartupHook(void);
static void SystemClock_Config(void);

/* system call functions */
void escalate_privilege() __attribute__((noinline, section("freertos_system_calls")));
void drop_privilege() __attribute__((noinline, section("freertos_system_calls")));
void bench_start() __attribute__((noinline, section("freertos_system_calls")));
void bench_result() __attribute__((noinline, section("freertos_system_calls")));
/*----------------------------------------------------------*/

/* Non-secure main. */
int main(void)
{
    mtb_enable(MTB_ALIAS_MAX);
    ARM_CM_DWT_CYCCNT = ARM_CM_DWT_EXCCNT = 0;

    /* start the schedule */
    vTaskStartScheduler();

    /* Should not reach here as the scheduler is already started. */
    for (;;)
    {
    }
}
/*-----------------------------------------------------------*/

__attribute__((optnone)) void escalate_privilege() // address: 0x0020206c
{
    portRAISE_PRIVILEGE();
    printf_int(10);
}

__attribute__((optnone)) void drop_privilege()
{
    portRESET_PRIVILEGE();
}

void bench_start() /* FREERTOS_SYSTEM_CALL */
{
    portRAISE_PRIVILEGE();
    ARM_CM_DWT_CYCCNT = ARM_CM_DWT_EXCCNT = 0;
    portRESET_PRIVILEGE();
}

void bench_result() /* FREERTOS_SYSTEM_CALL */
{
    portRAISE_PRIVILEGE();
    time_measurement_s(ARM_CM_DWT_CYCCNT + ARM_CM_DWT_EXCCNT);
    portRESET_PRIVILEGE();
}

#ifdef VULFOO

__attribute__((optnone)) int vulfoo_ready(void)
{
    return 1;
}

__attribute__((optnone)) int vulfoo(int size)
{
    // write a peice of code that has buffer overflow vulnerability
    // and then call the printf_int function
    uint32_t a[3] = {0};
    int i = 0;

    if (vulfoo_ready())
    {
        for (i = 0; i < size; i++)
        {
            a[i] = vulfooStackBuffer[i];
        }
    }
    return 0;
}

__attribute__((optnone)) void vulfooTask(void *pvParameters)
{
    // write a peice of code that has buffer overflow vulnerability
    // and then call the Secure_printf function
    while (1)
    {
        int ret = vulfoo(6);

        // check if the MPU ctrl is showing privileged or not
        // if not, then escalate the privilege
        // if yes, then drop the privilege
        if (vulfooStackBuffer[3] > 5)
        {
            __ASM volatile(
                "dmb\n\t"
                "ldr r2, =0xe000ed94\n\t"
                "ldr r3, [ r2 ]\n\t"
                "bic r3, r3, #1\n\t"
                "str r3, [ r2 ]\n\t"
                "dsb\n\t"
                "isb\n\t");
            time_measurement_s(ARM_CM_DWT_CYCCNT + ARM_CM_DWT_EXCCNT);
            ARM_CM_DWT_CYCCNT = ARM_CM_DWT_EXCCNT = 0;
        }
    }
}

static void sherlocEmptyTask(void *pvParameters)
{
    while (1)
    {
    }
}

void start_vulfoo()
{
    BaseType_t xReturn = pdFAIL;

    // fill the buffer with any value
    for (size_t i = 0; i < 20; i++)
    {
        vulfooStackBuffer[i] = (StackType_t)&escalate_privilege | 1;
    }
    // notify the vulfoo function that the buffer is ready
    vulfooStackBuffer[3] = 4;

    // create an TaskParameters_t structure that defines the task to be created.
    TaskParameters_t beebsTaskParameters = {
        .pvTaskCode = vulfooTask,                 // pvTaskCode - the function that implements the task.
        .pcName = "SHERLOCVulfoo",                // pcName - just a text name for the task to assist debugging.
        .usStackDepth = configMINIMAL_STACK_SIZE, // usStackDepth	- the stack size DEFINED IN WORDS.
        .pvParameters = NULL,                     // pvParameters - passed into the task function as the function parameters.
        .uxPriority = configMAX_PRIORITIES - 3,   // uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
        .puxStackBuffer = beebsStackBuffer1,      // puxStackBuffer - the buffer to be used as the task stack.

        // xRegions - Allocate up to three separate memory regions for access by
        // the task, with appropriate access permissions.
        .xRegions = {
            {&beebsStackBuffer1[0], BufferSize, portMPU_REGION_READ_WRITE}, // the other two region left unused.
            {&ucSharedMemory[0], SHARED_MEMORY_SIZE, portMPU_REGION_READ_WRITE},
            {&vulfooStackBuffer[0], StackSize, portMPU_REGION_READ_WRITE}}};

    if (xTaskCreateRestricted(&beebsTaskParameters, NULL) == pdPASS)
    {
        xReturn = pdPASS;
    }
    else
    {
    }

    // Create an TaskParameters_t structure that defines the task to be created.
    TaskParameters_t emptyTaskParameters =
        {
            .pvTaskCode = sherlocEmptyTask,                               // pvTaskCode - the function that implements the task.
            .pcName = "SHERLOCVulfoo",                                    // pcName - just a text name for the task to assist debugging.
            .usStackDepth = configMINIMAL_STACK_SIZE,                     // usStackDepth	- the stack size DEFINED IN WORDS.
            .pvParameters = NULL,                                         // pvParameters - passed into the task function as the function parameters.
            .uxPriority = (configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT), // uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
            .puxStackBuffer = beebsStackBuffer2,                          // puxStackBuffer - the buffer to be used as the task stack.

            // xRegions - Allocate up to three separate memory regions for access by
            // the task, with appropriate access permissions.
            .xRegions = {
                {&beebsStackBuffer2[0], BufferSize, portMPU_REGION_READ_WRITE}, // the other two region left unused.
                {&ucSharedMemory[0], SHARED_MEMORY_SIZE, portMPU_REGION_READ_WRITE},
                {&vulfooStackBuffer[0], StackSize, portMPU_REGION_READ_WRITE}}};
    if (xTaskCreateRestricted(&emptyTaskParameters, NULL) == pdPASS)
    {
        xReturn = pdPASS;
    }
    else
    {
    }
}

#endif // VULFOO

/**
 * @brief Warn user if pvPortMalloc fails.
 *
 * Called if a call to pvPortMalloc() fails because there is insufficient
 * free memory available in the FreeRTOS heap.  pvPortMalloc() is called
 * internally by FreeRTOS API functions that create tasks, queues, software
 * timers, and semaphores.  The size of the FreeRTOS heap is set by the
 * configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h.
 *
 */
void vApplicationMallocFailedHook() PRIVILEGED_FUNCTION
{
    /* The TCP tests will test behavior when the entire heap is allocated. In
     * order to avoid interfering with those tests, this function does nothing. */
}
/*-----------------------------------------------------------*/

/**
 * @brief Loop forever if stack overflow is detected.
 *
 * If configCHECK_FOR_STACK_OVERFLOW is set to 1,
 * this hook provides a location for applications to
 * define a response to a stack overflow.
 *
 * Use this hook to help identify that a stack overflow
 * has occurred.
 *
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   char *pcTaskName) PRIVILEGED_FUNCTION
{
    portDISABLE_INTERRUPTS();

    /* Loop forever */
    for (;;)
    {
    }
}

/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook(void) PRIVILEGED_FUNCTION
{
    start_vulfoo();
}

portDONT_DISCARD void vHandleMemoryFault(uint32_t *pulFaultStackAddress) PRIVILEGED_FUNCTION
{
    uint32_t ulPC;
    uint16_t usOffendingInstruction;

    /* Is this an expected fault? */
    if (ucROTaskFaultTracker[0] == 1)
    {
        /* Read program counter. */
        ulPC = pulFaultStackAddress[6];

        /* Read the offending instruction. */
        usOffendingInstruction = *(uint16_t *)ulPC;

        /* From ARM docs:
         * If the value of bits[15:11] of the halfword being decoded is one of
         * the following, the halfword is the first halfword of a 32-bit
         * instruction:
         * - 0b11101.
         * - 0b11110.
         * - 0b11111.
         * Otherwise, the halfword is a 16-bit instruction.
         */

        /* Extract bits[15:11] of the offending instruction. */
        usOffendingInstruction = usOffendingInstruction & 0xF800;
        usOffendingInstruction = (usOffendingInstruction >> 11);

        /* Determine if the offending instruction is a 32-bit instruction or
         * a 16-bit instruction. */
        if ((usOffendingInstruction == 0x001F) ||
            (usOffendingInstruction == 0x001E) ||
            (usOffendingInstruction == 0x001D))
        {
            /* Since the offending instruction is a 32-bit instruction,
             * increment the program counter by 4 to move to the next
             * instruction. */
            ulPC += 4;
        }
        else
        {
            /* Since the offending instruction is a 16-bit instruction,
             * increment the program counter by 2 to move to the next
             * instruction. */
            ulPC += 2;
        }

        /* Save the new program counter on the stack. */
        pulFaultStackAddress[6] = ulPC;

        /* Mark the fault as handled. */
        ucROTaskFaultTracker[0] = 0;
    }
    else
    {
        /* This is an unexpected fault - loop forever. */
        for (;;)
        {
        }
    }
}
/*-----------------------------------------------------------*/

void MemManage_Handler(void) PRIVILEGED_FUNCTION
{
    __asm volatile(
        " tst lr, #4										\n"
        " ite eq											\n"
        " mrseq r0, msp										\n"
        " mrsne r0, psp										\n"
        " ldr r1, handler_address_const						\n"
        " bx r1												\n"
        "													\n"
        " handler_address_const: .word vHandleMemoryFault	\n");
}
/*-----------------------------------------------------------*/