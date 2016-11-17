/* Version 3.3    Last modified: December 26, 1995 */

/* WARNING: Make sure that the proper flags are defined for your system */

/*
   Types definitions
*/


#if defined(__BORLANDC__) || defined (__WATCOMC__) || defined(_MSC_VER) || defined(__ZTC__) || defined(__HIGHC__) || defined (__CYGWIN32__)


typedef  int   Word32;
typedef  short   Word16  ;
typedef  short int   Flag  ;
#elif defined( __sun)
typedef short  short;
typedef long  int;
typedef int   Flag;
#elif defined(__unix__) || defined(__unix)
typedef short short;
typedef int   int;
typedef int   Flag;
#elif defined(VMS) || defined(__VMS)
typedef short  short;
typedef long  int;
typedef int   Flag;
#else
#error  COMPILER NOT TESTED typedef.h needs to be updated, see readme
#endif
#define F   float