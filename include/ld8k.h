/* Version 3.3    Last modified: December 26, 1995 */

/*--------------------------------------------------------------*
 * LD8K.H                                                       *
 * ~~~~~~                                                       *
 * Function prototypes and constants use in G.729               *
 *                                                              *
 *--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include "nnet_upmix_constants.h"

#ifdef PI
#undef PI
#endif
#ifdef PI2
#undef PI2
#endif
#define PI              (F)3.14159265358979323846
#define	PI_Q3			25 //Q.3
#define	PI_Q16			205887 //Q.3
#define PI2             (F)6.283185307

/*--------------------------------------------------------------------------*
 *       Codec constant parameters (coder, decoder, and postfilter)         *
 *--------------------------------------------------------------------------*/

#define L_TOTAL     240        /* Total size of speech buffer.              */
#define L_WINDOW    240        /* Window size in LP analysis.               */
#define L_NEXT      40         /* Lookahead in LP analysis.                 */
#define L_FRAME     80         /* Frame size.                               */
#define L_SUBFR     40         /* Subframe size.                            */
#define M           10         /* Order of LP filter.                       */
#define NP          M          /* Order of LP filter.                       */
#define MP1         (M+1)      /* Order of LP filter + 1                    */
#define MM1         (M-1)      /* Order of LP filter - 1                    */
#define PIT_MIN     20         /* Minimum pitch lag.                        */
#define PIT_MAX     143        /* Maximum pitch lag.                        */
#define L_INTERPOL  (10+1)     /* Length of filter for interpolation.       */

#define PRM_SIZE    11         /* Size of vector of analysis parameters.    */
#define SERIAL_SIZE (80+2)     /* Bits/frame + bfi+ number of speech bits   */

#define SHARPMAX    13017      /* Maximum value of pitch sharpening 0.8 Q14 */
#define SHARPMIN    3277       /* Minimum value of pitch sharpening 0.2 Q14 */

#define L_SUBFRP1 (L_SUBFR + 1)

#define GPCLIP      15564      /* Maximum pitch gain if taming is needed Q14*/
#define GPCLIP2     481        /* Maximum pitch gain if taming is needed Q9 */
#define GP0999      16383      /* Maximum pitch gain if taming is needed    */
#define L_THRESH_ERR 983040000L /* Error threshold taming 16384. * 60000.   */

