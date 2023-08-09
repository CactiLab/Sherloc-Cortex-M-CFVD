Vulfoo_S_NS project
========================

The **Vulfoo_S_NS** project is a demo program to run FreeRTOS+MPU in the non-secure state and run the Sherloc in the secure state.
Sherloc setup triggers to run CFI detection.
ARM **IOTKit_CM33** microcontroller using ARM **V2M-MPS2+** Evaluation Board.
Compliant to Cortex Microcontroller Software Interface Standard (CMSIS).

## Example functionality

Clock Settings:

- XTAL = 50 MHz
- CCLK = 25 MHz

The example demonstrates the interworking between secure and un-secure mode
on an IOTKit Cortex-M33 microcontroller.

Secure Application:

- Setup core.
- Initialize Sherloc: `cfi_test_c()`.
- Output text on serial port USART0 (via USART CMSIS driver)
  - USART0 settings: 115200 baud, 8 data bits, no parity, 1 stop bit

NonSecure Application:

- Initialize FreeRTOS kernel.
- Two tasks: vulfoo (privileged task) and empty (privileged task).
  - The vulfoo task is a vulnerable task that contains a buffer overflow vulnerability.

## Available targets

- V2M-MPS2+: configured for MPS2+ (uses onboard CMSIS-DAP as debugger)

MPS2+ image: AN505 (+ MB BIOS image V2.2.0)
