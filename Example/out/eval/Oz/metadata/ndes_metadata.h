
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 4
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 3; 
const uint32_t ibranch_count = 4; 
const uint32_t dbranch_count = 39;
const uint32_t dcall_count = 16; 
const uint32_t ret_count = 6; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b, 0x65a04e0, 0x66e04e0, 0x67e04e0 };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a6, 0x2004a8 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a6, 0x200864 };

const uint32_t icall_list[] = { 0x20065a, 0x20066e, 0x20067e };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004ca, 0x200502, 0x2005ae, 0x2005c6, 0x2006fe, 0x20070e, 0x20085a };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x2004f4, 0x2008d4 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a6, 0x20050e, 0x200520, 0x20057c, 0x200584, 0x20059c, 0x2005d4, 0x2005e4, 0x2005f2, 0x2005fa, 0x200604, 0x20063c, 0x20068a, 0x2006ac, 0x2006b0, 0x2006b4, 0x2006d0, 0x2006d8, 0x2006ea, 0x200718, 0x200722, 0x200752, 0x200794, 0x2007c2, 0x2007d8, 0x2007de, 0x20081c, 0x200824, 0x200838, 0x200844, 0x200854, 0x200862, 0x200864, 0x20086a };


#endif // BT_S
                