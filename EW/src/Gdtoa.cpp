// XXX cf http://www.tcl.tk/cgi-bin/tct/tip/132
// The code in this file is an adaption of parts of gdtoa by David M. Gay.

// This was necesary because MinGW used to use the Windows sscanf, which was
// several orders of magnitude slower than necessary.
// It is also used to provide efficient string representations, which cannot
// be done with standard library functions.

// The original source contains the copyright notice:

// The author of this software is David M. Gay.
//
// Copyright (C) 1998-2001 by Lucent Technologies
// All Rights Reserved
//
// Permission to use, copy, modify, and distribute this software and
// its documentation for any purpose and without fee is hereby
// granted, provided that the above copyright notice appear in all
// copies and that both that the copyright notice and this
// permission notice and warranty disclaimer appear in supporting
// documentation, and that the name of Lucent or any of its entities
// not be used in advertising or publicity pertaining to
// distribution of the software without specific, written prior
// permission.
//
// LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
// IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY
// SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
// IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
// THIS SOFTWARE.

// The changes are copyright 2009-2010 Bill Green and are licensed under the
// same terms as the original.

//Always use standard rounding, irrespective of rounding mode.
//Don't report inexact or inex

//XXX next: lshift, rshift
//XXX eliminate bn_ Bigint
//XXX make thread safe/eliminate static variables
//check where reached:
// {static int k;if(k==0){k=1;printf("XXXREACHED %04d\n",__LINE__);}}
//XXX v
// README:
// Systems with preemptive scheduling of multiple threads require some
// manual intervention.  On such systems, it's necessary to
// to provide (or suitably define) locks, acquired by
// ACQUIRE_DTOA_LOCK(n) and freed by FREE_DTOA_LOCK(n) for n = 0 or 1.
//
// By default, there is a private pool of memory of length 2000 bytes
// for intermediate quantities, and MALLOC (see gdtoaimp.h) is called only
// if the private pool does not suffice.   2000 is large enough that MALLOC
// is called only under very unusual circumstances (decimal -> binary
// conversion of very long strings) for conversions to and from double
// precision.
//
// If you wish to write helper functions for other kinds of
// IEEE-like arithmetic, some explanation of the bits
// array may be helpful.  Both gdtoa and strtodg operate on a bits array
// described by fpi_nbits, fpi_emin and fpi_emax parameters.
// The bits array is an array of 32-bit
// unsigned integer type.  Floating-point numbers have fpi_nbits bits,
// with the least significant 32 bits in bits[0], the next 32 bits in
// bits[1], etc.  These numbers are regarded as integers multiplied by
// 2^e (i.e., 2 to the power of the exponent e), where e is the second
// argument (be) to gdtoa and is stored in *exp by strtodg.  The minimum
// and maximum exponent values fpi_emin and fpi_emax for normalized
// floating-point numbers reflect this arrangement.  For example, the
// P754 standard for binary IEEE arithmetic specifies doubles as having
// 53 bits, with normalized values of the form 1.xxxxx... times 2^(b-1023),
// with 52 bits (the x's) and the biased exponent b represented explicitly;
// b is an unsigned integer in the range 1 <= b <= 2046 for normalized
// finite doubles, b = 0 for denormals, and b = 2047 for Infinities and NaNs.
// To turn an IEEE double into the representation used by strtodg and gdtoa,
// we multiply 1.xxxx... by 2^52 (to make it an integer) and reduce the
// exponent e = (b-1023) by 52:
//
//   fpi_emin = 1 - 1023 - 52
//   fpi_emax = 1046 - 1023 - 52
//
// In various wrappers for IEEE double, we actually write -53 + 1 rather
// than -52, to emphasize that there are 53 bits including one implicit bit.
//
// Normal floating-point numbers have
// bit fpi_nbits in the bits array on.  Denormals have it off, with
// exponent = fpi_emin.
//XXX ^

// mlo possibly uninit in gdtoa.c
// eliminate data: dtoa_result, freelist, p5s, pmem_next, private_mem
// use std array for hexdig
// ?HEX_NAN
// check use of double/float
// review aliasing
// review casts:
//  int
//  uint32_t
//  int32_t
//  const unsigned char *
//  int *
//  char *
//  uint32_t *
//  unsigned short *
//  unsigned short
//  Bigint *
// eliminate match
// adjust api
// localize/check included files
//  float.h -> DBL_DIG

// CHANGES:
// . Extracted just the source for strtopf, strtopd, g_dfmt and g_ffmt, with:
//     gdtoa mode 0
//     g_dfmt/g_ffmt ndigs 0
//     no hex NaN's
//     no hex floats
//     default rounding
//     no errno reporting
//     no rounding reporting
// . Assumed standard IEEE 754 or the x86 version of it (extra precision
//   registers), with default rounding.

//XXX v
// This is a variation on dtoa.c that converts arbitary binary
// floating-point formats to and from decimal notation.  It uses
// double-precision arithmetic internally.

#define XXX_OMIT_PRIVATE_MEMORY 1
// strtod for IEEE-arithmetic machines.
//
// This strtod returns a nearest machine number to the input decimal
// string (or sets errno to ERANGE).  Ties are
// broken by the IEEE round-even rule.
// Modifications:
//
//  1. We only require IEEE double-precision
//    arithmetic (not IEEE double-extended).
//  2. We get by with floating-point arithmetic in a case that
//    Clinger missed -- when we're computing d * 10^n
//    for a small integer d and the integer n is not too
//    much larger than 22 (the maximum integer k for which
//    we can represent 10^k exactly), we may be able to
//    compute (d*10^k) * 10^(e-k) with just one roundoff.
//  3. Rather than a bit-at-a-time adjustment of the binary
//    result in the hard case, we use floating-point
//    arithmetic to determine the adjustment to within
//    one bit; only in really hard cases do we need to
//    compute a second residual.
//  4. Because of 3., we don't need a large table of powers of 10
//    for ten-to-e (just some small tables, e.g. of 10^k
//    for 0 <= k <= 22).

// define Omit_Private_Memory to omit logic (added Jan. 1998) for making
//  memory allocations from a private pool of memory when possible.
//  When used, the private pool is PRIVATE_MEM bytes long:  2304 bytes.
//  This length
//  suffices to get rid of malloc calls except for unusual cases,
//  such as decimal-to-binary conversion of a very long string of
//  digits.

//XXX eliminate do, word0, L

#include "Gdtoa.h"

#include <stdint.h>
// These 2 for BYTE_ORDER and LITTLE_ENDIAN.
#include <sys/param.h>
#include <sys/types.h>
#include <cfloat>
#include <cstddef>
//XXX for debugging
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include "BigNum.h"

namespace {

//XXX static variable
//XXX eliminate  #defines
//XXX eliminate #if using uint64_t
#if BYTE_ORDER == LITTLE_ENDIAN
#define LWORD 0
#define HWORD 1
#else
#define HWORD 0
#define LWORD 1
#endif

  const int n_bigtens = 5;

  union dl_union_t { double d; uint32_t L[2]; };

//XXX eliminate  #defines
//XXX eliminate  using uint64_t
#define word0(x) (x)->L[HWORD]
#define word1(x) (x)->L[LWORD]

  const int Exp_shift = 20;
  const int Exp_shift1 = 20;
  const int Exp_msk1 = 0x100000;
  const int Exp_mask = 0x7ff00000;
  const int MANT_DIG = 53;
// DBL_MANT_DIG
  const int Bias = 1023;
  const int Exp_1 = 0x3ff00000;
  const int Exp_11 = 0x3ff00000;
  const int Frac_mask = 0xfffff;
  const int Frac_mask1 = 0xfffff;
  const int Ten_pmax = 22;
// Ten_pmax = floor(MANT_DIG*log(2)/log(5))
  const int Bletch = 0x10;
// Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16
  const int Quick_max = 14;
// Quick_max = floor((MANT_DIG-1)*log(FLT_RADIX)/log(10) - 1)
  const int Int_max = 14;
// Int_max = floor(MANT_DIG*log(FLT_RADIX)/log(10) - 1)

  const int FPI_D_NBITS = 53;
  const int FPI_D_EMIN = (1-1023-53+1);
  const int FPI_D_EMAX = (2046-1023-53+1);
  const int FPI_F_NBITS = 24;
  const int FPI_F_EMIN = (1-127-24+1);
  const int FPI_F_EMAX = (254-127-24+1);

  const int KSHIFT = 5;
  const int kmask = 31;

  const int Kmax = 9;

  struct Bigint {
    struct Bigint *next;
    int k, maxwds, wds;
    uint32_t x[1];
  };

