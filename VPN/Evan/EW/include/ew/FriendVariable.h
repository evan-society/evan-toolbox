#ifndef EW_FRIENDVARIABLE_H_INCLUDED
#define EW_FRIENDVARIABLE_H_INCLUDED

// wdkg 2010

// This header file is an internal part of the library.
// This class is not part of the library interface.

// This is used for private member variables of classes in the interface.

// This class is implemented entirely in this header.

// This class is a device to allow other classes access to a specific member
// variable.
// This is like a "friend" declaration that applies to only one member variable.

// The current standard disallows "friend class T" or "friend T" in a template
// definition.
// In c++0x, making a friend of a template parameter will be allowed.
// Until then, use template specializations.

namespace ew {
  template<typename T1, typename T2, typename T3 = T2>
  class FriendVariable {
  public:
    explicit inline FriendVariable(const T1 &that) {v = that;}
  private:
    T1 v;
    inline operator T1 &() {return v;}
    inline T1 &operator=(const T1 &that) {v = that; return v;}
// undefined
    FriendVariable();
    FriendVariable(const ew::FriendVariable<T1, T2, T3> &that);
    FriendVariable &operator=(const ew::FriendVariable<T1, T2, T3> &that);
  };

  class DataflowNetwork;
  class DataflowNode;
  class DataflowNotifier;
  class View3Curve;
  class View3Item;
  class View3Surface;
  class View3Widget;

  template<typename T1>
  class FriendVariable<T1, ew::DataflowNetwork, ew::DataflowNotifier> {
  friend class ew::DataflowNetwork;
  friend class ew::DataflowNotifier;
  public:
    explicit inline FriendVariable(const T1 &that) {v = that;}
  private:
    T1 v;
    inline operator T1 &() {return v;}
    inline T1 &operator=(const T1 &that) {v = that; return v;}
// undefined
    FriendVariable();
    FriendVariable(const ew::FriendVariable<T1, ew::DataflowNetwork,
     ew::DataflowNotifier> &that);
    FriendVariable &operator=(const ew::FriendVariable<T1, ew::DataflowNetwork,
     ew::DataflowNotifier> &that);
  };

  template<typename T1>
  class FriendVariable<T1, ew::DataflowNode> {
  friend class ew::DataflowNode;
  public:
    explicit inline FriendVariable(const T1 &that) {v = that;}
  private:
    T1 v;
    inline operator T1 &() {return v;}
    inline T1 &operator=(const T1 &that) {v = that; return v;}
// undefined
    FriendVariable();
    FriendVariable(const ew::FriendVariable<T1, ew::DataflowNode> &that);
    FriendVariable &operator=(const ew::FriendVariable<T1, ew::DataflowNode>
     &that);
  };

  template<typename T1>
  class FriendVariable<T1, ew::View3Curve> {
  friend class ew::View3Curve;
  public:
    explicit inline FriendVariable(const T1 &that) {v = that;}
  private:
    T1 v;
    inline operator T1 &() {return v;}
    inline T1 &operator=(const T1 &that) {v = that; return v;}
// undefined
    FriendVariable();
    FriendVariable(const ew::FriendVariable<T1, ew::View3Curve> &that);
    FriendVariable &operator=(const ew::FriendVariable<T1, ew::View3Curve>
     &that);
  };

  template<typename T1>
  class FriendVariable<T1, ew::View3Item> {
  friend class ew::View3Item;
  public:
    explicit inline FriendVariable(const T1 &that) {v = that;}
  private:
    T1 v;
    inline operator T1 &() {return v;}
    inline T1 &operator=(const T1 &that) {v = that; return v;}
// undefined
    FriendVariable();
    FriendVariable(const ew::FriendVariable<T1, ew::View3Item> &that);
    FriendVariable &operator=(const ew::FriendVariable<T1, ew::View3Item>
     &that);
  };

  template<typename T1>
  class FriendVariable<T1, ew::View3Surface> {
  friend class ew::View3Surface;
  public:
    explicit inline FriendVariable(const T1 &that) {v = that;}
  private:
    T1 v;
    inline operator T1 &() {return v;}
    inline T1 &operator=(const T1 &that) {v = that; return v;}
// undefined
    FriendVariable();
    FriendVariable(const ew::FriendVariable<T1, ew::View3Surface> &that);
    FriendVariable &operator=(const ew::FriendVariable<T1, ew::View3Surface>
     &that);
  };

  template<typename T1>
  class FriendVariable<T1, ew::View3Widget> {
  friend class ew::View3Widget;
  public:
    explicit inline FriendVariable(const T1 &that) {v = that;}
  private:
    T1 v;
    inline operator T1 &() {return v;}
    inline T1 &operator=(const T1 &that) {v = that; return v;}
// undefined
    FriendVariable();
    FriendVariable(const ew::FriendVariable<T1, ew::View3Widget> &that);
    FriendVariable &operator=(const ew::FriendVariable<T1, ew::View3Widget>
     &that);
  };
}

#endif
