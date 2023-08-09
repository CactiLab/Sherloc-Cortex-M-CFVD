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
 * @brief The hard fault handler.
 *
 * It calls a function called vHandleMemoryFault.
 */
void MemManage_Handler(void) __attribute__((naked));
/*-----------------------------------------------------------*/

/*------------------------------------------------*/

/* handlers */
TaskHandle_t xTaskHandle_vulfoo;
TaskHandle_t xTaskHandle_victim;
/*----------------------------------------------------------*/

/**
 * @brief Create all demo tasks.
 */
// void sherlocSyscall() FREERTOS_SYSTEM_CALL;
static void sherlocMicroTask(void *pvParameters);
static void sherlocVictimTask(void *pvParameters);

#define BufferSize configMINIMAL_STACK_SIZE * 2
#define StackSize configMINIMAL_STACK_SIZE * 4
static StackType_t victimStackBuffer[BufferSize];
static StackType_t vulfooStackBuffer[BufferSize];
static StackType_t sharedStackBuffer[BufferSize];

void vApplicationDaemonTaskStartupHook(void);
static void SystemClock_Config(void);

/*----------------------------------------------------------*/

typedef struct tskTaskControlBlock /* The old naming convention is used to prevent breaking kernel aware debuggers. */
{
    volatile StackType_t *pxTopOfStack; /*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */

#if (portUSING_MPU_WRAPPERS == 1)
    xMPU_SETTINGS xMPUSettings; /*< The MPU settings are defined as part of the port layer.  THIS MUST BE THE SECOND MEMBER OF THE TCB STRUCT. */
#endif

    ListItem_t xStateListItem;                                                                                                     /*< The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
    ListItem_t xEventListItem;                                                                                                     /*< Used to reference a task from an event list. */
    UBaseType_t uxPriority;                                                                                                        /*< The priority of the task.  0 is the lowest priority. */
    StackType_t *pxStack;                                                                                                          /*< Points to the start of the stack. */
    char pcTaskName[configMAX_TASK_NAME_LEN]; /*< Descriptive name given to the task when created.  Facilitates debugging only. */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

#if ((portSTACK_GROWTH > 0) || (configRECORD_STACK_HIGH_ADDRESS == 1))
    StackType_t *pxEndOfStack; /*< Points to the highest valid address for the stack. */
#endif

#if (portCRITICAL_NESTING_IN_TCB == 1)
    UBaseType_t uxCriticalNesting; /*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
#endif

#if (configUSE_TRACE_FACILITY == 1)
    UBaseType_t uxTCBNumber;  /*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
    UBaseType_t uxTaskNumber; /*< Stores a number specifically for use by third party trace code. */
#endif

#if (configUSE_MUTEXES == 1)
    UBaseType_t uxBasePriority; /*< The priority last assigned to the task - used by the priority inheritance mechanism. */
    UBaseType_t uxMutexesHeld;
#endif

#if (configUSE_APPLICATION_TASK_TAG == 1)
    TaskHookFunction_t pxTaskTag;
#endif

#if (configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0)
    void *pvThreadLocalStoragePointers[configNUM_THREAD_LOCAL_STORAGE_POINTERS];
#endif

#if (configGENERATE_RUN_TIME_STATS == 1)
    configRUN_TIME_COUNTER_TYPE ulRunTimeCounter; /*< Stores the amount of time the task has spent in the Running state. */
#endif

#if (configUSE_NEWLIB_REENTRANT == 1)

    /* Allocate a Newlib reent structure that is specific to this task.
     * Note Newlib support has been included by popular demand, but is not
     * used by the FreeRTOS maintainers themselves.  FreeRTOS is not
     * responsible for resulting newlib operation.  User must be familiar with
     * newlib and must provide system-wide implementations of the necessary
     * stubs. Be warned that (at the time of writing) the current newlib design
     * implements a system-wide malloc() that must be provided with locks.
     *
     * See the third party link http://www.nadler.com/embedded/newlibAndFreeRTOS.html
     * for additional information. */
    struct _reent xNewLib_reent;
#endif

#if (configUSE_TASK_NOTIFICATIONS == 1)
    volatile uint32_t ulNotifiedValue[configTASK_NOTIFICATION_ARRAY_ENTRIES];
    volatile uint8_t ucNotifyState[configTASK_NOTIFICATION_ARRAY_ENTRIES];
#endif

/* See the comments in FreeRTOS.h with the definition of
 * tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE. */
#if (tskSTATIC_AND_DYNAMIC_ALLOCATION_POSSIBLE != 0) /*lint !e731 !e9029 Macro has been consolidated for readability reasons. */
    uint8_t ucStaticallyAllocated;                   /*< Set to pdTRUE if the task is a statically allocated to ensure no attempt is made to free the memory. */
#endif

#if (INCLUDE_xTaskAbortDelay == 1)
    uint8_t ucDelayAborted;
#endif

#if (configUSE_POSIX_ERRNO == 1)
    int iTaskErrno;
#endif
};

