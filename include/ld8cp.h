/*
    ITU-T G.729 Annex I  - Reference C code for fixed point
                         implementation of G.729 Annex I
                         Version 1.1 of October 1999
*/
/*
 File : ld8cp.h
 */

/* from ld8e. G.729 Annex E Version 1.2  Last modified: May 1998 */
/* from ld8kd.h G.729 Annex D Version 1.2  Last modified: May 1998 */
/* from ld8k.h G.729 Annex B Version 1.3  Last modified: August 1997 */

/*---------------------------------------------------------------------------*
 * constants for bitstream packing                                           *
 *---------------------------------------------------------------------------*/
#define PRM_SIZE_E_fwd  18       /* Size of vector of analysis parameters.    */
#define PRM_SIZE_E_bwd  16       /* Size of vector of analysis parameters.    */
#define PRM_SIZE_E      18       /* Size of vector of analysis parameters.    */
#define PRM_SIZE_D      10       /* Size of vector of analysis parameters.    */
#define PRM_SIZE_SID    4        /* Size of vector of analysis parameters.    */

#define SERIAL_SIZE_E (116+4)  /* Bits/frame + bfi+ number of speech bits
                                             + bit for mode + protection */

#define RATE_6400       64      /* Low  rate  (6400 bit/s)       */
#define RATE_8000       80      /* Full rate  (8000 bit/s)       */
#define RATE_11800      118     /* High rate (11800 bit/s)       */
#define RATE_SID        15      /* SID                           */
#define RATE_0          0       /* 0 bit/s rate                  */

#define G729D           0      /* Low  rate  (6400 bit/s)       */
#define G729            1      /* Full rate  (8000 bit/s)       */
#define G729E           2      /* High rate (11800 bit/s)       */

/* backward LPC analysis parameters */
#define M_BWD       30         /* Order of Backward LP filter.              */
#define M_BWDP1     (M_BWD+1)  /* Order of Backward LP filter + 1           */
#define NRP         35
#define MEM_SYN_BWD     M_BWD + NRP
#define N1              M_BWD + L_FRAME
#define L_ANA_BWD       L_FRAME + MEM_SYN_BWD
#define L_ANA_BWD_M1    L_ANA_BWD - 1
#define W_FACT  10368
#define GAMMA_BWD 32113

/* short term pst parameters :                                              */
#define GAMMA1_PST_E  22938      /* denominator weighting factor   (Q15)      */
#define GAMMA2_PST_E  21300      /* numerator  weighting factor (Q15)         */
#define LONG_H_ST_E   32        /* impulse response length                   */
#define GAMMA_HARM_E 8192
#define GAMMA_HARM 16384

/* ACELP codebooks parameters */
#define NB_TRACK  5
#define Q15_1_5   6554
/* 6.4K GAIN quantizer constants*/
#define NCODE1_B_6K  3             /* number of Codebook-bit                */
#define NCODE2_B_6K  3             /* number of Codebook-bit                */
#define NCODE1_6K (1<<NCODE1_B_6K) /* Codebook 1 size                       */
#define NCODE2_6K (1<<NCODE2_B_6K) /* Codebook 2 size                       */
#define NCAN1_6K  6                /* Pre-selecting order for #1            */
#define NCAN2_6K  6                /* Pre-selecting order for #2            */
#define INV_COEF_6K  -28940        /* Q20                                   */



#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------*
 * Main coder and decoder functions                                         *
 *--------------------------------------------------------------------------*/
void Init_Coder_ld8c(short dtx_enable);

void Coder_ld8c(
  short ana[],     /* (o)     : analysis parameters                        */
 short frame,                   /* input : frame counter */
  short dtx_enable,               /* input : DTX enable flag */
  short rate           /* input   : rate selector/frame  =0 6.4kbps , =1 8kbps,= 2 11.8 kbps*/
);

void Init_Decod_ld8c(void);

void Decod_ld8c(
  short parm[],   /* (i)     : vector of synthesis parameters
                                  parm[0] = bad frame indicator (bfi)      */
  short voicing,  /* (i)     : voicing decision from previous frame       */
  short synth[],  /* (o)     : synthesized speech                         */
  short  A_t[],    /* (o)     : decoded LP filter for 2 subframes          */
  short *T0_first, /* (o)     : decoded pitch lag in first subframe        */
  short *stationnary,  /* output:  stationnarity indicator */
  short *m_pst,         /* output:  LPC order */
  short *Vad
);

