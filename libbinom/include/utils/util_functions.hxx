#ifndef UTIL_FUNCTIONS_HXX
#define UTIL_FUNCTIONS_HXX

#include <cstddef>
#include <climits>
#include <algorithm>

namespace utilfunc {

template <typename T>
void doLeftShift(T* arr, size_t size, size_t shift) noexcept {
  constexpr size_t type_bit_size = CHAR_BIT * sizeof (T);
  const size_t type_shift = shift / type_bit_size;
  const size_t bit_offset = shift % type_bit_size;

  if (bit_offset == 0)
    for (size_t i = size - 1; i >= type_shift; --i)
      arr[i] = arr[i - type_shift];
  else {
    const size_t sub_bit_offset = type_bit_size - bit_offset;
    for (size_t i = size - 1; i > type_shift; --i)
      arr[i] = ((arr[i - type_shift] << bit_offset) | (arr[i - type_shift - 1] >> sub_bit_offset));
    arr[type_shift] = arr[0] << bit_offset;
  }

  std::fill(arr, arr + type_shift, static_cast<T>(0));
}

template <typename T>
void doRightShift(T* arr, size_t size, size_t shift) noexcept {
  constexpr size_t type_bit_size = CHAR_BIT * sizeof (T);
  const size_t type_shift = shift / type_bit_size;
  const size_t bit_offset = shift % type_bit_size;
  const size_t limit = size - type_shift - 1;

  if (bit_offset == 0)
    for (size_t i = 0; i <= limit; ++i)
      arr[i] = arr[i + type_shift];
  else {
    const size_t sub_bit_offset = type_bit_size - bit_offset;
    for (size_t i = 0; i < limit; ++i)
      arr[i] = ((arr[i + type_shift] >> bit_offset) | (arr[i + type_shift + 1] << sub_bit_offset));
    arr[limit] = arr[size-1] >> bit_offset;
  }

  std::fill(arr + limit + 1, arr + size, static_cast<T>(0));
}

constexpr size_t getNearestPow2(size_t num) noexcept {
  num--;
  num |= num >> 1;
  num |= num >> 2;
  num |= num >> 4;
  num |= num >> 8;
  num |= num >> 16;
  num |= num >> 32;
  num++;
  return num;
  // Another option to implement the function:
  //  *reinterpret_cast<double*>(&num) = num;
  //  return 1 << (((*(reinterpret_cast<uint32_t*>(&num) + 1) & 0x7FF00000) >> 20) - 1022);
}

}

#endif // UTIL_FUNCTIONS_HXX
