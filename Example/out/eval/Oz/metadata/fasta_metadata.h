
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 1
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 0; 
const uint32_t ibranch_count = 1; 
const uint32_t dbranch_count = 16;
const uint32_t dcall_count = 16; 
const uint32_t ret_count = 7; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a6, 0x2004a8 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a6, 0x200562 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x200458, 0x200466, 0x20046a, 0x20046e, 0x20047a, 0x200480, 0x2004ca, 0x20050a, 0x20052e, 0x200536, 0x20053e, 0x200546, 0x200558 };
const uint32_t ret_list[] = { 0x200454, 0x20045c, 0x200460, 0x200496, 0x2004f8, 0x2005a8, 0x2005e0 };
const uint32_t dbranch_list[] = { 0x200418, 0x200444, 0x200492, 0x20049e, 0x2004a4, 0x2004a6, 0x2004e4, 0x2004f6, 0x20051c, 0x200526, 0x200550, 0x200560, 0x200562, 0x200566, 0x20059e, 0x2005aa };


#endif // BT_S
                