/*--------------------------------------------------------------------------*
 * protypes of functions  similar to G729                                   *
 * differences :                                                            *
 * list of arguments modified                                               *
 * local static variables and arrays are now passed as parameters           *
 * LPC order formerly constant is now passed as variable parameter          *
 * some temporary variables are now passed to the calling routine           *
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * LPC analysis and filtering                                               *
 *--------------------------------------------------------------------------*/
void Autocorrcp(
  short x[],       /* (i)     : input signal                               */
  short m,         /* (i)     : LPC order                                  */
  short r_h[],     /* (o)     : autocorrelations  (msb)                    */
  short r_l[],     /* (o)     : autocorrelations  (lsb)                    */
  short *exp_R0
);

void Levinsoncp(
  short m,        /* (i)    : LPC order                         */
  short Rh[],      /* (i)     : Rh[m+1] autocorrelation coefficients (msb) */
  short Rl[],      /* (i)     : Rl[m+1] autocorrelation coefficients (lsb) */
  short A[],       /* (o) Q12 : A[m]    LPC coefficients  (m = 10)         */
  short rc[],       /* (o) Q15 : rc[M]   Reflection coefficients.           */
  short old_A[], /* (i/o) Q12 : last stable filter LPC coefficients  */
  short old_rc[], /* (i/o) Q15 : last stable filter Reflection coefficients.         */
  short *Err
);

void Residue(
  short m,        /* (i)    : LPC order                         */
  short a[],    /* (i) Q12 : prediction coefficients                     */
  short x[],    /* (i)     : speech (values x[-m..-1] are needed         */
  short y[],    /* (o)     : residual signal                             */
  short lg      /* (i)     : size of filtering                           */
);
void Syn_filte(
  short m,        /* (i)    : LPC order                         */
  short a[],     /* (i) Q12 : a[m+1] prediction coefficients   (m=10)  */
  short x[],     /* (i)     : input signal                             */
  short y[],     /* (o)     : output signal                            */
  short lg,      /* (i)     : size of filtering                        */
  short mem[],   /* (i/o)   : memory associated with this filtering.   */
  short update   /* (i)     : 0=no update, 1=update of memory.         */
);

/*--------------------------------------------------------------------------*
 * LSP VQ functions.                                                        *
 *--------------------------------------------------------------------------*/
void Qua_lspe(
  short lsp[],     /* (i) Q15 : Unquantized LSP                            */
  short lsp_q[],   /* (o) Q15 : Quantized LSP                              */
  short ana[],      /* (o)     : indexes                                    */
  short freq_prev[MA_NP][M],    /* (i) Q13 : previous LSP MA vector        */
  short freq_cur[]   /* (o) Q13 : current LSP MA vector        */
);
void Lsp_encw_resete(
  short freq_prev[MA_NP][M]    /* (i) Q13 : previous LSP MA vector        */
);
void Lsp_qua_cse(
  short flsp_in[M],    /* (i) Q13 : Original LSP parameters    */
  short lspq_out[M],   /* (o) Q13 : Quantized LSP parameters   */
  short *code,         /* (o)     : codes of the selected LSP  */
  short freq_prev[MA_NP][M],    /* (i) Q13 : previous LSP MA vector        */
  short freq_cur[]     /* (o) Q13 : current LSP MA vector        */
);
void Lsp_get_quante(
  short lspcb1[][M],      /* (i) Q13 : first stage LSP codebook      */
  short lspcb2[][M],      /* (i) Q13 : Second stage LSP codebook     */
  short code0,               /* (i)     : selected code of first stage  */
  short code1,               /* (i)     : selected code of second stage */
  short code2,               /* (i)     : selected code of second stage */
  short fg[][M],          /* (i) Q15 : MA prediction coef.           */
  short freq_prev[][M],   /* (i) Q13 : previous LSP vector           */
  short lspq[],              /* (o) Q13 : quantized LSP parameters      */
  short fg_sum[],             /* (i) Q15 : present MA prediction coef.   */
  short freq_cur[]             /* (i) Q15 : present MA prediction coef.   */
);
void Relspwede(
  short lsp[],                     /* (i) Q13 : unquantized LSP parameters */
  short wegt[],                    /* (i) norm: weighting coefficients     */
  short lspq[],                    /* (o) Q13 : quantized LSP parameters   */
  short lspcb1[][M],            /* (i) Q13 : first stage LSP codebook   */
  short lspcb2[][M],            /* (i) Q13 : Second stage LSP codebook  */
  short fg[MODE][MA_NP][M],     /* (i) Q15 : MA prediction coefficients */
  short freq_prev[MA_NP][M],    /* (i) Q13 : previous LSP vector        */
  short fg_sum[MODE][M],        /* (i) Q15 : present MA prediction coef.*/
  short fg_sum_inv[MODE][M],    /* (i) Q12 : inverse coef.              */
  short code_ana[],                 /* (o)     : codes of the selected LSP  */
  short freq_cur[]             /* (o) Q13 : current LSP MA vector        */
);

