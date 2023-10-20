
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 0
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 0; 
const uint32_t ibranch_count = 0; 
const uint32_t dbranch_count = 25;
const uint32_t dcall_count = 10; 
const uint32_t ret_count = 4; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = {  };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x200454, 0x200456, 0x200458 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20044e, 0x200454, 0x200456, 0x20059c };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200404, 0x20040a, 0x200418, 0x20041c, 0x200420, 0x20042a, 0x200430, 0x20047a, 0x20049a, 0x200592 };
const uint32_t ret_list[] = { 0x20040e, 0x200412, 0x200446, 0x2005d4 };
const uint32_t dbranch_list[] = { 0x200442, 0x20044e, 0x200454, 0x200456, 0x2004ae, 0x2004c0, 0x2004ca, 0x2004e0, 0x2004ea, 0x2004fe, 0x200504, 0x200510, 0x20051e, 0x20052a, 0x20052e, 0x200532, 0x200548, 0x20055a, 0x20056c, 0x20057c, 0x200580, 0x20058c, 0x20059a, 0x20059c, 0x2005a2 };


#endif // BT_S
                