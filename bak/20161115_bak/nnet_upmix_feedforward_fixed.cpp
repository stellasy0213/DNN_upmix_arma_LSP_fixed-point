// nnetbin/nnet-forward.cc

// Copyright 2011-2013  Brno University of Technology (Author: Karel Vesely)

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#include "nnet_upmix_api.h"
#include "nnet_upmix_constants.h"

//extern Nnet NNET_UPMIX_nnet_transf;
//extern CuMatrix<BaseFloat> feats, feats_transf, nnet_out;
//extern Matrix<BaseFloat> nnet_feat_in; //nnet_feat_input buffer
//extern Matrix<BaseFloat> nnet_feat_out; //nnet_feat_output buffer


extern fmat addshift;
extern fmat rescale;

extern fmat W_0;
extern fmat W_1;
extern fmat W_2;
extern fmat W_3;
extern fmat W_4;
extern fmat W_5;

extern fmat B_0;
extern fmat B_1;
extern fmat B_2;
extern fmat B_3;
extern fmat B_4;
extern fmat B_5;





void nnet_upmix_feedforward_fixed(int * f_Feat, int * f_Feat_dec)
{
#if 0
	bool no_softmax = NO_SOFTMAX;
	bool apply_log = APPLY_LOG;
	std::string use_gpu=USE_GPU;  //init
#endif

    // [Comment]: frame-wise read
	int k, i, l;

	fmat Q_0;


	/*
	//Feature Trasnform [Splice, Addshift, Rescale]
	//--------------------Kaldi Region--------------
	for (i = 0; i < SPLICE21; i++)	{
		for (k = 0; k < NNET_UPMIX_FEAT_LEN; k++)		{
			(nnet_feat_in.Data())[k + i * NNET_UPMIX_FEAT_LEN] = f_Feat[k + i * NNET_UPMIX_FEAT_LEN];
		}
	}
      // push it 
      feats = nnet_feat_in;

#if HAVE_CUDA_OPT
	  // fwd-pass, feature transform,
	  NNET_UPMIX_nnet_transf.Feedforward(feats, &feats_transf);
#endif
	  fmat a_A((float*)((feats_transf).Data()), (feats_transf).NumRows(), (feats_transf).NumCols(), false, false);


	  // write
	  //feats_transf.CopyToMat(&nnet_feat_out); //CuMatrix 에서 Matrix로 Data copy
	  //for (k = 0; k < NNET_UPMIX_FEAT_LEN; k++)
	  //  f_Feat_dec[k] = (nnet_feat_out.Data())[k];
	  //--------------------------------------------
	  */

	  imat a_A(f_Feat, 1, FEAT_ORDER, false, false);

	   //Addshift
	  imat addshift_i = conv_to<imat>::from(addshift * 8192.0);
	  a_A = a_A + addshift_i;
	   //Rescale
	  imat rescale_i = conv_to<imat>::from(rescale * 128.0); //Q7
	  a_A = a_A % rescale_i;

	   //Affine-Transform & sigmoid
	  imat a_C_in;
	  imat a_C_out;
	  imat a_C_final(f_Feat_dec, FEAT_ORDER, 1, false, false);

	  //fmat a_C_out_f = conv_to<fmat>::from(a_C_out / 32768.0);
	  fmat a_C_out_f;

	  imat I_W_0 = conv_to<imat>::from(W_0 * 256.0); //Q8
	  imat I_W_1 = conv_to<imat>::from(W_1 * 256.0);
	  imat I_W_2 = conv_to<imat>::from(W_2 * 256.0);
	  imat I_W_3 = conv_to<imat>::from(W_3 * 256.0);
	  imat I_W_4 = conv_to<imat>::from(W_4 * 256.0);
	  imat I_W_5 = conv_to<imat>::from(W_5 * 256.0);

	  imat I_B_0 = conv_to<imat>::from(B_0 * 268435456.0); //Q28
	  imat I_B_1 = conv_to<imat>::from(B_1 * 268435456.0);
	  imat I_B_2 = conv_to<imat>::from(B_2 * 268435456.0);
	  imat I_B_3 = conv_to<imat>::from(B_3 * 268435456.0);
	  imat I_B_4 = conv_to<imat>::from(B_4 * 268435456.0);
	  imat I_B_5 = conv_to<imat>::from(B_5 * 268435456.0);

	  for (l = 0; l < HIDDEN_LAYER + 1; l++)
	  {
		  if (l == 0)
		  {
			  a_C_in = a_A;
			  a_C_out = (a_C_in*I_W_0) + I_B_0; //Affine Transform: C = AX + B
			  a_C_out_f = conv_to<fmat>::from(a_C_out);
			  a_C_out_f = a_C_out_f / 268435456.0;  //Q28 -> Q0
			  a_C_out_f = 1 / (1 + exp(-1 * a_C_out_f));// Sigmoid: y = 1/(1+e^-x)
			  a_C_out = conv_to<imat>::from(a_C_out_f * 1048576.0);  //Q28 -> Q20
		  }
		  else if(l == 1)
		  {
			  a_C_in = a_C_out;
			  a_C_out = (a_C_in*I_W_1) + I_B_1; //Affine Transform: C = AX + B
			  a_C_out_f = conv_to<fmat>::from(a_C_out);
			  a_C_out_f = a_C_out_f / 268435456.0;
			  a_C_out_f = 1 / (1 + exp(-1 * a_C_out_f));// Sigmoid: y = 1/(1+e^-x)
			  a_C_out = conv_to<imat>::from(a_C_out_f * 1048576.0);
		  }
		  else if (l == 2)
		  {
			  a_C_in = a_C_out;
			  a_C_out = (a_C_in*I_W_2) + I_B_2; //Affine Transform: C = AX + B
			  a_C_out_f = conv_to<fmat>::from(a_C_out);
			  a_C_out_f = a_C_out_f / 268435456.0;
			  a_C_out_f = 1 / (1 + exp(-1 * a_C_out_f));// Sigmoid: y = 1/(1+e^-x)
			  a_C_out = conv_to<imat>::from(a_C_out_f * 1048576.0);
		  }
		  else if (l == 3)
		  {
			  a_C_in = a_C_out;
			  a_C_out = (a_C_in*I_W_3) + I_B_3; //Affine Transform: C = AX + B
			  a_C_out_f = conv_to<fmat>::from(a_C_out);
			  a_C_out_f = a_C_out_f / 268435456.0;
			  a_C_out_f = 1 / (1 + exp(-1 * a_C_out_f));// Sigmoid: y = 1/(1+e^-x)
			  a_C_out = conv_to<imat>::from(a_C_out_f * 1048576.0);
		  }
		  else if (l == 4)
		  {
			  a_C_in = a_C_out;
			  a_C_final = (a_C_in*I_W_4) + I_B_4; //Affine Transform: C = AX + B
			  a_C_out_f = conv_to<fmat>::from(a_C_final);
			  a_C_out_f = a_C_out_f / 268435456.0;
			  a_C_out_f = 1 / (1 + exp(-1 * a_C_out_f));// Sigmoid: y = 1/(1+e^-x)
			  a_C_final = conv_to<imat>::from(a_C_out_f * 8192.0);
		  }
		  //else if (l == 5)
		  //{
		//	  a_C_in = a_C_out;
		//	  a_C_final = (a_C_in*W_5) + B_5; //Affine Transform: C = AX + B
		//	  a_C_final = 1 / (1 + exp(-1 * a_C_final));// Sigmoid: y = 1/(1+e^-x)
		 // }

	  }


      // fwd-pass, nnet,
	  //NNET_UPMIX_nnet.Feedforward(feats_transf, &nnet_out); //실제 feed-forward decoding 진행

	  
	  // write
	  //for (k = 0; k < NNET_UPMIX_FEAT_LEN; k++)
	  //	  f_Feat_dec[k] = a_C_out(k, 0);
		 

#if HAVE_CUDA==1
    if (kaldi::g_kaldi_verbose_level >= 1) {
      CuDevice::Instantiate().PrintProfile();
    }
#endif
	
}
