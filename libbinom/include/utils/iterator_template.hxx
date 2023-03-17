#ifndef ITERATOR_TEMPLATE
#define ITERATOR_TEMPLATE

#include "extended_cxx.hxx"
#include "extended_type_traits.hxx"
#include "pseudo_pointer.hxx"
#include <utility>
#include <type_traits>

//! Iterator templates
namespace itmpl {

//================================================================================================================
// Template constraits

template<typename T> concept BaseIteratorC = true;
template<typename T> concept ValueC = true;

template <typename T, typename BaseIteratorT>
concept MoverC = requires(T mover, BaseIteratorT& base_it) {
  {T::moveNext(base_it)} -> std::same_as<BaseIteratorT&>;
  {T::moveNextPostf(base_it)} -> std::same_as<BaseIteratorT>;
};

template <typename T, typename BaseIteratorT>
concept BackMovableMoverC = requires(T mover, BaseIteratorT& base_it) {
  requires MoverC<T, BaseIteratorT>;
  {T::movePrev(base_it)} -> std::same_as<BaseIteratorT&>;
  {T::movePrevPostf(base_it)} -> std::same_as<BaseIteratorT>;
};

template <typename T, typename BaseIteratorT, typename ValueT>
concept ExtractorC = requires(T extractor, BaseIteratorT& base_it, const BaseIteratorT& const_base_it) {
  {T::getRef(base_it)} -> extended_type_traits::is_any_of<const ValueT&, const ValueT, ValueT&, ValueT>;
  {T::getPtr(base_it)} -> extended_type_traits::is_any_of<ValueT*, const ValueT*, pseudo_ptr::PseudoPointer<ValueT>, pseudo_ptr::PseudoPointer<const ValueT>>;
};

template <typename T, typename BaseIteratorT, typename ValueT>
concept ConstExtractorC = requires(T extractor, BaseIteratorT& const_base_it) {
  requires ExtractorC<T, BaseIteratorT, ValueT>;
  {T::getRef(const_base_it)} -> extended_type_traits::is_any_of<const ValueT&, const ValueT, ValueT>;
  {T::getPtr(const_base_it)} -> extended_type_traits::is_any_of<const ValueT*, pseudo_ptr::PseudoPointer<const ValueT>, pseudo_ptr::PseudoPointer<ValueT>>;
};


//================================================================================================================
// Default strategies

template <typename BaseIterator, typename Value>
struct DefaultExtractor {
  static Value& getRef(BaseIterator& it) { return *it; }
  static Value* getPtr(BaseIterator& it) { return &*it; }
  static const Value& getRef(const BaseIterator& it) { return *it; }
  static const Value* getPtr(const BaseIterator& it) { return &*it; }
};

template <typename BaseIterator>
struct DefaultMover {
  static BaseIterator& moveNext(BaseIterator& it) { return ++it; }
  static BaseIterator moveNextPostf(BaseIterator& it) { return it++; }
  static BaseIterator& movePrev(BaseIterator& it) { return --it; }
  static BaseIterator movePrevPostf(BaseIterator& it) { return it--; }
};

//================================================================================================================
// Iterator templates


template <
  BaseIteratorC Base,
  ValueC Value,
  MoverC<Base> Mover = DefaultMover<Base>,
  ExtractorC<Base, Value> Extractor = DefaultExtractor<Base, Value>
>
class Iterator : public Base {
public:
  typedef Iterator Self;

protected:
  inline Base& upcast() {return dynamic_cast<Base&>(self);}
  inline const Base& upcast() const {return dynamic_cast<Base&>(self);}
  static inline Self& downcast(Base& base) {return reinterpret_cast<Self&>(base);}
  static inline const Self& downcast(const Base& base) {return reinterpret_cast<const Self&>(base);}

public:
  Iterator(const Base& base) : Base(base) {}
  Iterator(Base&& base) : Base(std::move(base)) {}
  Iterator(const Self& other) : Base(other.upcast()) {}
  Iterator(Self&& other) : Base(std::move(other.upcast())) {}

  auto operator*() { return Extractor::getRef(upcast()); }
  auto operator->() { return Extractor::getPtr(upcast()); }
  auto operator*() const requires ConstExtractorC<Extractor, Base, Value> || ConstExtractorC<Extractor, const Base, Value>
    { return Extractor::getRef(upcast()); }
  auto operator->() const requires ConstExtractorC<Extractor, Base, Value> || ConstExtractorC<Extractor, const Base, Value>
    { return Extractor::getPtr(upcast()); }

  inline Self& operator++() { return downcast(Mover::moveNext(upcast())); }
  inline Self operator++(int) { return downcast(Mover::moveNextPostf(upcast())); }
  inline Self& operator--() requires BackMovableMoverC<Mover, Base> { return downcast(Mover::movePrev(upcast())); }
  inline Self operator--(int) requires BackMovableMoverC<Mover, Base> { return downcast(Mover::movePrevPostf(upcast())); }
};




template <
  BaseIteratorC Base,
  ValueC Value,
  MoverC<Base> Mover = DefaultMover<Base>,
  ExtractorC<Base, Value> Extractor = DefaultExtractor<Base, Value>
>
class ReverseIterator : public Base {
public:
  typedef ReverseIterator Self;

protected:
  inline Base& upcast() {return dynamic_cast<Base&>(self);}
  inline const Base& upcast() const {return dynamic_cast<const Base&>(self);}
  static inline Self& downcast(Base& base) {return reinterpret_cast<Self&>(base);}
  static inline const Self& downcast(const Base& base) {return reinterpret_cast<const Self&>(base);}

public:
  ReverseIterator(const Base& base) : Base(base) {}
  ReverseIterator(Base&& base) : Base(std::move(base)) {}
  ReverseIterator(const Self& other) : Base(other.upcast()) {}
  ReverseIterator(Self&& other) : Base(std::move(other.upcast())) {}

