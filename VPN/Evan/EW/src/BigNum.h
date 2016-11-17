#ifndef EW_BIGNUM_H_INCLUDED
#define EW_BIGNUM_H_INCLUDED

// wdkg 2010

// This class is implemented entirely in this header.

// Simple arbitrary precision numbers, to support double decimal/binary
// conversions.

// nwords() is the number of uint32_t digits, excluding leading zeros, except
// if the BigNum == 0, in which case it is 1.
// It is a precondition of operator-= that (*this >= that).
// operator!= is non-standard, it returns an int: -1 if *this < that, +1 if
// *this > that, like std::strcmp.

#include <cstring>
#include <stdint.h>

namespace ew {
  class BigNum {
  public:
    inline explicit BigNum(uint32_t n = 0);
    inline explicit BigNum(const ew::BigNum &that);
//XXX temporary
    inline BigNum(int n, const uint32_t *a);
    inline ~BigNum();
    inline void reserve(int n);
    inline ew::BigNum &operator=(const uint32_t &that);
    inline ew::BigNum &operator=(const ew::BigNum &that);
    inline ew::BigNum &operator+=(const uint32_t &that);
//XXX    inline ew::BigNum &operator+=(const ew::BigNum &that);
    inline ew::BigNum &operator-=(const uint32_t &that);
//XXX    inline ew::BigNum &operator-=(const ew::BigNum &that);
    inline ew::BigNum &operator<<=(const uint32_t &that);
    inline int operator!=(const uint32_t &that) const;
    inline bool operator==(const uint32_t &that) const;
    inline bool operator<(const uint32_t &that) const;
    inline bool operator>(const uint32_t &that) const;
    inline bool operator>=(const uint32_t &that) const;
    inline bool operator<=(const uint32_t &that) const;
    inline int operator!=(const ew::BigNum &that) const;
    inline bool operator==(const ew::BigNum &that) const;
    inline bool operator<(const ew::BigNum &that) const;
    inline bool operator>(const ew::BigNum &that) const;
    inline bool operator>=(const ew::BigNum &that) const;
    inline bool operator<=(const ew::BigNum &that) const;
    inline int nwords() const;
    inline const uint32_t *words() const;
  private:
//XXX tune
    static const int STATIC_ALLOC = 4;
    uint32_t static_space[STATIC_ALLOC];
    uint32_t *digits;
    int adigits;
    int ndigits;
  };
}

inline
ew::BigNum::BigNum(uint32_t n)
{
  adigits = STATIC_ALLOC;
  digits = static_space;
  ndigits = 1;
  digits[0] = n;
}

inline
ew::BigNum::BigNum(const ew::BigNum &that)
{
  if (that.ndigits <= STATIC_ALLOC) {
    digits = static_space;
    adigits = STATIC_ALLOC;
  } else {
    adigits = STATIC_ALLOC;
    while (adigits < that.ndigits) {
      adigits *= 2;
    }
    digits = new uint32_t[adigits];
  }
  ndigits = that.ndigits;
  std::memcpy(digits, that.digits, ndigits * sizeof(uint32_t));
}

inline
ew::BigNum::BigNum(int n, const uint32_t *a)
{
  while (n > 0 && a[n - 1] == 0) {
    n -= 1;
  }
  if (n <= STATIC_ALLOC) {
    digits = static_space;
    adigits = STATIC_ALLOC;
  } else {
    adigits = STATIC_ALLOC;
    while (adigits < n) {
      adigits *= 2;
    }
    digits = new uint32_t[adigits];
  }
  ndigits = n;
  if (n == 0) {
    digits[0] = 0;
  } else {
    std::memcpy(digits, a, ndigits * sizeof(uint32_t));
  }
}

inline
ew::BigNum::~BigNum()
{
  if (adigits != STATIC_ALLOC) {
    delete[] digits;
  }
}

inline void
ew::BigNum::reserve(int n)
{
  if (n <= adigits) {
    return;
  }
  while (adigits < n) {
    adigits *= 2;
  }
  uint32_t *d = new uint32_t[adigits];
  std::memcpy(d, digits, ndigits * sizeof(uint32_t));
  if (digits != static_space) {
    delete[] digits;
  }
  digits = d;
}

inline ew::BigNum &
ew::BigNum::operator=(const uint32_t &that)
{
  ndigits = 1;
  digits[0] = that;
  return *this;
}

inline ew::BigNum &
ew::BigNum::operator=(const ew::BigNum &that)
{
  if (this == &that) {
    return *this;
  }
  if (adigits < that.ndigits) {
    while (adigits < that.ndigits) {
      adigits *= 2;
    }
    uint32_t *d = new uint32_t[adigits];
    delete[] digits;
    digits = d;
  }
  ndigits = that.ndigits;
  std::memcpy(digits, that.digits, ndigits * sizeof(uint32_t));
  return *this;
}

