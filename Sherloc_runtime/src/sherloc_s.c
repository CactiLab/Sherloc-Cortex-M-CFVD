#include "../inc/sherloc_s.h"

static char msg[MAXMSG];
volatile uint32_t mtb_buff[1024] __attribute__((section(".ARM.__at_0x24000000")));
static ST STACK[MAXTASK];
static TASK_ST TASK_LIST;
uint32_t finish_time = 0;
static uint32_t debug_mon_count = 0, cfi_test = 0;
static uint32_t in = 0, out = 0;
volatile uint32_t t_ibt_query = 0, t_ibt_query_start = 0, t_ibt_query_end = 0;
static int IRQ_count = 0, NESTED_IRQ_count = 0, PendSV_count = 0, test_count = 0;
uint32_t task_flag = 0; // a flag to show if it is an idirect call, 0: direct, 1: bx/blx, 2: pop, 3: pc changes
                        // if it is an PendSV entry, push special value 0xDEADBEEF
uint32_t sherloc_detection = 0, sherloc_hit = 0, buffer_read = 0, crs_creation = 0, ins_identification = 0;
uint32_t sherloc_read, sherloc_enter, sherloc_exit, sherloc_detection_start, sherloc_detection_end, buffer_read_start, buffer_read_end, ins_check_start, ins_check_end, crs_start, crs_end;

#if defined(EVAL_RT)
uint32_t ic_count = 0, ib_count = 0, dc_count = 0, db_count = 0, ret_count = 0, irq_count = 0;
#endif

static void task_stack_init()
{
    for (size_t i = 0; i < MAXTASK; i++)
    {
        memset(&STACK[i], 0, sizeof(ST));
        STACK[task_flag].top_pos = STACK_FULL;
        STACK[task_flag].top = STACK_EMPTY;
    }
}

static void task_list_init()
{
    memset(&TASK_LIST, 0, sizeof(TASK_ST));
    TASK_LIST.num = 0;
}

#if defined(TRIGGER) || defined(SS_IRQ) || defined(INS_IDENTIFY_IRQ) || defined(READ_IRQ) || defined(USE_SYSTICK_NS)
void printf_int(int num) __attribute__((cmse_nonsecure_entry));
void printf_int(int num)
{
    printf("You hacked me! %d\r\n", num);
}
#endif

int32_t time_measurement_s(uint32_t time) __attribute__((cmse_nonsecure_entry));
int32_t time_measurement_s(uint32_t time)
{
    uint32_t time_tmp = 0;
    finish_time++;

    // disable MTB
    MTB->MASTER &= ~(1 << 31);
    MTB->FLOW = 0;
    // clear haltreq bit
    MTB->MASTER &= ~(1 << 9);

    // if defined IRQ, disable non-secure systick interrupt
#if defined(SS_IRQ) || defined(INS_IDENTIFY_IRQ) || defined(READ_IRQ) || defined(USE_SYSTICK_NS)
    // disable non-secure systick interrupt
    disable_SysTick_ns();
#endif // IRQ

    __asm volatile("dsb\n"
                   "isb\n");

    time_tmp = time;
    // sherloc_detection is the detection time for running SHERLOC, depends on the macro, it can be ss, buffer_read, or ins identification
#if defined(TEST_CFI)
    printf("EVAL: %u, enter_exit: %u", time + sherloc_detection, time);

#if defined(EVAL_READ)
    printf(", sherloc_read: %u, sherloc_read_garbage: %u\r\n", sherloc_detection, sherloc_read);
    sherloc_detection = sherloc_read = 0;

#elif defined(EVAL_READ_NEW)
    printf(", sherloc_read_new: %u, sherloc_read_garbage: %u\r\n", sherloc_detection, sherloc_read);
    sherloc_detection = sherloc_read = 0;

#elif defined(EVAL_INS_IDENTIFY)
    printf(", sherloc_ins_identification: %u, ins_identification_garbage: %u\r\n", sherloc_detection, ins_identification);
    sherloc_detection = ins_identification = 0;

#elif defined(EVAL_HIT)
    printf(", sherloc_hit_cycles: %u, sherloc_hit: %u\r\n", sherloc_detection, sherloc_hit);
    sherloc_detection = sherloc_hit = 0;

#elif defined(EVAL_RT)
    printf(", ins_rt: %u, ic_count: %u, ib_count: %u, dc_count: %u, db_count: %u, ret_count: %u, irq_count: %u\r\n", sherloc_detection, ic_count, ib_count - ic_count, dc_count, db_count, ret_count, irq_count);
    sherloc_detection = ic_count = ib_count = dc_count = db_count = ret_count = irq_count = 0;

#elif defined(IBT_BASELINE_NO_IRQ) || defined(IBT_NO_IRQ) || defined(IBT_BASELINE_IRQ) || defined(IBT_IRQ)
    printf(", ibt_check: %u, ins_identification_garbage: %u\r\n", sherloc_detection, ins_identification);
    sherloc_detection = ins_identification = 0;

#elif defined(EVAL_SHERLOC_SPLIT)
    printf(", sherloc_detection: %u\r\n", sherloc_detection);
    sherloc_detection = 0;
#endif // TEST_CFI

#elif defined(TRIGGER)
    printf("Trigger: %u, enter_exit: %u", time + sherloc_detection, time);

#if defined(TRIGGER_READ_BUFF)
    printf(", sherloc_read: %u, sherloc_read_garbage: %u", sherloc_detection, sherloc_read);
    sherloc_detection = sherloc_read = 0;

#elif defined(TRIGGER_INS)
    printf(", sherloc_ins_identification: %u, ins_identification_garbage: %u", sherloc_detection, ins_identification);
    sherloc_detection = ins_identification = 0;

#endif // TRIGGER
    sherloc_detection = 0;

#else
    printf("NONE: %u&\r\n", time);
#endif // TEST_CFI

#if defined(SS_IRQ) || defined(INS_IDENTIFY_IRQ) || defined(READ_IRQ) || defined(USE_SYSTICK_NS) || defined(TRIGGER)
    printf("&\r\n");

#else
    if (finish_time == 5)
    {
        finish_time = 0;
        printf("F\r\n");
    }
#endif

    // enable MTB  0xFC3
#ifdef WATERMARK
    MTB->FLOW = WATERMARK_VALUE;
#endif

    // if defined IRQ, enable non-secure systick interrupt
#if defined(SS_IRQ) || defined(FULL_EMU) || defined(FULL_IRQ) || defined(USE_SYSTICK_NS)
    // enable non-secure systick interrupt and set the frequency to 100ms
    enable_SysTick_ns();
#endif // IRQ

    MTB->MASTER |= (1 << 31);
    DWT->CYCCNT = 0;
    __asm volatile("dsb\n"
                   "isb\n");

    return 0;
}

volatile void *TB __attribute__((section(".ARM.__at_0x30000100")));
volatile BtHeader *pstBt = (BtHeader *)&TB;

