#ifndef TRYMEM_H
#define TRYMEM_H

#include "exception.h"
#include <malloc.h>

void tryFree(void* ptr) {
  if(ptr == nullptr) return;
  try {free(ptr);}
  catch (...) {throw binom::SException(binom::ErrCode::memory_free_error, "Free memory error!");}
}

template <typename Type>
Type* tryMalloc() {
  Type* ptr = reinterpret_cast<Type*>(malloc(sizeof(Type)));
  if(ptr == nullptr) throw binom::SException(binom::ErrCode::memory_allocation_error, "Memory type allocation error!");
  else return ptr;
}

void* tryMalloc(size_t size) {
  void* ptr = malloc(size);
  if(ptr == nullptr) throw binom::SException(binom::ErrCode::memory_allocation_error, "Memory size allocation error!");
  else return ptr;
}

template <typename Type>
void* tryMalloc(size_t count) {
  Type* ptr = reinterpret_cast<Type*>(malloc(sizeof (Type) * count));
  if(ptr == nullptr) throw binom::SException(binom::ErrCode::memory_allocation_error, "Memory array allocation error!");
  else return ptr;
}

void* tryRealloc(void* ptr, size_t size) {
  void* new_ptr = realloc(ptr, size);
  if(new_ptr == nullptr) throw binom::SException(binom::ErrCode::memory_allocation_error, "Memory reallocation error!");
  else return new_ptr;
}

#endif // TRYMEM_H
