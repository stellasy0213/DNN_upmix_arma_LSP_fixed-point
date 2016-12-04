#ifndef __NNET_UPMIX_INTERFACE_H
#define __NNET_UPMIX_INTERFACE_H

#include "nnet_upmix_fixed_point.h"

#define WINDOWS32		 	//Windows
//#define RVDS_ARM9			//ARM9 Rvds
//#define ARM9_M6M			//Fujitsu M6M BE
//#define CORTEX_M4			//DRIMeV
//#define CORTEX_A9			//DRIMeIV
//#define CORTEX_A5			//Fujitsu M7M


#ifdef WINDOWS32
#define LITTLE_ENDIAN //this is already defined in other files(commented in order to remove the -Werror(warning error) in D5 build ;Rahul
#include "unix_comp.h"
#define FIXED_POINT 32
#endif

#ifdef RVDS_ARM9
#define ARM_ASM
#define LITTLE_ENDIAN    
#define FIXED_POINT 32
#endif

#ifdef ARM9_M6M
#define ARM_ASM
#define FIXED_POINT 32
#endif

#ifdef CORTEX_M4
#define ARM_ASM
#define LITTLE_ENDIAN    
#define FIXED_POINT 32
#endif

#ifdef CORTEX_A5
#define ARM_ASM
//#define LITTLE_ENDIAN 
#define FIXED_POINT 32   
#endif

#ifdef CORTEX_A9
#define NEON_ASM	
#define ARM_ASM
#define LITTLE_ENDIAN    
#define FIXED_POINT 32
#endif






#endif
