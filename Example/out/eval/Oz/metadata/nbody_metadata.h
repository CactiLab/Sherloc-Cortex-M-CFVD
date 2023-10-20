
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 2
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 0; 
const uint32_t ibranch_count = 2; 
const uint32_t dbranch_count = 119;
const uint32_t dcall_count = 32; 
const uint32_t ret_count = 48; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b, 0xd22004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x200500, 0x200502, 0x20050c };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x2004fa, 0x200500, 0x200502, 0x2006ba };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x200496, 0x2004a4, 0x2004a8, 0x2004ac, 0x2004b6, 0x2004bc, 0x2004c2, 0x2004ce, 0x20052e, 0x200552, 0x2005b6, 0x2005be, 0x2005d2, 0x20062e, 0x200650, 0x20065c, 0x200664, 0x200670, 0x200678, 0x200680, 0x2006b0, 0x2006f8, 0x20074a, 0x2008a4, 0x200aaa, 0x200ae0, 0x200c7c, 0x200ee0, 0x2010be };
const uint32_t ret_list[] = { 0x200474, 0x200492, 0x20049a, 0x20049e, 0x2004c8, 0x2004d4, 0x2004da, 0x2004f2, 0x2006c4, 0x20075c, 0x2007e8, 0x2007f4, 0x20080c, 0x20087c, 0x20089c, 0x2009cc, 0x2009e0, 0x200a6a, 0x200a90, 0x200ac4, 0x200ada, 0x200b08, 0x200c2a, 0x200c5c, 0x200c7a, 0x200cae, 0x200d38, 0x200d52, 0x200d5e, 0x200e5a, 0x200e6c, 0x200ed6, 0x200ef2, 0x200f86, 0x200f90, 0x200f9c, 0x200ff8, 0x201004, 0x20103e, 0x20104c, 0x20105c, 0x201070, 0x201086, 0x201096, 0x2010a0, 0x2010b6, 0x2010ce, 0x2010ec };
const uint32_t dbranch_list[] = { 0x200418, 0x200456, 0x200462, 0x200466, 0x20046e, 0x200472, 0x200486, 0x2004ee, 0x2004fa, 0x200500, 0x200502, 0x200508, 0x200564, 0x200576, 0x20058a, 0x2005a0, 0x2005e0, 0x2005ea, 0x2005f0, 0x200602, 0x200610, 0x200616, 0x20063e, 0x20068c, 0x200694, 0x20069e, 0x2006aa, 0x2006b8, 0x2006ba, 0x2006ca, 0x200722, 0x20076c, 0x200776, 0x20079a, 0x2007ae, 0x2007d8, 0x200812, 0x20083a, 0x20084e, 0x20085a, 0x200860, 0x20086a, 0x20086c, 0x200882, 0x20088a, 0x2008c4, 0x2008da, 0x20090c, 0x2009b4, 0x2009c8, 0x2009dc, 0x2009e6, 0x2009f4, 0x200a3a, 0x200a48, 0x200a5a, 0x200a76, 0x200a78, 0x200a7e, 0x200a9e, 0x200ab4, 0x200aca, 0x200aec, 0x200af0, 0x200af2, 0x200af4, 0x200af6, 0x200af8, 0x200b0a, 0x200b0c, 0x200b12, 0x200b14, 0x200b7c, 0x200bce, 0x200be6, 0x200c0a, 0x200c28, 0x200c4c, 0x200c6c, 0x200c84, 0x200c88, 0x200c94, 0x200c9a, 0x200ca6, 0x200cc6, 0x200cd2, 0x200cde, 0x200ce0, 0x200cf8, 0x200cfe, 0x200d14, 0x200d6c, 0x200d88, 0x200e54, 0x200e64, 0x200e66, 0x200ea0, 0x200ea8, 0x200eb0, 0x200eb8, 0x200ec0, 0x200eca, 0x200ecc, 0x200ee4, 0x200f04, 0x200f0e, 0x200f36, 0x200f50, 0x200f76, 0x200fb8, 0x200fca, 0x200fe2, 0x201006, 0x201014, 0x201056, 0x201066, 0x201080, 0x20108e, 0x2010a6 };


#endif // BT_S
                