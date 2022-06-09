#ifndef REVERSE_ITERATOR_HXX
#define REVERSE_ITERATOR_HXX

namespace reverse_iterator {

template<class IteratorType>
class ReverseIterator {
  IteratorType iterator;
public:
  ReverseIterator(IteratorType iterator)
    : iterator(iterator) {}
  constexpr ReverseIterator(const ReverseIterator& other)
    : iterator(other.iterator) {}
  constexpr ReverseIterator(const ReverseIterator&& other)
    : iterator(other.iterator) {}

  constexpr auto operator*() {return *iterator;}
  constexpr auto operator->() {return iterator.operator->();}
  constexpr auto operator*() const {return *iterator;}
  constexpr auto operator->() const {return iterator.operator->();}

  constexpr auto& operator++() {return --iterator;}
  constexpr auto operator++(int) {return iterator--;}
  constexpr auto& operator++() const {return --iterator;}
  constexpr auto operator++(int) const {return iterator--;}

  constexpr auto& operator--() {return ++iterator;}
  constexpr auto operator--(int) {return iterator++;}
  constexpr auto& operator--() const {return ++iterator;}
  constexpr auto operator--(int) const {return iterator++;}

  constexpr bool operator==(const ReverseIterator<IteratorType> other) const {return iterator == other.iterator;}
  constexpr bool operator!=(const ReverseIterator<IteratorType> other) const {return iterator != other.iterator;}
  constexpr bool operator==(const IteratorType other) const {return iterator == other;}
  constexpr bool operator!=(const IteratorType other) const {return iterator != other;}

  IteratorType getIterator() const {return iterator;}
};

template <class Range>
class ReverseRange {
  Range& range;
public:
  constexpr ReverseRange(Range& range) : range(range) {}

  auto begin() {return range.rbegin();}
  auto end() {return range.rend();}
};

}

#endif // REVERSE_ITERATOR_HXX
