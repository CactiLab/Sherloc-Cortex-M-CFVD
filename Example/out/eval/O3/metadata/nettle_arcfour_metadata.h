
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
const uint32_t dbranch_count = 17;
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
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a8, 0x200cb0 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004e4, 0x200c22, 0x200c2e, 0x200c3c, 0x200c4a, 0x200c58, 0x200c66, 0x200c74, 0x200c82, 0x200c90, 0x200c9e, 0x200cac };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x200c0c, 0x200cde };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a8, 0x2005b2, 0x2006ae, 0x2007da, 0x2008a6, 0x2009a2, 0x200c38, 0x200c54, 0x200c70, 0x200c8c, 0x200ca8, 0x200cb0 };


#endif // BT_S
                