  Bigint *freelist[Kmax+1];
#ifndef XXX_OMIT_PRIVATE_MEMORY
  const unsigned int PRIVATE_MEM = 2304;
  double private_mem[PRIVATE_MEM], *pmem_next = private_mem;
#endif

//XXX check
  Bigint *
  Balloc(int k)
  {
    int x;
    Bigint *rv;
#ifndef XXX_OMIT_PRIVATE_MEMORY
    unsigned int len;
#endif

// ACQUIRE_DTOA_LOCK(0);
// The k > Kmax case does not need ACQUIRE_DTOA_LOCK(0),
// but this case seems very unlikely.
    if (k <= Kmax && (rv = freelist[k]) !=0) {
      freelist[k] = rv->next;
    } else {
      x = 1 << k;
#ifdef XXX_OMIT_PRIVATE_MEMORY
      rv = static_cast<Bigint *>(malloc(sizeof(Bigint) +
       (x-1)*sizeof(uint32_t)));
#else
      len = (sizeof(Bigint) + (x-1)*sizeof(uint32_t) + sizeof(double) - 1)
       /sizeof(double);
      if (k <= Kmax && pmem_next - private_mem + len <= PRIVATE_MEM) {
        rv = reinterpret_cast<Bigint *>(pmem_next);
        pmem_next += len;
      } else {
        rv = static_cast<Bigint *>(malloc(len*sizeof(double)));
      }
#endif
      rv->k = k;
      rv->maxwds = x;
    }
// FREE_DTOA_LOCK(0);
    rv->wds = 0;
    return rv;
  }

  inline Bigint *
  Bcopy_bn(const ew::BigNum &y)
  {

    Bigint *b;
    int k = 1;
    while ((1 << k) < y.nwords()) {
      k += 1;
    }
    b = Balloc(k + 1);
    if(y.nwords() >= b->maxwds){printf("XXXXff\n");}
    b->wds = y.nwords();
    std::memcpy(&b->x,y.words(),y.nwords()*sizeof(uint32_t));
    return b;
  }

  inline void
  Bcopy(Bigint *x, Bigint *y)
  {
    std::memcpy(&x->wds,&y->wds,y->wds*sizeof(uint32_t) + sizeof(int));
  }

  inline int
  LeadingZeroBits(uint32_t x)
  {
    int k = 0, l = 0;
    for (int i = 16; i > 0; i >>= 1) {
      if (x < (static_cast<uint32_t>(1) << static_cast<uint32_t>(i + l))) {
        k += i;
      } else {
        l += i;
      }
      if (i == 1 && k == 31 && x == 0) {
        k += 1;
      }
    }
    return k;
  }

  inline int
  TrailingZeroBits(uint32_t x)
  {
#ifndef XXX
    if (x & 7) {
      if (x & 1) {
        return 0;
      } else if (x & 2) {
        return 1;
      } else {
        return 2;
      }
    }
#endif
    int k = 0;
    for (int i = 16; i > 0; i >>= 1) {
      if (
       (x &
       ((static_cast<uint32_t>(1) << static_cast<uint32_t>(i + k)) - 1)) ==
       0) {
        k += i;
      }
      if (i == 1 && k == 31 && x == 0) {
        k += 1;
      }
    }
    return k;
  }

//XXX check
  void
  Bfree
   (Bigint *v)
  {
    if (v) {
      if (v->k > Kmax) {
        free(static_cast<void *>(v));
      } else {
// ACQUIRE_DTOA_LOCK(0);
        v->next = freelist[v->k];
        freelist[v->k] = v;
// FREE_DTOA_LOCK(0);
      }
    }
  }

  Bigint *
  multadd(Bigint *b, int m, int a)
  {
    int i, wds;
    uint32_t *x;
    uint64_t carry, y;
    Bigint *b1;

    wds = b->wds;
    x = b->x;
    i = 0;
    carry = a;
    do {
      y = *x * static_cast<uint64_t>(m) + carry;
      carry = y >> 32;
      *x++ = y & 0xffffffffUL;
    } while(++i < wds);
    if (carry) {
      if (wds >= b->maxwds) {
        b1 = Balloc(b->k+1);
        Bcopy(b1, b);
        Bfree(b);
        b = b1;
      }
      b->x[wds++] = carry;
      b->wds = wds;
    }
    return b;
  }

  Bigint *
  mult(Bigint *a, Bigint *b)
  {
    Bigint *c;
    int k, wa, wb, wc;
    uint32_t *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
    uint32_t y;
    uint64_t carry, z;

    if (a->wds < b->wds) {
      c = a;
      a = b;
      b = c;
    }
    k = a->k;
    wa = a->wds;
    wb = b->wds;
    wc = wa + wb;
    if (wc > a->maxwds) {
      k += 1;
    }
    c = Balloc(k);
    for(x = c->x, xa = x + wc; x < xa; x += 1) {
      *x = 0;
    }
    xa = a->x;
    xae = xa + wa;
    xb = b->x;
    xbe = xb + wb;
    xc0 = c->x;
    for(; xb < xbe; xc0 += 1) {
      if ( (y = *xb++) !=0) {
        x = xa;
        xc = xc0;
        carry = 0;
        do {
          z = *x++ * static_cast<uint64_t>(y) + *xc + carry;
          carry = z >> 32;
          *xc++ = z & 0xffffffffUL;
        } while(x < xae);
        *xc = carry;
      }
    }
    for(xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) {
    }
    c->wds = wc;
    return c;
  }

  Bigint *p5s;

  Bigint *
  pow5mult
   (Bigint *b, int k)
  {
    Bigint *b1, *p5, *p51;
    int i;
    static const int p05[3] = { 5, 25, 125 };
//{static int k0;if(k>k0) {k0=k;printf("XXXX5 %d\n",k);}}

    if ( (i = k & 3) !=0) {
      b = multadd(b, p05[i-1], 0);
    }

    if (!(k >>= 2)) {
      return b;
    }
    if ((p5 = p5s) == 0) {
// first time
      ew::BigNum bn_p5(625);
      p5 = p5s = Bcopy_bn(bn_p5);
      p5->next = 0;
    }
    for(;;) {
      if (k & 1) {
        b1 = mult(b, p5);
        Bfree(b);
        b = b1;
      }
      if (!(k >>= 1)) {
        break;
      }
      if ((p51 = p5->next) == 0) {
        p51 = p5->next = mult(p5,p5);
        p51->next = 0;
      }
      p5 = p51;
    }
    return b;
  }

  Bigint *
  lshift(Bigint *b, int k)
  {
    int i, k1, n, n1;
    Bigint *b1;
    uint32_t *x, *x1, *xe, z;

    n = k >> KSHIFT;
    k1 = b->k;
    n1 = n + b->wds + 1;
    for(i = b->maxwds; n1 > i; i <<= 1) {
      k1 += 1;
    }
    b1 = Balloc(k1);
    x1 = b1->x;
    for(i = 0; i < n; i += 1) {
      *x1++ = 0;
    }
    x = b->x;
    xe = x + b->wds;
    if (k &= kmask) {
      k1 = 32 - k;
      z = 0;
      do {
        *x1++ = *x << k | z;
        z = *x++ >> k1;
      } while(x < xe);
      if ((*x1 = z) !=0) {
        n1 += 1;
      }
    } else {
      do
      *x1++ = *x++;
      while(x < xe);
    }
    b1->wds = n1 - 1;
    Bfree(b);
    return b1;
  }

  Bigint *
  diff(Bigint *a, Bigint *b)
  {
    Bigint *c;
    int i, wa, wb;
    uint32_t *xa, *xae, *xb, *xbe, *xc;
    uint64_t borrow, y;

    ew::BigNum bn_a(a->wds, a->x);
    ew::BigNum bn_b(b->wds, b->x);
    i = (bn_a != bn_b);
    if (!i) {
      c = Balloc(0);
      c->wds = 1;
      c->x[0] = 0;
      return c;
    }
    if (i < 0) {
      c = a;
      a = b;
      b = c;
      i = 1;
    } else {
      i = 0;
    }
    c = Balloc(a->k);
    wa = a->wds;
    xa = a->x;
    xae = xa + wa;
    wb = b->wds;
    xb = b->x;
    xbe = xb + wb;
    xc = c->x;
    borrow = 0;
    do {
      y = static_cast<uint64_t>(*xa++) - *xb++ - borrow;
      borrow = y >> 32 & 1UL;
      *xc++ = y & 0xffffffffUL;
    } while(xb < xbe);
    while(xa < xae) {
      y = *xa++ - borrow;
      borrow = y >> 32 & 1UL;
      *xc++ = y & 0xffffffffUL;
    }
    while(!*--xc) {
      wa -= 1;
    }
    c->wds = wa;
    return c;
  }

  double
  b2d
   (Bigint *a, int *e)
  {
    uint32_t *xa, *xa0, w, y, z;
    int k;
    dl_union_t d;

    xa0 = a->x;
    xa = xa0 + a->wds;
    y = *--xa;
    k = LeadingZeroBits(y);
    *e = 32 - k;
// 11 is the number of exponent bits
    if (k < 11) {
      word0(&d) = Exp_1 | y >> (11 - k);
      if (xa > xa0) {
        w = *--xa;
      } else {
        w = 0;
      }
      word1(&d) = y << ((32-11) + k) | w >> (11 - k);
      goto ret_d;
    }
    if (xa > xa0) {
      z = *--xa;
    } else {
      z = 0;
    }
    if (k -= 11) {
      word0(&d) = Exp_1 | y << k | z >> (32 - k);
      if (xa > xa0) {
        y = *--xa;
      } else {
        y = 0;
      }
      word1(&d) = z << k | y >> (32 - k);
    } else {
      word0(&d) = Exp_1 | y;
      word1(&d) = z;
    }
  ret_d:
    return d.d;
  }