int LoadBranchTable()
{
    printf("branch table size: %d, IRQ size: %d, Task size: %d, LOOP size: %d, tag_size: %d\r\n", pstBt->IBT_size, pstBt->IRQ_size, pstBt->TASK_size, pstBt->LOOP_size, pstBt->INS_tag_size);
    pstBt->IBT_entry = (uint32_t *)&TB + IBT_OFFSET;
    pstBt->IRQ_entry = (uint32_t *)&TB + IBT_OFFSET + pstBt->IBT_size;
    pstBt->TASK_entry = (uint32_t *)&TB + IBT_OFFSET + pstBt->IBT_size + pstBt->IRQ_size;
    pstBt->LOOP_entry = (uint32_t *)&TB + IBT_OFFSET + pstBt->IBT_size + pstBt->IRQ_size + pstBt->TASK_size * 2;

#ifdef SHOW_RECORD
    printf("PendSV_entry: 0x%08x\r\n", pstBt->PendSV);
    for (int i = 0; i < pstBt->IBT_size; i++)
    {
        printf("IBranches: 0x%08x\r\n", pstBt->IBT_entry[i]);
    }
    for (int i = 0; i < pstBt->IRQ_size; i++)
    {
        printf("IRQs: 0x%08x\r\n", pstBt->IRQ_entry[i]);
    }
    for (int i = 0; i < pstBt->TASK_size * 2; i += 2)
    {
        printf("Tasks: 0x%08x, 0x%08x\r\n", pstBt->TASK_entry[i], pstBt->TASK_entry[i + 1]);
    }
    for (int i = 0; i < pstBt->LOOP_size; i++)
    {
        printf("Loops: 0x%08x\r\n", pstBt->LOOP_entry[i]);
    }
#endif
    printf("\r\n----Load branch table done.---\r\n");
    return 0;
}

ALWAYS_INLINE int LOOP_QUERY(uint32_t record)
{
    return binary_search(record, pstBt->LOOP_entry, pstBt->LOOP_size - 1);
}

ALWAYS_INLINE int IRQ_QUERY(uint32_t record)
{
    return binary_search(record, pstBt->IRQ_entry, pstBt->IRQ_size - 1);
}

/*----------------------------------- TASK_ID ----------------------------------*
 * take an address as input, check whether it is in the task list
 *--------------------------------------------------------------------------------*/

ALWAYS_INLINE int TASK_QUERY(uint32_t record)
{
    int mid = 0;
    int l = 0;
    int r = pstBt->TASK_size * 2;

    while (l <= r)
    {
        mid = l + (r - l) / 2;
        if (record == pstBt->TASK_entry[mid])
        {
            return mid;
        }
        else if (record < pstBt->TASK_entry[mid])
            r = mid - 1;
        else
            l = mid + 1;
    }
    return -1;
}

/*----------------------------------- TASK_ID ----------------------------------*
 * take an address as input, return which task it belongs to
 *--------------------------------------------------------------------------------*/

ALWAYS_INLINE int TASK_ID(uint32_t record)
{
    task_flag = 0;
    for (uint32_t i = 0; i < pstBt->TASK_size * 2; i += 2)
    {
        if ((record >= pstBt->TASK_entry[i]) && (record <= pstBt->TASK_entry[i + 1]) && (i % 2 == 0))
        {
#ifdef MULTI_STACK
            // 0 is the main stack for kernel
            task_flag = i / 2 + 1;
#endif // multi-stack
        }
    }
    return task_flag;
}

/*----------------------------------- IBT_query ----------------------------------*
 * take the address pair as input, binary search the branch table,
 * if find the branch, return true, else return false
 *--------------------------------------------------------------------------------*/

ALWAYS_INLINE int IBT_QUERY(uint32_t src, uint32_t dst)
{
    // if the binary serach find the branch, return true, else return false
    return binary_search(CONCACT_UINT32(src, dst), pstBt->IBT_entry, pstBt->IBT_size - 1);
}

/* IRQ_in hase two cases: normal IRQ and PendSV
    Normal IRQ: store interrupted position
    PendSV: push TASK_FLAG (0xaaaabbbb)
*/

ALWAYS_INLINE int check_IRQ_in(uint32_t src_addr, uint32_t dst_addr)
{
    // if it is an IRQ in, push the interrupted address into the STACK
    if (IRQ_QUERY(dst_addr) > -1)
    {
        if (STACK_push(&STACK[task_flag], src_addr & LAST_BIT_CLEAR) < 0)
        {
            printf("[IRQ] stack is full!! top: %d\r\n", STACK[task_flag].top_pos);
            STACK_check_all(&STACK[task_flag]);
            dummy_handler();
            return -1;
        }

#ifdef LOG_IRQ
        printf("[%ld] non-secure IRQ in: (0x%08x, 0x%08x)\r\n", task_flag, src_addr, dst_addr);
        STACK_check_all(&STACK[task_flag]);
        // TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        IRQ_count++;
        return IRQ_IN;
    }
#ifdef RTOS
    else if (dst_addr == pstBt->PendSV)
    {
        // store the interrupted task by pendsv
        if (TASK_insert(&TASK_LIST, src_addr & LAST_BIT_CLEAR) < 0)
        {
            printf("[PendSV] task list is full! num: %d.\r\n", TASK_LIST.num);
            return -1;
        }

        // store the previous tasks to task list and clear it from current stack
        // we need to check if current stack has a value, if it has, store to the task list
        if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }

#ifdef LOG_IRQ
        printf("[%ld] PendSV in: 0x%08lx, 0x%08lx\r\n", task_flag, src_addr, dst_addr);
        STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        IRQ_count++;
        PendSV_count++;
        return PendSV_IN;
    }
#endif

    return -1;
}

ALWAYS_INLINE int check_IRQ_in_forward_edge(uint32_t src_addr, uint32_t dst_addr)
{
    // if it is an IRQ in, push the interrupted address into the STACK
    // printf("check_IRQ_in: (0x%08lx, 0x%08lx)\r\n", src_addr, dst_addr);
    if (IRQ_QUERY(dst_addr) > -1)
    {
#ifdef LOG_IRQ
        printf("[%ld] non-secure IRQ in: (0x%08lx, 0x%08lx)\r\n", task_flag, src_addr, dst_addr);
        STACK_check_all(&STACK[task_flag]);
        // TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        IRQ_count++;
        return IRQ_IN;
    }
#ifdef RTOS
    else if (dst_addr == pstBt->PendSV)
    {
        // store the interrupted task by pendsv
        if (TASK_insert(&TASK_LIST, src_addr & LAST_BIT_CLEAR) < 0)
        {
            printf("[PendSV] task list is full! num: %d.\r\n", TASK_LIST.num);
            return -1;
        }

        // store the previous tasks to task list and clear it from current stack
        // we need to check if current stack has a value, if it has, store to the task list
        if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }

#ifdef LOG_IRQ
        printf("[%ld] PendSV in: 0x%08lx, 0x%08lx\r\n", task_flag, src_addr, dst_addr);
        STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        IRQ_count++;
        PendSV_count++;
        return PendSV_IN;
    }
#endif

    return -1;
}

/*
IRQ_out has three cases:
Normal IRQ_OUT: check if the returned position is the interrupted one
PendSV: assume new task, just pop TASK_FLAG
Nested IRQ_IN: check if the the returned position is another IRQ entry
*/

