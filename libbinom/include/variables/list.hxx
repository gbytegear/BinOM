#ifndef DOUBLY_LINKED_LIST_HXX
#define DOUBLY_LINKED_LIST_HXX

#include "variable.hxx"
#include "../binom_impl/ram_storage_implementation/list_impl.hxx"

namespace binom {

class List : public Variable {
  operator Number& () = delete;
  operator BitArray& () = delete;
  operator BufferArray& () = delete;
  operator Array& () = delete;
  operator Map& () = delete;
  operator MultiMap& () = delete;

  Number& toNumber() = delete;
  BitArray& toBitArray() = delete;
  BufferArray& toBufferArray() = delete;
  Array& toArray() = delete;
  List& toList() = delete;
  Map& toMap() = delete;
  MultiMap& toMultiMap() = delete;

  operator const Number& () const = delete;
  operator const BitArray& () const = delete;
  operator const BufferArray& () const = delete;
  operator const Array& () const = delete;
  operator const Map& () const = delete;
  operator const MultiMap& () const = delete;

  const Number& toNumber() const = delete;
  const BitArray& toBitArray() const = delete;
  const BufferArray& toBufferArray() const = delete;
  const Array& toArray() const = delete;
  const Map& toMap() const = delete;
  const MultiMap& toMultiMap() const = delete;

  Variable& operator=(const Variable& other) = delete;
  Variable& operator=(Variable&& other) = delete;

  Variable& changeLink(const Variable& other) = delete;
  Variable& changeLink(Variable&& other) = delete;

  priv::ListImplementation*& getData() const noexcept;

  friend class Variable;
  List(priv::Link&& link);
public:
  typedef priv::ListImplementation::Iterator Iterator;
  typedef priv::ListImplementation::ReverseIterator ReverseIterator;
  typedef priv::ListImplementation::ConstIterator ConstIterator;
  typedef priv::ListImplementation::ConstReverseIterator ConstReverseIterator;

  List();
  List(const literals::list doubly_linked_list);
  List(const List& other) noexcept;
  List(const List&& other) noexcept;

  bool isEmpty() const;
  size_t getElementCount() const;

  List move() noexcept;
  const List move() const noexcept;

  Variable pushBack(Variable variable);
  Variable pushFront(Variable variable);

  Variable insert(Iterator it, Variable variable);

  void popBack();
  void popFront();

  void remove(Iterator it);
  void clear();

  Iterator begin();
  Iterator end();

  ConstIterator begin() const;
  ConstIterator end() const;

  ConstIterator cbegin() const;
  ConstIterator cend() const;

  ReverseIterator rbegin();
  ReverseIterator rend();

  ConstReverseIterator rbegin() const;
  ConstReverseIterator rend() const;

  ConstReverseIterator crbegin() const;
  ConstReverseIterator crend() const;
};

}

#endif // DOUBLY_LINKED_LIST_HXX
