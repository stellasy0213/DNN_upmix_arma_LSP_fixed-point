/*
    ITU-T G.729 Annex I  - Reference C code for fixed point
                         implementation of G.729 Annex I
                         Version 1.1 of October 1999
*/
/*
 File : tabld8cp.h
 */

/* from tab_ld8e.h G.729 Annex E Version 1.2  Last modified: May 1998 */
/* from tabld8kd.h G.729 Annex D Version 1.2  Last modified: May 1998 */
/* from tab_ld8k.h G.729 Annex B Version 1.3  Last modified: August 1997 */

#ifdef __cplusplus
extern "C" {
#endif

	extern short freq_prev_reset[M];
	extern short lag_h_B[M + 2];
	extern short lag_l_B[M + 2];
	extern short lag_h_bwd[M_BWD];
	extern short lag_l_bwd[M_BWD];
	extern short bitsno_B[PRM_SIZE_SID];
	extern short bitsno_D[PRM_SIZE_D];
	extern short bitsno_E_fwd[PRM_SIZE_E_fwd];
	extern short bitsno_E_bwd[PRM_SIZE_E_bwd];
	extern short hw[NRP + L_FRAME + M_BWD];
	extern short bitrates[2];
	extern short tab_log[17];
	extern short ipos[16];

	extern short gbk1_6k[NCODE1_6K][2];
	extern short gbk2_6k[NCODE2_6K][2];
	extern short coef_6k[2][2];
	extern int L_coef_6k[2][2];
	extern short thr1_6k[NCODE1_6K - NCAN1_6K];
	extern short thr2_6k[NCODE2_6K - NCAN2_6K];
	extern short map1_6k[NCODE1_6K];
	extern short imap1_6k[NCODE1_6K];
	extern short map2_6k[NCODE2_6K];
	extern short imap2_6k[NCODE2_6K];
	extern short trackTable0[16];
	extern short trackTable1[32];
	extern short grayEncode[32];
	extern short grayDecode[32];
	extern short ph_imp_low[L_SUBFR];
	extern short ph_imp_mid[L_SUBFR];

	extern short lspSid_reset[M];


#ifdef __cplusplus
}
#endif