ALWAYS_INLINE int check_IRQ_out(uint32_t src_addr, uint32_t dst_addr, int idx)
{
    // we only check the exit that target to the non-secure state
    uint32_t top = STACK[task_flag].top;
    // if it is a normal IRQ out after existing Debug monitor handler
#ifdef LOG_IRQ
    if (mtb_buff[idx - 2] & ~(1 << 0) < CODE_NS)
    {
        printf("check if it is a normal IRQ out: 0x%08lx, 0x%08lx, top: 0x%08lx\r\n", src_addr, dst_addr, top);
    }
#endif // LOG_IRQ

    if (dst_addr == top)
    {
        STACK_pop(&STACK[task_flag]);
#ifdef LOG_IRQ
        if (mtb_buff[idx - 2] & ~(1 << 0) < CODE_NS)
        {
            printf("[%ld] IRQ out: (0x%08lx, 0x%08lx), ( 0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
            STACK_check_all(&STACK[task_flag]);
            TASK_check_all(&TASK_LIST);
        }
#endif // LOG_IRQ
        return IRQ_OUT;
    }

    // nested non-secure IRQ
#ifdef LOG_IRQ
    printf("check nested non-secure IRQ.\r\n");
#endif // LOG_IRQ

    if (IRQ_QUERY(dst_addr) > -1)
    {
#ifdef LOG_IRQ
        printf("[%ld] Nested non-secure IRQ in: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx)\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
        STACK_check_all(&STACK[task_flag]);
#endif // LOG_IRQ
        IRQ_count++;
        NESTED_IRQ_count++;
        return NESTED_IRQ_IN;
    }

    // task scheduling exit
#ifdef RTOS
    // task schedule in
    if (dst_addr == pstBt->PendSV)
    {
        // store the interrupted pendsv tasks
        if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }
        // if the current stack is not empty, pop it and store to the task list
        if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }

#ifdef LOG_IRQ
        printf("[%ld] Nested PendSV in: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx)\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
        STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        IRQ_count++;
        // NESTED_PendSV_count++;
        PendSV_count++;
        return PendSV_IN;
    }
    // if the target address is another task
    else if (TASK_check(&TASK_LIST, dst_addr) > -1)
    {
#ifdef MULTI_STACK
        TASK_ID(dst_addr);
#endif

#ifdef LOG_IRQ
        printf("[%ld] PendSV out: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        return PendSV_OUT;
    }
    // PendSV may return to an address that does not saved in the stack, which can be a new scheduled task
    else if (TASK_QUERY(dst_addr) > -1)
    {
        // check if it is an idle task and it is already inside the task list. Then we ignore it.
        if (binary_search(dst_addr, pstBt->LOOP_entry, pstBt->LOOP_size - 1))
        {
        }
        // Or we store the interrupted pendsv tasks
        else if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }
        // we need to check if current stack has a value, if it has, store to the task list
        if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }

#ifdef LOG_IRQ
        printf("[%ld] Old task stacks:\r\n", task_flag);
        STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

#ifdef MULTI_STACK
        TASK_ID(dst_addr);
        STACK[task_flag].top_pos = STACK_FULL;
#endif // multi-stack

#ifdef LOG_IRQ
        printf("[%ld] New task scheduled: (0x%08lx, 0x%08lx)\r\n", task_flag, src_addr, dst_addr);
        // STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        return PendSV_OUT;
    }
#endif

    return -1;
}

ALWAYS_INLINE int check_IRQ_out_forward_edge(uint32_t src_addr, uint32_t dst_addr, int idx)
{
    // we only check the exit that target to the non-secure state
    uint32_t top = STACK[task_flag].top;
    // if it is a normal IRQ out after existing Debug monitor handler
#ifdef LOG_IRQ
    if (mtb_buff[idx - 2] & ~(1 << 0) < CODE_NS)
        printf("check if it is a normal IRQ out: 0x%08lx, 0x%08lx, top: 0x%08lx\r\n", src_addr, dst_addr, top);
#endif // LOG_IRQ

    if (dst_addr == top)
    {
        STACK_pop(&STACK[task_flag]);
#ifdef LOG_IRQ
        if (mtb_buff[idx - 2] & ~(1 << 0) < CODE_NS)
        {        
            printf("[%ld] IRQ out: (0x%08lx, 0x%08lx), ( 0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
            STACK_check_all(&STACK[task_flag]);
            TASK_check_all(&TASK_LIST);
        }
#endif // LOG_IRQ
        return IRQ_OUT;
    }

    // nested non-secure IRQ
    if (IRQ_QUERY(dst_addr) > -1)
    {
#ifdef LOG_IRQ
        printf("[%ld] Nested non-secure IRQ in: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx)\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
        STACK_check_all(&STACK[task_flag]);
#endif // LOG_IRQ

        IRQ_count++;
        NESTED_IRQ_count++;
        return NESTED_IRQ_IN;
    }
    // task scheduling exit
#ifdef RTOS
    // task schedule in
    if (dst_addr == pstBt->PendSV)
    {
        // store the interrupted pendsv tasks
        if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }
        // if the current stack is not empty, pop it and store to the task list
        if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }

#ifdef LOG_IRQ
        printf("[%ld] Nested PendSV in: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx)\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
        STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        IRQ_count++;
        PendSV_count++;
        return PendSV_IN;
    }
    // if the target address is another task
    else if (TASK_check(&TASK_LIST, dst_addr) > -1)
    {

#ifdef MULTI_STACK
        TASK_ID(dst_addr);
#endif

#ifdef LOG_IRQ
        printf("[%ld] PendSV out: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
        // STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        return PendSV_OUT;
    }
    // PendSV may return to an address that does not saved in the stack, which can be a new scheduled task
    else if (TASK_QUERY(dst_addr) > -1)
    {
        // check if it is an idle task and it is already inside the task list. Then we ignore it.
        if (binary_search(dst_addr, pstBt->LOOP_entry, pstBt->LOOP_size - 1))
        {
        }
        // Or we store the interrupted pendsv tasks
        else if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }
        // we need to check if current stack has a value, if it has, store to the task list
        if (TASK_insert(&TASK_LIST, STACK_pop(&STACK[task_flag])) < 0)
        {
            printf("[PendSV] task list is full!\r\n");
            return -1;
        }

#ifdef LOG_IRQ
        printf("[%ld] Old task stacks:\r\n", task_flag);
        STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

#ifdef MULTI_STACK
        TASK_ID(dst_addr);
        STACK[task_flag].top_pos = STACK_FULL;
#endif // multi-stack

#ifdef LOG_IRQ
        printf("[%ld] New task scheduled: (0x%08lx, 0x%08lx)\r\n", task_flag, src_addr, dst_addr);
        // STACK_check_all(&STACK[task_flag]);
        TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

        return PendSV_OUT;
    }
#endif

    return -1;
}

/*----------------------------------- branch filtering -------------------------------------
Instruction	Decode					Hex
B			0b1110 0xxx xxxx xxxx 	0xexxx
BL			0b1111 0xxx xxxx xxxx	0xfxxx
BLX/BLXNS	0b0100 0111 1xxx xxxx	0x47xx
BX/BXNS		0b0100 0111 0xxx xxxx	0x47xx
*--------------------------------------------------------------------------------
[+] conditional branch:
         |4   |4		   | 8	     |4		  |	12		|
T1 	B: 0b1101 cond != 111x imm8								2-byte:	0b11010000~0b11011111 0xd0~0xdf
T2	B: 0b1110 0 		imm11								2-byte: 0b11100000~0b11100111 0xe0~0xe7
T3	B: 0b1111 0 S cond != 111x imm6  1 0 J1 0 J2 imm11
4-byte:	0b11110011 10000000 10000000 00000000~0b11110111 11111111 10101111 11111111 0xf380 8000~0xf7ff afff
T4	B: 0b1111 0 S 		imm10	     1 0 J1 1 J2 imm11
4-byte: 0b11110000 00000000 10010000 00000000~0b11110111 11111111 10111111 11111111 0xf000 9000~0xf7ff bfff
    conditions:
        0000 EQ
        0001 NE
        0010 CS
        0011 CC
        0100 MI
        0101 PL
        0110 VS
        0111 VC
        1000 HI
        1001 LS
        1010 GE
        1011 LT
        1100 GT
        1101 LE
        1110 None (AL)
CBNZ: 	0b1011 10i1		0xb9~0xbb
CBZ:	0b1011 00i1		0xb1~0xb3
B
*--------------------------------------------------------------------------------
[+] Branches and miscellaneous control
BL: 	0b1111 0
        0b1111 0 S imm10 11 J1 1 J2 imm11	0b11110000 00000000 11010000 00000000~0b11110111 11111111 11111111 11111111 0xf000 d000~0xf7ff ffff
f7ff 1111 0111 1111 1111 bl 0xxx
BLX: 	0b0100 01 11 1
BX:		0b0100 01 11 0
4770: 0100 0111 0111 0000 bx lr
4788: 0100 0111 1000 1000 blx r1

*--------------------------------------------------------------------------------
[+] Return
    BX LR: 0x4770
    POP {..., pc}: 0xbd
*--------------------------------------------------------------------------------
[+] Interruption in:
    <any, any>
*--------------------------------------------------------------------------------
[+] Interruption out
    BX EXC_RETURN (0xFFFFFF00~0xFFFFFFFF)
EXC_RETURN Value	Mode to Return To	Stack to use
0xFFFFFFF1	Handler Mode	MSP
0xFFFFFFF9	Thread Mode	MSP
0xFFFFFFFD	Thread Mode	PSP
0xFFFFFFE1	Handler Mode (FPU Extended Frame)	MSP
0xFFFFFFE9	Thread Mode (FPU Extended Frame)	MSP
0xFFFFFFED	Thread Mode (FPU Extended Frame)	PSP
*--------------------------------------------------------------------------------
TBB:	0b1110 1000 1101 xxxx	0xe8d0~0xe8df
e8df f001 	tbb	[pc, r1]
*/

#if defined(EVAL_INS_IDENTIFY)
void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx] & LAST_BIT_CLEAR;
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        // filter return, the dst should equal to the top of the stack
        if (dst_addr == STACK[task_flag].top)
        {
            ins_identification++;
            continue;
        }

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // read bytes one by one to get the mc of instruction
                // uint32_t src_ins = (uint32_t *)src_addr;
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter directal call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

                    ins_identification++;

                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
                        continue;
                    }

                    // bx Rx, check the IBT
                    ins_identification++;
                }
                ins_identification++;
            }
        }
    }
}