void D_lspe(
  short prm[],          /* (i)     : indexes of the selected LSP */
  short lsp_q[],        /* (o) Q15 : Quantized LSP parameters    */
  short erase,           /* (i)     : frame erase information     */
  short freq_prev[MA_NP][M],    /* (i/o) Q13 : previous LSP MA vector        */
  short prev_lsp[],            /* (i/o) Q13 : previous LSP vector        */
  short *prev_ma               /* (i/o) previous MA prediction coef.*/
);
void Lsp_decw_resete(
  short freq_prev[MA_NP][M],    /* (o) Q13 : previous LSP MA vector        */
  short prev_lsp[],            /* (o) Q13 : previous LSP vector        */
  short *prev_ma               /* previous MA prediction coef.*/
);
void Lsp_iqua_cse(
 short prm[],          /* (i)     : indexes of the selected LSP */
 short lsp_q[],        /* (o) Q13 : Quantized LSP parameters    */
 short erase,           /* (i)     : frame erase information     */
  short freq_prev[MA_NP][M],    /* (i/o) Q13 : previous LSP MA vector        */
  short prev_lsp[],            /* (i/o) Q13 : previous LSP vector        */
  short *prev_ma               /* (i/o) previous MA prediction coef.*/
);
/*--------------------------------------------------------------------------*
 * Postfilter functions                                                     *
 *--------------------------------------------------------------------------*/
void Poste(
  short t0,        /* (i) : 1st subframe delay given by coder              */
  short *signal_ptr, /* (i) : input signal (pointer to current subframe    */
  short  *coeff,    /* (i) : LPC coefficients for current subframe          */
  short *sig_out,  /* (o) : postfiltered output                            */
  short *vo,        /* (o) : voicing decision 0 = uv,  > 0 delay            */
  short gamma1,            /* input: short term postfilt. den. weighting factor*/
  short gamma2,            /* input: short term postfilt. num. weighting factor*/
  short gamma_harm,        /* input: long term postfilter weighting factor*/
  short  long_h_st,    /* input: impulse response length*/
  short m_pst,          /* input:  LPC order */
  short Vad                /* input : active/inactive flag indicator */
);
/*--------------------------------------------------------------------------*
 * protypes of functions  containing G729 source code + specific G729E code *
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * gain VQ functions.                                                       *
 *--------------------------------------------------------------------------*/
void Dec_gaine(
   short index,        /* (i)     :Index of quantization.         */
   short code[],       /* (i) Q13 :Innovative vector.             */
   short L_subfr,      /* (i)     :Subframe length.               */
   short bfi,          /* (i)     :Bad frame indicator            */
   short *gain_pit,    /* (o) Q14 :Pitch gain.                    */
   short *gain_cod,     /* (o) Q1  :Code gain.                     */
   short rate,             /* input   : rate selector/frame  =0 8kbps,= 1 11.8 kbps*/
   short gain_pit_mem,
   short gain_cod_mem,
   short *c_muting,
   short count_bfi,
   short stationnary
);

