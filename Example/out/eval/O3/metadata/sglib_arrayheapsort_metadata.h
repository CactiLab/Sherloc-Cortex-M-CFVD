
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
const uint32_t dbranch_count = 38;
const uint32_t dcall_count = 22; 
const uint32_t ret_count = 8; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x200508, 0x20050c, 0x200510 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x200508, 0x20050c, 0x2007cc };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x200548, 0x20056a, 0x20073e, 0x20074a, 0x200758, 0x200766, 0x200774, 0x200782, 0x200790, 0x20079e, 0x2007ac, 0x2007ba, 0x2007c8 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x2004ea, 0x200506, 0x20072a, 0x2007fa };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a8, 0x2004be, 0x200508, 0x20050c, 0x200572, 0x20057c, 0x200586, 0x2005a0, 0x2005b2, 0x2005c8, 0x2005e8, 0x2005fa, 0x200610, 0x200634, 0x200646, 0x20065c, 0x20065e, 0x200668, 0x200672, 0x200684, 0x20069e, 0x2006b0, 0x2006ca, 0x2006de, 0x2006f6, 0x200708, 0x200722, 0x200724, 0x200754, 0x200770, 0x20078c, 0x2007a8, 0x2007c4, 0x2007cc };


#endif // BT_S
                