#elif defined(EVAL_READ)
void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        sherloc_read++;
    }
}

#elif defined(EVAL_READ_NEW)
void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        src_addr = mtb_buff[idx] & LAST_BIT_CLEAR;
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;
        sherloc_read += src_addr + dst_addr;
    }
}

#elif defined(EVAL_HIT)
void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        sherloc_read++;
        sherloc_hit++;
    }
}

#elif defined(EVAL_RT)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    STACK_pop(&STACK[task_flag]);
                    ret_count += 1;
#ifdef LOG_RET
                    printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    STACK_check_all(&STACK[task_flag]);
                    printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

                    continue;
                }
                if (src_addr & 0x1)
                {

                    if (check_IRQ_in(src_addr, dst_addr) > -1)
                    {
                        irq_count += 1;
                        continue;
                    }
#ifdef LOG_IRQ
                    else
                    {
                         printf("[%u]--no IRQ found: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    }
#endif
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    db_count += 1;
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    db_count += 1;
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        db_count += 1;
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    dc_count += 1;
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    ib_count += 1;
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if ((src_ins[0] != 0x70) && (IBT_QUERY(src_addr, dst_addr) < 0))
                    {
                        printf("[%d]!!! illegal indirect call: 0x%08x\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
                        ic_count += 1;

#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

#ifdef RTOS
                // it maybe a return from kernel functions, check the sorted task list
                // if the target address is another task
                else if (TASK_check(&TASK_LIST, dst_addr) > -1)
                {

#ifdef MULTI_STACK
                    TASK_ID(dst_addr);
#endif

#ifdef LOG_IRQ
                    printf("[%ld] return from kernel functions: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
                    // STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ
                    ret_count += 1;

                    continue;
                }
#endif // RTOS
       // other records, check the IBT
                if (IBT_QUERY(src_addr, dst_addr) < 0)
                {

#ifdef LOG_ERROR
                    printf("[%d]!!! illegal transfer: 0x%08x\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                    STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                    dummy_handler();
                }
                ib_count += 1;
                continue;
            }

            // secure debugmon_handler in
            else if ((dst_addr > CODE_S) && (dst_addr < 0x10004000))
            {
                if (STACK_push(&STACK[task_flag], src_addr & LAST_BIT_CLEAR) < 0)
                {
                    printf("[%u]Secure_IRQ stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                    STACK_check_all(&STACK[task_flag]);
                    dummy_handler();
                }

#ifdef LOG_IRQ_S
                printf("[%ld] Secure in: 0x%08lx, 0x%08lx\r\n", task_flag, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
#endif // LOG_IRQ

                continue;
            }
            continue;
        }
        // Check IRQ out
        else if ((src_addr > IRQ_EXIT) && (dst_addr < CODE_NS))
        {
            if (check_IRQ_out(src_addr, dst_addr, idx) == -1)
            {

#ifdef LOG_ERROR
                printf("[%u] Wrong IRQ exit: 0x%08x, 0x%08x, 0x%08x, 0x%08x\r\n", task_flag, mtb_buff[idx - 2] & LAST_BIT_CLEAR, mtb_buff[idx - 1] & LAST_BIT_CLEAR, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
                TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                dummy_handler();
            }
            continue;
        }

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(IBT_BASELINE_NO_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx] & LAST_BIT_CLEAR;
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // read bytes one by one to get the mc of instruction
                // uint32_t src_ins = (uint32_t *)src_addr;
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter bx/blx
                if (src_ins[1] == 0x47)
                {
                    // filter bx lr
                    if (src_ins[0] == 0x70)
                    {
                        continue;
                    }
                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
                        // bx/blx Rx
                        ins_identification++;
                        continue;
                    }

                    // bx Rx, check the IBT
                    ins_identification++;
                }
                ins_identification++;
            }
        }
    }
}

#elif defined(IBT_NO_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx] & LAST_BIT_CLEAR;
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter bx/blx
                if (src_ins[1] == 0x47)
                {
                    // filter bx lr
                    if (src_ins[0] == 0x70)
                    {
                        continue;
                    }
                    // blx Rx, store the address of next instruction
                    else if (src_ins[0] > 0x7f)
                    {
                        // bx/blx Rx
                        if (IBT_QUERY(src_addr, dst_addr) < 0)
                        {
#ifdef LOG_ERROR
                            printf("[%u]illegal bx/blx:0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                            STACK_check_all(&STACK[task_flag]);
                            TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR
                            dummy_handler();
                        }
                        continue;
                    }

                    // bx Rx
                    ins_identification++;
                }
                ins_identification++;
            }
        }
    }
}

