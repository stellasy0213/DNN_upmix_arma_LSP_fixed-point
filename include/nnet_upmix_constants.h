/*********************************************
	GIST NMF ENhancement Module
	2016-04-06
	- Kwang Myung Jeon
	Human Media Communication & Processing(HuCom) Lab.
***********************************************/


#ifndef  __NNET_UPMIX_CONSTANTS_H__
#define  __NNET_UPMIX_CONSTANTS_H__

#include "nnet_upmix_constants.h"

//=================Implementation option================

/*------------------------- common Constants -------------------------------*/
#define   NNET_UPMIX_MAX_CHANNEL         1
#define   NNET_UPMIX_FRM_LEN			512
#define   NNET_UPMIX_STACK_LEN				 2
#define   Q_NNET_UPMIX_OVERLAP_SCALE 0 // 2 / NNET_UPMIX_STACK_LEN = 0.5
#define   NNET_UPMIX_TWO_FRAME_LENGTH    1024
#define   NNET_UPMIX_FOUR_FRAME_LENGTH    2048
#define		NNET_UPMIX_FRAME_LENGTH_2BYTE    1024
#define		NNET_UPMIX_TWO_FRAME_LENGTH_2BYTE    2048
#define		NNET_UPMIX_FOUR_FRAME_LENGTH_2BYTE    4096
#define NNET_UPMIX_LPCOEFF 10


//=============NNET_UPMIX_FFT defined parameters====================
#define NNET_UPMIX_FFT_LEN 1024
#define NNET_UPMIX_FFT21_LEN 513
#define NNET_UPMIX_FFT_POINT 10


//=============Feature Extraction
#define NNET_UPMIX_FEAT_LEN NNET_UPMIX_FFT21_LEN //feature length
#define MAX_VALUE 17.3286 //log(abs(32767*NNET_UPMIX_FFT_LEN));
#define FLR 1 //flooring value (Q16.16)
#define EPSILON 2 //Delta coeffcient (0.5 @ Q2)
#define ETA 13//13 //Output Mixing coefficient (control blurrness) (0.406 @ Q5)

//=============NNET Decoder
//#define MODEL_PATH "nnet/nnet_5.dbn_dnn_iter100_learnrate0.003_tr0.00_cv0.00_splice0_e05" //Epsilon 0.5
//#define FEAT_TRANS_PATH "nnet/final.feature_transform_splice0_ep05"
//#define NO_SOFTMAX false
//#define APPLY_LOG false
//#define USE_GPU "no" // "use -2 to turn on GPU"
#define SPLICE21 1 //SPlice 5
#define SPLICE_HALF 1
#define NNET_FEEDFORWARD_ARMA 1 // [1 = Armadillo, 0 = Kaldi]
//#define NNET_CONVERT_ARMA 0 //0: off, 1 : turn on kaldi to armadillo transform [Must set 'NNET_FEEDFORWARD_ARMA 0']
#define HIDDEN_LAYER 4
#define FEAT_ORDER NNET_UPMIX_LPCOEFF //logmag: NNET_UPMIX_FFT21_LEN
#define MODEL_PATH_ARMA "nnet_arma/"

#define FIXED 1
//=======Post Proecssing Options
#define FRM_IDX_INIT 3

//=======DEBUG Options========
#define UPMIX_DEBUG 0
#define HAVE_CUDA_OPT 1
#endif
