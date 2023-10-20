
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 0
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 0; 
const uint32_t ibranch_count = 0; 
const uint32_t dbranch_count = 49;
const uint32_t dcall_count = 19; 
const uint32_t ret_count = 5; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = {  };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x200454, 0x200458, 0x20045c };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20044e, 0x200454, 0x200458, 0x200d18 };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200404, 0x20040a, 0x200418, 0x20041c, 0x200420, 0x20042a, 0x200430, 0x200494, 0x200c8a, 0x200c96, 0x200ca4, 0x200cb2, 0x200cc0, 0x200cce, 0x200cdc, 0x200cea, 0x200cf8, 0x200d06, 0x200d14 };
const uint32_t ret_list[] = { 0x20040e, 0x200412, 0x200446, 0x200c74, 0x200d46 };
const uint32_t dbranch_list[] = { 0x200442, 0x20044e, 0x200454, 0x200458, 0x2009da, 0x2009e8, 0x2009f0, 0x200a00, 0x200a08, 0x200a1c, 0x200a28, 0x200a34, 0x200a42, 0x200a4e, 0x200a5c, 0x200a68, 0x200a7a, 0x200a8e, 0x200a9e, 0x200aaa, 0x200ab6, 0x200ac4, 0x200ad0, 0x200ae0, 0x200aee, 0x200b02, 0x200b04, 0x200b0e, 0x200b3a, 0x200b4c, 0x200b66, 0x200b8e, 0x200ba0, 0x200bba, 0x200bce, 0x200be6, 0x200bf8, 0x200c12, 0x200c22, 0x200c3a, 0x200c4c, 0x200c68, 0x200c6a, 0x200ca0, 0x200cbc, 0x200cd8, 0x200cf4, 0x200d10, 0x200d18 };


#endif // BT_S
                