#elif defined(IBT_BASELINE_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        // id_flag = 0;
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (src_addr & 0x1)
                {
                    continue;
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }

                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }

                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    // printf("src_ins[1]: 0x%02x, src_ins[0]: 0x%02x, src_ins[3]: 0x%02x, src_ins[2]: 0x%02x\r\n", src_ins[1], src_ins[0], src_ins[3], src_ins[2]);
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }
#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if (src_ins[0] != 0x70)
                    {
                        continue;
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET
                        continue;
                    }
                    continue;
                }

#ifdef RTOS
                // it maybe a return from kernel functions, check the sorted task list
                // if the target address is another task
#endif // RTOS
       // other records, check the IBT. May be the pop, ignore for forward edge detection
                continue;
            }

            // secure debugmon_handler in
            else if ((dst_addr > CODE_S) && (dst_addr < 0x10004000))
            {
                continue;
            }
            continue;
        }
        // Check IRQ out, ignore all
        else if ((src_addr > IRQ_EXIT) && (dst_addr < CODE_NS))
        {
            continue;
        }

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(IBT_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (src_addr & 0x1)
                {
                    continue;
                }

                // printf("0x%08x, 0x%08x\r\n", mtb_buff[idx], mtb_buff[idx + 1]);
                // read bytes one by one to get the mc of instruction
                // uint32_t src_ins = (uint32_t *)src_addr;
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }
#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if ((src_ins[0] != 0x70) && (IBT_QUERY(src_addr, dst_addr) < 0))
                    {
                        printf("[%ld]!!! illegal indirect call: 0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET
                        continue;
                    }
                    continue;
                }