/*--------------------------------------------------------------------------*
 * Bitstream functions                                                      *
 *--------------------------------------------------------------------------*/
void bits2prm_ld8c(
    short bits[],  /* I   serial bits (nb_bits)                          */
    short prm[]   /* O   output: decoded parameters (11 parameters)     */
);


void  prm2bits_ld8c(
  short prm[],           /* input : encoded parameters  (PRM_SIZE parameters)  */
  short bits[]          /* output: serial bits (SERIAL_SIZE ) bits[0] = bfi
                                    bits[1] = nbbits */
);

/*--------------------------------------------------------------------------*
 * functions  specific to G729E                                             *
 *--------------------------------------------------------------------------*/
/* backward LPC analysis and switch forward/backward */
void autocorr_hyb_window(
    short  *x,         /* Input speech signal  */
    int  *r_bwd,        /* (out)    Autocorrelations    */
    int *rexp        /* (in/out) */
);
void Lag_window_bwd(int *r_bwd, short *r_h_bwd, short *r_l_bwd);
void Int_bwd(short *a_bwd, short *prev_filter, short *C_int );

void set_lpc_mode(short *signal_ptr, short *a_fwd, short *a_bwd,
  short *lp_mode, short *lspnew, short *lspold,
  short *bwd_dominant, short prev_lp_mode, short *prev_filter,
  short *C_int, short *glob_stat, short *stat_bwd, short *val_stat_bwd);
void update_bwd( short *lp_mode, short *bwd_dominant, short *C_int,
        short *glob_stat);
short ener_dB(short *synth, short L);
void tst_bwd_dominant(short *high_stat, short mode);

void perc_vare (
  short *gamma1,   /* Bandwidth expansion parameter */
  short *gamma2,   /* Bandwidth expansion parameter */
  short  high_stat /* high_stat indication (see file bwfw.c) */
);

 short Pitch_fr3cp( /* (o)     : pitch period.                              */
  short exc[],     /* (i)     : excitation buffer                          */
  short xn[],      /* (i)     : target vector                              */
  short h[],       /* (i) Q12 : impulse response of filters.               */
  short L_subfr,   /* (i)     : length of subframe                         */
  short t0_min,    /* (i)     : minimum value in the searched range.       */
  short t0_max,    /* (i)     : maximum value in the searched range.       */
  short i_subfr,   /* (i)     : indicator for first subframe.              */
  short *pit_frac,  /* (o)     : chosen fraction.                           */
  short rate        /* (i)     : frame rate */
);
short Enc_lag3cp(    /* (o)     : Return index of encoding                   */
  short T0,        /* (i)     : Pitch delay                                */
  short T0_frac,   /* (i)     : Fractional pitch delay                     */
  short *T0_min,   /* (i/o)   : Minimum search delay                       */
  short *T0_max,   /* (i/o)   : Maximum search delay                       */
  short pit_min,   /* (i)     : Minimum pitch delay                        */
  short pit_max,   /* (i)     : Maximum pitch delay                        */
  short pit_flag,   /* (i)     : Flag for 1st subframe                      */
  short rate        /* (i)     : frame rate */
);
void Dec_lag3cp(      /* (o)     : return integer pitch lag                   */
  short index,     /* (i)     : received pitch index                       */
  short pit_min,   /* (i)     : minimum pitch lag                          */
  short pit_max,   /* (i)     : maximum pitch lag                          */
  short i_subfr,   /* (i)     : subframe flag                              */
  short *T0,       /* (o)     : integer part of pitch lag                  */
  short *T0_frac,   /* (o)     : fractional part of pitch lag               */
  short rate        /* (i)     : frame rate */
);


/*--------------------------------------------------------------------------*
 * G729E fixed (ACELP) codebook excitation.                                               *
 *--------------------------------------------------------------------------*/

