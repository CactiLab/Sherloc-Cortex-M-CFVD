
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
const uint32_t dbranch_count = 55;
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
const uint32_t LOOP[] = { 0x20049e, 0x200508, 0x20050c, 0x2007a4 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x200548, 0x20056e, 0x200716, 0x200722, 0x200730, 0x20073e, 0x20074c, 0x20075a, 0x200768, 0x200776, 0x200784, 0x200792, 0x2007a0 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x2004ea, 0x200506, 0x200700, 0x2007d2 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a8, 0x2004be, 0x200508, 0x20050c, 0x20057e, 0x200584, 0x20059c, 0x2005a0, 0x2005ba, 0x2005d6, 0x2005e2, 0x2005ea, 0x2005fc, 0x200600, 0x20060a, 0x20060e, 0x200614, 0x200618, 0x200622, 0x20062c, 0x20062e, 0x200636, 0x200638, 0x20063c, 0x200640, 0x20064a, 0x200656, 0x20065a, 0x200662, 0x200666, 0x200670, 0x200674, 0x20067e, 0x200688, 0x20068a, 0x200694, 0x200696, 0x20069c, 0x2006ac, 0x2006b2, 0x2006ba, 0x2006d2, 0x2006dc, 0x2006e2, 0x2006f8, 0x20072c, 0x200748, 0x200764, 0x200780, 0x20079c, 0x2007a4 };


#endif // BT_S
                