/* **************************************************************************
*		Gwangju Institute of Science and Technology, HuCom Lab				*
*																			*
*							COPYRIGHT (C) 2016								*
*							All Rights Reserved								*
*																			*
*****************************************************************************

*****************************************************************************
* @file	: nr.c														*
*																			*
* @brief	:	NMF API	(Interspeech16)				 						*
*																			*
*---------------------------------------------------------------------------*
*    Author(s)        Date               Updation Details					*
*---------------------------------------------------------------------------*
*																			*
* Kwang Myung Jeon      2016-04-05       Initial C version of NNET_UPMIX_IS16_0322,*
* kmjeon@gist.ac.kr															*
* 																			*
* 																			*
*---------------------------------------------------------------------------*

*************************************************************************** */




#include "nnet_upmix_api.h"
#include "nnet_upmix_table.h"
#include "typedef.h"
#include "ld8k.h"
#include "ld8cp.h"


//====function definition
void LP_env_log(short * A, float * env);

//======global buffers
extern short * NNET_UPMIX_frm_cnt;
extern int *NNET_UPMIX_frm_stck_i;
extern int *NNET_UPMIX_frm_stck_o;
extern int *NNET_UPMIX_I_reorder;
extern kiss_fft_scalar  * NNET_UPMIX_FFT_time_i;
extern kiss_fft_cpx  * NNET_UPMIX_FFT_freq_i;
extern kiss_fft_scalar  * NNET_UPMIX_FFT_time_o;
extern float * NNET_UPMIX_f_LSF_buff;
extern float * NNET_UPMIX_f_LSF_dec_buff;

extern int * NNET_UPMIX_i_LSF_buff;
extern int * NNET_UPMIX_i_LSF_dec_buff;

extern double * NNET_UPMIX_FFT_mag_i;
extern double * NNET_UPMIX_FFT_angl_i;
extern float * f_LSF;
extern float * f_LSF_dec;
extern int * i_LSF_dec;
extern short NNET_UPMIX_frm_idx;
extern double init_scale;
//extern Nnet NNET_UPMIX_nnet_transf;
//extern Nnet NNET_UPMIX_nnet;



/*--------------------------------------------------------*
*         Static memory allocation.                      *
*--------------------------------------------------------*/


/* Speech vector */

static short old_speech[L_TOTAL];
static short *speech, *p_window;
short *new_speech;                    /* Global variable */

/* Weighted speech vector */

static short old_wsp[L_FRAME + PIT_MAX];
static short *wsp;

/* Excitation vector */
static short old_exc[L_FRAME + PIT_MAX + L_INTERPOL];
static short *exc;

/* Zero vector */
static short ai_zero[L_SUBFR + M_BWDP1];
static short *zero;

/* Lsp (Line spectral pairs) */
static short lsp_old[M] = {
	30000, 26000, 21000, 15000, 8000, 0, -8000, -15000, -21000, -26000 };
static short lsp_old_q[M];

static short lsp_out_old[M];


/* Filter's memory */

static short mem_syn[M_BWD], mem_w0[M_BWD], mem_w[M_BWD];
static short mem_err[M_BWD + L_SUBFR], *error;
static short sharp;

/* to tame the coder */
static int L_exc_err[4];

/* For G.729B */
/* DTX variables */
static short pastVad;
static short ppastVad;
static short seed;

/* for the backward analysis */
static short    prev_filter[M_BWDP1]; /* Previous selected filter */

static int  rexp[M_BWDP1];
static short synth[L_ANA_BWD];
static short *synth_ptr;
static short prev_lp_mode;
static short gamma1[2], gamma2[2];       /* Weighting factor for the 2 subframes */
static short A_t_bwd_mem[M_BWDP1];
static short bwd_dominant, C_int;              /* See file bwfw.c */
static short   glob_stat;  /* Mesure of global stationnarity Q8 */
static short   stat_bwd;       /* Nbre of consecutive backward frames */
static short   val_stat_bwd;   /* Value associated with stat_bwd */

/* Last backward A(z) for case of unstable filter */
static short old_A_bwd[M_BWDP1];
static short old_rc_bwd[2];
/* Last forkward A(z) for case of unstable filter */
static short old_A_fwd[MP1];
static short old_rc_fwd[2];
static short freq_prev[MA_NP][M];    /* Q13:previous LSP vector       */