  enum floating_point_t {
    FLOATING_POINT_ZERO,
    FLOATING_POINT_DENORMALIZED,
    FLOATING_POINT_NORMALIZED,
    FLOATING_POINT_INFINITY,
    FLOATING_POINT_NAN
  };

  struct double_data {
    uint64_t mantissa;
    int mantissa_bits;
    int exponent;
    int sign;
    int type;
  };

  inline void
  decompose_double(double_data *data, double d)
  {
// Evade gcc ULL constant warning.
    static const uint64_t U64_1 = 1;
    static const uint64_t SIGN_MASK = (U64_1 << 63);
    static const uint64_t FRACTION_MASK = ((~U64_1) >> 20);
    static const int FRACTION_BITS = 52;
    static const int EXPONENT_BIAS = 1023;
    static const int EXPONENT_MAX = 1023;
    static const int EXPONENT_MIN = -1022;
    std::memcpy(static_cast<void *>(&(data->mantissa)),
     static_cast<void *>(&d), sizeof(uint64_t));
    if (data->mantissa & SIGN_MASK) {
      data->sign = -1;
      data->mantissa &= ~SIGN_MASK;
    } else {
      data->sign = 1;
    }
    data->exponent = (data->mantissa >> FRACTION_BITS) - EXPONENT_BIAS;
    data->mantissa &= FRACTION_MASK;
    if (data->exponent > EXPONENT_MAX) {
      if (data->mantissa == 0) {
        data->type = FLOATING_POINT_INFINITY;
        data->mantissa_bits = 0;
      } else {
        data->type = FLOATING_POINT_NAN;

      }
    } else if (data->exponent < EXPONENT_MIN) {

    } else {

    }
  }

  Bigint *
  d2b
   (double dd, int *e, int *bits)
  {
    Bigint *b;
    dl_union_t d;
    int i;
    int de, k;
    uint32_t *x, y, z;
    d.d = dd;

    b = Balloc(1);
    x = b->x;

    z = word0(&d) & Frac_mask;
    word0(&d) &= 0x7fffffff;
// clear sign bit, which we ignore
    de = static_cast<int>(word0(&d) >> 20);
    if (de !=0) {
      z |= 0x100000;
    }
    if ( (y = word1(&d)) !=0) {
      if ( (k = TrailingZeroBits(y)) !=0) {
        y >>= k;
        x[0] = y | z << (32 - k);
        z >>= k;
      } else {
        x[0] = y;
      }
      x[1] = z;
      if (x[1] != 0) {
        i = b->wds = 2;
      } else {
        i = b->wds = 1;
      }
    } else {
      k = TrailingZeroBits(z);
      x[0] = (z >> k);
      i = b->wds = 1;
      k += 32;
    }
    if (de) {
      *e = de - Bias - (MANT_DIG-1) + k;
      *bits = MANT_DIG - k;
    } else {
      *e = de - Bias - (MANT_DIG-1) + 1 + k;
      *bits = 32*i - LeadingZeroBits(x[i-1]);
    }
    return b;
  }

  const double bigtens[] = {1e16, 1e32, 1e64, 1e128, 1e256};
  const double tinytens[] = {1e-16, 1e-32, 1e-64, 1e-128, 1e-256};
  const double tens[] = {
    1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
    1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
    1e20, 1e21, 1e22
  };

  void
  rshift(Bigint *b, int k)
  {
    uint32_t *x, *x1, *xe, y;
    int n;

    x = x1 = b->x;
    n = k >> KSHIFT;
    if (n < b->wds) {
      xe = x + b->wds;
      x += n;
      if (k &= kmask) {
        n = 32 - k;
        y = *x++ >> k;
        while(x < xe) {
          *x1++ = (y | (*x << n)) & 0xffffffff;
          y = *x++ >> k;
        }
        if ((*x1 = y) !=0) {
          x1 += 1;
        }
      } else {
        while(x < xe) {
          *x1++ = *x++;
        }
      }
    }
    if ((b->wds = x1 - b->x) == 0) {
      b->x[0] = 0;
    }
  }

  int
  trailz(Bigint *b)
  {
    uint32_t L, *x, *xe;
    int n = 0;

    x = b->x;
    xe = x + b->wds;
    for(n = 0; x < xe && !*x; x += 1) {
      n += 32;
    }
    if (x < xe) {
      L = *x;
      n += TrailingZeroBits(L);
    }
    return n;
  }

  const int
  fivesbits[] = {
    0,  3,  5,  7, 10, 12, 14, 17, 19, 21,
    24, 26, 28, 31, 33, 35, 38, 40, 42, 45,
    47, 49, 52
  };

  Bigint *
  s2b
   (const char *s, int nd0, int nd, uint32_t y9, int dplen)
  {
    Bigint *b;
    int i, k;
    int32_t x, y;

    x = (nd + 8) / 9;
    for(k = 0, y = 1; x > y; y <<= 1, k++) ;
    b = Balloc(k);
    b->x[0] = y9;
    b->wds = 1;

    i = 9;
    if (9 < nd0) {
      s += 9;
      do {
        b = multadd(b, 10, *s++ - '0');
      } while(++i < nd0);
      s += dplen;
    } else {
      s += dplen + 9;
    }
    for(; i < nd; i += 1) {
      b = multadd(b, 10, *s++ - '0');
    }
    return b;
  }

  double
  ratio
   (Bigint *a, Bigint *b)
  {
    dl_union_t da, db;
    int k, ka, kb;

    da.d = b2d(a, &ka);
    db.d = b2d(b, &kb);
    k = ka - kb + 32*(a->wds - b->wds);
    if (k > 0) {
      word0(&da) += k*Exp_msk1;
    } else {
      k = -k;
      word0(&db) += k*Exp_msk1;
    }
    return da.d / db.d;
  }

  int
  match
   (const char **sp, const char *t)
  {
    int c, d;
    const char *s = *sp;

    while( (d = *t++) !=0) {
      if ((c = *++s) >= 'A' && c <= 'Z') {
        c += 'a' - 'A';
      }
      if (c != d) {
        return 0;
      }
    }
    *sp = s + 1;
    return 1;
  }

  void
  copybits(uint32_t *c, int n, Bigint *b)
  {
    uint32_t *ce, *x, *xe;

    ce = c + ((n-1) >> KSHIFT) + 1;
    x = b->x;
    xe = x + b->wds;
    while(x < xe) {
      *c++ = *x++;
    }
    while(c < ce) {
      *c++ = 0;
    }
  }

//XXX check
  int
  BAnyOn(Bigint *b, int k)
  {
    int n, nwds;
    uint32_t *x, *x0, x1, x2;

    x = b->x;
    nwds = b->wds;
    n = k >> KSHIFT;
    if (n > nwds) {
      n = nwds;
    } else if (n < nwds && (k &= kmask)) {
      x1 = x2 = x[n];
      x1 >>= k;
      x1 <<= k;
      if (x1 != x2) {
        return 1;
      }
    }
    x0 = x;
    x += n;
    while(x > x0) {
      if (*--x) {
        return 1;
      }
    }
    return 0;
  }

  Bigint *
  sum(Bigint *a, Bigint *b)
  {
    Bigint *c;
    uint32_t carry, *xc, *xa, *xb, *xe, y;
    uint32_t z;

    if (a->wds < b->wds) {
      c = b; b = a; a = c;
    }
    c = Balloc(a->k);
    c->wds = a->wds;
    carry = 0;
    xa = a->x;
    xb = b->x;
    xc = c->x;
    xe = xc + b->wds;
    do {
      y = (*xa & 0xffff) + (*xb & 0xffff) + carry;
      carry = (y & 0x10000) >> 16;
      z = (*xa++ >> 16) + (*xb++ >> 16) + carry;
      carry = (z & 0x10000) >> 16;
      *xc = ((z << 16) | (y & 0xffff));
      xc += 1;
    } while(xc < xe);
    xe += a->wds - b->wds;
    while (xc < xe) {
      y = (*xa & 0xffff) + carry;
      carry = (y & 0x10000) >> 16;
      z = (*xa++ >> 16) + carry;
      carry = (z & 0x10000) >> 16;
      *xc = ((z << 16) | (y & 0xffff));
      xc += 1;
    }
    if (carry) {
      if (c->wds == c->maxwds) {
        b = Balloc(c->k + 1);
        Bcopy(b, c);
        Bfree(c);
        c = b;
      }
      c->x[c->wds++] = 1;
    }
    return c;
  }

//XXX check
  bool
  AllOnesN(Bigint *b, int n)
  {
    uint32_t *x = b->x;
    uint32_t *xe = x + (n >> 5);
    while (x < xe) {
      if (*x != 0xffffffff) {
        return false;
      }
      x += 1;
    }
    n &= 31;
    if (n == 0) {
      return true;
    }
    uint32_t u = *x;
    u |= (0xffffffff << n);
    if (u != 0xffffffff) {
      return false;
    }
    return true;
  }

#ifdef XXX
  static int
  all_on(Bigint *b, int n)
  {
    unsigned long *x, *xe;

    x = (unsigned long *)(b->x);
    xe = x + (n >> KSHIFT);
    while(x < xe) {
      if ((*x++ & 0xffffffff) != 0xffffffff) {
        return 0;
      }
    }
    if (n &= kmask) {
      return ((*x | (0xffffffff << n)) & 0xffffffff) == 0xffffffff;
    }
    return 1;
  }

