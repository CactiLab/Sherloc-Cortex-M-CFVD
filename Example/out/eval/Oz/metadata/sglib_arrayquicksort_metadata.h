
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
const uint32_t dbranch_count = 36;
const uint32_t dcall_count = 12; 
const uint32_t ret_count = 5; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a6, 0x2004a8 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a6, 0x20062e };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004ca, 0x2004ec, 0x200624 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x200668 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a6, 0x200502, 0x200516, 0x200522, 0x20052e, 0x200540, 0x200556, 0x20055e, 0x200562, 0x20056e, 0x200572, 0x20057c, 0x200580, 0x200584, 0x200594, 0x20059a, 0x2005ae, 0x2005ba, 0x2005c2, 0x2005c8, 0x2005ce, 0x2005d4, 0x2005e4, 0x2005f6, 0x2005fa, 0x200608, 0x200612, 0x20061e, 0x20062c, 0x20062e, 0x200632 };


#endif // BT_S
                