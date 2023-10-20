
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
const uint32_t dbranch_count = 219;
const uint32_t dcall_count = 34; 
const uint32_t ret_count = 23; 
const uint32_t handler_count = 3; 
const uint32_t PendSV = 0xdeadbeef;

const uint32_t PendSV_Entry = 0xdeadbeef;

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = { 0x432004b };

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = { 0x20050e, 0x200510, 0x2005bc };
const uint32_t TASK[] = {  };
const uint32_t LOOP[] = { 0x2004ba, 0x20050e, 0x200510, 0x201c9a };

const uint32_t icall_list[] = {  };
const uint32_t ibranch_list[] = {  };
const uint32_t dcall_list[] = { 0x200400, 0x200404, 0x20041a, 0x200476, 0x200484, 0x200488, 0x20048c, 0x200496, 0x20049c, 0x2005de, 0x20089c, 0x200a06, 0x200b76, 0x200cd0, 0x2010e4, 0x20112c, 0x201136, 0x201182, 0x20118a, 0x201192, 0x2016ee, 0x201710, 0x201804, 0x201826, 0x20191a, 0x20193c, 0x201bd8, 0x201be0, 0x201c08, 0x201c52, 0x201c8e, 0x2021be, 0x202492, 0x2030ea };
const uint32_t ret_list[] = { 0x200454, 0x200472, 0x20047a, 0x20047e, 0x2004b2, 0x2004fa, 0x20050c, 0x200548, 0x20057e, 0x20059e, 0x2005b8, 0x20104e, 0x201094, 0x201d7e, 0x201fc6, 0x202004, 0x202092, 0x2023b0, 0x2025e0, 0x2025ec, 0x2030d0, 0x2030de, 0x20331a };
const uint32_t dbranch_list[] = { 0x200418, 0x200444, 0x200466, 0x2004ae, 0x2004ba, 0x2004dc, 0x20050e, 0x200510, 0x2005fe, 0x20062a, 0x200630, 0x200636, 0x2007c6, 0x2007ca, 0x201066, 0x20106c, 0x20109a, 0x2010bc, 0x2010cc, 0x201110, 0x201120, 0x201140, 0x20114a, 0x201150, 0x201158, 0x20116a, 0x201178, 0x2011ea, 0x2011f4, 0x201206, 0x20120e, 0x20121e, 0x201222, 0x201242, 0x201248, 0x201256, 0x2013ce, 0x2013dc, 0x2013e2, 0x201bcc, 0x201bee, 0x201bfe, 0x201c10, 0x201c1e, 0x201c2e, 0x201c36, 0x201c4a, 0x201c5a, 0x201c66, 0x201c70, 0x201c78, 0x201c86, 0x201c96, 0x201c9a, 0x201ca2, 0x201cb6, 0x201cba, 0x201cc0, 0x201d04, 0x201d08, 0x201d0e, 0x201d6e, 0x201d70, 0x201e3e, 0x201e40, 0x201eb4, 0x201eba, 0x201ee8, 0x201fac, 0x202096, 0x2020b0, 0x2020b6, 0x202130, 0x202168, 0x2021a4, 0x2021a8, 0x2021b0, 0x2021d4, 0x2021e6, 0x2021f2, 0x2021fc, 0x202206, 0x202212, 0x20223a, 0x20223e, 0x202264, 0x202278, 0x20228e, 0x2022b2, 0x2022c6, 0x2022c8, 0x2022f2, 0x2022f8, 0x2022fc, 0x20230e, 0x20231a, 0x202320, 0x202334, 0x20233c, 0x202350, 0x202356, 0x20235a, 0x20237e, 0x202392, 0x2023a0, 0x2023ae, 0x2023b2, 0x2023ce, 0x2023ea, 0x2023fa, 0x2023fc, 0x202406, 0x20240e, 0x202410, 0x20244e, 0x202450, 0x202476, 0x202498, 0x2024ac, 0x2024b2, 0x2024bc, 0x2024d8, 0x2024de, 0x2024ea, 0x2024f2, 0x2024f4, 0x202508, 0x20250a, 0x20250c, 0x20252e, 0x202552, 0x20256e, 0x20258c, 0x202592, 0x2025c8, 0x2025e8, 0x2025fa, 0x202636, 0x20263e, 0x202650, 0x202662, 0x20267c, 0x202686, 0x2026a2, 0x2026a8, 0x2026c0, 0x2026d2, 0x2026da, 0x2026dc, 0x2026f6, 0x20271e, 0x202730, 0x202740, 0x202744, 0x20274a, 0x202762, 0x20277a, 0x20277c, 0x20277e, 0x202786, 0x20278a, 0x2027cc, 0x2027ce, 0x2027f0, 0x2027f8, 0x2027fc, 0x20281c, 0x202850, 0x2030c2, 0x2030dc, 0x2030e2, 0x2030f8, 0x2030fa, 0x203144, 0x203146, 0x20314a, 0x20314e, 0x203156, 0x203164, 0x203174, 0x203178, 0x203186, 0x20318e, 0x203192, 0x2031aa, 0x2031bc, 0x2031d4, 0x2031d8, 0x20321e, 0x20323a, 0x20323c, 0x203240, 0x203258, 0x20326c, 0x203280, 0x203286, 0x20328e, 0x203292, 0x203294, 0x203298, 0x2032fc, 0x203314, 0x20331e, 0x20332c, 0x203332, 0x20333a, 0x20339c, 0x2033a0, 0x2033b4, 0x2033be, 0x2033e2, 0x2033f6, 0x2033fa, 0x203404, 0x203420, 0x203438, 0x203446, 0x20344a, 0x20344e };


#endif // BT_S
                