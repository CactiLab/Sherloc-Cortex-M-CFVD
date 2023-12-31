
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
const uint32_t dbranch_count = 61;
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
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a8, 0x200790 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004e4, 0x2006aa, 0x2006dc, 0x2006ec, 0x200704, 0x200714, 0x20072c, 0x20073c, 0x200754, 0x200764, 0x20077c, 0x20078c };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x200694, 0x2007be };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a8, 0x200514, 0x200524, 0x200540, 0x200548, 0x20054c, 0x200552, 0x200556, 0x20055c, 0x200560, 0x200566, 0x20056a, 0x200572, 0x200574, 0x20057c, 0x200582, 0x200588, 0x2005ae, 0x2005b4, 0x2005b8, 0x2005be, 0x2005c2, 0x2005c8, 0x2005cc, 0x2005d2, 0x2005d6, 0x2005de, 0x2005e0, 0x2005e8, 0x2005ee, 0x2005f4, 0x20061c, 0x200624, 0x20062a, 0x200630, 0x200634, 0x20063a, 0x20063e, 0x200644, 0x200648, 0x200650, 0x200652, 0x200658, 0x20065e, 0x200666, 0x20066c, 0x200676, 0x20067e, 0x200686, 0x200690, 0x2006e6, 0x20070e, 0x200736, 0x20075e, 0x200786, 0x200790 };


#endif // BT_S
                