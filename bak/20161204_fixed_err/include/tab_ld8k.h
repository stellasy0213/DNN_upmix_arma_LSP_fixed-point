/* Version 3.3    Last modified: December 26, 1995 */

extern short hamwindow[L_WINDOW];
extern short lag_h[M];
extern short lag_l[M];
extern short table[65];
extern short slope[64];
extern short table2[64];
extern short slope_cos[64];
extern short slope_acos[64];
extern short lspcb1[NC0][M];
extern short lspcb2[NC1][M];
extern short fg[2][MA_NP][M];
extern short fg_sum[2][M];
extern short fg_sum_inv[2][M];
extern short grid[GRID_POINTS+1];
extern short inter_3[FIR_SIZE_ANA];
extern short inter_3l[FIR_SIZE_SYN];
extern short pred[4];
extern short gbk1[NCODE1][2];
extern short gbk2[NCODE2][2];
extern short map1[NCODE1];
extern short map2[NCODE2];
extern short coef[2][2];
extern int L_coef[2][2];
extern short thr1[NCODE1-NCAN1];
extern short thr2[NCODE2-NCAN2];
extern short imap1[NCODE1];
extern short imap2[NCODE2];
extern short tab_hup_s[SIZ_TAB_HUP_S];
extern short tab_hup_l[SIZ_TAB_HUP_L];
extern short b100[3];
extern short a100[3];
extern short b140[3];
extern short a140[3];
extern short bitsno[PRM_SIZE];
extern short tabpow[33];
extern short tablog[33];
extern short tabsqr[49];
extern short tab_zone[PIT_MAX+L_INTERPOL-1];
