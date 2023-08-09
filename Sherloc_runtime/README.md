<!-- insert a table -->

### Explanation of all MACROS

`off` means the MACRO is disabled, `on` means the MACRO is enabled.

- MACROS to enable or disable Sherloc functionalities:

    |MACRO | Default | Description
    | --- | --- | --- |
    |DEBUG_MON | off | on for CFVD, enable DebugMon Exception
    |TEST_CFI  | off | on for holistic CFVD, enable CFVD
    |WATERMARK | on, controlled by TEST_CFI |enable MTB WATERMARK for holistic CFVD
    |WATERMARK_VALUE | on, controlled by WATERMARK | watermark value

- MACROS to support event-triggered CFVD:

    |MACRO | Default | Description
    | --- | --- | --- |
    |TRIGGER | off | evaluate event-triggered CFVD
    |MPU_trigger | off | evaluate MPU event-triggered CFVD
    |LED_trigger | off | evaluate LED event-triggered CFVD
    |TRIGGER_READ_BUFF | off | evaluate event-triggered CFVD with read buffer only
    |TRIGGER_INS | off |evaluate event-triggered CFVD with instruction identification only
    |TRIGGER_IBT | off |evaluate event-triggered CFVD with IBT only
    |TRIGGER_IBT_BASELINE | off |evaluate event-triggered CFVD with IBT baseline only

- MACROS to control debug logging: 

    | MACRO | Default | Description
    | --- | --- | --- |
    | LOG_RET | off | print out the calculated return address and whether the return match or not
    | LOG_IRQ | off | print out IRQ in and out

- MACROS to evaluate sub-steps: 

    | MACRO | Default | Description
    | --- | --- | --- |
    | RTOS | off | on for RTOS, enable RTOS task schedule handling
    | VULFOO | off | evaluate buffer overflow attack on FreeRTOS
    | EVAL_INS_IDENTIFY | off |evaluate instruction identification of Sherloc
    | EVAL_READ | off | evaluate read from MTB trace buffer
    | SS_NO_IRQ | off | evaluate the reconstructed call stack of Sherloc without interrupt handling
    | EMU_NO_IRQ | off | evaluate Sherloc in emulation mode (for dynamic training)
    | FULL_NO_IRQ | off | evaluate Sherloc without interrupt handling
    | SS_IRQ | off | evaluate reconstructed call stack of Sherloc with interrupt handling
    | EMU_IRQ | off | evaluate Sherloc in emulation mode with interrupt handling (for dynamic training)
    | FULL_IRQ | off | evaluate Sherloc with interrupt handling and task schedule handling
    | FULL_EMU | off | evaluate Sherloc with interrupt handling and task schedule handling in emulation mode (for dynamic training)
    | IBT_BASELINE_NO_IRQ | off | evaluate IBT baseline for BEEBS
    | IBT_NO_IRQ | off | evaluate IBT for BEEBS
    | IBT_BASELINE_IRQ | off | evaluate IBT baseline for Blinky and FreeRTOS
    | IBT_IRQ | off | evaluate IBT for Blinky and FreeRTOS

- MACROS to control the runtime of Sherloc:

    | MACRO | Default | Description
    | --- | --- | --- |
    | USE_SYSTICK_NS | off  | on for Blinky and FreeRTOS, enable or disable non-secure SysTick
    | SPLIT | always on | split Sherloc enter and exit stage and detection stage
    | EVAL_SHERLOC_SPLIT | on, controlled by SPLIT | split Sherloc enter and exit stage and detection stage
    | FULL | off | print out the runtime of Sherloc without split stages
    | IRQ_CHECK | off | enable interrupt handling
    | MTB_ENABLE | off | enable MTB trace in Sherloc

### Runtime MACROs for evaluation

