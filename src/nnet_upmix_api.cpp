/* **************************************************************************
*		Gwangju Institute of Science and Technology, HuCom Lab				*
*																			*
*							COPYRIGHT (C) 2016								*
*							All Rights Reserved								*
*																			*
*****************************************************************************

*****************************************************************************
* @file	: nnet_upmix_api.c													*
*																			*
* @brief	:	NMF API	(Interspeech16)										*
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

short *NNET_UPMIX_frm_cnt; // [1]
int *NNET_UPMIX_frm_stck_i;	//[ch * 1024]
int *NNET_UPMIX_frm_stck_o;	//[ch * 1024]
int *NNET_UPMIX_I_reorder; //[ch * 512];
kiss_fft_scalar  * NNET_UPMIX_FFT_time_i; 	//[ch * 1024]
kiss_fft_scalar  * NNET_UPMIX_FFT_time_o; 	//[ch * 1024]
kiss_fft_cpx  * NNET_UPMIX_FFT_freq_i;	//[ch * 1024]
float * NNET_UPMIX_f_LSF_buff; // [ch * 10 * splice21]     //[Comment]: 입력 피쳐 블록 (Splice) 생성을 위한 버퍼 
float * NNET_UPMIX_f_LSF_dec_buff; // [ch * 10 * splice21]

int * NNET_UPMIX_i_LSF_buff; // [ch * 10 * splice21]     //[Comment]: 입력 피쳐 블록 (Splice) 생성을 위한 버퍼 
int * NNET_UPMIX_i_LSF_dec_buff; // [ch * 10 * splice21]

double * NNET_UPMIX_FFT_mag_i; //[NNET_UPMIX_FFT21_LEN]
double * NNET_UPMIX_FFT_angl_i; //[NNET_UPMIX_FFT21_LEN]
float * f_LSF; //[NNET_UPMIX_FEAT_LEN]
float * f_LSF_dec; //[NNET_UPMIX_FEAT_LEN]
int * i_LSF_dec; //[NNET_UPMIX_FEAT_LEN]

short NNET_UPMIX_frm_idx = 0;
double init_scale = 0.0;

//Nnet NNET_UPMIX_nnet_transf; //[Comment]: Nnet을 초기에 1회만 load 하도록 전역변수화 (기존에는 Decode 전에 불러와서 framw-wise에 적용 시 계산량 소모가 컸음)
//Nnet NNET_UPMIX_nnet;



//Model mem declaration
imat addshift_i;
imat rescale_i;

imat I_W_0;
imat I_W_1;
imat I_W_2;
imat I_W_3;
imat I_W_4;
imat I_W_5;

imat I_B_0;
imat I_B_1;
imat I_B_2;
imat I_B_3;
imat I_B_4;
imat I_B_5;


//CuMatrix<BaseFloat> feats;
//CuMatrix<BaseFloat> feats_transf;
//CuMatrix<BaseFloat> nnet_out;
//Matrix<BaseFloat> nnet_feat_in(SPLICE21, NNET_UPMIX_FEAT_LEN); //nnet_feat_input buffer
//Matrix<BaseFloat> nnet_feat_out(SPLICE21, NNET_UPMIX_FEAT_LEN); //nnet_feat_output buffer




void NNET_UPMIX_Init(int * sRAMNMF)
{
	int i;

	//Memory Map
	NNET_UPMIX_frm_cnt = (short *)&sRAMNMF[0];
	NNET_UPMIX_frm_stck_i = (int *)((short * )&NNET_UPMIX_frm_cnt[1]);
	NNET_UPMIX_frm_stck_o = (int *)&NNET_UPMIX_frm_stck_i[NNET_UPMIX_FFT_LEN*NNET_UPMIX_MAX_CHANNEL];
	NNET_UPMIX_I_reorder = (int *)&NNET_UPMIX_frm_stck_o[NNET_UPMIX_FFT_LEN*NNET_UPMIX_MAX_CHANNEL];
	NNET_UPMIX_FFT_time_i = (kiss_fft_scalar *)((int *)&NNET_UPMIX_I_reorder[NNET_UPMIX_FRM_LEN*NNET_UPMIX_MAX_CHANNEL]);
	NNET_UPMIX_FFT_time_o = (kiss_fft_scalar *)&NNET_UPMIX_FFT_time_i[NNET_UPMIX_FFT_LEN*NNET_UPMIX_MAX_CHANNEL];
	NNET_UPMIX_FFT_freq_i = (kiss_fft_cpx *)((kiss_fft_scalar *)&NNET_UPMIX_FFT_time_o[NNET_UPMIX_FFT_LEN*NNET_UPMIX_MAX_CHANNEL]);
	NNET_UPMIX_f_LSF_buff = (float *)((kiss_fft_cpx *)&NNET_UPMIX_FFT_freq_i[NNET_UPMIX_FFT_LEN*NNET_UPMIX_MAX_CHANNEL]);
	NNET_UPMIX_f_LSF_dec_buff = (float *)&NNET_UPMIX_f_LSF_buff[NNET_UPMIX_LPCOEFF*NNET_UPMIX_MAX_CHANNEL * SPLICE21];
	NNET_UPMIX_FFT_mag_i = (double *)((float *)&NNET_UPMIX_f_LSF_dec_buff[NNET_UPMIX_LPCOEFF*NNET_UPMIX_MAX_CHANNEL * SPLICE21]);
	NNET_UPMIX_FFT_angl_i = (double *)&NNET_UPMIX_FFT_mag_i[NNET_UPMIX_FFT21_LEN];
	f_LSF = (float *)((double *)&NNET_UPMIX_FFT_angl_i[NNET_UPMIX_FFT21_LEN]);
	f_LSF_dec = (float *)&f_LSF[NNET_UPMIX_LPCOEFF];

	//fixed memory added
	i_LSF_dec = (int *)((float *)&f_LSF_dec[NNET_UPMIX_LPCOEFF]);
	NNET_UPMIX_i_LSF_buff = (int *)&i_LSF_dec[NNET_UPMIX_LPCOEFF];
	NNET_UPMIX_i_LSF_dec_buff = (int *)&NNET_UPMIX_i_LSF_buff[NNET_UPMIX_LPCOEFF*NNET_UPMIX_MAX_CHANNEL * SPLICE21];

	//Elements initialization
	NNET_UPMIX_frm_cnt[0] = 0;
	memset(NNET_UPMIX_frm_stck_i, 0, NNET_UPMIX_FFT_LEN * NNET_UPMIX_MAX_CHANNEL * sizeof(int)); //mem initialization for fft
	memset(NNET_UPMIX_f_LSF_buff, 0.0, NNET_UPMIX_LPCOEFF * NNET_UPMIX_MAX_CHANNEL * SPLICE21 * sizeof(float));
	memset(NNET_UPMIX_i_LSF_buff, 0.0, NNET_UPMIX_LPCOEFF * NNET_UPMIX_MAX_CHANNEL * SPLICE21 * sizeof(int));
	memset(NNET_UPMIX_FFT_mag_i, 0.0, NNET_UPMIX_FFT21_LEN * sizeof(double));
	memset(NNET_UPMIX_FFT_angl_i, 0.0, NNET_UPMIX_FFT21_LEN * sizeof(double));
	
	//Load DNN models
	//std::string feature_transform = FEAT_TRANS_PATH; // [Comment]: 입력 피쳐 -> 입력 피쳐 블록 (Splice) 변환 식 (모델 형태 파일)
	//std::string model_filename = MODEL_PATH; // [Comment]: nnet 모델 파일이름
	
	//kaldi 내장 함수, 모델 파일 read
	//NNET_UPMIX_nnet_transf.Read(feature_transform);
	//NNET_UPMIX_nnet.Read(model_filename); 


	//Delete arma_nnet dump files
	system("del arma_nnet_*");

	
#if NNET_FEEDFORWARD_ARMA == 1


	fmat addshift;
	fmat rescale;

	fmat W_0;
	fmat W_1;
	fmat W_2;
	fmat W_3;
	fmat W_4;
	fmat W_5;

	fmat B_0;
	fmat B_1;
	fmat B_2;
	fmat B_3;
	fmat B_4;
	fmat B_5;


	addshift.load("nnet_arma/arma_nnet_addshift_mPI", arma_ascii);
	rescale.load("nnet_arma/arma_nnet_rescale_dPI", arma_ascii);
	
	//Load Armadillo nnet files
	W_0.load("nnet_arma/arma_nnet_W_0", arma_binary);	
	W_1.load("nnet_arma/arma_nnet_W_1", arma_binary);
	W_2.load("nnet_arma/arma_nnet_W_2", arma_binary);
	W_3.load("nnet_arma/arma_nnet_W_3", arma_binary);
	W_4.load("nnet_arma/arma_nnet_W_4", arma_binary);
	//W_5.load("nnet_arma/arma_nnet_W_5", arma_binary);

	B_0.load("nnet_arma/arma_nnet_B_0", arma_binary);
	B_1.load("nnet_arma/arma_nnet_B_1", arma_binary);
	B_2.load("nnet_arma/arma_nnet_B_2", arma_binary);
	B_3.load("nnet_arma/arma_nnet_B_3", arma_binary);
	B_4.load("nnet_arma/arma_nnet_B_4", arma_binary);
	//B_5.load("nnet_arma/arma_nnet_B_5", arma_binary);


	//Convert fmat to imat of models
	addshift_i = conv_to<imat>::from(addshift * 8192.0);
	rescale_i = conv_to<imat>::from(rescale * 128.0); //Q7


	I_W_0 = conv_to<imat>::from(W_0 * 256.0); //Q8
	I_W_1 = conv_to<imat>::from(W_1 * 256.0);
	I_W_2 = conv_to<imat>::from(W_2 * 256.0);
	I_W_3 = conv_to<imat>::from(W_3 * 256.0);
	I_W_4 = conv_to<imat>::from(W_4 * 256.0);
	I_W_5 = conv_to<imat>::from(W_5 * 256.0);

	I_B_0 = conv_to<imat>::from(B_0 * 268435456.0); //Q28
	I_B_1 = conv_to<imat>::from(B_1 * 268435456.0);
	I_B_2 = conv_to<imat>::from(B_2 * 268435456.0);
	I_B_3 = conv_to<imat>::from(B_3 * 268435456.0);
	I_B_4 = conv_to<imat>::from(B_4 * 268435456.0);
	I_B_5 = conv_to<imat>::from(B_5 * 268435456.0);


#endif

}

void NNET_UPMIX_Process(short * sig_inbuf ,short * sig_outbuf, unsigned short Channel )
{	

	NNET_UPMIX_Execution(	sig_inbuf,		//input
							sig_outbuf,		//ouput
							Channel );		// number of channel
	
}
