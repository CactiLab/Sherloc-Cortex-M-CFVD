
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
const uint32_t dbranch_count = 39;
const uint32_t dcall_count = 26; 
const uint32_t ret_count = 8; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a8, 0x2004ac };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a8, 0x200970 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004e4, 0x2008ce, 0x2008d8, 0x2008de, 0x2008ec, 0x2008f8, 0x2008fe, 0x20090c, 0x200918, 0x20091e, 0x20092c, 0x200938, 0x20093e, 0x20094c, 0x200958, 0x20095e, 0x20096c };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x20050e, 0x20061c, 0x2008b8, 0x20099e };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a8, 0x2004fc, 0x20052a, 0x200534, 0x20053c, 0x200542, 0x20054a, 0x200550, 0x200558, 0x200564, 0x200566, 0x200574, 0x20057e, 0x20058c, 0x2005ac, 0x2005b6, 0x2005c4, 0x2005ca, 0x2005d4, 0x2005de, 0x2005e8, 0x2005f8, 0x2005fa, 0x2005fe, 0x200602, 0x20060c, 0x200610, 0x200618, 0x2008e8, 0x200908, 0x200928, 0x200948, 0x200968, 0x200970 };


#endif // BT_S
                