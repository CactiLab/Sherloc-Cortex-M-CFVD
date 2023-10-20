
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 1
#define NUM_HANDLER 6
#define NUM_TASK 4
#define NUM_LOOP 19

const uint32_t icall_count = 3; 
const uint32_t ibranch_count = 6; 
const uint32_t dbranch_count = 201;
const uint32_t dcall_count = 88; 
const uint32_t ret_count = 30; 
const uint32_t handler_count = 6; 
const uint32_t PendSV = 0x201478;

const uint32_t PendSV_Entry = 0x201478;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0xf900f18 };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x201390, 0x201524, 0x2017b8, 0x2017ba, 0x2017e8, 0x201800 };
const uint32_t TASK[] = { 0x200cec, 0x200d58, 0x200e20, 0x201040, 0x201ad4, 0x201bda, 0x201bda, 0x201bdc };
const uint32_t LOOP[] = { 0x20067c, 0x200682, 0x200688, 0x20068e, 0x2006de, 0x20073a, 0x200aca, 0x200bec, 0x200d54, 0x200db6, 0x201036, 0x20103c, 0x201218, 0x20126a, 0x201314, 0x20131c, 0x201380, 0x20158e, 0x2015da };

const uint32_t icall_list[] = { 0x200fd6, 0x201018, 0x201196 };
const uint32_t ibranch_list[] = { 0xf900f18 };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x2004f6, 0x200542, 0x20056c, 0x2005a6, 0x2005ea, 0x200642, 0x200646, 0x20065e, 0x200678, 0x20067e, 0x200684, 0x20068a, 0x2006a4, 0x2006cc, 0x2006da, 0x2006fe, 0x200736, 0x200894, 0x200978, 0x2009bc, 0x2009c6, 0x2009e8, 0x2009ee, 0x2009f8, 0x200a3a, 0x200a88, 0x200aaa, 0x200abc, 0x200ac6, 0x200b6c, 0x200b76, 0x200be8, 0x200c08, 0x200cfe, 0x200d0a, 0x200d1a, 0x200d2e, 0x200d34, 0x200d50, 0x200db2, 0x200e2c, 0x200e32, 0x200e3e, 0x200e44, 0x200e68, 0x200e72, 0x200e8e, 0x200eae, 0x200eec, 0x200ef2, 0x200ef6, 0x200f0c, 0x200f14, 0x200f26, 0x200f48, 0x200f58, 0x200f6c, 0x200f7e, 0x200f84, 0x200fb6, 0x201002, 0x201010, 0x201026, 0x20102c, 0x201032, 0x201038, 0x2010dc, 0x20110c, 0x201128, 0x201168, 0x20118a, 0x201208, 0x201214, 0x201266, 0x201270, 0x201310, 0x201318, 0x201336, 0x20134e, 0x20137c, 0x201392, 0x201398, 0x2014aa, 0x20158a, 0x2015d6 };
const uint32_t ret_list[] = { 0x200474, 0x200492, 0x2004cc, 0x2004ee, 0x2006e2, 0x200880, 0x20099a, 0x200a02, 0x200a1e, 0x200a6c, 0x200a94, 0x200ac2, 0x200bce, 0x200ce0, 0x200e10, 0x2010f4, 0x20116e, 0x201172, 0x20119c, 0x201212, 0x2012e8, 0x201316, 0x201362, 0x20137a, 0x201430, 0x201468, 0x201476, 0x201550, 0x201588, 0x2015d4 };
const uint32_t dbranch_list[] = { 0x200418, 0x200456, 0x200462, 0x200466, 0x20046e, 0x200472, 0x200486, 0x2004a6, 0x2004b4, 0x2004b6, 0x200508, 0x200514, 0x200522, 0x20052c, 0x20053a, 0x200560, 0x200566, 0x200570, 0x200572, 0x200578, 0x200588, 0x20058e, 0x200598, 0x20059e, 0x2005aa, 0x2005ac, 0x2005b4, 0x2005be, 0x2005c4, 0x2005cc, 0x2005e0, 0x2005e2, 0x20062c, 0x20063a, 0x200654, 0x200658, 0x200662, 0x200668, 0x200674, 0x20067c, 0x200682, 0x200688, 0x20068e, 0x2006a0, 0x2006a8, 0x2006d8, 0x2006de, 0x20070c, 0x200716, 0x200722, 0x20072a, 0x200734, 0x20073a, 0x20076c, 0x200778, 0x200780, 0x200794, 0x2007aa, 0x2007ac, 0x2007b2, 0x2007e8, 0x2007f6, 0x20087a, 0x2008a2, 0x2008a8, 0x2008b6, 0x2008be, 0x2008cc, 0x2008e2, 0x20097e, 0x200988, 0x2009c0, 0x2009ca, 0x2009f4, 0x200a0e, 0x200a16, 0x200a40, 0x200a42, 0x200a74, 0x200a80, 0x200aa8, 0x200ab6, 0x200aca, 0x200ad4, 0x200aea, 0x200b68, 0x200b6a, 0x200b72, 0x200b7a, 0x200b80, 0x200b8a, 0x200ba0, 0x200bc0, 0x200bde, 0x200be6, 0x200bec, 0x200c14, 0x200c26, 0x200c38, 0x200c56, 0x200cb2, 0x200cfc, 0x200d24, 0x200d28, 0x200d2a, 0x200d38, 0x200d3e, 0x200d4e, 0x200d54, 0x200d6a, 0x200d92, 0x200db0, 0x200db6, 0x200e52, 0x200e5a, 0x200e70, 0x200e76, 0x200e78, 0x200e7c, 0x200e80, 0x200eb4, 0x200efa, 0x200f0a, 0x200f1c, 0x200f22, 0x200f2e, 0x200f34, 0x200f52, 0x200f5c, 0x200f74, 0x200f7a, 0x200f8e, 0x200fbc, 0x200fc4, 0x200fce, 0x200fd8, 0x200fe6, 0x200ff8, 0x201006, 0x20101a, 0x201022, 0x20102a, 0x201030, 0x201036, 0x20103c, 0x2010cc, 0x2010d2, 0x2010e0, 0x2010e4, 0x201116, 0x201120, 0x20114a, 0x201152, 0x201156, 0x201158, 0x201160, 0x20118e, 0x20119a, 0x2011ac, 0x2011b8, 0x201204, 0x201218, 0x201220, 0x20122c, 0x20122e, 0x201236, 0x201246, 0x20124c, 0x201252, 0x20125e, 0x201264, 0x20126a, 0x20129c, 0x2012b6, 0x2012cc, 0x2012d0, 0x2012d6, 0x2012f2, 0x201306, 0x20130e, 0x201314, 0x20131c, 0x201344, 0x20136e, 0x201378, 0x201380, 0x201388, 0x20139c, 0x2013ac, 0x20154a, 0x201566, 0x201568, 0x20156e, 0x20158e, 0x2015a2, 0x2015a6, 0x2015c2, 0x2015ca, 0x2015d2, 0x2015da, 0x2015e0 };


#endif // BT_S
                