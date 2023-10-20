
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
const uint32_t PendSV = 0x201e5c;

const uint32_t PendSV_Entry = 0x201e5c;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x18201624 };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x201d64, 0x201f14, 0x202218, 0x20221c, 0x202258, 0x202270 };
const uint32_t TASK[] = { 0x200eb8, 0x200f9a, 0x201080, 0x201a7c, 0x2027f0, 0x2028f6, 0x2028f8, 0x2028fa };
const uint32_t LOOP[] = { 0x2006ec, 0x2006f4, 0x2006fc, 0x200704, 0x20070c, 0x200714, 0x2007b4, 0x200b64, 0x200bc4, 0x200cb4, 0x200d64, 0x200f90, 0x200f98, 0x200ffc, 0x2019e8, 0x2019f0, 0x2019f8, 0x201a00, 0x201b34, 0x201be0, 0x201cb4, 0x201cbc, 0x201fe4, 0x20204c };

const uint32_t icall_list[] = { 0x20121c, 0x20126c, 0x2012ee, 0x201620, 0x20171c, 0x201774, 0x2017f6, 0x201904 };
const uint32_t ibranch_list[] = { 0x18201624 };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x2004a8, 0x20051c, 0x200548, 0x200588, 0x200670, 0x200674, 0x2006a4, 0x2006e6, 0x2006ee, 0x2006f6, 0x2006fe, 0x200706, 0x20070e, 0x20073a, 0x2007b0, 0x200990, 0x200b2c, 0x200b5e, 0x200b78, 0x200bb6, 0x200cb0, 0x200cdc, 0x200d5e, 0x200eea, 0x200ef2, 0x200f50, 0x200f64, 0x200f8a, 0x200f92, 0x200ff6, 0x2010be, 0x2010fe, 0x201104, 0x201144, 0x201170, 0x201176, 0x2011d2, 0x201324, 0x201350, 0x2013a4, 0x2015d2, 0x2015d6, 0x20164c, 0x201680, 0x201690, 0x2016c2, 0x20183e, 0x2019e4, 0x2019ea, 0x2019f2, 0x2019fa, 0x201b20, 0x201b2e, 0x201bdc, 0x201cb0, 0x201cb6, 0x201d66, 0x201d6c, 0x201e8e, 0x201fe0, 0x202048 };
const uint32_t ret_list[] = { 0x200474, 0x200492, 0x2006d6, 0x200984, 0x200b5a, 0x200bbe, 0x200d38, 0x200eb4, 0x20107e, 0x201b2a, 0x201c8a, 0x201e10, 0x201e48, 0x201e58, 0x201f4e, 0x201fa4, 0x20203e, 0x20204e };
const uint32_t dbranch_list[] = { 0x200418, 0x200456, 0x200462, 0x200466, 0x20046e, 0x200472, 0x200486, 0x2004c8, 0x2004d4, 0x2004e2, 0x2004ee, 0x2004fc, 0x200506, 0x200514, 0x20053a, 0x200540, 0x20054e, 0x200550, 0x200556, 0x200568, 0x20056e, 0x20057a, 0x200580, 0x20058e, 0x200594, 0x200596, 0x20059e, 0x2005ac, 0x2005b4, 0x2005c6, 0x2005ca, 0x200648, 0x200654, 0x200660, 0x20068e, 0x20069e, 0x2006b4, 0x2006c0, 0x2006e2, 0x2006ec, 0x2006f4, 0x2006fc, 0x200704, 0x20070c, 0x200714, 0x200742, 0x20074c, 0x200756, 0x200760, 0x20076a, 0x200774, 0x20077e, 0x200788, 0x200792, 0x2007ae, 0x2007b4, 0x2007be, 0x2007e8, 0x200836, 0x20083a, 0x20086c, 0x200878, 0x200882, 0x2008b4, 0x2008be, 0x2008f0, 0x2009d4, 0x2009dc, 0x2009ee, 0x2009f8, 0x200b1c, 0x200b34, 0x200b40, 0x200b64, 0x200b7c, 0x200b9c, 0x200ba4, 0x200bc4, 0x200bd2, 0x200bea, 0x200c98, 0x200ca6, 0x200cae, 0x200cb4, 0x200cd6, 0x200cea, 0x200cf6, 0x200d12, 0x200d24, 0x200d4e, 0x200d5c, 0x200d64, 0x200d88, 0x200d90, 0x200dbc, 0x200dc0, 0x200dd2, 0x200dde, 0x200e22, 0x200e34, 0x200e54, 0x200e76, 0x200e7e, 0x200ee4, 0x200ef0, 0x200f40, 0x200f5a, 0x200f5e, 0x200f60, 0x200f68, 0x200f70, 0x200f88, 0x200f90, 0x200f98, 0x200f9e, 0x200fce, 0x200ff4, 0x200ffc, 0x2010c2, 0x201148, 0x2011c0, 0x2011c8, 0x2011ce, 0x201206, 0x20120c, 0x201226, 0x201256, 0x201262, 0x201266, 0x20127a, 0x201288, 0x201296, 0x20129e, 0x2012a6, 0x2012b0, 0x2012b2, 0x2012b8, 0x2012be, 0x2012dc, 0x2012e6, 0x2012fe, 0x201302, 0x20130a, 0x20130c, 0x201328, 0x20132c, 0x201330, 0x201392, 0x2013ae, 0x20141a, 0x201462, 0x20146c, 0x201496, 0x2014ae, 0x2014ba, 0x2014c2, 0x2014ca, 0x2014d4, 0x2014d6, 0x2014de, 0x2014ee, 0x2014f8, 0x201502, 0x20150e, 0x201510, 0x201528, 0x201534, 0x20153c, 0x201544, 0x20154e, 0x201550, 0x201558, 0x201560, 0x201566, 0x20156c, 0x201570, 0x20159e, 0x2015ac, 0x2015de, 0x2015f6, 0x2015fc, 0x201604, 0x201622, 0x201630, 0x201668, 0x20166c, 0x20168a, 0x201694, 0x2016b0, 0x2016c8, 0x2016d0, 0x2016d8, 0x20170a, 0x20170c, 0x201726, 0x201758, 0x201768, 0x20176c, 0x201786, 0x201794, 0x2017a6, 0x2017ae, 0x2017b6, 0x2017c0, 0x2017c2, 0x2017cc, 0x2017d2, 0x2017ec, 0x2017f8, 0x20181c, 0x201832, 0x20183a, 0x201842, 0x20184c, 0x20186c, 0x201870, 0x201876, 0x201878, 0x20188e, 0x2018b0, 0x2018be, 0x2018c6, 0x2018ce, 0x2018d8, 0x2018da, 0x2018ea, 0x2018ee, 0x2018f4, 0x2018fe, 0x201912, 0x201916, 0x20191c, 0x20191e, 0x201924, 0x20192e, 0x20193a, 0x201942, 0x20194a, 0x201954, 0x201956, 0x20195c, 0x201964, 0x201970, 0x201976, 0x201984, 0x201988, 0x2019a8, 0x2019b0, 0x2019be, 0x2019c8, 0x2019d0, 0x2019dc, 0x2019de, 0x2019e2, 0x2019e8, 0x2019f0, 0x2019f8, 0x201a00, 0x201aa0, 0x201aae, 0x201b1e, 0x201b34, 0x201b4e, 0x201b5e, 0x201b6e, 0x201b72, 0x201b7a, 0x201b7e, 0x201b84, 0x201b88, 0x201b8e, 0x201b92, 0x201b98, 0x201ba0, 0x201ba2, 0x201bac, 0x201bb2, 0x201bb8, 0x201bbe, 0x201bc6, 0x201bd4, 0x201bda, 0x201be0, 0x201be4, 0x201bf6, 0x201bfc, 0x201c02, 0x201c0a, 0x201c0c, 0x201c12, 0x201c1e, 0x201c2a, 0x201c30, 0x201c82, 0x201ca6, 0x201cae, 0x201cb4, 0x201cbc, 0x201cf4, 0x201cfa, 0x201d00, 0x201d08, 0x201d0a, 0x201d10, 0x201d1e, 0x201d2e, 0x201d3c, 0x201d60, 0x201d70, 0x201d86, 0x201f40, 0x201f78, 0x201f8e, 0x201fb2, 0x201fb8, 0x201fba, 0x201fbe, 0x201fc2, 0x201fc8, 0x201fcc, 0x201fda, 0x201fde, 0x201fe4, 0x201fe8, 0x201fec, 0x201ffa, 0x201ffe, 0x202020, 0x20202e, 0x202044, 0x20204c };


#endif // BT_S
                