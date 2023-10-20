
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS 1
#define NUM_HANDLER 6
#define NUM_TASK 4
#define NUM_LOOP 24

const uint32_t icall_count = 8; 
const uint32_t ibranch_count = 5; 
const uint32_t dbranch_count = 331;
const uint32_t dcall_count = 64; 
const uint32_t ret_count = 18; 
const uint32_t handler_count = 6; 
const uint32_t PendSV = 0x201e8c;

const uint32_t PendSV_Entry = 0x201e8c;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x1858165c };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x201d9c, 0x201f44, 0x202258, 0x20225c, 0x202260, 0x202278 };
const uint32_t TASK[] = { 0x200eb8, 0x200f9a, 0x201080, 0x201ab4, 0x20267c, 0x20267e, 0x2026d0, 0x20272a };
const uint32_t LOOP[] = { 0x2006ec, 0x2006f4, 0x2006fc, 0x200704, 0x20070c, 0x200714, 0x2007b4, 0x200b64, 0x200bc4, 0x200cb4, 0x200d64, 0x200f90, 0x200f98, 0x200ffc, 0x201a20, 0x201a28, 0x201a30, 0x201a38, 0x201b6c, 0x201c18, 0x201cec, 0x201cf4, 0x202014, 0x20207c };

const uint32_t icall_list[] = { 0x201254, 0x2012a4, 0x201326, 0x201658, 0x201754, 0x2017ac, 0x20182e, 0x20193c };
const uint32_t ibranch_list[] = { 0x1858165c };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x2004a8, 0x20051c, 0x200548, 0x200588, 0x200670, 0x200674, 0x2006a4, 0x2006e6, 0x2006ee, 0x2006f6, 0x2006fe, 0x200706, 0x20070e, 0x20073a, 0x2007b0, 0x200990, 0x200b2c, 0x200b5e, 0x200b78, 0x200bb6, 0x200cb0, 0x200cdc, 0x200d5e, 0x200eea, 0x200ef2, 0x200f50, 0x200f64, 0x200f8a, 0x200f92, 0x200ff6, 0x2010fc, 0x201138, 0x20113e, 0x20117e, 0x2011aa, 0x2011b0, 0x20120c, 0x20135c, 0x201388, 0x2013dc, 0x20160a, 0x20160e, 0x201684, 0x2016b8, 0x2016c8, 0x2016fa, 0x201876, 0x201a1c, 0x201a22, 0x201a2a, 0x201a32, 0x201b58, 0x201b66, 0x201c14, 0x201ce8, 0x201cee, 0x201d9e, 0x201da4, 0x201ebe, 0x202010, 0x202078 };
const uint32_t ret_list[] = { 0x200474, 0x200492, 0x2006d6, 0x200984, 0x200b5a, 0x200bbe, 0x200d38, 0x200eb4, 0x20107e, 0x201b62, 0x201cc2, 0x201e40, 0x201e78, 0x201e88, 0x201f7e, 0x201fd4, 0x20206e, 0x20207e };
const uint32_t dbranch_list[] = { 0x200418, 0x200456, 0x200462, 0x200466, 0x20046e, 0x200472, 0x200486, 0x2004c8, 0x2004d4, 0x2004e2, 0x2004ee, 0x2004fc, 0x200506, 0x200514, 0x20053a, 0x200540, 0x20054e, 0x200550, 0x200556, 0x200568, 0x20056e, 0x20057a, 0x200580, 0x20058e, 0x200594, 0x200596, 0x20059e, 0x2005ac, 0x2005b4, 0x2005c6, 0x2005ca, 0x200648, 0x200654, 0x200660, 0x20068e, 0x20069e, 0x2006b4, 0x2006c0, 0x2006e2, 0x2006ec, 0x2006f4, 0x2006fc, 0x200704, 0x20070c, 0x200714, 0x200742, 0x20074c, 0x200756, 0x200760, 0x20076a, 0x200774, 0x20077e, 0x200788, 0x200792, 0x2007ae, 0x2007b4, 0x2007be, 0x2007e8, 0x200836, 0x20083a, 0x20086c, 0x200878, 0x200882, 0x2008b4, 0x2008be, 0x2008f0, 0x2009d4, 0x2009dc, 0x2009ee, 0x2009f8, 0x200b1c, 0x200b34, 0x200b40, 0x200b64, 0x200b7c, 0x200b9c, 0x200ba4, 0x200bc4, 0x200bd2, 0x200bea, 0x200c98, 0x200ca6, 0x200cae, 0x200cb4, 0x200cd6, 0x200cea, 0x200cf6, 0x200d12, 0x200d24, 0x200d4e, 0x200d5c, 0x200d64, 0x200d88, 0x200d90, 0x200dbc, 0x200dc0, 0x200dd2, 0x200dde, 0x200e22, 0x200e34, 0x200e54, 0x200e76, 0x200e7e, 0x200ee4, 0x200ef0, 0x200f40, 0x200f5a, 0x200f5e, 0x200f60, 0x200f68, 0x200f70, 0x200f88, 0x200f90, 0x200f98, 0x200f9e, 0x200fce, 0x200ff4, 0x200ffc, 0x201100, 0x201182, 0x2011fa, 0x201202, 0x201208, 0x201240, 0x201246, 0x20125e, 0x20128e, 0x20129a, 0x20129e, 0x2012b2, 0x2012c0, 0x2012ce, 0x2012d6, 0x2012de, 0x2012e8, 0x2012ea, 0x2012f0, 0x2012f6, 0x201314, 0x20131e, 0x201336, 0x20133a, 0x201342, 0x201344, 0x201360, 0x201364, 0x201368, 0x2013ca, 0x2013e6, 0x201452, 0x20149a, 0x2014a4, 0x2014ce, 0x2014e6, 0x2014f2, 0x2014fa, 0x201502, 0x20150c, 0x20150e, 0x201516, 0x201526, 0x201530, 0x20153a, 0x201546, 0x201548, 0x201560, 0x20156c, 0x201574, 0x20157c, 0x201586, 0x201588, 0x201590, 0x201598, 0x20159e, 0x2015a4, 0x2015a8, 0x2015d6, 0x2015e4, 0x201616, 0x20162e, 0x201634, 0x20163c, 0x20165a, 0x201668, 0x2016a0, 0x2016a4, 0x2016c2, 0x2016cc, 0x2016e8, 0x201700, 0x201708, 0x201710, 0x201742, 0x201744, 0x20175e, 0x201790, 0x2017a0, 0x2017a4, 0x2017be, 0x2017cc, 0x2017de, 0x2017e6, 0x2017ee, 0x2017f8, 0x2017fa, 0x201804, 0x20180a, 0x201824, 0x201830, 0x201854, 0x20186a, 0x201872, 0x20187a, 0x201884, 0x2018a4, 0x2018a8, 0x2018ae, 0x2018b0, 0x2018c6, 0x2018e8, 0x2018f6, 0x2018fe, 0x201906, 0x201910, 0x201912, 0x201922, 0x201926, 0x20192c, 0x201936, 0x20194a, 0x20194e, 0x201954, 0x201956, 0x20195c, 0x201966, 0x201972, 0x20197a, 0x201982, 0x20198c, 0x20198e, 0x201994, 0x20199c, 0x2019a8, 0x2019ae, 0x2019bc, 0x2019c0, 0x2019e0, 0x2019e8, 0x2019f6, 0x201a00, 0x201a08, 0x201a14, 0x201a16, 0x201a1a, 0x201a20, 0x201a28, 0x201a30, 0x201a38, 0x201ad8, 0x201ae6, 0x201b56, 0x201b6c, 0x201b86, 0x201b96, 0x201ba6, 0x201baa, 0x201bb2, 0x201bb6, 0x201bbc, 0x201bc0, 0x201bc6, 0x201bca, 0x201bd0, 0x201bd8, 0x201bda, 0x201be4, 0x201bea, 0x201bf0, 0x201bf6, 0x201bfe, 0x201c0c, 0x201c12, 0x201c18, 0x201c1c, 0x201c2e, 0x201c34, 0x201c3a, 0x201c42, 0x201c44, 0x201c4a, 0x201c56, 0x201c62, 0x201c68, 0x201cba, 0x201cde, 0x201ce6, 0x201cec, 0x201cf4, 0x201d2c, 0x201d32, 0x201d38, 0x201d40, 0x201d42, 0x201d48, 0x201d56, 0x201d66, 0x201d74, 0x201d98, 0x201da8, 0x201dbe, 0x201f70, 0x201fa8, 0x201fbe, 0x201fe2, 0x201fe8, 0x201fea, 0x201fee, 0x201ff2, 0x201ff8, 0x201ffc, 0x20200a, 0x20200e, 0x202014, 0x202018, 0x20201c, 0x20202a, 0x20202e, 0x202050, 0x20205e, 0x202074, 0x20207c };


#endif // BT_S
                