  inline bool
  AllOnesN(Bigint *b, int n)
  {
    bool r1 = AllOnesN_XXX(b, n);
    bool r2 = all_on(b, n);
    if (r1 != r2) {
      std::printf("XXX gdtoa\n");
    }
    return r1;
  }
#endif

  Bigint *
  set_ones(Bigint *b, int n)
  {
    int k;
    uint32_t *x, *xe;

    k = (n + ((1 << KSHIFT) - 1)) >> KSHIFT;
    if (b->k < k) {
      Bfree(b);
      b = Balloc(k);
    }
    k = n >> KSHIFT;
    if (n &= kmask) {
      k += 1;
    }
    b->wds = k;
    x = b->x;
    xe = x + k;
    while(x < xe) {
      *x++ = 0xffffffff;
    }
    if (n) {
      x[-1] >>= 32 - n;
    }
    return b;
  }

  int
  mantbits(dl_union_t *d)
  {
    uint32_t L;
    if ( (L = word1(d)) !=0) {
      return MANT_DIG - TrailingZeroBits(L);
    }
    L = word0(d) | Exp_msk1;
    return MANT_DIG - 32 - TrailingZeroBits(L);
  }

  int
  quorem(Bigint *b, Bigint *S)
  {
    int n;
    uint32_t *bx, *bxe, q, *sx, *sxe;
    uint64_t borrow, carry, y, ys;

    n = S->wds;
    if (b->wds < n) {
      return 0;
    }
    sx = S->x;
    sxe = sx + --n;
    bx = b->x;
    bxe = bx + n;
    q = *bxe / (*sxe + 1);
// ensure q <= true quotient
    if (q) {
      borrow = 0;
      carry = 0;
      do {
        ys = *sx++ * static_cast<uint64_t>(q) + carry;
        carry = ys >> 32;
        y = *bx - (ys & 0xffffffffUL) - borrow;
        borrow = y >> 32 & 1UL;
        *bx++ = y & 0xffffffffUL;
      } while(sx <= sxe);
      if (!*bxe) {
        bx = b->x;
        while(--bxe > bx && !*bxe) {
          n -= 1;
        }
        b->wds = n;
      }
    }
    ew::BigNum bn_b(b->wds, b->x);
    ew::BigNum bn_S(S->wds, S->x);
    if (bn_b >= bn_S) {
      q += 1;
      borrow = 0;
      carry = 0;
      bx = b->x;
      sx = S->x;
      do {
        ys = *sx++ + carry;
        carry = ys >> 32;
        y = *bx - (ys & 0xffffffffUL) - borrow;
        borrow = y >> 32 & 1UL;
        *bx++ = y & 0xffffffffUL;
      } while(sx <= sxe);
      bx = b->x;
      bxe = bx + n;
      if (!*bxe) {
        while(--bxe > bx && !*bxe) {
          n -= 1;
        }
        b->wds = n;
      }
    }
    return q;
  }

   static Bigint *
  bitstob(uint32_t *bits, int nbits, int *bbits)
  {
    int i, k;
    Bigint *b;
    uint32_t *be, *x, *x0;

    i = 32;
    k = 0;
    while(i < nbits) {
      i <<= 1;
      k += 1;
    }
    b = Balloc(k);
    be = bits + ((nbits - 1) >> KSHIFT);
    x = x0 = b->x;
    do {
      *x++ = *bits & 0xffffffff;
    } while(++bits <= be);
    i = x - x0;
    while(!x0[--i]) {
      if (!i) {
        b->wds = 0;
        *bbits = 0;
        goto ret;
      }
    }
    b->wds = i + 1;
    *bbits = i*32 + 32 - LeadingZeroBits(b->x[i]);
  ret:
    return b;
  }

  int
  rvOK(dl_union_t *d, int fpi_nbits, int fpi_emin, int fpi_emax, int32_t *exp,
   uint32_t *bits, int exact, int *irv)
  {
    Bigint *bi_b;
    uint32_t carry, inex, lostbits;
    int bdif, e, j, k, k1, nb, rv;

    carry = rv = 0;
    bi_b = d2b(d->d, &e, &bdif);
    bdif -= nb = fpi_nbits;
    e += bdif;
    if (bdif <= 0) {
      if (exact) {
        goto trunc;
      }
      goto ret;
    }
    if (MANT_DIG == nb) {
// not reached
      goto trunc;
    }
    k = bdif - 1;
    if (k < 0) {
// not reached
      goto trunc;
    }
    if (!k) {
// not reached
      if (!exact) {
        goto ret;
      }
      if (bi_b->x[0] & 2) {
// round up 1 bit, then truncate; bdif > 0
        carry = 1;
      }
      goto trunc;
    }
    if (bi_b->x[k>>KSHIFT] & (static_cast<uint32_t>(1) << (k & kmask))) {
// round up 1 bit, then truncate; bdif > 0
      carry = 1;
    }
  trunc:
    inex = lostbits = 0;
    if (bdif > 0) {
      if ( (lostbits = BAnyOn(bi_b, bdif)) !=0) {
        inex = ew::Gdtoa::INEXLO;
      }
      rshift(bi_b, bdif);
      if (carry) {
        inex = ew::Gdtoa::INEXHI;
        ew::BigNum bn_b(bi_b->wds,bi_b->x);
        bn_b += 1;
        Bfree(bi_b);
        bi_b = Bcopy_bn(bn_b);
        if ( (j = nb & kmask) !=0) {
          j = 32 - j;
        }
        if (LeadingZeroBits(bi_b->x[bi_b->wds - 1]) != j) {
// not reached
          if (!lostbits) {
// not reached
            lostbits = bi_b->x[0] & 1;
          }
          rshift(bi_b, 1);
          e += 1;
        }
      }
    } else if (bdif < 0) {
      bi_b = lshift(bi_b, -bdif);
    }
    if (e < fpi_emin) {
// not reached
      k = fpi_emin - e;
      e = fpi_emin;
      if (k > nb) {
        bi_b->wds = inex = 0;
        *irv = ew::Gdtoa::UNDERFLOW | ew::Gdtoa::INEXLO;
      } else {
        k1 = k - 1;
        if (k1 > 0 && !lostbits) {
          lostbits = BAnyOn(bi_b, k1);
        }
        if (!lostbits && !exact) {
          goto ret;
        }
        lostbits |=
         carry = bi_b->x[k1>>KSHIFT] & (1 << (k1 & kmask));
        rshift(bi_b, k);
        *irv = ew::Gdtoa::DENORMAL;
        if (carry) {
          ew::BigNum bn_b(bi_b->wds,bi_b->x);
          bn_b += 1;
          Bfree(bi_b);
          bi_b = Bcopy_bn(bn_b);
          inex = ew::Gdtoa::INEXHI | ew::Gdtoa::UNDERFLOW;
        } else if (lostbits) {
          inex = ew::Gdtoa::INEXLO | ew::Gdtoa::UNDERFLOW;
        }
      }
    } else if (e > fpi_emax) {
// not reached
      e = fpi_emax + 1;
      *irv = ew::Gdtoa::INFINITE | ew::Gdtoa::OVERFLOW | ew::Gdtoa::INEXHI;
      bi_b->wds = inex = 0;
    }
    *exp = e;
    copybits(bits, nb, bi_b);
    *irv |= inex;
    rv = 1;
  ret:
    Bfree(bi_b);
    return rv;
  }