static short lag_buf[5] = { 20, 20, 20, 20, 20 };
static short pgain_buf[5] = { 11469, 11469, 11469, 11469, 11469 };

/* Lsp (Line spectral pairs) */


/*****************************************************************

int NNET_UPMIX_Execution(	short* input,
						short* output,
						short ch_num
					);

This module is main function for NMF Audio IP,
This include the below operations 
1-	FFT
2-  NMF
3-	NR
4-	OL
5-  IFFT

*****************************************************************/



int NNET_UPMIX_Execution(	short* input,
								short* output,
								short ch_num)

{

	/* LPC analysis */
	short r_l_fwd[NP + 1], r_h_fwd[NP + 1];    /* Autocorrelations low and hi (forward) */
	int r_bwd[M_BWDP1];      /* Autocorrelations (backward) */
	short r_l_bwd[M_BWDP1];      /* Autocorrelations low (backward) */
	short r_h_bwd[M_BWDP1];      /* Autocorrelations high (backward) */
	short rc_fwd[M];                 /* Reflection coefficients : forward analysis */
	short rc_bwd[M_BWD];         /* Reflection coefficients : backward analysis */
	short A_t_fwd[MP1 * 2];          /* A(z) forward unquantized for the 2 subframes */
	short A_t_fwd_q[MP1 * 2];      /* A(z) forward quantized for the 2 subframes */
	short A_t_bwd[2 * M_BWDP1];    /* A(z) backward for the 2 subframes */
	short *Aq;           /* A(z) "quantized" for the 2 subframes */
	short *Ap;           /* A(z) "unquantized" for the 2 subframes */
	short *pAp, *pAq;
	short Ap1[M_BWDP1];          /* A(z) with spectral expansion         */
	short Ap2[M_BWDP1];          /* A(z) with spectral expansion         */
	short lsp_new[M], lsp_new_q[M]; /* LSPs at 2th subframe                 */
	short lsf_int[M];               /* Interpolated LSF 1st subframe.       */
	short lsf_new[M];
	int lsf_new_i[M];


	short lsp_out_new[M], lsp_out_new_q[M]; /* LSPs at 2th subframe                 */
	short lsf_out_int[M];               /* Interpolated LSF 1st subframe.       */
	short lsf_out_new[M];

	short lp_mode;                  /* Backward / Forward Indication mode */
	short m_ap, m_aq, i_gamma;
	short code_lsp[2];

	/* Other vectors */

	short h1[L_SUBFR];            /* Impulse response h1[]              */
	short xn[L_SUBFR];            /* Target vector for pitch search     */
	short xn2[L_SUBFR];           /* Target vector for codebook search  */
	short code[L_SUBFR];          /* Fixed codebook excitation          */
	short y1[L_SUBFR];            /* Filtered adaptive excitation       */
	short y2[L_SUBFR];            /* Filtered fixed codebook excitation */
	short g_coeff[4];             /* Correlations between xn & y1       */
	short res2[L_SUBFR];          /* residual after long term prediction*/
	short g_coeff_cs[5];
	short exp_g_coeff_cs[5];      /* Correlations between xn, y1, & y2
								   <y1,y1>, -2<xn,y1>,
								   <y2,y2>, -2<xn,y2>, 2<y1,y2> */
	/* Scalars */
	short i, j, k, i_subfr;
	short T_op, T0, T0_min, T0_max, T0_frac;
	short gain_pit, gain_code, index;
	short taming, pit_sharp;
	short sat_filter;
	int L_temp;
	short freq_cur[M];

	/* For G.729B */
	short rh_nbe[MP1];
	short lsfq_mem[MA_NP][M];
	short exp_R0, Vad;
	short tmp1, tmp2, avg_lag;

	short temp, Energy_db;


	short ch, ch_idx, ch_fft_idx, ch_feat_idx;
	
	int *p_I_reorder = NNET_UPMIX_I_reorder;
	short *p_O_reorder = output;
	int fixed_out_sample;
	short NNET_UPMIX_FFT_time_i_Q0[NNET_UPMIX_FFT_LEN];
	float LP_Mag_dec[NNET_UPMIX_FFT21_LEN];


	//Internal buffer
	double NNET_UPMIX_FFT_mag_out_i[NNET_UPMIX_FFT21_LEN];
	
	

		if(ch_num == 2) /*SRI-B: Mono/Stereo switching implemented NNET_UPMIX_FFT_time_i Runtime */
		{	
			for(i = 0;i<NNET_UPMIX_FRM_LEN;i++)
			{
				*p_I_reorder = *(input++);
				*(p_I_reorder + NNET_UPMIX_FRM_LEN) = *(input++);
				p_I_reorder++;
			}
		}
		else//Mono case
		{
			for(i = 0;i<NNET_UPMIX_FRM_LEN;i++)
			{
				*p_I_reorder = *(input++);
				p_I_reorder++;
			}
		}


	//Channel wise processing begins
	ch_idx = 0; ch_fft_idx = 0; ch_feat_idx = 0;
	for (ch = 0; ch < ch_num; ch++)
	{

		//Buffer shift (2개 모아 1회 처리, 50% overlap)
		memmove(NNET_UPMIX_frm_stck_i + ch_fft_idx,
			NNET_UPMIX_frm_stck_i + NNET_UPMIX_FRM_LEN + ch_fft_idx,
			NNET_UPMIX_FRM_LEN *(NNET_UPMIX_STACK_LEN - 1) * sizeof(float));
		memcpy(NNET_UPMIX_frm_stck_i + NNET_UPMIX_FRM_LEN*(NNET_UPMIX_STACK_LEN - 1) + ch_fft_idx,
			NNET_UPMIX_I_reorder + ch_idx,
			NNET_UPMIX_FRM_LEN * sizeof(float));


		//------Windowing------
		for (k = 0; k < NNET_UPMIX_FFT_LEN; ++k)
		{
			NNET_UPMIX_FFT_time_i[k] = NNET_UPMIX_frm_stck_i[k + ch_fft_idx] * NNET_UPMIX_win_shift[k]; //apply OLA window (Q15)
			NNET_UPMIX_FFT_time_i_Q0[k] = (short)(NNET_UPMIX_FFT_time_i[k] >> 15);
		}


		//------begin FFT------
		kiss_fftr_cfg  cfg = kiss_fftr_alloc(NNET_UPMIX_FFT_LEN, 0, 0, 0);
		kiss_fftr(cfg, NNET_UPMIX_FFT_time_i, NNET_UPMIX_FFT_freq_i);



		//=======Conduct frame-wise spectral processing here=============

		//reform abs and angle of complex spetrum
		double d_re, d_im;
		for (k = 0; k < NNET_UPMIX_FFT21_LEN; ++k)
		{
			d_re = (double)(NNET_UPMIX_FFT_freq_i[k].r >> Q_FFT); //FFT Fraction: Q5
			d_im = (double)(NNET_UPMIX_FFT_freq_i[k].i >> Q_FFT);
			*(NNET_UPMIX_FFT_mag_i + k) = sqrt(pow(d_re, 2) + pow(d_im, 2)); //abs
			*(NNET_UPMIX_FFT_angl_i + k) = atan2(d_im, d_re);	//angle
		}

		//==================================================================================================
		//==============Begin Frame-wise Upmixing===========================================================
		//==================================================================================================

		//1) Extract Input features (.ark)
		//-----begin LPC---------
		/* LP analysis */

		/*
		Autocorrcp(NNET_UPMIX_FFT_time_i_Q0, NP, r_h_fwd, r_l_fwd, &exp_R0);                   // Autocorrelations //
		Lag_window(NP, r_h_fwd, r_l_fwd);                           // Lag windowing    //
		Levinsoncp(M, r_h_fwd, r_l_fwd, &A_t_fwd[MP1], rc_fwd, old_A_fwd, old_rc_fwd, &temp); // Levinson Durbin  //
		//  input: autocorr coeff. / output: LPC coeff.(a[0] = 1)/ output: reflection coeff.  //
		Az_lsp(A_t_fwd, lsp_new, lsp_old);          // From A(z) to lsp //
		Lsp_lsf2(lsp_new, lsf_new, M); // Output: Q13
		*/

		
		Autocorrcp(NNET_UPMIX_FFT_time_i_Q0, NP, r_h_fwd, r_l_fwd, &exp_R0);                   // Autocorrelations //
		Lag_window(NP, r_h_fwd, r_l_fwd);                           // Lag windowing    //
		Levinsoncp(M, r_h_fwd, r_l_fwd, &A_t_fwd[MP1], rc_fwd, old_A_fwd, old_rc_fwd, &temp); // Levinson Durbin  //
		//  input: autocorr coeff. / output: LPC coeff.(a[0] = 1)/ output: reflection coeff.  //
		Az_lsp(&A_t_fwd[MP1], lsp_new, lsp_old);          // From A(z) to lsp //
		Lsp_lsf2(lsp_new, lsf_new, M); // Output: Q13
	

		

		
#if UPMIX_DEBUG == 1
		for (int z = 0; z < NNET_UPMIX_LPCOEFF; z++)
			printf("%f\n", f_LSF[z]);

		printf("------LSF_in-----\n");
#endif

		/*
		// fwrite lsp_new
		FILE *buf;
		int count = 0, arr1[2000] = { 0 };

		fopen_s(&buf, "RESULT_LSP.txt", "ab");

		for (int z = 0; z < NNET_UPMIX_LPCOEFF; z++)
		fprintf(buf, "%lf", lsp_new[z]);
		fprintf(buf, "\n");
		*/

#if FIXED == 1
		for (int z = 0; z < NNET_UPMIX_LPCOEFF; z++)
		{
			f_LSF[z] = (float)lsf_new[z] / (float)PI; //normalize DNN input to have 0~1
			lsf_new_i[z] = (int)f_LSF[z];
		}

		//Set Splice frames
		int * p_i_LSF_buff = NNET_UPMIX_i_LSF_buff;

		memmove(NNET_UPMIX_i_LSF_buff + ch_feat_idx,
			NNET_UPMIX_i_LSF_buff + NNET_UPMIX_LPCOEFF + ch_feat_idx,
			NNET_UPMIX_LPCOEFF *(SPLICE21 - 1) * sizeof(int));

		memcpy(NNET_UPMIX_i_LSF_buff + NNET_UPMIX_LPCOEFF*(SPLICE21 - 1) + ch_feat_idx,
			lsf_new_i + ch_idx,
			NNET_UPMIX_LPCOEFF * sizeof(int));

		//2) nnet Decode
		int * p_i_LSF_dec = i_LSF_dec; //Q13

#if NNET_FEEDFORWARD_ARMA == 1


		nnet_upmix_feedforward_fixed(p_i_LSF_buff, p_i_LSF_dec);


		for (int z = 0; z < NNET_UPMIX_LPCOEFF; z++)
		{
			lsf_out_new[z] =(int)((float)p_i_LSF_dec[z] *PI); //Denormalization, (Q13)
		}


#else
		nnet_upmix_decoder_kaldi(p_f_LSF_buff, p_f_LSF_dec);
#endif

#else
		for (int z = 0; z < NNET_UPMIX_LPCOEFF; z++)
		{
			f_LSF[z] = (float)lsf_new[z] / (float)8192.0;
			f_LSF[z] /= (float)PI; //normalize DNN input to have 0~1
		}

		//Set Splice frames
		float * p_f_LSF_buff = NNET_UPMIX_f_LSF_buff;
		
		memmove(NNET_UPMIX_f_LSF_buff + ch_feat_idx,
			NNET_UPMIX_f_LSF_buff + NNET_UPMIX_LPCOEFF + ch_feat_idx,
			NNET_UPMIX_LPCOEFF *(SPLICE21 - 1) * sizeof(float));

		memcpy(NNET_UPMIX_f_LSF_buff + NNET_UPMIX_LPCOEFF*(SPLICE21 - 1) + ch_feat_idx,
			f_LSF + ch_idx,
			NNET_UPMIX_LPCOEFF * sizeof(float));

		//2) nnet Decode
		float * p_f_LSF_dec = f_LSF_dec;

#if NNET_FEEDFORWARD_ARMA == 1


		nnet_upmix_feedforward(p_f_LSF_buff, p_f_LSF_dec);


		for (int z = 0; z < NNET_UPMIX_LPCOEFF; z++)
		{
			lsf_out_new[z] = (short)(p_f_LSF_dec[z] * 8192.0 * PI); //Denormalization, (Q13)
		}


#else
		nnet_upmix_decoder_kaldi(p_f_LSF_buff, p_f_LSF_dec);
#endif
#endif


		//3) wave synthesis

		//----------Get LP-Mag from LSP-----------
		Lsf_lsp2(lsf_out_new, lsp_out_new, M);

		/* ---------------- */
		/* LSP quantization */
		/* ---------------- */
		

#if UPMIX_DEBUG == 1
		for (int z = 0; z < NNET_UPMIX_LPCOEFF; z++)
		{
			printf("%f\n", lsf_out_new[z]);
		}
		printf("------LSF_out------\n");
#endif
		
		Int_qlpc(lsp_out_old, lsp_out_new, &A_t_fwd[0]); //Convert LSP to LPC coeff. (Out Q12)
		
#if UPMIX_DEBUG == 1
		for (int z = 0; z < MP1; z++)
		{
			printf("%f\n", A_out_t[z]);
		}
		printf("-------A_out-------\n");
#endif

		//Get LP envelope from A_out_t
		LP_env_log(A_t_fwd, LP_Mag_dec);

#if UPMIX_DEBUG == 1
		for (int z = 0; z < NNET_UPMIX_FFT21_LEN; z++)
		{
			printf("%f\n", LP_Mag_dec[z]);
		}
		printf("-------LP_env_log-------\n");
#endif

		/* update the LSPs for the next frame */
		Copy(lsp_out_new, lsp_out_old, M);
		Copy(lsp_new, lsp_old, M);

		//delta feature de-normalization
		for (k = 0; k < NNET_UPMIX_FEAT_LEN; ++k)
		{				
			NNET_UPMIX_FFT_mag_out_i[k] = exp((double)*(LP_Mag_dec + k) + EPSILON * log(*(NNET_UPMIX_FFT_mag_i + k)));
		}
 
		//Power normalization of out_mag by matching with in_mag
		double pow_buff_in = 0.0, pow_buff_out = 0.0;
		if (NNET_UPMIX_frm_idx < FRM_IDX_INIT)
		{
			for (k = 0; k < NNET_UPMIX_FEAT_LEN; ++k)
			{
				pow_buff_in += pow(*(NNET_UPMIX_FFT_mag_i + k), 2.0);
				pow_buff_out += pow(*(NNET_UPMIX_FFT_mag_out_i + k), 2.0);
			}
			init_scale += sqrt(pow_buff_in / pow_buff_out);
			init_scale /= 2;
		}
		for (k = 0; k < NNET_UPMIX_FEAT_LEN; ++k)
		{
			NNET_UPMIX_FFT_mag_out_i[k] *= init_scale;
		}

		// Spectral mixing between Out_Mag and In_Mag
		for (k = 0; k < NNET_UPMIX_FEAT_LEN; ++k)
		{
			NNET_UPMIX_FFT_mag_out_i[k] = ETA * NNET_UPMIX_FFT_mag_out_i[k] + (1 - ETA) * NNET_UPMIX_FFT_mag_i[k];
			NNET_UPMIX_FFT_mag_out_i[k] *= (1 - ETA);
		}
//==================================================================================================
//==================================================================================================

		//reform abs and angle of complex spetrum
		for (k = 0; k < NNET_UPMIX_FFT21_LEN; ++k)
		{
			NNET_UPMIX_FFT_freq_i[k].r = (int)(NNET_UPMIX_FFT_mag_out_i[k] * cos(NNET_UPMIX_FFT_angl_i[k]) * 32.0);
			//NNET_UPMIX_FFT_freq_i[k].r = (NNET_UPMIX_FFT_mag_out_i[k] * cos(NNET_UPMIX_FFT_angl_i[k])) >> 5;
			//NNET_UPMIX_FFT_freq_i[k].r = (int)NNET_UPMIX_FFT_freq_i[k].r;
			NNET_UPMIX_FFT_freq_i[k].i = (int)(NNET_UPMIX_FFT_mag_out_i[k] * sin(NNET_UPMIX_FFT_angl_i[k]) * 32.0);
			//NNET_UPMIX_FFT_freq_i[k].i = (NNET_UPMIX_FFT_mag_out_i[k] * sin(NNET_UPMIX_FFT_angl_i[k]));
			//NNET_UPMIX_FFT_freq_i[k].i = (int)NNET_UPMIX_FFT_freq_i[k].i >> 5;
		}


		//------begin inverse FFT------
		kiss_fftr_cfg  cfg_inv = kiss_fftr_alloc(NNET_UPMIX_FFT_LEN, 1, 0, 0);
		kiss_fftri(cfg_inv, NNET_UPMIX_FFT_freq_i, NNET_UPMIX_FFT_time_o);

		for (k = 0; k < NNET_UPMIX_FFT_LEN; k++)
		{
			fixed_out_sample = (((NNET_UPMIX_FFT_time_o[k] >> (Q_FFT)) * NNET_UPMIX_win_shift[k]) >> (Q15 + Q_NNET_UPMIX_OVERLAP_SCALE)); // windowing (Q15)
			if (k < (NNET_UPMIX_STACK_LEN - 1) * NNET_UPMIX_FRM_LEN)
				NNET_UPMIX_frm_stck_o[k + ch_fft_idx] = NNET_UPMIX_frm_stck_o[k + NNET_UPMIX_FRM_LEN + ch_fft_idx] + fixed_out_sample;
			else
				NNET_UPMIX_frm_stck_o[k + ch_fft_idx] = fixed_out_sample;
		}

		ch_idx += NNET_UPMIX_FRM_LEN;
		ch_fft_idx += NNET_UPMIX_FFT_LEN;
		ch_feat_idx += FEAT_ORDER;
	}

	//Output sample reordering
	if (ch_num == 2) /*SRI-B: Mono/Stereo switching implemented NNET_UPMIX_FFT_time_i Runtime */
	{
		for (i = 0; i<NNET_UPMIX_FRM_LEN; i++)
		{
			*(p_O_reorder++) = NNET_UPMIX_frm_stck_o[i];
			*(p_O_reorder++) = NNET_UPMIX_frm_stck_o[i + NNET_UPMIX_FFT_LEN];
		}
	}
	else//Mono case
	{
		for (i = 0; i<NNET_UPMIX_FRM_LEN; i++)
		{
			*(p_O_reorder++) = NNET_UPMIX_frm_stck_o[i];
		}
	}

	if (NNET_UPMIX_frm_idx < FRM_IDX_INIT)
		NNET_UPMIX_frm_idx++;

	return 0;

}


