
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
const uint32_t dbranch_count = 18;
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
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a6, 0x2005de };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004ca, 0x200592, 0x2005b2, 0x2005b8, 0x2005bc, 0x2005c2, 0x2005d4 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x20052c, 0x200570, 0x200618 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a6, 0x2004f6, 0x200522, 0x20053a, 0x200540, 0x20054e, 0x200568, 0x2005a4, 0x2005ae, 0x2005cc, 0x2005dc, 0x2005de, 0x2005e2 };


#endif // BT_S
                