/* Non-secure main. */
int main(void)
{
    mtb_enable(MTB_ALIAS_MAX);

    /* start the schedule */
    vTaskStartScheduler();

    /* Should not reach here as the scheduler is already started. */
    for (;;)
    {
    }
}
/*-----------------------------------------------------------*/

#ifdef VULFOO

__attribute__((optnone)) int vulfoo()
{
    // write a peice of code that has buffer overflow vulnerability
    // and then call the printf_int function

    // We assume this task is a kernel task, and the attack already know the context of paused task. Thus, the attack can modify the re-entry address of the task.
    StackType_t *victimPConStack = (StackType_t *)0x28204768;
    // we set a flag to notify the attack that he can proceed with attacks. The benign value is 0.
    if (sharedStackBuffer[0])
    {
        // the stack is filled with patterns with initialization, we need to wait for this stack is used by the victim task. 
        if (victimStackBuffer[114] != 0x08080808)
        {
            // modify the re-entry address of the victim task
            *victimPConStack = 0x0020264c;
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
        vulfoo();
    }
}

__attribute__((optnone)) static void sherlocVictimTask(void *pvParameters)
{
    while (1)
    {
        StackType_t *TopOfStack;
        TopOfStack = xTaskHandle_victim->pxTopOfStack;
    }
}

void start_vulfoo()
{
    BaseType_t xReturn = pdFAIL;

    sharedStackBuffer[0] = 1;

    // create an TaskParameters_t structure that defines the task to be created.
    TaskParameters_t vulfooTaskParameters = {
        .pvTaskCode = vulfooTask,                                     // pvTaskCode - the function that implements the task.
        .pcName = "SHERLOCVulfoo",                                    // pcName - just a text name for the task to assist debugging.
        .usStackDepth = configMINIMAL_STACK_SIZE,                     // usStackDepth	- the stack size DEFINED IN WORDS.
        .pvParameters = NULL,                                         // pvParameters - passed into the task function as the function parameters.
        .uxPriority = (configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT), // uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
        .puxStackBuffer = vulfooStackBuffer,                          // puxStackBuffer - the buffer to be used as the task stack.

        // xRegions - Allocate up to three separate memory regions for access by
        // the task, with appropriate access permissions.
        .xRegions = {
            {&vulfooStackBuffer[0], BufferSize, portMPU_REGION_READ_WRITE}, // the other two region left unused.
            {&victimStackBuffer[0], BufferSize, portMPU_REGION_READ_WRITE},
            {&sharedStackBuffer[0], BufferSize, portMPU_REGION_READ_WRITE}}};

    if (xTaskCreateRestricted(&vulfooTaskParameters, &xTaskHandle_vulfoo) == pdPASS)
    {
        xReturn = pdPASS;
    }
    else
    {
    }

    // Create an TaskParameters_t structure that defines the task to be created.
    TaskParameters_t victimTaskParameters =
        {
            .pvTaskCode = sherlocVictimTask,                              // pvTaskCode - the function that implements the task.
            .pcName = "SHERLOCVulfoo",                                    // pcName - just a text name for the task to assist debugging.
            .usStackDepth = configMINIMAL_STACK_SIZE,                     // usStackDepth	- the stack size DEFINED IN WORDS.
            .pvParameters = NULL,                                         // pvParameters - passed into the task function as the function parameters.
            .uxPriority = (configMAX_PRIORITIES - 3 | portPRIVILEGE_BIT), // uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
            .puxStackBuffer = victimStackBuffer,                          // puxStackBuffer - the buffer to be used as the task stack.

            // xRegions - Allocate up to three separate memory regions for access by
            // the task, with appropriate access permissions.
            .xRegions = {
                {&victimStackBuffer[0], BufferSize, portMPU_REGION_READ_WRITE}, // the other two region left unused.
                {0, 0, 0},
                {0, 0, 0}}};
    if (xTaskCreateRestricted(&victimTaskParameters, &xTaskHandle_victim) == pdPASS)
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
#if defined(VULFOO)
    start_vulfoo();
#endif
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