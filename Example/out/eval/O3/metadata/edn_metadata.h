
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
const uint32_t dbranch_count = 26;
const uint32_t dcall_count = 73; 
const uint32_t ret_count = 8; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x200508, 0x20050c, 0x200510 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x200508, 0x20050c, 0x200fc4 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x200548, 0x200568, 0x200582, 0x200f36, 0x200f42, 0x200f50, 0x200f5e, 0x200f6c, 0x200f7a, 0x200f88, 0x200f96, 0x200fa4, 0x200fb2, 0x200fc0, 0x20118c, 0x201190, 0x20119c, 0x2011a0, 0x2011ac, 0x2011b0, 0x2011bc, 0x2011c0, 0x2011cc, 0x2011d0, 0x2011dc, 0x2011e0, 0x2011ec, 0x2011f0, 0x2011fc, 0x201200, 0x20120c, 0x201210, 0x20121c, 0x201220, 0x20122c, 0x201230, 0x20123c, 0x201240, 0x20124c, 0x201250, 0x20125c, 0x201260, 0x20126c, 0x201270, 0x20127c, 0x201280, 0x20128c, 0x201290, 0x20129c, 0x2012a0, 0x2012ac, 0x2012b0, 0x2012bc, 0x2012c0, 0x2012cc, 0x2012d0, 0x2012dc, 0x2012e0, 0x2012ec, 0x2012f0, 0x2012fc, 0x201300, 0x20130c, 0x201310 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x2004ea, 0x200506, 0x200f1e, 0x200ff2 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a8, 0x2004be, 0x200508, 0x20050c, 0x2005ec, 0x200624, 0x200688, 0x2006b2, 0x200754, 0x200770, 0x200a1e, 0x200b28, 0x200c6c, 0x200c7e, 0x200ddc, 0x200f18, 0x200f4c, 0x200f68, 0x200f84, 0x200fa0, 0x200fbc, 0x200fc4 };


#endif // BT_S
                