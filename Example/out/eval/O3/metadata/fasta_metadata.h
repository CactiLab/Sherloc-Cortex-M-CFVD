
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
const uint32_t dbranch_count = 38;
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
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a8, 0x2009ec };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x200458, 0x200466, 0x20046a, 0x20046e, 0x20047a, 0x200480, 0x2004e4, 0x20095e, 0x20096a, 0x200978, 0x200986, 0x200994, 0x2009a2, 0x2009b0, 0x2009be, 0x2009cc, 0x2009da, 0x2009e8 };
const uint32_t ret_list[] = { 0x200454, 0x20045c, 0x200460, 0x200496, 0x200944, 0x200a1a };
const uint32_t dbranch_list[] = { 0x200418, 0x200444, 0x200492, 0x20049e, 0x2004a4, 0x2004a8, 0x200674, 0x20067c, 0x20068e, 0x2006d6, 0x2006dc, 0x2006f4, 0x20070c, 0x200732, 0x20077a, 0x200780, 0x20079a, 0x2007b4, 0x2007ca, 0x2007dc, 0x2007e8, 0x2007fe, 0x200846, 0x20084c, 0x200864, 0x20087c, 0x2008a6, 0x2008ee, 0x2008f4, 0x20090e, 0x200928, 0x20093e, 0x200974, 0x200990, 0x2009ac, 0x2009c8, 0x2009e4, 0x2009ec };


#endif // BT_S
                