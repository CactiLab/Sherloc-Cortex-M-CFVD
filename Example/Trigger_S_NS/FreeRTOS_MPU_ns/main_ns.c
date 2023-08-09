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
#include "Secure_Functions.h"
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
#define BEEBS

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
static void sherlocMicroTask(void *pvParameters);
static void sherlocBeebsTask(void *pvParameters);
static void sherlocEmptyTask(void *pvParameters);

#define BufferSize configMINIMAL_STACK_SIZE * 2
static StackType_t microStackBuffer1[BufferSize];
static StackType_t microStackBuffer2[BufferSize];

static StackType_t beebsStackBuffer1[BufferSize];
static StackType_t beebsStackBuffer2[BufferSize];

void vApplicationDaemonTaskStartupHook(void);
static void SystemClock_Config(void);

/* system call functions */
void bench_start() __attribute__((noinline, section("freertos_system_calls")));
void bench_result() __attribute__((noinline, section("freertos_system_calls")));
/*----------------------------------------------------------*/

/* Non-secure main. */
int main(void)
{
    mtb_enable(MTB_ALIAS_MAX);
    ARM_CM_DWT_CYCCNT = ARM_CM_DWT_EXCCNT = 0;
    LED_Initialize(); /* Initializ LEDs */

    /* start the schedule */
    vTaskStartScheduler();

    /* Should not reach here as the scheduler is already started. */
    for (;;)
    {
    }
}
/*-----------------------------------------------------------*/

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

#ifdef BEEBS
__attribute__((optnone)) static void sherlocBeebsTask(void *pvParameters)
{
    int i, local_count = 5;
    int result = 0;
    uint32_t LEDOn = 1;
    uint32_t LEDOff = 0;
    uint32_t ledMax = LED_GetCount() - 1;
    uint32_t ledNum = 0;
    int32_t dir = 1;

    while (local_count--)
    {
        ARM_CM_DWT_CYCCNT = ARM_CM_DWT_EXCCNT = 0;

        for (i = 0; i < REPEAT_FACTOR; i++)
        {
            initialise_benchmark_crc32();
            result = benchmark_crc32();
        }

        if (verify_benchmark_crc32(result))
        {
            time_measurement_s(ARM_CM_DWT_CYCCNT + ARM_CM_DWT_EXCCNT);
        }
        else
        {
            time_measurement_s(ARM_CM_DWT_CYCCNT + ARM_CM_DWT_EXCCNT);
        }
				
        if (LEDOn)
        {
            LEDOn = 0;
            LED_On(ledNum); /* Turn specified LED on */
        }

        else
        {
            LEDOn = 1;
            LED_Off(ledNum); /* Turn specified LED off */

            ledNum = (uint32_t)((int32_t)ledNum + dir); /* Change LED number */

            if (dir == 1 && ledNum == ledMax)
            {
                dir = -1; /* Change direction to down */
            }
            else if (ledNum == 0)
            {
                dir = 1; /* Change direction to up */
            }
        }
    }

    for (;;)
    {
    }
}

static void sherlocEmptyTask(void *pvParameters)
{
    while (1)
    {
    }
}

void start_beebsbenchmark()
{
    BaseType_t xReturn = pdFAIL;

    // create an TaskParameters_t structure that defines the task to be created.
    TaskParameters_t beebsTaskParameters = {
        .pvTaskCode = sherlocBeebsTask,                               // pvTaskCode - the function that implements the task.
        .pcName = "BEEBSBenchmark",                                   // pcName - just a text name for the task to assist debugging.
        .usStackDepth = configMINIMAL_STACK_SIZE,                     // usStackDepth	- the stack size DEFINED IN WORDS.
        .pvParameters = NULL,                                         // pvParameters - passed into the task function as the function parameters.
        .uxPriority = (configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT), // uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
        .puxStackBuffer = beebsStackBuffer1,                          // puxStackBuffer - the buffer to be used as the task stack.

        // xRegions - Allocate up to three separate memory regions for access by
        // the task, with appropriate access permissions.
        .xRegions = {
            {&beebsStackBuffer1[0], BufferSize, portMPU_REGION_READ_WRITE}, // the other two region left unused.
            {(void *)0x28204980, 0x1a0, portMPU_REGION_READ_WRITE},
            {&ucSharedMemory[0], SHARED_MEMORY_SIZE, portMPU_REGION_READ_WRITE}}};

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
            .pcName = "BEEBSBenchmark",                                   // pcName - just a text name for the task to assist debugging.
            .usStackDepth = configMINIMAL_STACK_SIZE,                     // usStackDepth	- the stack size DEFINED IN WORDS.
            .pvParameters = NULL,                                         // pvParameters - passed into the task function as the function parameters.
            .uxPriority = (configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT), // uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
            .puxStackBuffer = beebsStackBuffer2,                          // puxStackBuffer - the buffer to be used as the task stack.

            // xRegions - Allocate up to three separate memory regions for access by
            // the task, with appropriate access permissions.
            .xRegions = {
                {&beebsStackBuffer2[0], BufferSize, portMPU_REGION_READ_WRITE}, // the other two region left unused.
                {0, 0, 0},
                {0, 0, 0}}};
    if (xTaskCreateRestricted(&emptyTaskParameters, NULL) == pdPASS)
    {
        xReturn = pdPASS;
    }
    else
    {
    }
}
#endif // BEEBS

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
    start_beebsbenchmark();
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