#ifndef PSEUDO_POINTER_HXX
#define PSEUDO_POINTER_HXX

namespace pseudo_ptr {

template<typename T>
class PseudoPointer{
  T value;
public:
  PseudoPointer(const T& value) : value(const_cast<T&>(value)) {}
  PseudoPointer(T&& value) : value(value) {}
  T& operator*() noexcept {return value;}
  const T& operator*() const noexcept {return value;}
  T* operator->() noexcept {return &value;}
  const T* operator->() const noexcept {return &value;}
};

}

#endif // PSEUDO_POINTER_HXX
