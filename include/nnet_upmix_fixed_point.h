#ifndef __NNET_UPMIX_FIXED_POINT_H_
#define __NNET_UPMIX_FIXED_POINT_H_

//typedefs
//#define		int32	int					
//#define 	int64	long long			
//#define		Uint32	unsigned int		
//#define 	Uint64	unsigned long long


//macro functions
#define QCONVERT(x,Q)			(int32)(x *(1LL<<Q)) //returns int32 value
#define QCONVERTLONG(x,Q)		(int64)(x *(1LL<<Q)) //returns int_64 value

#define FIXED_MUL(x,y,z)		(int32)(((int64)x * y)>>z) //returns 32bit value
#define FIXED_MULLONG(x,y,z)	(int64)(((int64)x * y)>>z) //returns 32bit value

#define FIXED_DIV(x,y,z)		(int32)(((int64)x<<z)/ y) //returns 32bit value
#define FIXED_DIVLONG(x,y,z)	(int64)(((int64)x<<z)/ y) //returns 32bit value


//macros
#define Q_FFT 5
#define Q_SCALE 3
#define Q_SCALE2 0
#define Q10 10
#define Q16 16
#define Q15 15
#define	Q20	20
#define Q22	22
#define	Q28	28
#define Q32	32
#define Q33	33
#define Q34 34
#define Q35	35
#define Q36	36



//variables


#endif
