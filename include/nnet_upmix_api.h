#ifndef __NNET_UPMIX_API_H_
#define __NNET_UPMIX_API_H_ 

#include "nnet_upmix_constants.h"
#include "nnet_upmix_typedef.h"
#include <iostream>
#include <armadillo>
#include <stdio.h>
#include <math.h>
#include "kiss_fftr.h"
//#include "nnet_upmix_kaldi_trunk.h"

using namespace std;
using namespace arma;


#ifdef __cplusplus
extern "C" {
#endif


int NNET_UPMIX_Execution(	short* input,
						short* output,
						short ch);

void NNET_UPMIX_Init(int * sRAMNMF);
void NNET_UPMIX_Process(short * sig_inbuf, short * sig_outbuf, unsigned short Channel);

#if NNET_FEEDFORWARD_ARMA == 1
void nnet_upmix_feedforward(float * f_Mag, float * f_Mag_dec);
void nnet_upmix_feedforward_fixed(int * f_Mag, int * f_Mag_dec);
#else
void nnet_upmix_decoder_kaldi(float * f_Mag, float * f_Mag_dec);
#endif

#ifdef __cplusplus
}
#endif

#endif
