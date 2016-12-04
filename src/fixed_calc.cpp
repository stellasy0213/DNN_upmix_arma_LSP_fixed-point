/*
Copyright (c) 2011, Mads A. Elvheim
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the organization nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Mads A. Elvheim BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include<stdio.h>

#define base 16



/* Computing the number of leading zeros in a word. */
static float clz(int x)
{
	int n;

	/* See "Hacker's Delight" book for more details */
	if (x == 0) return 32;
	n = 0;
	if (x <= 0x0000FFFF) { n = n + 16; x = x << 16; }
	if (x <= 0x00FFFFFF) { n = n + 8; x = x << 8; }
	if (x <= 0x0FFFFFFF) { n = n + 4; x = x << 4; }
	if (x <= 0x3FFFFFFF) { n = n + 2; x = x << 2; }
	if (x <= 0x7FFFFFFF) { n = n + 1; }

	return  n;
}

static int fp_reci(int val)
{
    //x = x(2 - dx)
    //Use 2^28 as base, since we normalize "val" into range of 0.5<=val<=1.0
    int x;
    int scale;
    int estimate_offs = 757935405; //(48/17) = 2.823529412 * 2^28
    int estimate_scale = 505290270; //(32/17) = 1.882352941 * 2^28

    if(!val || val<0)
        return 0;

    scale = 31 - clz(val);
    scale = scale - 27;
    // Normalize in range 0.5 <= val <= 1.0 with 2^28 as base
    if(scale >= 1){
        val = val>>scale;
    } else {
        val = val<<((-scale));
    }
    x = estimate_offs + (((int)estimate_scale * val)>>28);
    x = ((int)x * ((2<<28) - (((int)x*val)>>28)))>>28;
    x = ((int)x * ((2<<28) - (((int)x*val)>>28)))>>28;
    x = ((int)x * ((2<<28) - (((int)x*val)>>28)))>>28;

    if(scale >= 1)
        x <<= scale;
    else
        x >>= scale;
    return x;
}

static int fp_div(int numerator, int divisor)
{
    int neg;
    if(divisor < 0){
        divisor = -divisor;
        neg = -1;
    } else {
        neg = 1;
    }

    if(!divisor)
        return 0;
    divisor = fp_reci(divisor) * neg;
    return ((int)numerator * divisor)>>base;
}

static int fp_ln(int val)
{
    int fracv, intv, y, ysq, fracr, bitpos;
    /*
    fracv    -    initial fraction part from "val"
    intv    -    initial integer part from "val"
    y        -    (fracv-1)/(fracv+1)
    ysq        -    y*y
    fracr    -    ln(fracv)
    bitpos    -    integer part of log2(val)
    */
    
    const int ILN2 = 94548;        /* 1/ln(2) with 2^16 as base*/
    const int ILOG2E = 45426;    /* 1/log2(e) with 2^16 as base */

    const int ln_denoms[] = {
        (1<<base)/1,
        (1<<base)/3,
        (1<<base)/5,
        (1<<base)/7,
        (1<<base)/9,
        (1<<base)/11,
        (1<<base)/13,
        (1<<base)/15,
        (1<<base)/17,
        (1<<base)/19,
        (1<<base)/21,
    };

    /* compute fracv and intv */
    bitpos = 15 - clz(val);
    if(bitpos >= 0){
        ++bitpos;
        fracv = val>>bitpos;
    } else if(bitpos < 0){
        /* fracr = val / 2^-(bitpos) */
        ++bitpos;
        fracv = val<<(-bitpos);
    }

    // bitpos is the integer part of ln(val), but in log2, so we convert
    // ln(val) = log2(val) / log2(e)
    intv = bitpos * ILOG2E;

    // y = (ln_fraction_value−1)/(ln_fraction_value+1)
    y = ((int)(fracv-(1<<base))<<base) / (fracv+(1<<base));

    ysq = (y*y)>>base;
    fracr = ln_denoms[10];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[9];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[8];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[7];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[6];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[5];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[4];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[3];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[2];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[1];
    fracr = (((int)fracr * ysq)>>base) + ln_denoms[0];
    fracr =  ((int)fracr * (y<<1))>>base;

    return intv + fracr;
}

static int fp_exp(int val)
{
    int x;

    x = val;
    x = x - (((int)x*(fp_ln(x) - val))>>base);
    x = x - (((int)x*(fp_ln(x) - val))>>base);
    x = x - (((int)x*(fp_ln(x) - val))>>base);
    x = x - (((int)x*(fp_ln(x) - val))>>base);
    return x;
}

static int fp_pow(int ebase, int exponent)
{
    return fp_exp(((int)exponent * fp_ln(ebase))>>base);
}

static unsigned fp_sqrt(unsigned val)
{
    unsigned x;
    int bitpos;
    unsigned long long v;

    if(!val)
        return val;

    /* clz = count-leading-zeros. bitpos is the position of the most significant bit,
        relative to "1" or 1 << base */
    bitpos = base - clz(val);
    
    /* Calculate our first estimate.
        We use the identity 2^a * 2^a = 2^(2*a) or:
         sqrt(2^a) = 2^(a/2)
    */
    if(bitpos > 0u) /* val > 1 */
        x = (1u<<base)<<(bitpos >> 1u);
    else if(bitpos < 0) /* 0 < val < 1 */
        x = (1u<<base)<<((unsigned)(-bitpos) << 1u);
    else /* val == 1 */
        x = (1u<<base);
    
    /* We need to scale val with base due to the division.
       Also val /= 2, hence the subtraction of one*/
    v = (unsigned long long)val <<  (base - 1u);

    /* The actual iteration */
    x = (x >> 1u) + v/x;
    x = (x >> 1u) + v/x;
    x = (x >> 1u) + v/x;
    x = (x >> 1u) + v/x;
    return x;
}

int main(void)
{
	int test = 1;
	test = fp_exp(test);
	printf("fp_exp test value: %d \n", test );

	return 0;
}