
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 3
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 1; 
const uint32_t ibranch_count = 3; 
const uint32_t dbranch_count = 105;
const uint32_t dcall_count = 28; 
const uint32_t ret_count = 12; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b, 0x8980894, 0x89808ca };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x2004a4, 0x2004a8, 0x2004ac };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x2004a4, 0x2004a8, 0x200704 };

const uint32_t icall_list[] = { 0x200898 };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2004e4, 0x200520, 0x2005b8, 0x2005ca, 0x2005ea, 0x2005f6, 0x20061e, 0x200650, 0x200660, 0x200678, 0x200688, 0x2006a0, 0x2006b0, 0x2006c8, 0x2006d8, 0x2006f0, 0x200700, 0x200724, 0x200766 };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x200606, 0x200760, 0x2007a4, 0x200802, 0x20081a, 0x20081c, 0x200970, 0x2009a0 };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a4, 0x2004a8, 0x20050c, 0x20052c, 0x200534, 0x200538, 0x200542, 0x20054c, 0x200550, 0x200554, 0x20055e, 0x200566, 0x200568, 0x20056c, 0x200576, 0x20057e, 0x200580, 0x200584, 0x20058e, 0x200596, 0x200598, 0x20059c, 0x2005aa, 0x2005be, 0x2005d2, 0x2005f0, 0x2005fe, 0x20065a, 0x200682, 0x2006aa, 0x2006d2, 0x2006fa, 0x200704, 0x20070e, 0x200718, 0x20071a, 0x20071e, 0x20072c, 0x200734, 0x20073a, 0x200740, 0x200744, 0x20074a, 0x200750, 0x200756, 0x200758, 0x20076e, 0x200776, 0x20077c, 0x200782, 0x200786, 0x20078c, 0x200792, 0x200798, 0x2007aa, 0x2007ae, 0x2007b4, 0x2007be, 0x2007c4, 0x2007c8, 0x2007ce, 0x2007de, 0x2007e2, 0x2007e8, 0x2007f6, 0x2007fa, 0x200838, 0x200842, 0x200848, 0x200858, 0x200860, 0x20087c, 0x200882, 0x200884, 0x20088a, 0x200892, 0x20089e, 0x2008a0, 0x2008aa, 0x2008ac, 0x2008ae, 0x2008d8, 0x2008e8, 0x2008f6, 0x2008f8, 0x200904, 0x20090e, 0x200912, 0x200918, 0x200924, 0x20092c, 0x20092e, 0x200934, 0x200940, 0x200948, 0x20094a, 0x200950, 0x20095c, 0x200964, 0x200966, 0x20096c };


#endif // BT_S
                