
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 3
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 1; 
const uint32_t ibranch_count = 3; 
const uint32_t dbranch_count = 77;
const uint32_t dcall_count = 21; 
const uint32_t ret_count = 12; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b, 0x7360732, 0x7360788 };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a6, 0x2004a8 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a6, 0x2005ba };

const uint32_t icall_list[] = { 0x200736 };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004ca, 0x2004ec, 0x200564, 0x20057a, 0x200588, 0x200594, 0x20059c, 0x2005b0, 0x2005e8, 0x200620, 0x2007da, 0x2007ee };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x20061a, 0x2006d6, 0x2006d8, 0x2007b8, 0x2007e0, 0x2007e8, 0x2007f4, 0x200822 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a6, 0x200508, 0x20051a, 0x200530, 0x20053a, 0x200544, 0x200546, 0x20054a, 0x20054e, 0x20056e, 0x200580, 0x20058c, 0x200598, 0x2005a0, 0x2005aa, 0x2005b8, 0x2005ba, 0x2005be, 0x2005d2, 0x2005dc, 0x2005de, 0x2005e2, 0x2005f0, 0x2005f8, 0x2005fe, 0x200604, 0x200608, 0x20060e, 0x200612, 0x200628, 0x200630, 0x200636, 0x20063c, 0x200640, 0x200646, 0x20064c, 0x200652, 0x20065c, 0x200662, 0x200666, 0x20066c, 0x200678, 0x20067e, 0x200682, 0x200688, 0x200698, 0x20069c, 0x2006a2, 0x2006b4, 0x2006b8, 0x2006be, 0x2006f8, 0x2006fe, 0x20070c, 0x200714, 0x200728, 0x20072e, 0x200730, 0x20073c, 0x20073e, 0x200744, 0x20074c, 0x200752, 0x20075a, 0x200766, 0x20076a, 0x20076c, 0x20076e, 0x20079a, 0x2007aa, 0x2007b6, 0x2007ca };


#endif // BT_S
                