To evaluate Sherloc with different purposes, we should combine different MACROs.
The emulation mode will dynamically update the IBT based on the runtime trace.
If it runs correctly, the evaluation log should be like this:
```log
Monitor Mode Debug Enabled!
branch table size: 34, IRQ size: 6, Task size: 0, LOOP size: 4, tag_size: 0

----Load branch table done.---
----Binary search enabled.---
Start MTB tracing.
Hello World (secure[0]icall:0x08560634
 Sherloc_Blinky_ns_10ms_emu_blinky.log insert new record bytearray(b'0x08560634') to IBT.
Original IBT_data: b'<\x042\x04\xe8\x0et\x05\xe8\x0ez\x05\xe8\x0e\x86\x05\xe8\x0e\x8c\x054\x06\xe6\x064\x06\xec\x064\x06\xf8\x064\x06\x06\x07\xfc\x06\x06\x074\x06\x0c\x074\x06\x16\x074\x06H\x08D\x08H\x08\\\x08H\x084\x06L\x084\x06P\x08\xc2\x07\x82\x08\xfc\x07\x0e\t\xfc\x07\x1a\t\xe8\x0e$\t\xe8\x0e*\t\xe8\x0e0\t\xe8\x0e6\tD\x05\x06\x0bD\x05\x0e\x0bD\x05\x16\x0bD\x05B\x0b\x90\x0e\xb6\x0e\x90\x0e\xba\x0e\x90\x0e\xbe\x0e\x90\x0e\xfe\x0e\x90\x0e\x02\x0f\x90\x0e\x06\x0f'
Inserting IBT entry: bytearray(b'0x08560634')
count of IBT_data: 35
sorted IBT_data: b'<\x042\x04\xe8\x0et\x05\xe8\x0ez\x05\xe8\x0e\x86\x05\xe8\x0e\x8c\x054\x06\xe6\x064\x06\xec\x064\x06\xf8\x064\x06\x06\x07\xfc\x06\x06\x074\x06\x0c\x074\x06\x16\x074\x06H\x08D\x08H\x08\\\x08H\x084\x06L\x084\x06P\x084\x06V\x08\xc2\x07\x82\x08\xfc\x07\x0e\t\xfc\x07\x1a\t\xe8\x0e$\t\xe8\x0e*\t\xe8\x0e0\t\xe8\x0e6\tD\x05\x06\x0bD\x05\x0e\x0bD\x05\x16\x0bD\x05B\x0b\x90\x0e\xb6\x0e\x90\x0e\xba\x0e\x90\x0e\xbe\x0e\x90\x0e\xfe\x0e\x90\x0e\x02\x0f\x90\x0e\x06\x0f'
Sherloc_Blinky_ns_10ms_emu_blinky.log insert success.
reset board success
```

### Case 1. evaluation on BEEBS:
1. baseline: refer to `host_tools/evaluation/elf_s/baseline.axf`
    <!-- insert a table -->
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | off |
    | TRIGGER   | off |
    | TEST_CFI  | off |
2. Sherloc read trace buffer: refer to `host_tools/evaluation/elf_s/sherloc_read.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | EVAL_READ | on |
3. Sherloc transfer type identification: refer to `host_tools/evaluation/elf_s/ins.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | EVAL_INS_IDENTIFY | on |
4. Sherloc forward-edge CFVD baseline: refer to `host_tools/evaluation/elf_s/ibt_baseline_no_systick.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | IBT_BASELINE_NO_IRQ | on |
5. Sherloc forward-edge CFVD: refer to `host_tools/evaluation/elf_s/ibt_no_systick.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | IBT_NO_IRQ | on |
6. Sherloc holistic CFVD with emulation: refer to `host_tools/evaluation/elf_s/emu_no_irq.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | EMU_NO_IRQ | on |
7. Sherloc holistic CFVD: refer to `host_tools/evaluation/elf_s/full_no_irq.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | FULL_NO_IRQ | on |
### Case 2: evaluation on Blinky:
1. baseline: refer to `host_tools/evaluation/elf_s/baseline_blinky.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | off |
    | TRIGGER   | off |
    | TEST_CFI  | off |
    | USE_SYSTICK_NS | on |
    | MTB_ENABLE | on |
2. Sherloc read trace buffer: refer to `host_tools/evaluation/elf_s/sherloc_read_systick.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | MTB_ENABLE | on |
    | EVAL_READ | on |
3. Sherloc transfer type identification: refer to `host_tools/evaluation/elf_s/ins_systick.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | MTB_ENABLE | on |
    | EVAL_INS_IDENTIFY | on |
4. Sherloc IBT query baseline: refer to `host_tools/evaluation/elf_s/ibt_baseline_systick.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | MTB_ENABLE | on |
    | IBT_BASELINE_IRQ | on |
