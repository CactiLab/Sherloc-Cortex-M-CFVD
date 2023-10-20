
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 5
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 1; 
const uint32_t ibranch_count = 5; 
const uint32_t dbranch_count = 29;
const uint32_t dcall_count = 16; 
const uint32_t ret_count = 6; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b, 0x5ee05e6, 0x5ee05ea, 0x5ee05fa, 0x5ee0604 };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a6, 0x2004a8 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a6, 0x2005ba };

const uint32_t icall_list[] = { 0x2005ee };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004ca, 0x2004ea, 0x200576, 0x200586, 0x200590, 0x200598, 0x2005b0 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x200612, 0x200640 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a6, 0x20050a, 0x20051a, 0x200538, 0x200556, 0x20055c, 0x200560, 0x20056c, 0x20057e, 0x20058a, 0x200594, 0x20059c, 0x2005aa, 0x2005b8, 0x2005ba, 0x2005be, 0x2005e4, 0x2005e8, 0x2005f4, 0x2005f8, 0x2005fc, 0x200600, 0x200602, 0x200608 };


#endif // BT_S
                