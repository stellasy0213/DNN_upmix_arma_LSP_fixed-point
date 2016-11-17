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


void nnet_upmix_feedforward(float * f_Feat, float * f_Feat_dec)
{
#if 0
	bool no_softmax = NO_SOFTMAX;
	bool apply_log = APPLY_LOG;
	std::string use_gpu=USE_GPU;  //init
#endif

	
    // [Comment]: frame-wise read
	int k, i, l;


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

		fmat a_A(f_Feat, 1, FEAT_ORDER, false, false);

	   //Addshift
	   a_A = a_A + addshift;
	   //Rescale
	   a_A = a_A % rescale;

	   //Affine-Transform & sigmoid
	  fmat a_C_in;
	  fmat a_C_out;
	  fmat a_C_final(f_Feat_dec, FEAT_ORDER, 1, false, false);

	  for (l = 0; l < HIDDEN_LAYER + 1; l++)
	  {
		  if (l == 0)
		  {
			  a_C_in = a_A;
			  a_C_out = (a_C_in*W_0) + B_0; //Affine Transform: C = AX + B
			  a_C_out = 1 / (1 + exp(-1 * a_C_out));// Sigmoid: y = 1/(1+e^-x)
		  }
		  else if(l == 1)
		  {
			  a_C_in = a_C_out;
			  a_C_out = (a_C_in*W_1) + B_1; //Affine Transform: C = AX + B
			  a_C_out = 1 / (1 + exp(-1 * a_C_out));// Sigmoid: y = 1/(1+e^-x)
		  }
		  else if (l == 2)
		  {
			  a_C_in = a_C_out;
			  a_C_out = (a_C_in*W_2) + B_2; //Affine Transform: C = AX + B
			  a_C_out = 1 / (1 + exp(-1 * a_C_out));// Sigmoid: y = 1/(1+e^-x)
		  }
		  else if (l == 3)
		  {
			  a_C_in = a_C_out;
			  a_C_out = (a_C_in*W_3) + B_3; //Affine Transform: C = AX + B
			  a_C_out = 1 / (1 + exp(-1 * a_C_out));// Sigmoid: y = 1/(1+e^-x)
		  }
		  else if (l == 4)
		  {
			  a_C_in = a_C_out;
			  a_C_final = (a_C_in*W_4) + B_4; //Affine Transform: C = AX + B
			  a_C_final = 1 / (1 + exp(-1 * a_C_final));// Sigmoid: y = 1/(1+e^-x)
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