  auto operator*() { return Extractor::getRef(upcast()); }
  auto operator->() { return Extractor::getPtr(upcast()); }
  auto operator*() const requires ConstExtractorC<Extractor, Base, Value> || ConstExtractorC<Extractor, const Base, Value>
    { return Extractor::getRef(upcast()); }
  auto operator->() const requires ConstExtractorC<Extractor, Base, Value> || ConstExtractorC<Extractor, const Base, Value>
    { return Extractor::getPtr(upcast()); }

  inline Self& operator++() requires BackMovableMoverC<Mover, Base> { return downcast(Mover::movePrev(upcast())); }
  inline Self operator++(int) requires BackMovableMoverC<Mover, Base> { return downcast(Mover::movePrevPostf(upcast())); }
  inline Self& operator--() { return downcast(Mover::moveNext(upcast())); }
  inline Self operator--(int) { return downcast(Mover::moveNextPostf(upcast())); }
};




template <
  BaseIteratorC Base,
  ValueC Value,
  MoverC<Base> Mover = DefaultMover<Base>,
  ConstExtractorC<Base, Value> Extractor = DefaultExtractor<Base, Value>
>
class ConstIterator : public Base {
public:
  typedef ConstIterator Self;

protected:
  inline Base& upcast() {return dynamic_cast<Base&>(self);}
  inline const Base& upcast() const {return dynamic_cast<Base&>(self);}
  static inline Self& downcast(Base& base) {return reinterpret_cast<Self&>(base);}
  static inline const Self& downcast(const Base& base) {return reinterpret_cast<const Self&>(base);}

public:
  ConstIterator(const Base& base) : Base(base) {}
  ConstIterator(Base&& base) : Base(std::move(base)) {}
  ConstIterator(const Self& other) : Base(other.upcast()) {}
  ConstIterator(Self&& other) : Base(std::move(other.upcast())) {}

  auto operator*() { return Extractor::getRef(upcast()); }
  auto operator->() { return Extractor::getPtr(upcast()); }
  auto operator*() const requires ConstExtractorC<Extractor, Base, Value> || ConstExtractorC<Extractor, const Base, Value>
    { return Extractor::getRef(upcast()); }
  auto operator->() const requires ConstExtractorC<Extractor, Base, Value> || ConstExtractorC<Extractor, const Base, Value>
    { return Extractor::getPtr(upcast()); }

  inline Self& operator++() { return downcast(Mover::moveNext(upcast())); }
  inline Self operator++(int) { return downcast(Mover::moveNextPostf(upcast())); }
  inline Self& operator--() requires BackMovableMoverC<Mover, Base> { return downcast(Mover::movePrev(upcast())); }
  inline Self operator--(int) requires BackMovableMoverC<Mover, Base> { return downcast(Mover::movePrevPostf(upcast())); }
};




template <
  BaseIteratorC Base,
  ValueC Value,
  MoverC<Base> Mover = DefaultMover<Base>,
  ConstExtractorC<Base, Value> Extractor = DefaultExtractor<Base, Value>
>
class ConstReverseIterator : public Base {
public:
  typedef ConstReverseIterator Self;

protected:
  inline Base& upcast() {return dynamic_cast<Base&>(self);}
  inline const Base& upcast() const {return dynamic_cast<Base&>(self);}
  static inline Self& downcast(Base& base) {return reinterpret_cast<Self&>(base);}
  static inline const Self& downcast(const Base& base) {return reinterpret_cast<const Self&>(base);}

public:
  ConstReverseIterator(const Base& base) : Base(base) {}
  ConstReverseIterator(Base&& base) : Base(std::move(base)) {}
  ConstReverseIterator(const Self& other) : Base(other.upcast()) {}
  ConstReverseIterator(Self&& other) : Base(std::move(other.upcast())) {}

  auto operator*() { return Extractor::getRef(upcast()); }
  auto operator->() { return Extractor::getPtr(upcast()); }
  auto operator*() const requires ConstExtractorC<Extractor, Base, Value> || ConstExtractorC<Extractor, const Base, Value>
    { return Extractor::getRef(upcast()); }
  auto operator->() const requires ConstExtractorC<Extractor, Base, Value> || ConstExtractorC<Extractor, const Base, Value>
    { return Extractor::getPtr(upcast()); }

  inline Self& operator++() requires BackMovableMoverC<Mover, Base> { return downcast(Mover::movePrev(upcast())); }
  inline Self operator++(int) requires BackMovableMoverC<Mover, Base> { return downcast(Mover::movePrevPostf(upcast())); }
  inline Self& operator--() { return downcast(Mover::moveNext(upcast())); }
  inline Self operator--(int) { return downcast(Mover::moveNextPostf(upcast())); }
};

};

#endif