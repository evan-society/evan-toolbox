#ifndef EW_GDTOA_H_INCLUDED
#define EW_GDTOA_H_INCLUDED

// David Gay's floating point conversion library (partial, modified).

namespace ew {

  class Gdtoa {
  public:
    enum {
      ZERO = 0,
      NORMAL = 1,
      DENORMAL = 2,
      INFINITE = 3,
      NAN = 4,
      NANBITS = 5,
      NONUMBER = 6,
      RETMASK = 7,
      NEG = 0x08,
      INEXLO = 0x10,
      INEXHI = 0x20,
      INEXACT = 0x30,
      UNDERFLOW = 0x40,
      OVERFLOW = 0x80
    };
    static char* dfmt(char*, double*, int, int);
    static char* ffmt(char*, float*,  int, int);
    static int strtopd(const char*, char**, double *d);
    static int strtopf(const char*, char**, float *f);
  private:
// undefined
    Gdtoa();
  };

}

#endif
