
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
const uint32_t dbranch_count = 63;
const uint32_t dcall_count = 14; 
const uint32_t ret_count = 7; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a6, 0x2004a8 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a6, 0x20055c };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004ca, 0x2004ee, 0x200552, 0x2006fe, 0x20073c };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x20059c, 0x2005c8, 0x2006be };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a6, 0x200506, 0x200514, 0x200524, 0x20053e, 0x20054c, 0x20055a, 0x20055c, 0x200562, 0x2005cc, 0x2005da, 0x2005f0, 0x2005f4, 0x2005fa, 0x20062a, 0x200630, 0x200634, 0x20063a, 0x20064a, 0x20065a, 0x200688, 0x2006b8, 0x2006bc, 0x2006ce, 0x2006de, 0x2006ee, 0x2006f8, 0x20072e, 0x200744, 0x20075e, 0x200778, 0x20077a, 0x20077c, 0x2007a8, 0x2007ac, 0x2007c2, 0x2007d2, 0x2007e2, 0x200822, 0x200832, 0x200842, 0x200860, 0x200868, 0x20086c, 0x200876, 0x2008c6, 0x2008d6, 0x2008e0, 0x2008e4, 0x2008e6, 0x200920, 0x200924, 0x200926, 0x20092c, 0x200936, 0x200946, 0x200960, 0x200982 };


#endif // BT_S
                