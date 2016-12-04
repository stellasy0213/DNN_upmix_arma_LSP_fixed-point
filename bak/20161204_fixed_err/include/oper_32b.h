/* Version 3.3    Last modified: December 26, 1995 */

/* Double precision operations */

void   L_Extract(int L_32, short *hi, short *lo);
int L_Comp(short hi, short lo);
int Mpy_32(short hi1, short lo1, short hi2, short lo2);
int Mpy_32_16(short hi, short lo, short n);
int Div_32(int L_num, short denom_hi, short denom_lo);