inline ew::BigNum &
ew::BigNum::operator+=(const uint32_t &that)
{
  digits[0] += that;
  if (digits[0] < that) {
    for (int i = 1; i < ndigits; i += 1) {
      digits[i] += 1;
      if (digits[i] != 0) {
        return *this;
      }
    }
    reserve(ndigits + 1);
    digits[ndigits] = 1;
    ndigits += 1;
  }
  return *this;
}

#ifdef XXX
inline ew::BigNum &
ew::BigNum::operator+=(const ew::BigNum &that)
{

  return *this;
}
#endif

inline ew::BigNum &
ew::BigNum::operator-=(const uint32_t &that)
{
  if (digits[0] >= that) {
    digits[0] -= that;
  } else {
    digits[0] -= that;
    for (int i = 1; i < ndigits; i += 1) {
      if (digits[i] != 0) {
        digits[i] -= 1;
        break;
      }
      digits[i] -= 1;
    }
    while (ndigits > 1 && digits[ndigits - 1] == 0) {
      ndigits -= 1;
    }
  }
  return *this;
}

#ifdef XXX
inline void
ew::BigNum::add(const ew::BigNum *n)
{
  uint32_t carry = 0;
  if (ndigits >= n->ndigits) {
    for (int i = 0; i < n->ndigits; i += 1) {
      uint64_t s = static_cast<uint64_t>(digits[i]) +
       static_cast<uint64_t>(n->digits[i]) + carry;
      digits[i] = static_cast<uint32_t>(s);
      carry = static_cast<uint32_t>(s >> 32);
    }
    for (int i = n->ndigits; i < ndigits; i += 1) {
      if (carry == 0) {
        return;
      }
      uint64_t s = static_cast<uint64_t>(digits[i]) + carry;
      digits[i] = static_cast<uint32_t>(s);
      carry = static_cast<uint32_t>(s >> 32);
    }
  } else {
    for (int i = 0; i < ndigits; i += 1) {
      uint64_t s = static_cast<uint64_t>(digits[i]) +
       static_cast<uint64_t>(n->digits[i]) + carry;
      digits[i] = static_cast<uint32_t>(s);
      carry = static_cast<uint32_t>(s >> 32);
    }
// Over allocate to prevent re-allocation below.
    alloc(n->ndigits + 1);
    for (int i = ndigits; i < n->ndigits; i += 1) {
      uint64_t s = static_cast<uint64_t>(n->digits[i]) + carry;
      digits[i] = static_cast<uint32_t>(s);
      carry = static_cast<uint32_t>(s >> 32);
    }
    ndigits = n->ndigits;
  }
  if (carry == 0) {
    return;
  }
  reserve(ndigits + 1);
  digits[ndigits] = carry;
  ndigits += 1;
}
#endif

inline ew::BigNum &
ew::BigNum::operator<<=(const uint32_t &that)
{
	//!? 
	return *this;
}

inline int
ew::BigNum::operator!=(const uint32_t &that) const
{
  if (ndigits != 1) {
    return 1;
  }
  if (digits[0] == that) {
    return 0;
  } else if (digits[0] < that) {
    return -1;
  } else {
    return 1;
  }
}

inline bool
ew::BigNum::operator==(const uint32_t &that) const
{
  return ((*this != that) == 0);
}

inline bool
ew::BigNum::operator<(const uint32_t &that) const
{
  return ((*this != that) < 0);
}

inline bool
ew::BigNum::operator>(const uint32_t &that) const
{
  return ((*this != that) > 0);
}

inline bool
ew::BigNum::operator<=(const uint32_t &that) const
{
  return ((*this != that) <= 0);
}

inline bool
ew::BigNum::operator>=(const uint32_t &that) const
{
  return ((*this != that) >= 0);
}

inline int
ew::BigNum::operator!=(const ew::BigNum &that) const
{
  if (ndigits != that.ndigits) {
    if (ndigits < that.ndigits) {
      return -1;
    } else {
      return 1;
    }
  }
  for (int i = ndigits - 1; i >= 0; i -= 1) {
    if (digits[i] != that.digits[i]) {
      if (digits[i] < that.digits[i]) {
        return -1;
      } else {
        return 1;
      }
    }
  }
  return 0;
}

inline bool
ew::BigNum::operator==(const ew::BigNum &that) const
{
  return ((*this != that) == 0);
}

inline bool
ew::BigNum::operator<(const ew::BigNum &that) const
{
  return ((*this != that) < 0);
}

inline bool
ew::BigNum::operator>(const ew::BigNum &that) const
{
  return ((*this != that) > 0);
}

inline bool
ew::BigNum::operator<=(const ew::BigNum &that) const
{
  return ((*this != that) <= 0);
}

inline bool
ew::BigNum::operator>=(const ew::BigNum &that) const
{
  return ((*this != that) >= 0);
}

inline int
ew::BigNum::nwords() const
{
  return ndigits;
}

inline const uint32_t *
ew::BigNum::words() const
{
  return digits;
}

#endif