#ifdef RTOS
                // it maybe a return from kernel functions, check the sorted task list
                // if the target address is another task
                else if (TASK_check(&TASK_LIST, dst_addr) > -1)
                {

#ifdef MULTI_STACK
                    TASK_ID(dst_addr);
#endif

#ifdef LOG_IRQ
                    printf("[%ld] return from kernel functions: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
                    // STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

                    continue;
                }
#endif // RTOS
       // other records, check the IBT. May be the pop, ignore for forward edge detection
                continue;
            }

            continue;
        }
        // Check IRQ out, ignore all
        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(SS_NO_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;
        src_addr = mtb_buff[idx] & LAST_BIT_CLEAR;

        // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
        if (dst_addr == STACK[task_flag].top)
        {
            STACK_pop(&STACK[task_flag]);

#ifdef LOG_RET
            printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr dst_addr);
            STACK_check_all(&STACK[task_flag]);
            printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

            continue;
        }

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

                // other records
                continue;
            }

            // secure debugmon_handler in
            continue;
        }
        // Check IRQ out
        continue;

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(EMU_NO_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;
        src_addr = mtb_buff[idx] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    STACK_pop(&STACK[task_flag]);

#ifdef LOG_RET
                    printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    STACK_check_all(&STACK[task_flag]);
                    printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

                    continue;
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }

                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }

                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                        // continue;
                        // sys_reset();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if (IBT_QUERY(src_addr, dst_addr) < 0)
                    {
#ifdef LOG_ERROR
                        printf("[%u]bx/blx#:0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

                // other records, check the IBT
                if (IBT_QUERY(src_addr, dst_addr) < 0)
                {
                    printf("ib/call#:0x%08lx\r\n", CONCACT_UINT32(src_addr, dst_addr));
                    dummy_handler();
                }
                continue;
            }

            // secure debugmon_handler in
            continue;
        }
        // Check IRQ out
        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(FULL_NO_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;
        src_addr = mtb_buff[idx] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    STACK_pop(&STACK[task_flag]);

#ifdef LOG_RET
                    printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    STACK_check_all(&STACK[task_flag]);
                    printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

                    continue;
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if (IBT_QUERY(src_addr, dst_addr) < 0)
                    {
#ifdef LOG_ERROR
                        printf("[%u]illegal bx/blx:0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

                // other records, check the IBT
                if (IBT_QUERY(src_addr, dst_addr) < 0)
                {
                    printf("illegal ib/call:0x%08lx\r\n", CONCACT_UINT32(src_addr, dst_addr));
                    dummy_handler();
                }
                continue;
            }

            // secure debugmon_handler in
            continue;
        }
        // Check IRQ out
        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(SS_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx + 1];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    STACK_pop(&STACK[task_flag]);

#ifdef LOG_RET
                    printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    STACK_check_all(&STACK[task_flag]);
                    printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

                    continue;
                }
                // get the last bit of the src_addr, and the other bits are the real address
                // if the last bit is 1, it is an exception, we need to call IRQ_check_in, else, we keep going
                if (src_addr & 0x1)
                {
                    if (check_IRQ_in(src_addr, dst_addr) > -1)
                    {
                        continue;
                    }
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

                continue;
            }

            // secure debugmon_handler in
            else if ((dst_addr > CODE_S) && (dst_addr < 0x10004000))
            {
                if (STACK_push(&STACK[task_flag], src_addr) < 0)
                {
                    printf("[%u]Secure_IRQ stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                    STACK_check_all(&STACK[task_flag]);
                    dummy_handler();
                }

#ifdef LOG_IRQ
                printf("[%ld] Secure in: 0x%08lx, 0x%08lx\r\n", task_flag, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
#endif // LOG_IRQ

                continue;
            }
            continue;
        }
        // Check IRQ out
        else if ((src_addr > IRQ_EXIT) && (dst_addr < CODE_NS))
        {
            if (check_IRQ_out(src_addr, dst_addr, idx) == -1)
            {
#ifdef LOG_ERROR
                printf("[%u] Wrong IRQ exit: 0x%08x, 0x%08x, 0x%08x, 0x%08x\r\n", task_flag, mtb_buff[idx - 2] & LAST_BIT_CLEAR, mtb_buff[idx - 1] & LAST_BIT_CLEAR, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
                TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                dummy_handler();
            }
            continue;
        }

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(EMU_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    STACK_pop(&STACK[task_flag]);

#ifdef LOG_RET
                    printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    STACK_check_all(&STACK[task_flag]);
                    printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

                    continue;
                }
                if (src_addr & 0x1)
                {
                    if (check_IRQ_in(src_addr, dst_addr) > -1)
                    {
                        continue;
                    }
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if ((src_ins[0] != 0x70) && (IBT_QUERY(src_addr, dst_addr) < 0))
                    {
#ifdef LOG_ERROR
                        printf("[%u]blx#:0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {

#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

                // other records, check the IBT
                if (IBT_QUERY(src_addr, dst_addr) < 0)
                {
                    printf("[%u]ib/call#:0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                    dummy_handler();
                }
                continue;
            }

            // secure debugmon_handler in
            else if ((dst_addr > CODE_S) && (dst_addr < 0x10004000))
            {
                if (STACK_push(&STACK[task_flag], src_addr & LAST_BIT_CLEAR) < 0)
                {
                    printf("[%u]Secure_IRQ stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                    STACK_check_all(&STACK[task_flag]);
                    dummy_handler();
                }

#ifdef LOG_IRQ
                printf("[%ld] Secure in: 0x%08lx, 0x%08lx\r\n", task_flag, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
#endif // LOG_IRQ

                continue;
            }
            continue;
        }
        // Check IRQ out
        else if ((src_addr > IRQ_EXIT) && (dst_addr < CODE_NS))
        {
            if (check_IRQ_out(src_addr, dst_addr, idx) == -1)
            {

#ifdef LOG_ERROR
                printf("[%u] Wrong IRQ exit: 0x%08x, 0x%08x, 0x%08x, 0x%08x\r\n", task_flag, mtb_buff[idx - 2] & LAST_BIT_CLEAR, mtb_buff[idx - 1] & LAST_BIT_CLEAR, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
                TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                dummy_handler();
            }
            continue;
        }

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(FULL_IRQ)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    STACK_pop(&STACK[task_flag]);

#ifdef LOG_RET
                    printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    STACK_check_all(&STACK[task_flag]);
                    printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

                    continue;
                }
                if (src_addr & 0x1)
                {
                    if (check_IRQ_in(src_addr, dst_addr) > -1)
                    {
                        continue;
                    }
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if ((src_ins[0] != 0x70) && (IBT_QUERY(src_addr, dst_addr) < 0))
                    {
                        printf("[%ld]!!! illegal indirect call: 0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {

#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

#ifdef RTOS
                // it maybe a return from kernel functions, check the sorted task list
                // if the target address is another task
                else if (TASK_check(&TASK_LIST, dst_addr) > -1)
                {

#ifdef MULTI_STACK
                    TASK_ID(dst_addr);
#endif

#ifdef LOG_IRQ
                    printf("[%ld] return from kernel functions: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
                    // STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

                    continue;
                }
#endif // RTOS
       // other records, check the IBT
                if (IBT_QUERY(src_addr, dst_addr) < 0)
                {

#ifdef LOG_ERROR
                    printf("[%ld]!!! illegal transfer: 0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                    STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                    dummy_handler();
                }
                continue;
            }

            // secure debugmon_handler in
            else if ((dst_addr > CODE_S) && (dst_addr < 0x10004000))
            {
                if (STACK_push(&STACK[task_flag], src_addr & LAST_BIT_CLEAR) < 0)
                {
                    printf("[%u]Secure_IRQ stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                    STACK_check_all(&STACK[task_flag]);
                    dummy_handler();
                }

#ifdef LOG_IRQ
                printf("[%ld] Secure in: 0x%08lx, 0x%08lx\r\n", task_flag, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
#endif // LOG_IRQ

                continue;
            }
            continue;
        }
        // Check IRQ out
        else if ((src_addr > IRQ_EXIT) && (dst_addr < CODE_NS))
        {
            if (check_IRQ_out(src_addr, dst_addr, idx) == -1)
            {

#ifdef LOG_ERROR
                printf("[%u] Wrong IRQ exit: 0x%08x, 0x%08x, 0x%08x, 0x%08x\r\n", task_flag, mtb_buff[idx - 2] & LAST_BIT_CLEAR, mtb_buff[idx - 1] & LAST_BIT_CLEAR, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
                TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                dummy_handler();
            }
            continue;
        }

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(FULL_EMU)

void cfi_validation()
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = 0; idx < record_count / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    STACK_pop(&STACK[task_flag]);

#ifdef LOG_RET
                    printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    STACK_check_all(&STACK[task_flag]);
                    printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

                    continue;
                }
                if (src_addr & 0x1)
                {
                    if (check_IRQ_in(src_addr, dst_addr) > -1)
                    {
                        continue;
                    }
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if ((src_ins[0] != 0x70) && (IBT_QUERY(src_addr, dst_addr) < 0))
                    {
                        printf("[%ld]icall#:0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {

#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

#ifdef RTOS
                // it maybe a return from kernel functions, check the sorted task list
                // if the target address is another task
                else if (TASK_check(&TASK_LIST, dst_addr) > -1)
                {

#ifdef MULTI_STACK
                    TASK_ID(dst_addr);
#endif

#ifdef LOG_IRQ
                    printf("[%ld] return from kernel functions: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
                    // STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

                    continue;
                }
#endif // RTOS
       // other records, check the IBT
                if (IBT_QUERY(src_addr, dst_addr) < 0)
                {

#ifdef LOG_ERROR
                    printf("[%ld]!!! illegal transfer#:0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                    STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                    dummy_handler();
                }
                continue;
            }

            // secure debugmon_handler in
            else if ((dst_addr > CODE_S) && (dst_addr < 0x10004000))
            {
                if (STACK_push(&STACK[task_flag], src_addr & LAST_BIT_CLEAR) < 0)
                {
                    printf("[%u]Secure_IRQ stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                    STACK_check_all(&STACK[task_flag]);
                    dummy_handler();
                }

#ifdef LOG_IRQ
                printf("[%ld] Secure in: 0x%08lx, 0x%08lx\r\n", task_flag, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
#endif // LOG_IRQ

                continue;
            }
            continue;
        }
        // Check IRQ out
        else if ((src_addr > IRQ_EXIT) && (dst_addr < CODE_NS))
        {
            if (check_IRQ_out(src_addr, dst_addr, idx) == -1)
            {

#ifdef LOG_ERROR
                printf("[%u] Wrong IRQ exit: 0x%08x, 0x%08x, 0x%08x, 0x%08x\r\n", task_flag, mtb_buff[idx - 2] & LAST_BIT_CLEAR, mtb_buff[idx - 1] & LAST_BIT_CLEAR, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
                TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                dummy_handler();
            }
            continue;
        }

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(TRIGGER_READ_BUFF)

int cfi_validation_trigger(uint32_t start_pos, uint32_t end_pos)
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = start_pos; idx < end_pos / 4; idx += 2)
    {
        // if the src is empty, we return
        sherloc_read++;
    }
}

#elif defined(TRIGGER_INS)

int cfi_validation_trigger(uint32_t start_pos, uint32_t end_pos)
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = start_pos; idx < end_pos / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        // if the src is empty, we return
        if (src_addr == 0)
        {
            ins_identification++;
            return 0;
        }

#ifdef LOG_RET
        printf("[%u]record: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
#endif // LOG_RET

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    ins_identification++;
                }
                if (src_addr & 0x1)
                {
                    continue;
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    // printf("src_ins[1]: 0x%02x, src_ins[0]: 0x%02x, src_ins[3]: 0x%02x, src_ins[2]: 0x%02x\r\n", src_ins[1], src_ins[0], src_ins[3], src_ins[2]);
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    ins_identification++;
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    ins_identification++;

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {
                        ins_identification++;
                    }
                    continue;
                }

#ifdef RTOS
                // it maybe a return from kernel functions, check the sorted task list
                // if the target address is another task
                else if (TASK_check(&TASK_LIST, dst_addr) > -1)
                {
                    ins_identification++;
                }
#endif // RTOS

                // other records, check the IBT
                ins_identification++;
                continue;
            }

#ifdef IRQ_CHECK
            // secure debugmon_handler in
            else if ((dst_addr > CODE_S) && (dst_addr < 0x10004000))
            {
                ins_identification++;
            }
#endif // IRQ_CHECK
            continue;
        }
        // Check IRQ out

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#elif defined(TRIGGER_IBT_BASELINE)

int cfi_validation_trigger(uint32_t start_pos, uint32_t end_pos)
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = start_pos; idx < end_pos / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        // if the src is empty, we return
        if (src_addr == 0)
        {
            return 0;
        }

#ifdef LOG_RET
        printf("[%u]record: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
#endif // LOG_RET

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter bx/blx
                if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    if (src_ins[0] == 0x70)
                    {
                        continue;
                    }
                    // bx/blx Rx

                    // blx Rx, store the address of next instruction
                    continue;
                }

                continue;
            }

            continue;
        }
    }
}

#elif defined(TRIGGER_IBT)

int cfi_validation_trigger(uint32_t start_pos, uint32_t end_pos)
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = start_pos; idx < end_pos / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        // if the src is empty, we return
        if (src_addr == 0)
        {
            return 0;
        }

#ifdef LOG_RET
        printf("[%u]record: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
#endif // LOG_RET

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter bx/blx
                if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    if (src_ins[0] == 0x70)
                    {
                        continue;
                    }
                    // bx/blx Rx
                    if (IBT_QUERY(src_addr, dst_addr) < 0)
                    {
                        printf("!!! illegal indirect call: 0x%08lx\r\n", CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    continue;
                }

                continue;
            }

            continue;
        }
    }
}

#elif defined(TRIGGER)

int cfi_validation_trigger(uint32_t start_pos, uint32_t end_pos)
{
    uint32_t record_count = MTB->POSITION;
    uint32_t src_addr, dst_addr;
    MTB->POSITION = 0;

    for (int idx = start_pos; idx < end_pos / 4; idx += 2)
    {
        // check non-secure branches, code region: [0x00200000~0x00400000]
        src_addr = mtb_buff[idx];
        dst_addr = mtb_buff[idx + 1] & LAST_BIT_CLEAR;

        // if the src is empty, we return
        if (src_addr == 0)
        {
            return 0;
        }

#ifdef LOG_RET
        printf("[%u]record: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
#endif // LOG_RET

        if (src_addr < CODE_NS)
        {
            if (dst_addr < CODE_NS)
            {
                // if defined IRQ_ENABLE, filter return, those are other the dst should equal to the top of the stack
                if (dst_addr == STACK[task_flag].top)
                {
                    STACK_pop(&STACK[task_flag]);

#ifdef LOG_RET
                    printf("[%u]--match return: (0x%08x, 0x%08x).\r\n", task_flag, src_addr, dst_addr);
                    STACK_check_all(&STACK[task_flag]);
                    printf("[%u]new stack top: 0x%08x\r\n", task_flag, STACK[task_flag].top);
#endif // LOG_RET

                    continue;
                }
                if (src_addr & 0x1)
                {
                    continue;
                }

                // read bytes one by one to get the mc of instruction
                uint8_t *src_ins = (uint32_t *)src_addr;
                // filter direct branch (B: {d0000~e7ff}):
                if (src_ins[1] >= 0xd0 && src_ins[1] <= 0xe7)
                {
                    continue;
                }
                // filter CBNZ, CBZ {b100~bbff}
                else if (src_ins[1] >= 0xb1 && src_ins[1] <= 0xbb)
                {
                    continue;
                }
                // filter direct call (BL) and special direct branch with size of 4 bytes (conditional B: {f000 8000~f7ff bfff})
                else if ((src_ins[1] >= 0xf0) && (src_ins[1] <= 0xf7))
                {
                    if (src_ins[3] < 0xd0)
                    {
                        continue;
                    }

#ifdef LOG_RET
                    printf("[%u]direct call: 0x%08x, 0x%08x ", task_flag, src_addr, dst_addr);
                    printf("++next: 0x%08x.\r\n", src_addr + 4);
#endif // LOG_RET

                    // bl
                    if (STACK_push(&STACK[task_flag], src_addr + 4) < 0)
                    {
                        printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                        STACK_check_all(&STACK[task_flag]);
                        dummy_handler();
                        // continue;
                        // sys_reset();
                    }
                    continue;
                }
                // filter bx/blx
                else if (src_ins[1] == 0x47)
                {
                    // filter out bx lr at the very beginning
                    // the record is an indirect call/branch, check the IBT
                    // bx/blx Rx
                    if (IBT_QUERY(src_addr, dst_addr) < 0)
                    {
                        printf("[%ld]!!! illegal indirect call: 0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                        STACK_check_all(&STACK[task_flag]);
                        TASK_check_all(&TASK_LIST);
                        dummy_handler();
                    }

                    // blx Rx, store the address of next instruction
                    if (src_ins[0] > 0x7f)
                    {

#ifdef LOG_RET
                        printf("[%u]indirect call: 0x%08x --", task_flag, (uint32_t)src_ins);
                        printf("++next: 0x%08x.\r\n", src_addr + 2);
#endif // LOG_RET

                        if (STACK_push(&STACK[task_flag], src_addr + 2) < 0)
                        {
                            printf("[%u]stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                            STACK_check_all(&STACK[task_flag]);
                            dummy_handler();
                        }
                        continue;
                    }
                    continue;
                }

#ifdef RTOS
                // it maybe a return from kernel functions, check the sorted task list
                // if the target address is another task
                else if (TASK_check(&TASK_LIST, dst_addr) > -1)
                {

#ifdef MULTI_STACK
                    TASK_ID(dst_addr);
#endif

#ifdef LOG_IRQ
                    printf("[%ld] return from kernel functions: (0x%08lx, 0x%08lx), (0x%08lx, 0x%08lx).\r\n", task_flag, mtb_buff[idx - 2] & ~(1 << 0), mtb_buff[idx - 1] & ~(1 << 0), src_addr, dst_addr);
                    // STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_IRQ

                    continue;
                }
#endif // RTOS
       // other records, check the IBT
                if (IBT_QUERY(src_addr, dst_addr) < 0)
                {

#ifdef LOG_ERROR
                    printf("[%ld]!!! illegal transfer: 0x%08lx\r\n", task_flag, CONCACT_UINT32(src_addr, dst_addr));
                    STACK_check_all(&STACK[task_flag]);
                    TASK_check_all(&TASK_LIST);
#endif // LOG_ERROR

                    dummy_handler();
                }
                continue;
            }

#ifdef IRQ_CHECK
            // secure debugmon_handler in
            else if ((dst_addr > CODE_S) && (dst_addr < 0x10004000))
            {
                if (STACK_push(&STACK[task_flag], src_addr & LAST_BIT_CLEAR) < 0)
                {
                    printf("[%u]Secure_IRQ stack is full! top: %d\r\n", task_flag, STACK[task_flag].top_pos);
                    STACK_check_all(&STACK[task_flag]);
                    dummy_handler();
                }

#ifdef LOG_IRQ
                printf("[%ld] Secure in: 0x%08lx, 0x%08lx\r\n", task_flag, src_addr, dst_addr);
                STACK_check_all(&STACK[task_flag]);
#endif // LOG_IRQ

                continue;
            }
#endif // IRQ_CHECK
            continue;
        }

        // src_addr > S, the record is from the Secure state, we consider it is trusted
    }
}

#endif // CONTROLLERS

void DebugMon_Handler_Main(sContextStateFrame *stack)
{

#ifdef TRIGGER
    MTB->MASTER &= ~(1 << 31);
    disable_SysTick_ns();
    // cumulatively get the cycles for debugmon handler in, store the current cycle in the debugmon_out
    // we use another routine to calculate the smaller steps
    sherloc_enter = ARM_CM_DWT_CYCCNT - 2;
    // reset the cycle to 0 to count detection time
    ARM_CM_DWT_CYCCNT = 0;

    // if it is a dwt event
    if (SCB->DFSR & SCB_DFSR_DWTTRAP_Msk)
    {
        // We have served this event so clear mask
        SCB->DFSR = CLEARED_MASK_VALUE;
        // clear the function match
        if (DWT->FUNCTION0 & DWT_FUNCTION_MATCHED_Msk)
        {
            DWT->FUNCTION0 &= ~DWT_FUNCTION_MATCHED_Msk;
            int record_count = MTB->POSITION;

            if (MPU_NS->CTRL == 0x00000004)
            {
                // printf("\r\nMPU is disabled!\r\n");
                // record_count = MTB->POSITION;
                cfi_validation_trigger(record_count, MTB_BUFF);
                cfi_validation_trigger(0, record_count);
            }
        }
        else if (DWT->FUNCTION1 & DWT_FUNCTION_MATCHED_Msk)
        {
            DWT->FUNCTION1 &= ~DWT_FUNCTION_MATCHED_Msk;
            int record_count = MTB->POSITION;

#ifdef LOG_RET
            printf("\r\nMTB->POSITION: %u.\r\n", MTB->POSITION);
#endif // LOG_RET

            cfi_validation_trigger(record_count, MTB_BUFF);
            cfi_validation_trigger(0, record_count);
        }
        else if (DWT->FUNCTION2 & DWT_FUNCTION_MATCHED_Msk)
        {
            DWT->FUNCTION2 &= ~DWT_FUNCTION_MATCHED_Msk;
            printf("DWT2 event triggered.\r\n");
        }
        else if (DWT->FUNCTION3 & DWT_FUNCTION_MATCHED_Msk)
        {
            DWT->FUNCTION3 &= ~DWT_FUNCTION_MATCHED_Msk;
            printf("DWT3 event triggered.\r\n");
            printf("MTB->MASTER: 0x%08lx, MTB->POSITION: 0x%08lx, MTB->TSTOP: 0x%08lx\r\n", MTB->MASTER, MTB->POSITION, MTB->TSTOP);
            printf("trace buffer\r\n");

            for (size_t i = 0; i < MTB->POSITION; i += 2)
            {
                printf("0x%08lx, 0x%08lx\r\n", mtb_buff[i], mtb_buff[i + 1]);
            }
        }
        else
        {
            printf("DWT event triggered.\r\n");
        }

        MTB->POSITION = 0;
        MTB->MASTER |= (1 << 31);

        // clear stacks
        task_stack_init();
        enable_SysTick_ns();
    }
    // get the detection time
    sherloc_exit = ARM_CM_DWT_CYCCNT;
    sherloc_detection += sherloc_exit;
    // restore the time entering detection
    ARM_CM_DWT_CYCCNT = sherloc_enter;
#endif // TRIGGER

#ifdef TEST_CFI
    if (SCB->DFSR & SCB_DFSR_EXTERNAL_Msk)
    // if (MTB->MASTER & (1 << 9))
    {
        // We have served this event so clear mask
        // SCB->DFSR &= ~SCB_DFSR_EXTERNAL_Msk;
        SCB->DFSR = CLEARED_MASK_VALUE;

        // disable debugmonitor
        // DCB->DEMCR &= ~DCB_DEMCR_MON_EN_Msk;
        DCB->DEMCR = DEBUGMON_DISABLE;

        // if defined IRQ, disable non-secure systick
#if defined(SS_IRQ) || defined(FULL_EMU) || defined(FULL_IRQ) || defined(USE_SYSTICK_NS)
        // disable non-secure systick interrupt
        disable_SysTick_ns();
#endif // IRQ

#ifdef TEST_SHERLOC_ENTER_EXIT
        // disable DWT cycle counting
        DCB->DEMCR &= ~DCB_DEMCR_TRCENA_Msk;
#endif // TEST_SHERLOC_ENTER_EXIT

        // disable MTB, replaced by a fixed value to reduce instructions
        MTB->MASTER = MTB_CLEAR_VALUE;
        MTB->FLOW = 0;

        __asm volatile("dsb\n"
                       "isb\n");

#ifdef EVAL_SHERLOC_SPLIT
        // cumulatively get the cycles for debugmon handler in, store the current cycle in the debugmon_out
        // we use another routine to calculate the smaller steps
        sherloc_enter = ARM_CM_DWT_CYCCNT - REGISTER_RED_INIT_CYCLES_PIPE;
        // reset the cycle to 0 to count detection time
        ARM_CM_DWT_CYCCNT = 0;
#endif

        cfi_validation();

#ifdef EVAL_SHERLOC_SPLIT
        // get the detection time
        sherloc_exit = ARM_CM_DWT_CYCCNT;
        sherloc_detection += sherloc_exit;
        // restore the time entering detection
        ARM_CM_DWT_CYCCNT = sherloc_enter;
#endif
        // enable debug monitor
        // DCB->DEMCR |= DCB_DEMCR_MON_EN_Msk;
        DCB->DEMCR = DEBUGMON_ENABLE;

#ifdef TEST_SHERLOC_ENTER_EXIT
        // enable DWT time counting
        DCB->DEMCR |= DCB_DEMCR_TRCENA_Msk;
        DWT->CTRL = DWT_TRACE_ENABLE;
#endif // TEST_SHERLOC_ENTER_EXIT

        // if defined IRQ, enable non-secure systick
#if defined(SS_IRQ) || defined(FULL_EMU) || defined(FULL_IRQ) || defined(USE_SYSTICK_NS)
        // enable non-secure systick interrupt and set the frequency to 100ms
        enable_SysTick_ns();
#endif // IRQ

        // enable MTB  0xFC3
        MTB->FLOW = WATERMARK_VALUE;
        // MTB->MASTER |= (1 << 31);  // replace it by a fix value to reduce the instructions
        MTB->MASTER = MTB_RESET_VALUE;

        __asm volatile("dsb\n"
                       "isb\n");
    }

#endif // test_cfi
}

void config_dwt()
{
// MPU_CTRL: 0xe000ed94
#if defined(MPU_trigger)
    // install a dwt watchpoint to monitor the write access to MPU_CTRL register
    dwt_install_watchpoint(0, (uint32_t)&MPU->CTRL, DWT_FUNCTION_W);

#elif defined(LED_trigger)
    // install a dwt watchpoint to monitor the read/write access to LEB peripheral
    // MPS2_SCC->CFG_REG1;        /* Offset: 0x004 (R/W)  Controls the MCC user LEDs */
    dwt_install_watchpoint(1, (uint32_t)&MPS2_SCC->CFG_REG1, DWT_FUNCTION_W);
#endif // TRIGGER

    printf("Start MTB tracing.\r\n");
    mtb_enable(MTB_ALIAS_MAX);

    // configure MTB_MASTER register to enable TSTOPEN
    MTB->MASTER |= 1 << 6;
}

void cfi_test_s()
{
    task_stack_init();
    task_list_init();

#ifdef DEBUG_MON
    if (debug_monitor_enable())
    {
        printf("\r\nMonitor Mode Debug Enabled!\r\n");
    }
#endif

    LoadBranchTable();

    printf("----Binary search enabled.---\r\n");

    measurement_init();

#ifdef MTB_ENABLE
    printf("Start MTB tracing.\r\n");
    mtb_enable(MTB_ALIAS_MAX);
    // reset the MTB position to 0
    MTB->POSITION = 0;
#endif

#ifdef TRIGGER
    config_dwt();
    MTB->POSITION = 0;
#endif // TRIGGER
}