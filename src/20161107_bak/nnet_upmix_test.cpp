/* **************************************************************************
*		Gwangju Institute of Science and Technology, HuCom Lab				*
*																			*
*							COPYRIGHT (C) 2016								*
*							All Rights Reserved								*
*																			*
*****************************************************************************

*****************************************************************************
* @file	: nnet_upmix_test.c													*
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

#include <stdio.h>
#include "nnet_upmix_api.h"
#include "nnet_upmix_typedef.h"
#include "nnet_upmix_interface.h"

#define WAV_INPUT
//#define TIME_CHECK   // Profile 
//#define BIG_ENDIAN   //Input little to Big endian change 

#define SWAP16(s) \
    (((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)))


//#define PROFILE //for linux build
#ifdef PROFILE 
#include <sys/time.h>
static unsigned long d4_gettimetick()
{
	struct timeval tval;
	gettimeofday(&tval, 0);
	return (tval.tv_usec + tval.tv_sec* 1000000 );

}
static unsigned long start, end;
#endif

#ifdef TIME_CHECK
#include <time.h>
time_t start;
time_t end;
#endif


/*Memory details for NMF:  input, output and internal memory*/
short sig_inbuf[NNET_UPMIX_FRM_LEN*NNET_UPMIX_MAX_CHANNEL];
short sig_outbuf[NNET_UPMIX_FRM_LEN*NNET_UPMIX_MAX_CHANNEL];
int sRAMNMF[2000000]; //4MB


int main( int argc , char *argv[])
{
	FILE *sig_FIn, *sig_FOut;
	short wav_header_in[22]; //44byte wave header buffer
	size_t sig_samplen;

	short Channel;
	int l = 0;

		
	if(argc <3)
	{
		printf("No Input/Output\n");
		return 0;
	}

	NNET_UPMIX_Init(sRAMNMF);
		
	if((sig_FIn = fopen(argv[1],"rb"))==NULL)
	{
		printf("Input file open error\n");
		return 0;
	}
	if((sig_FOut = fopen(argv[2],"wb"))==NULL)
	{
		printf("Output file open error\n");
		return 0;
	}



#ifdef WAV_INPUT
	//Wave header read
	fread(wav_header_in,sizeof(short),22,sig_FIn);
	//Wave header write
	fwrite(wav_header_in, 22,sizeof(short),sig_FOut);
#endif

	l = 0;
	
	Channel=wav_header_in[11];
	
	while(1)
	{
			
		sig_samplen=fread(sig_inbuf,sizeof(short),NNET_UPMIX_FRM_LEN*Channel,sig_FIn); 
		
		if(sig_samplen!=(NNET_UPMIX_FRM_LEN*Channel))
		{	
			fclose(sig_FIn);
			fclose(sig_FOut);
			return 0;
		}
		

#ifdef TIME_CHECK
	start=clock();
#endif

#ifdef PROFILE 
			start = d4_gettimetick();
#endif	
		// Processing
		NNET_UPMIX_Process((short*)sig_inbuf ,(short*)sig_outbuf,Channel );

		// Exract NNET parameters from Kaldi nnet for use of Armadillo
#if NNET_CONVERT_ARMA == 1
		return 0;
#endif
		
#ifdef PROFILE
			end = d4_gettimetick();
			printf( "The execution time for NMF is %d ms /n" , ((end - start)/1000));
#endif

#ifdef TIME_CHECK
	end=clock();
	printf("%f\n",(float)((end-start)));	
#endif

		l++;
		printf("FrameCount=%d\n",l);

		fwrite(sig_outbuf,(NNET_UPMIX_FRM_LEN*Channel),sizeof(short),sig_FOut);	
			
	}
	
	return 0;

}
