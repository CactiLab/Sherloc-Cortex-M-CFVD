
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
const uint32_t dbranch_count = 35;
const uint32_t dcall_count = 19; 
const uint32_t ret_count = 8; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b, 0x6600658, 0x660065c, 0x6600668, 0x6600670 };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a6, 0x2004a8 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a6, 0x2005dc };

const uint32_t icall_list[] = { 0x200660 };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004ca, 0x2004ee, 0x20058c, 0x200594, 0x2005a4, 0x2005ae, 0x2005be, 0x2005d2, 0x200608, 0x200624 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x20060e, 0x200648, 0x20067e, 0x2006ac };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a6, 0x20050c, 0x20051a, 0x20052e, 0x200536, 0x200542, 0x200560, 0x200566, 0x20056a, 0x200588, 0x20059a, 0x2005aa, 0x2005b2, 0x2005c2, 0x2005cc, 0x2005da, 0x2005dc, 0x2005e2, 0x20060c, 0x200616, 0x200628, 0x200632, 0x200644, 0x200656, 0x20065a, 0x200662, 0x200666, 0x20066a, 0x20066e, 0x200674 };


#endif // BT_S
                