  int
  strtodg
   (const char *s00, char **se, int fpi_nbits, int fpi_emin, int fpi_emax,
   int32_t *exp, uint32_t *bits)
  {
    int abe, abits, asub;
    int bb0, bb2, bb5, bbe, bd2, bd5, bbbits, bs2, c, decpt, denorm;
    int dsign, e, e1, e2, emin, esign, finished, i, inex, irv;
    int j, k, nbits, nd, nd0, nf, nz, nz0, rvbits, rve, rve1, sign;
    int sudden_underflow;
    const char *s, *s0, *s1;
    double adj0, tol;
    int32_t L;
    dl_union_t adj, rv;
    uint32_t y, z;
    Bigint *ab, *bb, *bb1, *bd, *bd0, *bs, *delta, *rvb, *rvb0;

    irv = ew::Gdtoa::ZERO;
    denorm = sign = nz0 = nz = 0;
    rv.d = 0.;
    rvb = 0;
    nbits = fpi_nbits;
    for(s = s00;;s += 1) {
      switch(*s) {
      case '-':
        sign = 1;
// no break
      case '+':
        if (*++s) {
          goto break2;
        }
// no break
      case 0:
        sign = 0;
        irv = ew::Gdtoa::NONUMBER;
        s = s00;
        goto ret;
      case '\t':
      case '\n':
      case '\v':
      case '\f':
      case '\r':
      case ' ':
        continue;
      default:
        goto break2;
      }
    }
  break2:
    if (*s == '0') {
      nz0 = 1;
      while(*++s == '0') ;
      if (!*s) {
        goto ret;
      }
    }
    sudden_underflow = 0;
    s0 = s;
    y = z = 0;
    for(decpt = nd = nf = 0; (c = *s) >= '0' && c <= '9'; nd++, s++) {
      if (nd < 9) {
        y = 10*y + c - '0';
      } else if (nd < 16) {
        z = 10*z + c - '0';
      }
    }
    nd0 = nd;
    if (c == '.') {
      c = *++s;
      decpt = 1;
      if (!nd) {
        for(; c == '0'; c = *++s) {
          nz += 1;
        }
        if (c > '0' && c <= '9') {
          s0 = s;
          nf += nz;
          nz = 0;
          goto have_dig;
        }
        goto dig_done;
      }
      for(; c >= '0' && c <= '9'; c = *++s) {
  have_dig:
        nz += 1;
        if (c -= '0') {
          nf += nz;
          for(i = 1; i < nz; i += 1) {
            if (nd++ < 9) {
              y *= 10;
            } else if (nd <= DBL_DIG + 1) {
              z *= 10;
            }
          }
          if (nd++ < 9) {
            y = 10*y + c;
          } else if (nd <= DBL_DIG + 1) {
            z = 10*z + c;
          }
          nz = 0;
        }
      }
    }
  dig_done:
    e = 0;
    if (c == 'e' || c == 'E') {
      if (!nd && !nz && !nz0) {
        irv = ew::Gdtoa::NONUMBER;
        s = s00;
        goto ret;
      }
      s00 = s;
      esign = 0;
      switch(c = *++s) {
      case '-':
        esign = 1;
      case '+':
        c = *++s;
      }
      if (c >= '0' && c <= '9') {
        while(c == '0') {
          c = *++s;
        }
        if (c > '0' && c <= '9') {
          L = c - '0';
          s1 = s;
          while((c = *++s) >= '0' && c <= '9') {
            L = 10*L + c - '0';
          }
          if (s - s1 > 8 || L > 19999) {
// Avoid confusion from exponents
// so large that e might overflow.
            e = 19999;
// safe for 16 bit ints
          } else {
            e = static_cast<int>(L);
          }
          if (esign) {
            e = -e;
          }
        } else {
          e = 0;
        }
      } else {
        s = s00;
      }
    }
    if (!nd) {
      if (!nz && !nz0) {
// Check for Nan and Infinity
        if (!decpt) {
          switch(c) {
          case 'i':
          case 'I':
            if (match(&s,"nf")) {
              s -= 1;
              if (!match(&s,"inity")) {
                s += 1;
              }
              irv = ew::Gdtoa::INFINITE;
              goto infnanexp;
            }
            break;
          case 'n':
          case 'N':
            if (match(&s, "an")) {
              irv = ew::Gdtoa::NAN;
              *exp = fpi_emax + 1;
              goto infnanexp;
            }
          }
        }
        irv = ew::Gdtoa::NONUMBER;
        s = s00;
      }
      goto ret;
    }

    irv = ew::Gdtoa::NORMAL;
    e1 = e -= nf;

// Now we have nd0 digits, starting at s0, followed by a
// decimal point, followed by nd-nd0 digits.  The number we're
// after is the integer represented by those digits times
// 10**e

    if (!nd0) {
      nd0 = nd;
    }
//XXX  k = nd < DBL_DIG + 1 ? nd : DBL_DIG + 1;
    k = std::min(nd, DBL_DIG + 1);
    rv.d = y;
    if (k > 9) {
      rv.d = tens[k - 9] * rv.d + z;
    }
    bd0 = 0;
    if (nbits <= MANT_DIG && nd <= DBL_DIG) {
      if (!e) {
        if (rvOK(&rv, fpi_nbits, fpi_emin, fpi_emax, exp, bits, 1, &irv)) {
          goto ret;
        }
      } else if (e > 0) {
        if (e <= Ten_pmax) {
          i = fivesbits[e] + mantbits(&rv) <= MANT_DIG;
// rv =
          rv.d *= tens[e];
          if (rvOK(&rv, fpi_nbits, fpi_emin, fpi_emax, exp, bits, i, &irv)) {
            goto ret;
          }
          e1 -= e;
          goto rv_notOK;
        }
        i = DBL_DIG - nd;
        if (e <= Ten_pmax + i) {
// A fancier test would sometimes let us do
// this for larger i values.
          e2 = e - i;
          e1 -= i;
          rv.d *= tens[i];
// rv =
          rv.d *= tens[e2];
          if (rvOK(&rv, fpi_nbits, fpi_emin, fpi_emax, exp, bits, 0, &irv)) {
            goto ret;
          }
          e1 -= e2;
        }
      } else if (e >= -Ten_pmax) {
// rv =
        rv.d /= tens[-e];
        if (rvOK(&rv, fpi_nbits, fpi_emin, fpi_emax, exp, bits, 0, &irv)) {
          goto ret;
        }
        e1 -= e;
      }
    }
  rv_notOK:
    e1 += nd - k;

// Get starting approximation = rv * 10**e1

    e2 = 0;
    if (e1 > 0) {
      if ( (i = e1 & 15) !=0) {
        rv.d *= tens[i];
      }
      if (e1 &= ~15) {
        e1 >>= 4;
        while(e1 >= (1 << (n_bigtens-1))) {
          e2 += ((word0(&rv) & Exp_mask)
           >> Exp_shift1) - Bias;
          word0(&rv) &= ~Exp_mask;
          word0(&rv) |= Bias << Exp_shift1;
          rv.d *= bigtens[n_bigtens-1];
          e1 -= 1 << (n_bigtens-1);
        }
        e2 += ((word0(&rv) & Exp_mask) >> Exp_shift1) - Bias;
        word0(&rv) &= ~Exp_mask;
        word0(&rv) |= Bias << Exp_shift1;
        for(j = 0; e1 > 0; j++, e1 >>= 1) {
          if (e1 & 1) {
            rv.d *= bigtens[j];
          }
        }
      }
    } else if (e1 < 0) {
      e1 = -e1;
      if ( (i = e1 & 15) !=0) {
        rv.d /= tens[i];
      }
      if (e1 &= ~15) {
        e1 >>= 4;
        while(e1 >= (1 << (n_bigtens-1))) {
          e2 += ((word0(&rv) & Exp_mask)
           >> Exp_shift1) - Bias;
          word0(&rv) &= ~Exp_mask;
          word0(&rv) |= Bias << Exp_shift1;
          rv.d *= tinytens[n_bigtens-1];
          e1 -= 1 << (n_bigtens-1);
        }
        e2 += ((word0(&rv) & Exp_mask) >> Exp_shift1) - Bias;
        word0(&rv) &= ~Exp_mask;
        word0(&rv) |= Bias << Exp_shift1;
        for(j = 0; e1 > 0; j++, e1 >>= 1) {
          if (e1 & 1) {
            rv.d *= tinytens[j];
          }
        }
      }
    }
    rvb = d2b(rv.d, &rve, &rvbits);
// rv = rvb * 2^rve
    rve += e2;
    if ((j = rvbits - nbits) > 0) {
      rshift(rvb, j);
      rvbits = nbits;
      rve += j;
    }
    bb0 = 0;
// trailing zero bits in rvb
    e2 = rve + rvbits - nbits;
    if (e2 > fpi_emax + 1) {
      goto huge;
    }
    rve1 = rve + rvbits - nbits;
    if (e2 < (emin = fpi_emin)) {
      denorm = 1;
      j = rve - emin;
      if (j > 0) {
        rvb = lshift(rvb, j);
        rvbits += j;
      } else if (j < 0) {
        rvbits += j;
        if (rvbits <= 0) {
          if (rvbits < -1) {
            rvb->wds = 0;
            rvb->x[0] = 0;
            *exp = emin;
            irv = ew::Gdtoa::UNDERFLOW | ew::Gdtoa::INEXLO;
            goto ret;
          }
          rvb->x[0] = rvb->wds = rvbits = 1;
        } else {
          rshift(rvb, -j);
        }
      }
      rve = rve1 = emin;
    }

// Now the hard part -- adjusting rv to the correct value.

// Put digits into bd: true value = bd * 10^e

    bd0 = s2b(s0, nd0, nd, y, 1);

    for(;;) {
      bd = Balloc(bd0->k);
      Bcopy(bd, bd0);
      bb = Balloc(rvb->k);
      Bcopy(bb, rvb);
      bbbits = rvbits - bb0;
      bbe = rve + bb0;
      {
        ew::BigNum bn_bs(1);
        bs = Bcopy_bn(bn_bs);
      }

      if (e >= 0) {
        bb2 = bb5 = 0;
        bd2 = bd5 = e;
      } else {
        bb2 = bb5 = -e;
        bd2 = bd5 = 0;
      }
      if (bbe >= 0) {
        bb2 += bbe;
      } else {
        bd2 -= bbe;
      }
      bs2 = bb2;
      j = nbits + 1 - bbbits;
      i = bbe + bbbits - nbits;
      if (i < emin) {
// denormal
        j += i - emin;
      }
      bb2 += j;
      bd2 += j;
//XXX    i = bb2 < bd2 ? bb2 : bd2;
      i = std::min(bb2, bd2);
      if (i > bs2) {
        i = bs2;
      }
      if (i > 0) {
        bb2 -= i;
        bd2 -= i;
        bs2 -= i;
      }
      if (bb5 > 0) {
        bs = pow5mult(bs, bb5);
        bb1 = mult(bs, bb);
        Bfree(bb);
        bb = bb1;
      }
      bb2 -= bb0;
      if (bb2 > 0) {
        bb = lshift(bb, bb2);
      } else if (bb2 < 0) {
        rshift(bb, -bb2);
      }
      if (bd5 > 0) {
        bd = pow5mult(bd, bd5);
      }
      if (bd2 > 0) {
        bd = lshift(bd, bd2);
      }
      if (bs2 > 0) {
        bs = lshift(bs, bs2);
      }
      asub = 1;
      inex = ew::Gdtoa::INEXHI;
      delta = diff(bb, bd);
      if (delta->wds <= 1 && !delta->x[0]) {
        break;
      }
      ew::BigNum bn_bb(bb->wds, bb->x);
      ew::BigNum bn_bd(bd->wds, bd->x);
      dsign = (bn_bb < bn_bd);
      finished = 0;
      L = 0;
      ew::BigNum bn_delta(delta->wds, delta->x);
      ew::BigNum bn_bs(bs->wds, bs->x);
      i = (bn_delta != bn_bs);
      if (i < 0) {
// Error is less than half an ulp -- check for
// special case of mantissa a power of two.
        if (dsign) {
          irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXLO;
        } else {
          irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXHI;
        }
        if (dsign || bbbits > 1 || denorm || rve1 == emin) {
          break;
        }
        delta = lshift(delta,1);
        ew::BigNum bn_delta(delta->wds, delta->x);
        ew::BigNum bn_bs(bs->wds, bs->x);
        if (bn_delta > bn_bs) {
          irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXLO;
          goto drop_down;
        }
        break;
      }
      if (i == 0) {
// exactly half-way between
//XXX std::printf("@@@ %s @@@ %d %d\n", s00, dsign, denorm);
        if (dsign) {
          if (denorm && AllOnesN(rvb, rvbits)) {
// boundary case -- increment exponent
            rvb->wds = 1;
            rvb->x[0] = 1;
            rve = emin + nbits - (rvbits = 1);
            irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXHI;
            denorm = 0;
            break;
          }
          irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXLO;
        } else if (bbbits == 1) {
          irv = ew::Gdtoa::NORMAL;
  drop_down:
// boundary case -- decrement exponent
          if (rve1 == emin) {
            irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXHI;
            if (rvb->wds == 1 && rvb->x[0] == 1) {
              sudden_underflow = 1;
            }
            break;
          }
          rve -= nbits;
          rvb = set_ones(rvb, rvbits = nbits);
          break;
        } else {
          irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXHI;
        }
        if ((bbbits < nbits && !denorm) || !(rvb->x[0] & 1)) {
          break;
        }
        if (dsign) {
// XXX not reached
          ew::BigNum bn_rvb(rvb->wds,rvb->x);
          bn_rvb += 1;
          Bfree(rvb);
          rvb = Bcopy_bn(bn_rvb);
          j = kmask & (32 - (rvbits & kmask));
          if (LeadingZeroBits(rvb->x[rvb->wds - 1]) != j) {
            rvbits += 1;
          }
          irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXHI;
        } else {
          if (bbbits == 1) {
            goto undfl;
          }
// unreached
          ew::BigNum bn_rvb(rvb->wds,rvb->x);
          bn_rvb -= 1;
          Bfree(rvb);
          rvb = Bcopy_bn(bn_rvb);
          irv = ew::Gdtoa::NORMAL | ew::Gdtoa::INEXLO;
        }
        break;
      }
      if ((adj.d = ratio(delta, bs)) <= 2.) {
        inex = ew::Gdtoa::INEXLO;
        if (dsign) {
          asub = 0;
          inex = ew::Gdtoa::INEXHI;
        } else if (denorm && bbbits <= 1) {
  undfl:
          rvb->wds = 0;
          rve = emin;
          irv = ew::Gdtoa::UNDERFLOW | ew::Gdtoa::INEXLO;
          break;
        }
        adj0 = adj.d = 1.;
      } else {
        adj0 = adj.d *= 0.5;
        if (dsign) {
          asub = 0;
          inex = ew::Gdtoa::INEXLO;
        }
        if (adj.d < 2147483647.) {
          L = static_cast<int32_t>(adj0);
          adj0 -= L;
          if (adj0 >= .5) {
            L += 1;
            inex = ew::Gdtoa::INEXACT - inex;
          }
          adj.d = L;
        }
      }
      y = rve + rvbits;

// adj *= ulp(rv.d);
// if (asub) rv -= adj; else rv += adj;

      if (!denorm && rvbits < nbits) {
        rvb = lshift(rvb, j = nbits - rvbits);
        rve -= j;
        rvbits = nbits;
      }
      ab = d2b(adj.d, &abe, &abits);
      if (abe < 0) {
        rshift(ab, -abe);
      } else if (abe > 0) {
        ab = lshift(ab, abe);
      }
      rvb0 = rvb;
      if (asub) {
// rv -= adj;
        j = LeadingZeroBits(rvb->x[rvb->wds-1]);
        rvb = diff(rvb, ab);
        k = rvb0->wds - 1;
        if (denorm) {
        } else if (rvb->wds <= k
         || LeadingZeroBits( rvb->x[k]) >
         LeadingZeroBits(rvb0->x[k])) {
// unlikely; can only have lost 1 high bit
          if (rve1 == emin) {
            rvbits -= 1;
            denorm = 1;
          } else {
            rvb = lshift(rvb, 1);
            rve -= 1;
            rve1 -= 1;
            L = finished = 0;
          }
        }
      } else {
        rvb = sum(rvb, ab);
        k = rvb->wds - 1;
        if (k >= rvb0->wds
         || LeadingZeroBits(rvb->x[k]) < LeadingZeroBits(rvb0->x[k])) {
          if (denorm) {
            if (++rvbits == nbits) {
              denorm = 0;
            }
          } else {
            rshift(rvb, 1);
            rve += 1;
            rve1 += 1;
            L = 0;
          }
        }
      }
      Bfree(ab);
      Bfree(rvb0);
      if (finished) {
        break;
      }

      z = rve + rvbits;
      if (y == z && L) {
// Can we stop now?
        tol = adj.d * 5e-16;
// > max rel error
        adj.d = adj0 - .5;
        if (adj.d < -tol) {
          if (adj0 > tol) {
            irv |= inex;
            break;
          }
        } else if (adj.d > tol && adj0 < 1. - tol) {
          irv |= inex;
          break;
        }
      }
      if (denorm) {
        bb0 = 0;
      } else {
        bb0 = trailz(rvb);
      }
      Bfree(bb);
      Bfree(bd);
      Bfree(bs);
      Bfree(delta);
    }
    if (!denorm && (j = nbits - rvbits)) {
      if (j > 0) {
        rvb = lshift(rvb, j);
      } else {
        rshift(rvb, -j);
      }
      rve -= j;
    }
    *exp = rve;
    Bfree(bb);
    Bfree(bd);
    Bfree(bs);
    Bfree(bd0);
    Bfree(delta);
    if (rve > fpi_emax) {
  huge:
      rvb->wds = 0;
      irv = ew::Gdtoa::INFINITE | ew::Gdtoa::OVERFLOW | ew::Gdtoa::INEXHI;
  infnanexp:
      *exp = fpi_emax + 1;
    }
  ret:
    if (denorm) {
      if (sudden_underflow) {
        rvb->wds = 0;
        irv = ew::Gdtoa::UNDERFLOW | ew::Gdtoa::INEXLO;
      } else {
        if (rvb->wds > 0) {
          irv = (irv & ~ew::Gdtoa::RETMASK) | ew::Gdtoa::DENORMAL;
        } else {
          irv = (irv & ~ew::Gdtoa::RETMASK) | ew::Gdtoa::ZERO;
        }
        if (irv & ew::Gdtoa::INEXACT) {
          irv |= ew::Gdtoa::UNDERFLOW;
        }
      }
    }
    if (se) {
      *se = const_cast<char *>(s);
    }
    if (sign) {
      irv |= ew::Gdtoa::NEG;
    }
    if (rvb) {
      copybits(bits, nbits, rvb);
      Bfree(rvb);
    }
    return irv;
  }

//XXX check
  void
  ULtod(uint32_t L[2], uint32_t bits[2], int32_t exp, int fptype)
  {
    switch(fptype & ew::Gdtoa::RETMASK) {
    case ew::Gdtoa::NONUMBER:
    case ew::Gdtoa::ZERO:
      L[LWORD] = L[HWORD] = 0;
      break;
    case ew::Gdtoa::DENORMAL:
      L[LWORD] = bits[0];
      L[HWORD] = bits[1];
      break;
    case ew::Gdtoa::NORMAL:
    case ew::Gdtoa::NANBITS:
      L[LWORD] = bits[0];
      L[HWORD] = ((bits[1] & ~0x100000) | ((exp + 0x3ff + 52) << 20));
      break;
    case ew::Gdtoa::INFINITE:
      L[LWORD] = 0;
      L[HWORD] = 0x7ff00000;
      break;
    case ew::Gdtoa::NAN:
      L[LWORD] = 0;
      L[HWORD] = 0x7ff80000;
      break;
    }
    if (fptype & ew::Gdtoa::NEG) {
      L[HWORD] |= 0x80000000L;
    }
  }

//XXX check
  void
  ULtof(uint32_t L[1], uint32_t bits[1], int32_t exp, int fptype)
  {
    switch(fptype & ew::Gdtoa::RETMASK) {
    case ew::Gdtoa::NONUMBER:
    case ew::Gdtoa::ZERO:
      L[0] = 0;
      break;
    case ew::Gdtoa::NORMAL:
    case ew::Gdtoa::NANBITS:
      L[0] = (bits[0] & 0x7fffff) | ((exp + 0x7f + 23) << 23);
      break;
    case ew::Gdtoa::DENORMAL:
      L[0] = bits[0];
      break;
    case ew::Gdtoa::INFINITE:
      L[0] = 0x7f800000;
      break;
    case ew::Gdtoa::NAN:
      L[0] = 0x7fc00000;
      break;
    }
    if (fptype & ew::Gdtoa::NEG) {
      L[0] |= 0x80000000L;
    }
  }

// dtoa for IEEE arithmetic (dmg): convert double to ASCII string.
//
// Inspired by "How to Print Floating-Point Numbers Accurately" by
// Guy L. Steele, Jr. and Jon L. White [Proc. ACM SIGPLAN '90, pp. 112-126].
//
// Modifications:
//  1. Rather than iterating, we use a simple numeric overestimate
//     to determine k = floor(log10(d)).  We scale relevant
//     quantities using O(log2(k)) rather than O(k) multiplications.
//  3. Under the assumption that input will be rounded nearest,
//     mode 0 renders 1e23 as 1e23 rather than 9.999999999999999e22.
//     That is, we allow equality in stopping tests when the
//     round-nearest rule will give the same floating-point value
//     as would satisfaction of the stopping test with strict
//     inequality.
//  4. We remove common factors of powers of 2 from relevant
//     quantities.
//  5. When converting floating-point integers less than 1e16,
//     we use floating-point arithmetic rather than resorting
//     to multiple-precision integers.
//  6. When asked to produce fewer than 15 digits, we first try
//     to get by with floating-point arithmetic; we resort to
//     multiple-precision integer arithmetic only if we cannot
//     guarantee that the floating-point calculation has given
//     the correctly rounded result.  For k requested digits and
//     "uniformly" distributed input, the probability is
//     something like 10^(k-15) that we must resort to the Long
//     calculation.

//XXX ?? use double calc for float conversion ??
  void
  gdtoa(char *decimals, int fpi_nbits, int fpi_emin, int fpi_emax, int be,
   uint32_t *bits, int *decpt, char **rve)
  {
// the returned string.  If not null, *rve is set to point
// to the end of the return value.
//  Sufficient space is allocated to the return value
//  to hold the suppressed trailing zeros.

    int bbits, b2, b5, be0, dig, i;
    int j, j1, k, k_check, m2, nbits;
    int s2, s5, spec_case;
    Bigint *b, *b1, *mlo, *mhi, *S;
    double ds;
    char *s, *s0;
    dl_union_t d;

    b = bitstob(bits, nbits = fpi_nbits, &bbits);
    be0 = be;
    i = trailz(b);
    if (i !=0) {
      rshift(b, i);
      be += i;
      bbits -= i;
    }
    if (!b->wds) {
      printf("XXX shouldn't get here\n");
    }

    d.d = b2d(b, &i);
    i = be + bbits - 1;
    word0(&d) &= Frac_mask1;
    word0(&d) |= Exp_11;

// log(x)  ~=~ log(1.5) + (x-1.5)/1.5
// log10(x)   =  log(x) / log(10)
//    ~=~ log(1.5)/log(10) + (x-1.5)/(1.5*log(10))
// log10(&d) = (i-Bias)*log(2)/log(10) + log10(d2)
//
// This suggests computing an approximation k to log10(&d) by
//
// k = (i - Bias)*0.301029995663981
//  + ( (d2-1.5)*0.289529654602168 + 0.176091259055681 );
//
// We want k to be too large rather than too small.
// The error in the first-order Taylor series approximation
// is in our favor, so we just round up the constant enough
// to compensate for any error in the multiplication of
// (i - Bias) by 0.301029995663981; since |i - Bias| <= 1077,
// and 1077 * 0.30103 * 2^-52 ~=~ 7.2e-14,
// adding 1e-13 to the constant term more than suffices.
// Hence we adjust the constant term to 0.1760912590558.
// (We could get a more accurate k by invoking log10,
//  but this is probably not worthwhile.)
    ds = (d.d-1.5)*0.289529654602168 + 0.1760912590558 + i*0.301029995663981;

// correct assumption about exponent range
    if ((j = i) < 0) {
      j = -j;
    }
    if ((j -= 1077) > 0) {
      ds += j * 7e-17;
    }

    k = static_cast<int>(ds);
    if (ds < 0. && ds != k) {
      k -= 1;
    }
// want k = floor(ds)
    k_check = 1;
    word0(&d) += (be + bbits - 1) << Exp_shift;
    if (k >= 0 && k <= Ten_pmax) {
      if (d.d < tens[k]) {
        k -= 1;
      }
      k_check = 0;
    }
    j = bbits - i - 1;
    if (j >= 0) {
      b2 = 0;
      s2 = j;
    } else {
      b2 = -j;
      s2 = 0;
    }
    if (k >= 0) {
      b5 = 0;
      s5 = k;
      s2 += k;
    } else {
      b2 -= k;
      b5 = -k;
      s5 = 0;
    }
    s = s0 = decimals;

// Do we have a "small" integer?

    if (be >= 0 && k <= Int_max) {
// Yes.
      ds = tens[k];
      for(i = 1;; i++, d.d *= 10.) {
        int32_t L = static_cast<int32_t>(d.d / ds);
        d.d -= L*ds;
        *s++ = '0' + static_cast<int>(L);
        if (d.d == 0.) {
          break;
        }
      }
      goto ret1;
    }

    m2 = b2;
    mhi = mlo = 0;
    i = nbits - bbits;
    if (be - i++ < fpi_emin) {
// denormal
      i = be - fpi_emin + 1;
    }
    b2 += i;
    s2 += i;
    {
      ew::BigNum bn_mhi(1);
      mhi = Bcopy_bn(bn_mhi);
    }
    if (m2 > 0 && s2 > 0) {
      i = std::min(m2, s2);
      b2 -= i;
      m2 -= i;
      s2 -= i;
    }
    if (b5 > 0) {
      if (b5 > 0) {
        mhi = pow5mult(mhi, b5);
        b1 = mult(mhi, b);
        Bfree(b);
        b = b1;
      }
    }
    {
      ew::BigNum bn_S(1);
      S = Bcopy_bn(bn_S);
    }
    if (s5 > 0) {
      S = pow5mult(S, s5);
    }

// Check for special case that d is a normalized power of 2.

    spec_case = 0;
    if (bbits == 1 && be0 > fpi_emin + 1) {
// The special case
      b2 += 1;
      s2 += 1;
      spec_case = 1;
    }

// Arrange for convenient computation of quotients:
// shift left if necessary so divisor has 4 leading 0 bits.
//
// Perhaps we should just compute leading 28 bits of S once
// and for all and pass them and a shift to quorem, so it
// can do shifts and ors to compute the numerator for q.
    if (s5) {
      i = ((LeadingZeroBits(S->x[S->wds-1])) - s2 - 4) & kmask;
    } else {
      i = ((32 - 1) - s2 - 4) & kmask;
    }
    m2 += i;
    if ((b2 += i) > 0) {
      b = lshift(b, b2);
    }
    if ((s2 += i) > 0) {
      S = lshift(S, s2);
    }
    if (k_check) {
      ew::BigNum bn_b(b->wds, b->x);
      ew::BigNum bn_S(S->wds, S->x);
      if (bn_b < bn_S) {
        k -= 1;
        b = multadd(b, 10, 0);
// we botched the k estimate
        mhi = multadd(mhi, 10, 0);
      }
    }
    if (m2 > 0) {
      mhi = lshift(mhi, m2);
    }

// Compute mlo -- check for special case
// that d is a normalized power of 2.

    mlo = mhi;
    if (spec_case) {
      mhi = Balloc(mhi->k);
      Bcopy(mhi, mlo);
      mhi = lshift(mhi, 1);
    }

    for(i = 1;;i += 1) {
      dig = quorem(b,S) + '0';
// Do we yet have the shortest decimal string
// that will round to d?
      ew::BigNum bn_b(b->wds, b->x);
      ew::BigNum bn_mlo(mlo->wds, mlo->x);
      j = (bn_b != bn_mlo);
      ew::BigNum bn_S(S->wds, S->x);
      ew::BigNum bn_mhi(mhi->wds, mhi->x);
      if (bn_S < bn_mhi) {
        j1 = 1;
      } else {
        Bigint *delta = diff(S, mhi);
        ew::BigNum bn_b(b->wds, b->x);
        ew::BigNum bn_delta(delta->wds, delta->x);
        j1 = (bn_b != bn_delta);
        Bfree(delta);
      }
      if (j1 == 0 && !(bits[0] & 1)) {
        if (dig == '9') {
          goto round_9_up;
        }
        if (j > 0) {
          dig += 1;
        }
        *s++ = dig;
        goto ret;
      }
      if (j < 0 || (j == 0 && !(bits[0] & 1))) {
        if (j1 > 0) {
          b = lshift(b, 1);
          ew::BigNum bn_b(b->wds, b->x);
          ew::BigNum bn_S(S->wds, S->x);
          j1 = (bn_b != bn_S);
          if ((j1 > 0 || (j1 == 0 && dig & 1))
           && dig++ == '9') {
            goto round_9_up;
          }
        }
        *s++ = dig;
        goto ret;
      }
      if (j1 > 0) {
        if (dig == '9') {
// possible if i == 1
  round_9_up:
          *s++ = '9';
          goto roundoff;
        }
        *s++ = dig + 1;
        goto ret;
      }
      *s++ = dig;
      b = multadd(b, 10, 0);
      if (mlo == mhi) {
        mlo = mhi = multadd(mhi, 10, 0);
      } else {
        mlo = multadd(mlo, 10, 0);
        mhi = multadd(mhi, 10, 0);
      }
    }

// Round off last digit
  roundoff:
    while(*--s == '9') {
      if (s == s0) {
        k += 1;
        *s++ = '1';
        goto ret;
      }
    }
    ++*s++;
  ret:
    Bfree(S);
    if (mhi) {
      if (mlo && mlo != mhi) {
        Bfree(mlo);
      }
      Bfree(mhi);
    }
  ret1:
    Bfree(b);
    *s = 0;
    *decpt = k + 1;
    if (rve) {
      *rve = s;
    }
  }

