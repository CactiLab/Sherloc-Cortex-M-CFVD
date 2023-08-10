# Sherloc_Blinky_S_NS project

The **Sherloc_Blinky_S_NS** project is a demo program for
ARM **Cortex-IOTKit_CM33_FP** microcontroller using ARM **V2M-MPS2+** Evaluation Board.
It runs Blinky in the secure state and runs Sherloc in the secure state.
Compliant to Cortex Microcontroller Software Interface Standard (CMSIS).

## Example functionality

Clock Settings:

- XTAL = 50 MHz
- CCLK = 25 MHz

The example demonstrates the Blinky in non-secure mode
on an IOTKit Cortex-M33 microcontroller.

Secure Application (Sherloc):

- Setup core.
- Initialize Sherloc: `cfi_test_c()`.
- Output text on serial port USART0 (via USART CMSIS driver)
  - USART0 settings: 115200 baud, 8 data bits, no parity, 1 stop bit

NonSecure Application:

- Initialize LEDs.
- Initialize GLCD.
- Initialize Buttons.
- Timer0 is used in interrupt mode
- LEDs are blinking with speed depending on Timer0 interrupt period
- color GLCD display shows:
  - init message
  - Button status
  - LED status
  - Touch status

## Available targets

- V2M-MPS2+: configured for MPS2+ (uses onboard CMSIS-DAP as debugger)

MPS2+ image: AN505 (+ MB BIOS image V2.2.0)
