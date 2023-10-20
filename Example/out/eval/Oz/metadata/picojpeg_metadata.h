
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 21
#define NUM_HANDLER 3
#define NUM_TASK 0
#define NUM_LOOP 4

const uint32_t icall_count = 15; 
const uint32_t ibranch_count = 21; 
const uint32_t dbranch_count = 238;
const uint32_t dcall_count = 73; 
const uint32_t ret_count = 25; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b, 0x82a06c4, 0x82e06c4, 0x83206c4, 0x83806c4, 0x90006c4, 0x90e06c4, 0x91606c4, 0x91e06c4, 0xebe0ece, 0xee00ece, 0xf040ece, 0xf720ece, 0x100c15c4, 0x100c17e4, 0x101215c4, 0x101815c4, 0x116806c4, 0x116e06c4, 0x117406c4, 0x12b81358 };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x200592, 0x200594, 0x2005a4 };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x20049e, 0x200592, 0x200594, 0x201244 };

const uint32_t icall_list[] = { 0x20082a, 0x20082e, 0x200832, 0x200838, 0x200900, 0x20090e, 0x200916, 0x20091e, 0x20100c, 0x201012, 0x201018, 0x20101e, 0x201168, 0x20116e, 0x201174 };
const uint32_t ibranch_list[] = { 0xebe0ece, 0xee00ece, 0xf040ece, 0xf720ece, 0x12b81358 };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x20045a, 0x200468, 0x20046c, 0x200470, 0x20047a, 0x200480, 0x2005c6, 0x200628, 0x200660, 0x200694, 0x2006f8, 0x20073c, 0x200748, 0x20074e, 0x200754, 0x200764, 0x20078a, 0x2007e2, 0x200854, 0x200880, 0x200894, 0x20089a, 0x2008a4, 0x2008bc, 0x2008d4, 0x200a34, 0x200a94, 0x200ab0, 0x200aba, 0x200b0e, 0x200b1c, 0x200b4e, 0x200eca, 0x200eee, 0x200ef6, 0x200fcc, 0x200fd4, 0x200fde, 0x200fe6, 0x200ff0, 0x200ff8, 0x20102c, 0x201040, 0x201072, 0x201078, 0x2010a8, 0x2010ae, 0x2010dc, 0x2010e6, 0x201184, 0x20120e, 0x20121a, 0x201224, 0x20122a, 0x201236, 0x2012a0, 0x2012aa, 0x201342, 0x201368, 0x20137a, 0x201384, 0x20139c, 0x2013d4, 0x2013de, 0x201438, 0x201442, 0x201458, 0x2014a0, 0x2014c2, 0x201c4e };
const uint32_t ret_list[] = { 0x200456, 0x20045e, 0x200462, 0x200496, 0x20052c, 0x200574, 0x200590, 0x2005f2, 0x20063e, 0x2006c0, 0x200714, 0x20075a, 0x200776, 0x2007aa, 0x2007ce, 0x201530, 0x20154a, 0x2015ba, 0x2015ec, 0x2016d4, 0x20176c, 0x20180c, 0x2018f4, 0x201990, 0x201a3c };
const uint32_t dbranch_list[] = { 0x200418, 0x20044a, 0x200492, 0x20049e, 0x2004a6, 0x2004ae, 0x2004e0, 0x2004e6, 0x2004f8, 0x200532, 0x200548, 0x200592, 0x200594, 0x200598, 0x20059e, 0x200606, 0x200620, 0x200624, 0x200652, 0x20067c, 0x20068a, 0x2006a6, 0x2006c6, 0x2006cc, 0x2006d6, 0x2006da, 0x200704, 0x200728, 0x200742, 0x200746, 0x20074c, 0x200784, 0x200788, 0x200794, 0x2007b6, 0x2007c2, 0x2007f2, 0x200802, 0x200846, 0x200850, 0x20085e, 0x200864, 0x20086c, 0x200878, 0x200886, 0x200890, 0x2008a0, 0x2008b0, 0x2008b8, 0x2008c8, 0x2008d0, 0x2008e2, 0x2008ee, 0x2008f8, 0x200928, 0x200934, 0x200938, 0x20093c, 0x20094a, 0x200956, 0x20096c, 0x200978, 0x200984, 0x200990, 0x20099c, 0x2009b0, 0x2009b8, 0x2009c2, 0x2009cc, 0x2009f0, 0x200a3a, 0x200a4a, 0x200a50, 0x200a56, 0x200a64, 0x200aae, 0x200ab4, 0x200b08, 0x200b18, 0x200b22, 0x200b2e, 0x200b30, 0x200b46, 0x200b66, 0x200b6e, 0x200b74, 0x200b7c, 0x200b90, 0x200b9a, 0x200b9e, 0x200bb2, 0x200bba, 0x200bea, 0x200c9c, 0x200cb2, 0x200cbe, 0x200cee, 0x200e74, 0x200eb2, 0x200ebc, 0x200ed6, 0x200ede, 0x200ee8, 0x200efa, 0x200f02, 0x200f0e, 0x200f12, 0x200f1a, 0x200f1e, 0x200f22, 0x200f2e, 0x200f68, 0x200f70, 0x200f7a, 0x200f7e, 0x200f8a, 0x200fc6, 0x200fd8, 0x200fea, 0x200ffc, 0x201000, 0x201004, 0x201020, 0x201028, 0x201032, 0x201036, 0x20103a, 0x201046, 0x201052, 0x201080, 0x201088, 0x201094, 0x20109e, 0x2010a4, 0x2010c4, 0x2010ca, 0x2010d0, 0x2010e4, 0x2010f4, 0x201104, 0x201108, 0x20112a, 0x201142, 0x20115c, 0x201182, 0x20118c, 0x20119c, 0x2011ae, 0x2011c4, 0x2011c6, 0x2011ce, 0x2011ea, 0x2011ec, 0x2011fe, 0x201208, 0x20122e, 0x201240, 0x201244, 0x20124a, 0x2012a8, 0x2012b4, 0x20133c, 0x201348, 0x201356, 0x20135c, 0x201362, 0x20136e, 0x201376, 0x201380, 0x20138a, 0x201398, 0x2013a6, 0x2013d0, 0x2013dc, 0x2013ee, 0x20140a, 0x20141e, 0x201434, 0x201436, 0x20143e, 0x20144a, 0x201454, 0x201460, 0x20146c, 0x20149e, 0x2014ae, 0x2014be, 0x2014c0, 0x2014ce, 0x2014de, 0x2014ec, 0x2014fc, 0x20151e, 0x201524, 0x20152a, 0x201536, 0x20154e, 0x201550, 0x201600, 0x2016a4, 0x2016aa, 0x2016de, 0x201736, 0x20173c, 0x201778, 0x2017d6, 0x2017dc, 0x201820, 0x2018c4, 0x2018ca, 0x2018fe, 0x201956, 0x20195c, 0x20199c, 0x2019fa, 0x201a00, 0x201ab4, 0x201aba, 0x201abc, 0x201ace, 0x201bda, 0x201c04, 0x201c12, 0x201c1e, 0x201c2a, 0x201c3c, 0x201c48, 0x201c4c, 0x201c6c, 0x201c76, 0x201c82, 0x201c90, 0x201c98, 0x201c9e, 0x201cb0, 0x201cc2, 0x201cce, 0x201cf2 };


#endif // BT_S
                