5. Sherloc IBT query: refer to `host_tools/evaluation/elf_s/ibt_systick.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | MTB_ENABLE | on |
    | IBT_IRQ | on |    
6. Sherloc holistic CFVD with emulation: refer to `host_tools/evaluation/elf_s/emu_blinky.axf` 
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on | 
    | MTB_ENABLE | on |
    | EMU_IRQ | on |
7. Sherloc holistic CFVD: refer to `host_tools/evaluation/elf_s/full_blinky.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on | 
    | MTB_ENABLE | on |
    | FULL_IRQ | on |    

### Case 3: evaluation on FreeRTOS:
1. baseline: refer to `host_tools/evaluation/elf_s/baseline_rtos.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | off |
    | TRIGGER   | off |
    | TEST_CFI  | off |
    | USE_SYSTICK_NS | on |
2. Sherloc read trace buffer: refer to `host_tools/evaluation/elf_s/read_rtos.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | EVAL_READ | on |
3. Sherloc transfer type identification: refer to `host_tools/evaluation/elf_s/ins_rtos.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | EVAL_INS_IDENTIFY | on |
4. Sherloc IBT query baseline: refer to `host_tools/evaluation/elf_s/ibt_freertos_baseline.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | IBT_BASELINE_IRQ | on |
5. Sherloc IBT query: refer to `host_tools/evaluation/elf_s/ibt_freertos.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | IBT_IRQ | on |
6. Sherloc holistic CFVD with emulation: refer to `host_tools/evaluation/elf_s/full_emu.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | FULL_EMU | on |
8. Sherloc holistic CFVD: refer to `host_tools/evaluation/elf_s/full_rtos.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | off|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | FULL_IRQ | on |
### Case 4: evaluation on event-triggered CFVD:

#### Case 4.1: evaluation on MPU event-triggered CFVD:
1. baseline: same as FreeRTOS
2. Sherloc read trace buffer: refer to `host_tools/evaluation/elf_s/MPU_trigger_read.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | MPU_trigger | on |
    | EVAL_READ | on |
3. Sherloc transfer type identification: refer to `host_tools/evaluation/elf_s/MPU_trigger_ins.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | MPU_trigger | on |
    | EVAL_INS_IDENTIFY | on |
4. Sherloc IBT query baseline: refer to `host_tools/evaluation/elf_s/MPU_trigger_IBT_baseline.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | MPU_trigger | on |
    | IBT_BASELINE_IRQ | on |
5. Sherloc IBT query: refer to `host_tools/evaluation/elf_s/MPU_trigger_IBT.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | MPU_trigger | on |
    | IBT_IRQ | on |
6. Sherloc event-triggered CFVD with emulation: same as FreeRTOS holistic CFVD with emulation, refer to `host_tools/evaluation/elf_s/full_emu.axf`
7. Sherloc event-triggered CFVD:  refer to `host_tools/evaluation/elf_s/MPU_trigger.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | MPU_trigger | on |
    | FULL_IRQ | on |

#### Case 4.2: evaluation on LED event-triggered CFVD:
1. baseline: same as FreeRTOS
2. Sherloc read trace buffer:  refer to `host_tools/evaluation/elf_s/led_trigger_read.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | LED_trigger | on |
    | EVAL_READ | on |
3. Sherloc transfer type identification: refer to `host_tools/evaluation/elf_s/led_trigger_ins.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | LED_trigger | on |
    | EVAL_INS_IDENTIFY | on |
4. Sherloc IBT query baseline: refer to `host_tools/evaluation/elf_s/led_trigger_IBT_baseline.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | LED_trigger | on |
    | IBT_BASELINE_IRQ | on |
5. Sherloc IBT query: refer to `host_tools/evaluation/elf_s/led_trigger_IBT.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | LED_trigger | on |
    | IBT_IRQ | on |
6. Sherloc event-triggered CFVD with emulation: same as FreeRTOS holistic CFVD with emulation, refer to `host_tools/evaluation/elf_s/full_emu.axf`. 
7. Sherloc event-triggered CFVD: refer to `host_tools/evaluation/elf_s/led_trigger.axf`
    | MACRO | status |
    | --- | --- |
    | DEBUG_MON | on |
    | TRIGGER   | on|
    | TEST_CFI  | on |
    | USE_SYSTICK_NS | on |
    | RTOS | on |
    | LED_trigger | on |
    | FULL_IRQ | on |