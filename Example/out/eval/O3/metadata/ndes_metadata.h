
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
const uint32_t dbranch_count = 23;
const uint32_t dcall_count = 37; 
const uint32_t ret_count = 7; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a8, 0x2004ac };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a8, 0x200cb0 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004e4, 0x20073e, 0x20075a, 0x200776, 0x200792, 0x2007ae, 0x2007ca, 0x2007e6, 0x200802, 0x20081e, 0x20083a, 0x200856, 0x200872, 0x20088e, 0x2008aa, 0x2008c6, 0x2008e2, 0x200b16, 0x200b72, 0x200b8a, 0x200bbc, 0x200bd4, 0x200c04, 0x200c1c, 0x200c4c, 0x200c64, 0x200c94, 0x200cac };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x200914, 0x200ace, 0x200cde };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a8, 0x20052e, 0x2005f8, 0x20070e, 0x2008f6, 0x200902, 0x20090a, 0x200918, 0x20092e, 0x200942, 0x200ac0, 0x200b02, 0x200b7e, 0x200bc8, 0x200c10, 0x200c58, 0x200ca0, 0x200cb0 };


#endif // BT_S
                