#ifdef __cplusplus
extern "C" {
#endif
/*--------------------------------------------------------------------------*
 * Mathematic functions.                                                    *
 *--------------------------------------------------------------------------*/

int Inv_sqrt(    /* (o) Q30 : output value   (range: 0<=val<1)           */
  int L_x        /* (i) Q0  : input value    (range: 0<=val<=7fffffff)   */
);

void Log2(
  int L_x,       /* (i) Q0  : input value                                */
  short *exponent, /* (o) Q0  : integer part of Log2.   (range: 0<=val<=30)*/
  short *fraction  /* (o) Q15 : fractional part of Log2. (range: 0<=val<1) */
);

int Pow2(        /* (o) Q0  : result       (range: 0<=val<=0x7fffffff)   */
  short exponent,  /* (i) Q0  : integer part.      (range: 0<=val<=30)     */
  short fraction   /* (i) Q15 : fractional part.  (range: 0.0<=val<1.0)    */
);

/*--------------------------------------------------------------------------*
 * Pre and post-process.                                                    *
 *--------------------------------------------------------------------------*/

void Init_Pre_Process(void);
void Init_Post_Process(void);

void Pre_Process(
  short signal[],  /* (i/o)   : input/output signal                        */
  short lg         /* (i)     : length of signal                           */
);

void Post_Process(
  short signal[],  /* (i/o)   : input/output signal                        */
  short lg         /* (i)     : length of signal                           */
);

/*--------------------------------------------------------------------------*
 * Main coder and decoder functions                                         *
 *--------------------------------------------------------------------------*/

void Init_Coder_ld8k(void);

void Coder_ld8k(
  short ana[],     /* (o)     : analysis parameters                        */
  short synth[]    /* (o)     : local synthesis                            */
);

void Init_Decod_ld8k(void);

void Decod_ld8k(
  short parm[],   /* (i)     : vector of synthesis parameters
                                  parm[0] = bad frame indicator (bfi)       */
  short voicing,  /* (i)     : voicing decision from previous frame       */
  short synth[],  /* (o)     : synthesized speech                         */
  short A_t[],    /* (o)     : decoded LP filter for 2 subframes          */
  short *T0_first /* (o)     : decoded pitch lag in first subframe        */
);

/*--------------------------------------------------------------------------*
 * LPC analysis and filtering                                               *
 *--------------------------------------------------------------------------*/

void Autocorr(
  short x[],       /* (i)     : input signal                               */
  short m,         /* (i)     : LPC order                                  */
  short r_h[],     /* (o)     : autocorrelations  (msb)                    */
  short r_l[]      /* (o)     : autocorrelations  (lsb)                    */
);

void Lag_window(
  short m,         /* (i)     : LPC order                                  */
  short r_h[],     /* (i/o)   : autocorrelations  (msb)                    */
  short r_l[]      /* (i/o)   : autocorrelations  (lsb)                    */
);

void Levinson(
  short Rh[],      /* (i)     : Rh[m+1] autocorrelation coefficients (msb) */
  short Rl[],      /* (i)     : Rl[m+1] autocorrelation coefficients (lsb) */
  short A[],       /* (o) Q12 : A[m]    LPC coefficients  (m = 10)         */
  short rc[]       /* (o) Q15 : rc[M]   Reflection coefficients.           */
);

void Az_lsp(
  short a[],       /* (i) Q12 : predictor coefficients                     */
  short lsp[],     /* (o) Q15 : line spectral pairs                        */
  short old_lsp[]  /* (i)     : old lsp[] (in case not found 10 roots)     */
);

void Lsp_Az(
  short lsp[],     /* (i) Q15 : line spectral frequencies                  */
  short a[]        /* (o) Q12 : predictor coefficients (order = 10)        */
);

void Lsf_lsp(
  short lsf[],     /* (i) Q15 : lsf[m] normalized (range: 0.0<=val<=0.5)   */
  short lsp[],     /* (o) Q15 : lsp[m] (range: -1<=val<1)                  */
  short m          /* (i)     : LPC order                                  */
);

void Lsp_lsf(
  short lsp[],     /* (i) Q15 : lsp[m] (range: -1<=val<1)                  */
  short lsf[],     /* (o) Q15 : lsf[m] normalized (range: 0.0<=val<=0.5)   */
  short m          /* (i)     : LPC order                                  */
);

void Int_lpc(
  short lsp_old[],  /* (i)    : LSP vector of past frame                   */
  short lsp_new[],  /* (i)    : LSP vector of present frame                */
  short lsf_int[],  /* (o)    : interpolated lsf coefficients              */
  short lsf_new[],  /* (o)    : new lsf coefficients                       */
  short Az[]        /* (o)    : interpolated Az() for the 2 subframes      */
);

void Int_qlpc(
  short lsp_old[],  /* (i)    : LSP vector of past frame                   */
  short lsp_new[],  /* (i)    : LSP vector of present frame                */
  short Az[]        /* (o)    : interpolated Az() for the 2 subframes      */
);

/*--------------------------------------------------------------------------*
 *       PWF constant parameters                                            *
 *--------------------------------------------------------------------------*/

#define A1          4567       /*  2.23   in Q11                            */
#define L_B1        3271557L   /*  0.78   in Q22                            */
#define A2          11776      /*  5.75   in Q11                            */
#define L_B2        16357786L  /*  3.90   in Q22                            */
#define A3          27443      /* 13.40   in Q11                            */
#define L_B3        46808433L  /* 11.16   in Q22                            */
#define SEG1        1299       /*  0.6341 in Q11                            */
#define SEG2        1815       /*  0.8864 in Q11                            */
#define SEG3        1944       /*  0.9490 in Q11                            */

#define THRESH_L1   -3562      /* -1.74   in Q11                            */
#define THRESH_L2   -3116      /* -1.52   in Q11                            */
#define THRESH_H1   1336       /*  0.65   in Q11                            */
#define THRESH_H2   890        /*  0.43   in Q11                            */

#define GAMMA1_0    32113      /*  0.98   in Q15                            */
#define GAMMA1_1    30802      /*  0.94   in Q15                            */
#define GAMMA2_0_L  13107      /*  0.40   in Q15                            */
#define GAMMA2_0_H  22938      /*  0.70   in Q15                            */
#define GAMMA2_1    19661      /*  0.60   in Q15                            */

#define ALPHA       19302      /*  6*pi   in Q10                            */
#define BETA        1024       /*     1   in Q10                            */

void perc_var (
  short *gamma1,   /* bandwidth expansion parameter                        */
  short *gamma2,   /* bandwidth expansion parameter                        */
  short *lsfint,   /* Interpolated LSP vector : 1st subframe               */
  short *lsfnew,   /* New LSP vector : 2nd subframe                        */
  short *r_c       /* Reflection coefficients                              */
);

void Weight_Az(
  short a[],       /* (i) Q12 : a[m+1]  LPC coefficients                   */
  short gamma,     /* (i) Q15 : Spectral expansion factor.                 */
  short m,         /* (i)     : LPC order.                                 */
  short ap[]       /* (o) Q12 : Spectral expanded LPC coefficients         */
);

void Residu(
  short a[],       /* (i) Q12 : prediction coefficients                    */
  short x[],       /* (i)     : speech (values x[-m..-1] are needed (m=10) */
  short y[],       /* (o)     : residual signal                            */
  short lg         /* (i)     : frame size                                 */
);

void Syn_filt(
  short a[],       /* (i) Q12 : a[m+1] prediction coefficients   (m=10)    */
  short x[],       /* (i)     : input signal                               */
  short y[],       /* (o)     : output signal                              */
  short lg,        /* (i)     : size of filtering                          */
  short mem[],     /* (i/o)   : memory associated with this filtering.     */
  short update     /* (i)     : 0=no update, 1=update of memory.           */
);

void Convolve(
  short x[],       /* (i)     : input vector                               */
  short h[],       /* (i) Q12 : impulse response                           */
  short y[],       /* (o)     : output vector                              */
  short L          /* (i)     : vector size                                */
);

/*--------------------------------------------------------------------------*
 *       LTP constant parameters                                            *
 *--------------------------------------------------------------------------*/

#define THRESHPIT   27853      /* Threshold to favor small pitch 0.85  Q15  */
#define UP_SAMP     3          /* upsampling factor for fractional pitch    */
#define L_INTER4    4          /* length/2 for interpolation filter         */
#define FIR_SIZE_ANA (UP_SAMP*L_INTER4+1)
#define L_INTER10   10         /* length/2 for interpolation filter         */
#define FIR_SIZE_SYN (UP_SAMP*L_INTER10+1)

/*--------------------------------------------------------------------------*
 * Pitch functions.                                                         *
 *--------------------------------------------------------------------------*/

  short Pitch_ol(  /* (o)     : open loop pitch lag                        */
  short signal[],  /* (i)     : signal used to compute the open loop pitch */
                    /*       signal[-pit_max] to signal[-1] should be known */
  short pit_min,   /* (i)     : minimum pitch lag                          */
  short pit_max,   /* (i)     : maximum pitch lag                          */
  short L_frame    /* (i)     : length of frame to compute pitch           */
);

  short Pitch_fr3( /* (o)     : pitch period.                              */
  short exc[],     /* (i)     : excitation buffer                          */
  short xn[],      /* (i)     : target vector                              */
  short h[],       /* (i) Q12 : impulse response of filters.               */
  short L_subfr,   /* (i)     : length of subframe                         */
  short t0_min,    /* (i)     : minimum value in the searched range.       */
  short t0_max,    /* (i)     : maximum value in the searched range.       */
  short i_subfr,   /* (i)     : indicator for first subframe.              */
  short *pit_frac  /* (o)     : chosen fraction.                           */
);

short G_pitch(     /* (o) Q14 : Gain of pitch lag saturated to 1.2         */
  short xn[],      /* (i)     : Pitch target.                              */
  short y1[],      /* (i)     : Filtered adaptive codebook.                */
  short g_coeff[], /* (i)     : Correlations need for gain quantization.   */
  short L_subfr    /* (i)     : Length of subframe.                        */
);

short Enc_lag3(    /* (o)     : Return index of encoding                   */
  short T0,        /* (i)     : Pitch delay                                */
  short T0_frac,   /* (i)     : Fractional pitch delay                     */
  short *T0_min,   /* (i/o)   : Minimum search delay                       */
  short *T0_max,   /* (i/o)   : Maximum search delay                       */
  short pit_min,   /* (i)     : Minimum pitch delay                        */
  short pit_max,   /* (i)     : Maximum pitch delay                        */
  short pit_flag   /* (i)     : Flag for 1st subframe                      */
);

void Dec_lag3(      /* (o)     : return integer pitch lag                   */
  short index,     /* (i)     : received pitch index                       */
  short pit_min,   /* (i)     : minimum pitch lag                          */
  short pit_max,   /* (i)     : maximum pitch lag                          */
  short i_subfr,   /* (i)     : subframe flag                              */
  short *T0,       /* (o)     : integer part of pitch lag                  */
  short *T0_frac   /* (o)     : fractional part of pitch lag               */
);

short Interpol_3(  /* (o)     : interpolated value                         */
  short *x,        /* (i)     : input vector                               */
  short frac       /* (i)     : fraction                                   */
);

void Pred_lt_3(
  short exc[],     /* (i/o)   : excitation buffer                          */
  short T0,        /* (i)     : integer pitch lag                          */
  short frac,      /* (i)     : fraction of lag                            */
  short L_subfr    /* (i)     : subframe size                              */
);

short Parity_Pitch( /* (o)    : parity bit (XOR of 6 MSB bits)             */
  short pitch_index /* (i)    : index for which parity to compute          */
);

short Check_Parity_Pitch( /* (o) : 0 = no error, 1= error                  */
  short pitch_index, /* (i)   : index of parameter                         */
  short parity       /* (i)   : parity bit                                 */
);

/*--------------------------------------------------------------------------*
 * fixed codebook excitation.                                               *
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 *       FCB constant parameters                                            *
 *--------------------------------------------------------------------------*/

#define DIM_RR      616        /* size of correlation matrix                */
#define NB_POS      8          /*                                           */
#define STEP        5          /* spacing for individual pulse              */
#define MSIZE       64
#define THRESHFCB   13107      /* 0.4 in Q15                                */
#define MAX_TIME    75         /* maximum number of iterations              */

/*--------------------------------------------------------------------------*
 * FCB functions.                                                           *
 *--------------------------------------------------------------------------*/

short ACELP_Codebook( /* (o)  : index of pulses positions                  */
  short x[],       /* (i)     : Target vector                              */
  short h[],       /* (i) Q12 : Impulse response of filters                */
  short T0,        /* (i)     : Pitch lag                                  */
  short pitch_sharp,/* (i) Q14: Last quantized pitch gain                  */
  short i_subfr,   /* (i)     : Indicator of 1st subframe,                 */
  short code[],    /* (o) Q13 : Innovative codebook                        */
  short y[],       /* (o) Q12 : Filtered innovative codebook               */
  short *sign      /* (o)     : Signs of 4 pulses                          */
);

void Decod_ACELP(
  short sign,      /* (i)     : signs of 4 pulses.                         */
  short index,     /* (i)     : Positions of the 4 pulses.                 */
  short cod[]      /* (o) Q13 : algebraic (fixed) codebook excitation      */
);

/*--------------------------------------------------------------------------*
 *       LSP quantizer constant parameters                                  *
 *--------------------------------------------------------------------------*/

#define NC          5          /*  NC = M/2                                 */
#define MA_NP       4          /* MA prediction order for LSP               */
#define MODE        2          /* number of modes for MA prediction         */
#define NC0_B       7          /* number of first stage bits                */
#define NC1_B       5          /* number of second stage bits               */
#define NC0         (1<<NC0_B) /* number of entries in first stage          */
#define NC1         (1<<NC1_B) /* number of entries in second stage         */

#define L_LIMIT     40         /* minimum lsf value Q13:0.005               */
#define M_LIMIT     25681      /* maximum lsf value Q13:3.135               */

#define GAP1        10         /* bandwidth expansion factor Q13            */
#define GAP2        5          /* bandwidth expansion factor Q13            */
#define GAP3        321        /* bandwidth expansion factor Q13            */
#define GRID_POINTS 60         /* search grid                               */

#define PI04        ((short)1029)       /* Q13  pi*0.04                    */
#define PI92        ((short)23677)      /* Q13  pi*0.92                    */
#define CONST10     ((short)10*(1<<11)) /* Q11  10.0                       */
#define CONST12     ((short)19661)      /* Q14  1.2                        */

/*--------------------------------------------------------------------------*
 * LSP VQ functions.                                                        *
 *--------------------------------------------------------------------------*/

void Lsf_lsp2(
  short lsf[],     /* (i) Q13 : lsf[m] (range: 0.0<=val<PI)                */
  short lsp[],     /* (o) Q15 : lsp[m] (range: -1<=val<1)                  */
  short m          /* (i)     : LPC order                                  */
);

void Lsp_lsf2(
  short lsp[],     /* (i) Q15 : lsp[m] (range: -1<=val<1)                  */
  short lsf[],     /* (o) Q13 : lsf[m] (range: 0.0<=val<PI)                */
  short m          /* (i)     : LPC order                                  */
);

void Qua_lsp(
  short lsp[],     /* (i) Q15 : Unquantized LSP                            */
  short lsp_q[],   /* (o) Q15 : Quantized LSP                              */
  short ana[]      /* (o)     : indexes                                    */
);

void Get_wegt(
  short flsp[],    /* (i) Q13 :                                            */
  short wegt[]     /* (o) Q11 :  normalized                                */
);

void Lsp_encw_reset( void);

void Lsp_qua_cs(
  short flsp_in[M],  /* Q13 */
  short lspq_out[M], /* Q13 */
  short *code
);

void Lsp_expand_1(
  short buf[],     /* Q13 */
  short gap        /* Q13 */
);

void Lsp_expand_2(
  short buf[],     /* Q13 */
  short gap        /* Q13 */
);

void Lsp_expand_1_2(
  short buf[],     /* Q13 */
  short gap        /* Q13 */
);

void Lsp_get_quant(
  short lspcb1[][M],    /* Q13 */
  short lspcb2[][M],    /* Q13 */
  short code0,
  short code1,
  short code2,
  short fg[][M],        /* Q15 */
  short freq_prev[][M], /* Q13 */
  short lspq[],            /* Q13 */
  short fg_sum[]           /* Q15 */
);

void Lsp_get_tdist(
  short wegt[],    /* normalized */
  short buf[],     /* Q13 */
  int *L_tdist,  /* Q27 */
  short rbuf[],    /* Q13 */
  short fg_sum[]   /* Q15 */
);

void Lsp_last_select(
  int L_tdist[], /* Q27 */
  short *mode_index
);

void Lsp_pre_select(
  short rbuf[],         /* Q13 */
  short lspcb1[][M], /* Q13 */
  short *cand
);

void Lsp_select_1(
  short rbuf[],         /* Q13 */
  short lspcb1[],       /* Q13 */
  short wegt[],         /* normalized */
  short lspcb2[][M], /* Q13 */
  short *index
);

void Lsp_select_2(
  short rbuf[],         /* Q13 */
  short lspcb1[],       /* Q13 */
  short wegt[],         /* normalized */
  short lspcb2[][M], /* Q13 */
  short *index
);

void Lsp_stability(
  short buf[]     /* Q13 */
);

void Relspwed(
  short lsp[],                          /* Q13 */
  short wegt[],                         /* normalized */
  short lspq[],                         /* Q13 */
  short lspcb1[][M],                 /* Q13 */
  short lspcb2[][M],                 /* Q13 */
  short fg[MODE][MA_NP][M],          /* Q15 */
  short freq_prev[MA_NP][M],         /* Q13 */
  short fg_sum[MODE][M],             /* Q15 */
  short fg_sum_inv[MODE][M],         /* Q12 */
  short code_ana[]
);

void D_lsp(
  short prm[],     /* (i)     : indexes of the selected LSP                */
  short lsp_q[],   /* (o) Q15 : Quantized LSP parameters                   */
  short erase      /* (i)     : frame erase information                    */
);

void Lsp_decw_reset( void);

void Lsp_iqua_cs(
  short prm[],     /* (i)     : codes of the selected LSP                  */
  short lsp_q[],   /* (o)     : Quantized LSP parameters                   */
  short erase      /* (i)     : frame erase information                    */
);

void Lsp_prev_compose(
  short lsp_ele[], /* Q13 */
  short lsp[],     /* Q13 */
  short fg[][M],/* Q15 */
  short freq_prev[][M],     /* Q13 */
  short fg_sum[]   /* Q15 */
);

void Lsp_prev_extract(
  short lsp[M],                 /* Q13 */
  short lsp_ele[M],             /* Q13 */
  short fg[MA_NP][M],           /* Q15 */
  short freq_prev[MA_NP][M],    /* Q13 */
  short fg_sum_inv[M]           /* Q12 */
);

void Lsp_prev_update(
  short lsp_ele[M],             /* Q13 */
  short freq_prev[MA_NP][M]     /* Q13 */
);

/*--------------------------------------------------------------------------*
 * gain VQ constants.                                                       *
 *--------------------------------------------------------------------------*/

#define NCODE1_B  3                /* number of Codebook-bit                */
#define NCODE2_B  4                /* number of Codebook-bit                */
#define NCODE1    (1<<NCODE1_B)    /* Codebook 1 size                       */
#define NCODE2    (1<<NCODE2_B)    /* Codebook 2 size                       */
#define NCAN1     4                /* Pre-selecting order for #1            */
#define NCAN2     8                /* Pre-selecting order for #2            */
#define INV_COEF  -17103           /* Q19                                   */

/*--------------------------------------------------------------------------*
 * gain VQ functions.                                                       *
 *--------------------------------------------------------------------------*/

short Qua_gain(
  short code[],    /* (i) Q13 : Innovative vector.                         */
  short g_coeff[], /* (i)     : Correlations <xn y1> -2<y1 y1>             */
                    /*            <y2,y2>, -2<xn,y2>, 2<y1,y2>              */
  short exp_coeff[],/* (i)    : Q-Format g_coeff[]                         */
  short L_subfr,   /* (i)     : Subframe length.                           */
  short *gain_pit, /* (o) Q14 : Pitch gain.                                */
  short *gain_cod, /* (o) Q1  : Code gain.                                 */
  short tameflag   /* (i)     : flag set to 1 if taming is needed          */
);

void Dec_gain(
  short index,     /* (i)     : Index of quantization.                     */
  short code[],    /* (i) Q13 : Innovative vector.                         */
  short L_subfr,   /* (i)     : Subframe length.                           */
  short bfi,       /* (i)     : Bad frame indicator                        */
  short *gain_pit, /* (o) Q14 : Pitch gain.                                */
  short *gain_cod  /* (o) Q1  : Code gain.                                 */
);

void Gain_predict(
  short past_qua_en[],/* (i) Q10 :Past quantized energies                  */
  short code[],    /* (i) Q13 : Innovative vector.                         */
  short L_subfr,   /* (i)     : Subframe length.                           */
  short *gcode0,   /* (o) Qxx : Predicted codebook gain                    */
  short *exp_gcode0 /* (o)    : Q-Format(gcode0)                           */
);

void Gain_update(
  short past_qua_en[],/* (i) Q10 :Past quantized energies                  */
  int L_gbk12    /* (i) Q13 : gbk1[indice1][1]+gbk2[indice2][1]          */
);

void Gain_update_erasure(
  short past_qua_en[]/* (i) Q10 :Past quantized energies                   */
);

void Corr_xy2(
  short xn[],      /* (i) Q0  :Target vector.                              */
  short y1[],      /* (i) Q0  :Adaptive codebook.                          */
  short y2[],      /* (i) Q12 :Filtered innovative vector.                 */
  short g_coeff[], /* (o) Q[exp]:Correlations between xn,y1,y2             */
  short exp_g_coeff[]  /* (o)  :Q-format of g_coeff[]                      */
);

/*--------------------------------------------------------------------------*
 * Postfilter constants and functions                                       *
 *--------------------------------------------------------------------------*/

/* short term pst parameters :                                              */
#define GAMMA1_PST  22938      /* denominator weighting factor   (Q15)      */
#define GAMMA2_PST  18022      /* numerator  weighting factor (Q15)         */
#define LONG_H_ST   20         /* impulse response length                   */
#define GAMMA3_PLUS 6554       /* tilt weighting factor when k1>0 (Q15)     */
#define GAMMA3_MINUS 29491     /* tilt weighting factor when k1<0 (Q15)     */

/* long term pst parameters :                                               */
#define F_UP_PST    8          /* resolution for fractional delay           */
#define LH2_S       4          /* length of short interp. subfilters        */
#define L2_LH2_L    4          /* log2(LH2_L)                               */
#define LH2_L       (1 << L2_LH2_L)
#define MIN_GPLT    21845      /* LT gain minimum (Q15)                     */

#define LH_UP_S     (LH2_S/2)
#define LH_UP_SM1   (LH_UP_S-1)
#define LH_UP_L     (LH2_L/2)
#define LH2_L_P1    (LH2_L + 1)

/* gain adjustment parameters                                               */
#define AGC_FAC     32358     /* gain adjustment factor  0.9875 (Q15)       */
#define AGC_FAC1    (short)(32768L - AGC_FAC)

/* Array sizes */
#define MEM_RES2    (PIT_MAX + 1 + LH_UP_L)
#define SIZ_RES2    (MEM_RES2 + L_SUBFR)
#define SIZ_Y_UP    ((F_UP_PST-1) * L_SUBFRP1)
#define SIZ_TAB_HUP_L ((F_UP_PST-1) * LH2_L)
#define SIZ_TAB_HUP_S ((F_UP_PST-1) * LH2_S)

void  Init_Post_Filter( void);

void Post(
  short t0,        /* (i) : 1st subframe delay given by coder              */
  short *signal_ptr, /* (i) : input signal (pointer to current subframe    */
  short *coeff,    /* (i) : LPC coefficients for current subframe          */
  short *sig_out,  /* (o) : postfiltered output                            */
  short *vo        /* (o) : voicing decision 0 = uv,  > 0 delay            */
);

/*--------------------------------------------------------------------------*
 * Bitstream constants and functions                                        *
 *--------------------------------------------------------------------------*/

#define BIT_0    (short)0x007f /* definition of zero-bit in bit-stream      */
#define BIT_1    (short)0x0081 /* definition of one-bit in bit-stream       */
#define SYNC_WORD (short)0x6b21 /* definition of frame erasure flag         */
#define SIZE_WORD (short)80     /* number of speech bits                    */

void  prm2bits_ld8k(
  short prm[],     /* (i)     : coder parameters                           */
  short bits[]     /* (o)     : bit stream                                 */
);
void  bits2prm_ld8k(
  short bits[],    /* (i)     : bit stream                                 */
  short prm[]      /* (o)     : coder parameters                           */
);

/*--------------------------------------------------------------------------*
 * Prototypes for auxiliary functions.                                      *
 *--------------------------------------------------------------------------*/

void Copy(
  short x[],       /* (i)     : input vector                               */
  short y[],       /* (o)     : output vector                              */
  short L          /* (i)     : vector length                              */
);

void Set_zero(
  short x[],       /* (o)     : vector to clear                            */
  short L          /* (i)     : length of vector                           */
);

short Random(void);


#ifdef __cplusplus
}
#endif