  char *
  g__fmt(char *b, char *s, char *se, int decpt, uint32_t sign, int blen)
  {
    int i, j, k;
    char *be;
    int len;
    len = (se-s) + 6;
// 6 = sign + e+dd + trailing null
    if (blen < len) {
      goto ret0;
    }
    be = b + blen - 1;
    if (sign) {
      *b++ = '-';
    }
    if (decpt <= -4 || decpt > se - s + 5) {
      *b++ = *s++;
      if (*s) {
        *b++ = '.';
        while((*b = *s++) !=0) {
          b += 1;
        }
      }
      *b++ = 'e';
// sprintf(b, "%+.2d", decpt - 1);
      if (--decpt < 0) {
        *b++ = '-';
        decpt = -decpt;
      } else {
        *b++ = '+';
      }
      for(j = 2, k = 10; 10*k <= decpt; j++, k *= 10){
      }
      for(;;) {
        i = decpt / k;
        if (b >= be) {
          goto ret0;
        }
        *b++ = i + '0';
        if (--j <= 0) {
          break;
        }
        decpt -= i*k;
        decpt *= 10;
      }
      *b = 0;
    } else if (decpt <= 0) {
      *b++ = '.';
      if (be < b - decpt + (se - s)) {
        goto ret0;
      }
      for(; decpt < 0; decpt += 1) {
        *b++ = '0';
      }
      while((*b = *s++) != 0) {
        b += 1;
      }
    } else {
      while((*b = *s++) != 0) {
        b += 1;
        if (--decpt == 0 && *s) {
          *b++ = '.';
        }
      }
      if (b + decpt > be) {
  ret0:
        b = 0;
        goto ret;
      }
      for(; decpt > 0; decpt -= 1) {
        *b++ = '0';
      }
      *b = 0;
    }
  ret:
    return b;
  }

}