void LP_env_log(short * A, float * env)
{
	//Freqz

	kiss_fft_scalar  A_in[NNET_UPMIX_FFT_LEN] = { 0, };
	kiss_fft_cpx  freq_A[NNET_UPMIX_FFT21_LEN];
	int i, k;
	int A_int;
	double d_re, d_im, d_pow;


	//Convert float(double) to int32_t(float)
	for (i = 0; i < MP1 ; i++)
	{
		//A_in[i] = (kiss_fft_scalar)(((int)A[i]) >> 1);
		A_int = A[i];
		A_in[i] = (kiss_fft_scalar)(A_int << 3);
		
	}

	//FFT
	kiss_fftr_cfg  cfg = kiss_fftr_alloc(NNET_UPMIX_FFT_LEN, 0, 0, 0);

	kiss_fftr(cfg, A_in, freq_A);

	
	//reform abs and angle of complex spetrum
	for (k = 0; k < NNET_UPMIX_FFT21_LEN; ++k)
	{
		//get 1/freq_A[k] (Complex number inverse)
		d_re = (double)freq_A[k].r / 32.0; //FFT Fraction: Q5
		d_im = (double)freq_A[k].i / 32.0;
		d_pow = pow(d_re, 2) + pow(d_im, 2) + FLR; //abs

		d_re /= d_pow;
		d_im /= d_pow;
		d_im *= -1;

		//Get envelope of d_im
		env[k] = log(sqrt(pow(d_re, 2) + pow(d_im, 2))); //abs & log
	}

}