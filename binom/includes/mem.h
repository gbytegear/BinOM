#ifndef TRYMEM_H
#define TRYMEM_H

#include "exception.h"
#include <malloc.h>

//#define M_DBG

#ifdef M_DBG

#define MDBG(expression) std::clog << "\n\n+-------- " << __FUNCTION__ << '\n' << expression << "+--------\n\n";

#else

#define MDBG(expression)

#endif

void tryFree(void* ptr);

template <typename Type>
Type* tryMalloc() {

  Type* ptr = reinterpret_cast<Type*>(malloc(sizeof(Type)));

  MDBG("|Allocated:" << sizeof(Type) << '\n'
    << "|Allocated addess:" << static_cast<void*>(ptr) << '\n')

  if(ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error, "Memory type allocation error!");
  else return ptr;
}

void* tryMalloc(size_t size);

template <typename Type>
Type* tryMalloc(size_t count) {
  Type* ptr = reinterpret_cast<Type*>(malloc(sizeof (Type) * count));

  MDBG("|Allocated:" << sizeof(Type)*count << '\n'
    << "|Allocated addess:" << static_cast<void*>(ptr) << '\n')

  if(ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error, "Memory array allocation error!");
  else return ptr;
}

void* tryRealloc(void* ptr, size_t size);

template <typename Type>
Type* tryRealloc(Type* ptr, size_t count) {
  Type* new_ptr = reinterpret_cast<Type*>(realloc(ptr, sizeof (Type) * count));

  MDBG("|Reallocated:" << size << '\n'
    << "|Reallocated from:" << ptr << '\n'
    << "|Reallocated to:" << new_ptr << '\n')

  if(new_ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error, "Memory array allocation error!");
  else return new_ptr;
}

#endif // TRYMEM_H
