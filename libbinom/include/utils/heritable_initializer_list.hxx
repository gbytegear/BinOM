#ifndef HERITABLE_INITIALIZER_LIST_HXX
#define HERITABLE_INITIALIZER_LIST_HXX

#include <cstddef>
#include <initializer_list>

namespace heritable_initializer_list {

template<class T>
class HeritableInitializerList {
public:
  typedef T           value_type;
  typedef const T&    reference;
  typedef const T&    const_reference;
  typedef size_t      size_type;
  typedef const T*    iterator;
  typedef const T*    const_iterator;

private:
  iterator      array;
  size_type     size;

public:
  constexpr HeritableInitializerList(std::initializer_list<T> init_list)
    : array(init_list.begin()), size(init_list.size()) {}

  constexpr HeritableInitializerList(const HeritableInitializerList<T>& init_list)
    : array(init_list.array), size(init_list.size) {}

  constexpr HeritableInitializerList(const HeritableInitializerList<T>&& init_list)
    : array(init_list.array), size(init_list.size) {}

  constexpr HeritableInitializerList() noexcept
    : array(0), size(0) { }

  constexpr size_type getSize() const noexcept { return size; }
  constexpr const_iterator begin() const noexcept { return array; }
  constexpr const_iterator end() const noexcept { return begin() + getSize(); }
};

/**
 *  @brief  Return an iterator pointing to the first element of
 *          the initializer_list.
 *  @param  __ils  Initializer list.
 */
template<class _Tp>
constexpr const _Tp*
begin(HeritableInitializerList<_Tp> __ils) noexcept
{ return __ils.begin(); }

/**
 *  @brief  Return an iterator pointing to one past the last element
 *          of the initializer_list.
 *  @param  __ils  Initializer list.
 */
template<class _Tp>
constexpr const _Tp*
end(HeritableInitializerList<_Tp> __ils) noexcept
{ return __ils.end(); }

}

#endif // HERITABLE_INITIALIZER_LIST_HXX
