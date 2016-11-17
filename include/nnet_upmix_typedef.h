#ifndef nnet_upmix_typedefs_h
#define nnet_upmix_typedefs_h "$Id $"
#ifdef __cplusplus
extern "C" {
#endif

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
//#include <float.h>
//#include <limits.h>
/*
********************************************************************************
*                         DEFINITION OF CONSTANTS 
********************************************************************************
*/
/*
 ********* define char type
 */


///All Module's typedefs ;Rahul


//typedef		int					 int32;
//typedef 	long long			 int64;
//typedef		short				 int16;
//typedef		unsigned int		 U_int32;
//typedef     unsigned char		 Uint8 ;
//typedef		unsigned short		 Uint16;
typedef 	long long 			 Word64;
typedef 	int 				 LoopIndex;
//typedef 	int 				 Flag;
typedef 	signed char 		 Word8;
typedef 	unsigned char	     UWord8;
typedef 	unsigned short 		 Ushort;
typedef 	unsigned int 		 Uint;
typedef 	short 				 Int16;
typedef 	long int             Lint;
typedef 	int             	 Word;
typedef 	char 				 Char;

typedef char                   Int8; 
typedef char                *  pInt8;
typedef unsigned char       *  pUint8; 
typedef short int           *  pInt16; 
typedef unsigned short int  *  pUint16;
typedef int                    Int32;
typedef int                 *  pInt32;
typedef unsigned long int      Uint32;
typedef unsigned long int   *  pUint32;
typedef float                  Float;  
typedef float               *  pFloat; 
typedef double                 Uint64;
typedef double              *  pUint64;
typedef char                   Bool;   
typedef void                   Void;   
typedef void                *  pVoid; 



/*
 ********* define 8 bit signed/unsigned types & constants
 */
//#if SCHAR_MAX == 127
//typedef signed char Word8;
#define minWord8  (-128)
#define maxWord8  127

//typedef unsigned char UWord8;
#define minUWord8 0
#define maxUWord8 255
//#else
//#error cannot find 8-bit type
//#endif


/*
 ********* define 16 bit signed/unsigned types & constants
 */
//#if INT_MAX == 32767
//typedef int short;
//#define minshort     INT_MIN
//#define maxshort     INT_MAX
//typedef unsigned int Ushort;
//#define minUshort    0
//#define maxUshort    UINT_MAX
//#elif SHRT_MAX == 32767
//typedef short short;
#define minshort     (-0x8000)
#define maxshort     0x7fff
//typedef unsigned short Ushort;
#define minUshort    0
#define maxUshort    65535
//#else
//#error cannot find 16-bit type
//#endif


/*
 ********* define 32 bit signed/unsigned types & constants
 */
//#if LONG_MAX == 2147483647
//typedef long int;
//typedef int int;

#define minint     (~0x7fffffffL)
#define maxint     0x7fffffffL
//typedef unsigned long Uint;
#define minUint    0
#define maxUint    0xffffffffUL
//#elif INT_MAX == 2147483647
//typedef int int;
//#define minint     INT_MIN
//#define maxint     INT_MAX
//typedef unsigned int Uint;
//#define minUint    0
//#define maxUint    UINT_MAX
//#else
//#error cannot find 32-bit type
//#endif

/*
 ********* define floating point type & constants
 */
/* use "#if 0" below if Float should be double;
   use "#if 1" below if Float should be float
 */
//#if 1
//typedef float Float;
//#define maxFloat      FLT_MAX
//#define minFloat      FLT_MIN
//#else
//typedef double Float;
//#define maxFloat      DBL_MAX
//#define minFloat      DBL_MIN
//#endif

/*
 ********* define complex type
 */
//typedef struct {
//  Float r;  /* real      part */
//  Float i;  /* imaginary part */
//} CPX;

/*
 ********* define boolean type
 */
//typedef int Bool;
#if 0 // rightloc.lee deleted..
#define false 0
#define true 1
#endif 
/*
 ********* Check current platform
 */
//#if (defined __MSDOS__) || (defined WIN32)
#define PC
#define PLATFORM "PC"
#define LSBFIRST
//#elif defined(__osf__)
//#define OSF
//#define PLATFORM "OSF"
//#define LSBFIRST
//#elif defined(__sun__) || defined(__sun)
//#define SUN
//#define PLATFORM "SUN"
//#undef LSBFIRST
//#elif defined(linux) && defined(i386)
//#define PC
//#define PLATFORM "PC"
//#define LSBFIRST
//#else
//#error "can't determine architecture; adapt typedefs.h to your platform"
//#define PC
//#define PLATFORM "PC"
//#define LSBFIRST
//#endif

#ifdef __cplusplus
}
#endif

#endif
