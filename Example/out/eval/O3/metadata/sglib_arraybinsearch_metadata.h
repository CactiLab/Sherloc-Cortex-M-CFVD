
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
const uint32_t dbranch_count = 29;
const uint32_t dcall_count = 21; 
const uint32_t ret_count = 6; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a8, 0x2004ac };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a8, 0x2006e4 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004e4, 0x200656, 0x200662, 0x200670, 0x20067e, 0x20068c, 0x20069a, 0x2006a8, 0x2006b6, 0x2006c4, 0x2006d2, 0x2006e0 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x200642, 0x200712 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a8, 0x200504, 0x20051c, 0x200536, 0x200544, 0x20054a, 0x200572, 0x200580, 0x200586, 0x2005ae, 0x2005bc, 0x2005c2, 0x2005ea, 0x2005f8, 0x2005fe, 0x200626, 0x200636, 0x20063c, 0x20066c, 0x200688, 0x2006a4, 0x2006c0, 0x2006dc, 0x2006e4 };


#endif // BT_S
                