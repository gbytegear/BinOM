#ifndef ARRAY_IMPL_HXX
#define ARRAY_IMPL_HXX

#include "../../variables/variable.hxx"

namespace binom::priv {

class ArrayImplementation {
public:
  typedef Variable* Iterator;
  typedef const Variable* ConstIterator;
  typedef reverse_iterator::ReverseIterator<Variable*> ReverseIterator;
  typedef const reverse_iterator::ReverseIterator<Variable*> ConstReverseIterator;
private:
  size_t count;
  size_t capacity;

  ArrayImplementation(const literals::arr& value_list);
  ArrayImplementation(const ArrayImplementation& other);

  static size_t calculateCapacity(size_t count) noexcept;

  static Iterator increaseSize(ArrayImplementation*& implementation, size_t count);
  static void reduceSize(ArrayImplementation*& implementation, size_t count);
  static Iterator insert(ArrayImplementation*& implementation, size_t at, size_t count);

public:

  static ArrayImplementation* create(const literals::arr& value_list);
  static ArrayImplementation* copy(const ArrayImplementation* other);

  size_t getElementCount() const noexcept;
  size_t getCapacity() const noexcept;
  size_t getSize() const noexcept;

  Variable* getData() const;

  static Variable pushBack(ArrayImplementation*& implementation, Variable variable);
  static Iterator pushBack(ArrayImplementation*& implementation, const literals::arr variable_list);

  static Variable pushFront(ArrayImplementation*& implementation, Variable variable);
  static Iterator pushFront(ArrayImplementation*& implementation, const literals::arr& variable_list);

  static Variable insert(ArrayImplementation*& implementation, size_t at, Variable variable);
  static Iterator insert(ArrayImplementation*& implementation, size_t at, const literals::arr& variable_list);

  static void popBack(ArrayImplementation*& implementation, size_t count = 1);
  static void popFront(ArrayImplementation*& implementation, size_t count = 1);

  static void remove(ArrayImplementation*& implementation, size_t at, size_t count);
  static void clear(ArrayImplementation*& implementation);

  Variable operator[](size_t index);

  Iterator begin();
  Iterator end();

  ReverseIterator rbegin();
  ReverseIterator rend();

  inline ConstIterator begin() const {return cbegin();}
  inline ConstIterator end() const {return cend();}

  inline ConstReverseIterator rbegin() const {return crbegin();}
  inline ConstReverseIterator rend() const {return crend();}

  ConstIterator cbegin() const;
  ConstIterator cend() const;
  ConstReverseIterator crbegin() const;
  ConstReverseIterator crend() const;

  void operator delete(void* ptr);
};


}

#endif // ARRAY_IMPL_HXX