int
ew::Gdtoa::strtopf(const char *s, char **sp, float *f)
{
  uint32_t bits[1];
  int32_t exp;
  int fptype;
  union {
    float f;
    uint32_t L[1];
  } un;

  fptype = strtodg(s, sp, FPI_F_NBITS, FPI_F_EMIN, FPI_F_EMAX, &exp, bits);
  ULtof(un.L, bits, exp, fptype);
  *f = un.f;
  return fptype;
}

int
ew::Gdtoa::strtopd(const char *s, char **sp, double *d)
{
  uint32_t bits[2];
  int32_t exp;
  int fptype;
  union {
    double d;
    uint32_t L[2];
  } un;

  fptype = strtodg(s, sp, FPI_D_NBITS, FPI_D_EMIN, FPI_D_EMAX, &exp, bits);
  ULtod(un.L, bits, exp, fptype);
  *d = un.d;
  return fptype;
}

char*
ew::Gdtoa::dfmt(char *buf, double *d, int ndig, int bufsize)
{
  char *b, *se, decimals[18];
  uint32_t bits[2], *L, sign;
  int decpt, ex;

  if (bufsize < 10) {
    return 0;
  }

  L = reinterpret_cast<uint32_t *>(d);
  sign = L[HWORD] & 0x80000000L;
  if ((L[HWORD] & 0x7ff00000) == 0x7ff00000) {
// Infinity or NaN
    if (bufsize < 10) {
      return 0;
    }
    if (L[HWORD] & 0xfffff || L[LWORD]) {
      std::strcpy(buf, "NaN");
      return buf + 3;
    }
    b = buf;
    if (sign) {
      *b++ = '-';
    }
    std::strcpy(b, "Infinity");
    return b + 8;
  }
  if (L[LWORD] == 0 && (L[HWORD] ^ sign) == 0) {
// d == 0.
    b = buf;
    if (L[HWORD] & 0x80000000L) {
      *b++ = '-';
    }
    *b++ = '0';
    *b = 0;
    return b;
  }
  bits[0] = L[LWORD];
  bits[1] = L[HWORD] & 0xfffff;
  if ( (ex = (L[HWORD] >> 20) & 0x7ff) !=0) {
    bits[1] |= 0x100000;
  } else {
    ex = 1;
  }
  ex -= 0x3ff + 52;
  gdtoa(decimals, FPI_D_NBITS, FPI_D_EMIN, FPI_D_EMAX, ex, bits, &decpt, &se);
  return g__fmt(buf, decimals, se, decpt, sign, bufsize);
}

