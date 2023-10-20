
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
const uint32_t dbranch_count = 31;
const uint32_t dcall_count = 13; 
const uint32_t ret_count = 9; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = {  };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x200494, 0x200496, 0x200498 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20048e, 0x200494, 0x200496, 0x20063c };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200404, 0x20040a, 0x200418, 0x20041c, 0x200420, 0x20042a, 0x200430, 0x2004ba, 0x2004de, 0x2004e8, 0x2005d6, 0x200610, 0x200632 };
const uint32_t ret_list[] = { 0x20040e, 0x200412, 0x20045c, 0x200460, 0x200468, 0x200470, 0x200484, 0x2005c4, 0x200674 };
const uint32_t dbranch_list[] = { 0x200438, 0x20043e, 0x200442, 0x200452, 0x200458, 0x200464, 0x20046c, 0x200480, 0x20048e, 0x200494, 0x200496, 0x200518, 0x200520, 0x200526, 0x20052e, 0x200542, 0x200558, 0x200568, 0x200572, 0x200596, 0x2005a8, 0x2005e4, 0x2005f4, 0x2005fe, 0x200608, 0x20061c, 0x200620, 0x20062c, 0x20063a, 0x20063c, 0x200642 };


#endif // BT_S
                