void ACELP_12i40_44bits(
  short x[],    /* (i) Q0 : target vector                                 */
  short cn[],   /* (i) Q0 : residual after long term prediction           */
  short H[],    /* (i) Q12: impulse response of weighted synthesis filter */
  short code[], /* (o) Q12: algebraic (fixed) codebook excitation         */
  short y[],    /* (o) Q11: filtered fixed codebook excitation            */
  short indx[]  /* (o)    : index 5 words: 13,10,7,7,7 = 44 bits          */
);
void ACELP_10i40_35bits(
  short x[],    /* (i) Q0 : target vector                                 */
  short cn[],   /* (i) Q0 : residual after long term prediction           */
  short H[],    /* (i) Q12: impulse response of weighted synthesis filter */
  short code[], /* (o) Q12: algebraic (fixed) codebook excitation         */
  short y[],    /* (o) Q11: filtered fixed codebook excitation            */
  short indx[]  /* (o)    : index 5 words: 7,7,7,7,7 = 35 bits            */
);
void Dec_ACELP_12i40_44bits(
  short *index,  /* (i)     : 5 words index (positions & sign)      */
  short cod[]    /* (o) Q13 : algebraic (fixed) codebook excitation */
);
void Dec_ACELP_10i40_35bits(
  short *index,  /* (i)     : 5 words index (positions & sign)      */
  short cod[]    /* (o) Q13 : algebraic (fixed) codebook excitation */
);


short ACELP_Codebook64( /*(o) : index of pulses positions                  */
  short x[],       /* (i)     : Target vector                              */
  short h[],       /* (i) Q12 : Impulse response of filters                */
  short T0,        /* (i)     : Pitch lag                                  */
  short pitch_sharp,/* (i) Q14: Last quantized pitch gain                  */
  short code[],    /* (o) Q13 : Innovative codebook                        */
  short y[],       /* (o) Q12 : Filtered innovative codebook               */
  short *sign      /* (o)     : Signs of 4 pulses                          */
);
void Decod_ACELP64(
  short sign,      /* (i)     : signs of 4 pulses.                         */
  short index,     /* (i)     : Positions of the 4 pulses.                 */
  short cod[]      /* (o) Q13 : algebraic (fixed) codebook excitation      */
);

void PhDisp(
  short x[],       /* (i)     : input signal                               */
  short y[],       /* (o)     : output signal                              */
  short cbGain,    /* (i) Q1  : codebook gain                              */
  short ltpGain,   /* (i) Q14 : pitch gain                                 */
  short inno[]     /* (i/o)   : innovation vector                          */
);

void Update_PhDisp(
  short ltpGain,   /* (i) Q14 : pitch gain                                 */
  short cbGain     /* (i) Q1  : codebook gain                              */
);




/* pitch tracking routine */
void track_pit(short *T0, short *T0_frac, short *prev_pitch,
        short *stat_pitch, short *pitch_sta,  short *frac_sta);
short Qua_gain_6k(
  short code[],    /* (i) Q13 : Innovative vector.                         */
  short g_coeff[], /* (i)     : Correlations <xn y1> -2<y1 y1>             */
                    /*            <y2,y2>, -2<xn,y2>, 2<y1,y2>              */
  short exp_coeff[],/* (i)    : Q-Format g_coeff[]                         */
  short L_subfr,   /* (i)     : Subframe length.                           */
  short *gain_pit, /* (o) Q14 : Pitch gain.                                */
  short *gain_cod, /* (o) Q1  : Code gain.                                 */
  short tameflag   /* (i)     : flag set to 1 if taming is needed          */
);
void Dec_gain_6k(
  short index,     /* (i)     : Index of quantization.                     */
  short code[],    /* (i) Q13 : Innovative vector.                         */
  short L_subfr,   /* (i)     : Subframe length.                           */
  short bfi,       /* (i)     : Bad frame indicator                        */
  short *gain_pit, /* (o) Q14 : Pitch gain.                                */
  short *gain_cod  /* (o) Q1  : Code gain.                                 */
);


short Random_g729cp(short *seed);

/*--------------------------------------------------------------------------*
 * taming functions                                                         *
 *--------------------------------------------------------------------------*/
void Init_exc_err(void);
short test_err(short t0, short t0_frac);
void update_exc_err(short gain_pit, short t0);


void musdetect( short rate, short r_h, short r_l, short exp_R0, short *rc,
               short *lags, short *pgains, short stat_flg,
               short frm_count, short prev_vad, short *Vad, short Energy_db);

#ifdef __cplusplus
}
#endif