char *
ew::Gdtoa::ffmt(char *buf, float *f, int ndig, int bufsize)
{
  char *b, *se, decimals[18];
  uint32_t bits[1], *L, sign;
  int decpt, ex;

  if (bufsize < 10) {
    return 0;
  }

  L = reinterpret_cast<uint32_t *>(f);
  sign = L[0] & 0x80000000L;
  if ((L[0] & 0x7f800000) == 0x7f800000) {
// Infinity or NaN
    if (L[0] & 0x7fffff) {
      std::strcpy(buf, "NaN");
      return buf + 3;
    }
    b = buf;
    if (sign) {
      *b++ = '-';
    }
    std::strcpy(b, "Infinity");
    return b + 8;
  }
  if (*f == 0.) {
    b = buf;
    if (L[0] & 0x80000000L) {
      *b++ = '-';
    }
    *b++ = '0';
    *b = 0;
    return b;
  }
  bits[0] = L[0] & 0x7fffff;
  if ( (ex = (L[0] >> 23) & 0xff) !=0) {
    bits[0] |= 0x800000;
  } else {
    ex = 1;
  }
  ex -= 0x7f + 23;
  if (bufsize < 16) {
    return 0;
  }
  gdtoa(decimals, FPI_F_NBITS, FPI_F_EMIN, FPI_F_EMAX, ex, bits, &decpt, &se);
  return g__fmt(buf, decimals, se, decpt, sign, bufsize);
}
