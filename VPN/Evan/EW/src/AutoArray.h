#ifndef EW_AUTOARRAY_H_INCLUDED
#define EW_AUTOARRAY_H_INCLUDED

// wdkg 2010

// The class is not part of the library interface.

// This class is implemented entirely in this header.

// Partial adaption of auto_ptr for arrays.
// This is for where uninitialized arrays of basic types are required.
// In this situation, std::vector does have a measurable overhead.

namespace ew {

  template<typename T>
  class AutoArray {
  public:
    inline AutoArray();
    explicit inline AutoArray(int n);
    inline ~AutoArray();
    inline operator T *();
    inline void resize(int n);
  private:
    T *ptr;
// undefined
    AutoArray(const AutoArray &that);
    AutoArray &operator=(const AutoArray &that);
  };

}

template<typename T>
inline
ew::AutoArray<T>::AutoArray() :
 ptr(0)
{
}

template<typename T>
inline
ew::AutoArray<T>::AutoArray(int n)
// ptr uninitialised
{
  if (n == 0) {
    ptr = 0;
  } else {
    ptr = new T[n];
  }
}

template<typename T>
inline
ew::AutoArray<T>::~AutoArray()
{
  delete[] ptr;
}

template<typename T>
inline
ew::AutoArray<T>::operator T *()
{
  return ptr;
}

template<typename T>
inline void
ew::AutoArray<T>::resize(int n)
{
  delete[] ptr;
  if (n > 0) {
    ptr = new T[n];
  } else {
    ptr = 0;
  }
}

#endif
