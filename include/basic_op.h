/* Version 3.3    Last modified: December 26, 1995 */

#ifdef __cplusplus
extern "C" {
#endif

/*___________________________________________________________________________
 |                                                                           |
 |   Constants and Globals                                                   |
 |___________________________________________________________________________|
*/
extern Flag Overflow;
extern Flag Carry;

#define MAX_32 (int)0x7fffffffL
#define MIN_32 (int)0x80000000L

#define MAX_16 (short)0x7fff
#define MIN_16 (short)0x8000


/*___________________________________________________________________________
 |                                                                           |
 |   Operators prototypes                                                    |
 |___________________________________________________________________________|
*/

short sature(int L_var1);             /* Limit to 16 bits,    1 */
short add(short var1, short var2);     /* Short add,           1 */
short sub(short var1, short var2);     /* Short sub,           1 */
short abs_s(short var1);                /* Short abs,           1 */
short shl(short var1, short var2);     /* Short shift left,    1 */
short shr(short var1, short var2);     /* Short shift right,   1 */
short mult(short var1, short var2);    /* Short mult,          1 */
int L_mult(short var1, short var2);  /* Long mult,           1 */
short negate(short var1);               /* Short negate,        1 */
short extract_h(int L_var1);          /* Extract high,        1 */
short extract_l(int L_var1);          /* Extract low,         1 */
short round_fixed(int L_var1);              /* round_fixed,               1 */
int L_mac(int L_var3, short var1, short var2); /* Mac,    1 */
int L_msu(int L_var3, short var1, short var2); /* Msu,    1 */
int L_macNs(int L_var3, short var1, short var2);/* Mac without sat, 1*/
int L_msuNs(int L_var3, short var1, short var2);/* Msu without sat, 1*/

int L_add(int L_var1, int L_var2);   /* Long add,        2 */
int L_sub(int L_var1, int L_var2);   /* Long sub,        2 */
int L_add_c(int L_var1, int L_var2); /*Long add with c,  2 */
int L_sub_c(int L_var1, int L_var2); /*Long sub with c,  2 */
int L_negate(int L_var1);               /* Long negate,     2 */
short mult_r(short var1, short var2);  /* Mult with round_fixed,     2 */
int L_shl(int L_var1, short var2); /* Long shift left,     2 */
int L_shr(int L_var1, short var2); /* Long shift right,    2 */
short shr_r(short var1, short var2);/* Shift right with round_fixed, 2 */
short mac_r(int L_var3, short var1, short var2);/* Mac with round_fixeding, 2*/
short msu_r(int L_var3, short var1, short var2);/* Msu with round_fixeding, 2*/
int L_deposit_h(short var1);       /* 16 bit var1 -> MSB,     2 */
int L_deposit_l(short var1);       /* 16 bit var1 -> LSB,     2 */

int L_shr_r(int L_var1, short var2);/* Long shift right with round_fixed,  3*/
int L_abs(int L_var1);            /* Long abs,              3 */

int L_sat(int L_var1);            /* Long saturation,       4 */

short norm_s(short var1);             /* Short norm,           15 */

short div_s(short var1, short var2); /* Short division,       18 */

short norm_l(int L_var1);           /* Long norm,            30 */

#ifdef __cplusplus
}
#endif