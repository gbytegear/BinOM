#ifndef PSEUDO_POINTER_HXX
#define PSEUDO_POINTER_HXX

namespace pseudo_ptr {

template<typename T>
struct PseudoPointer{
  T value;
  T& operator*() noexcept {return value;}
  const T& operator*() const noexcept {return value;}
  T* operator->() noexcept {return &value;}
  const T* operator->() const noexcept {return &value;}
};

}

#endif // PSEUDO_POINTER_HXX
