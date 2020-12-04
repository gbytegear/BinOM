#include "binom/includes/mem.h"


void tryFree(void* ptr) {
  if(ptr == nullptr) return;
  try {free(ptr);}
  catch (...) {throw binom::SException(binom::ErrCode::memory_free_error, "Free memory error!");}
}

void* tryMalloc(size_t size) {
  void* ptr = malloc(size);
  if(ptr == nullptr) throw binom::SException(binom::ErrCode::memory_allocation_error, "Memory size allocation error!");
  else return ptr;
}

void* tryRealloc(void* ptr, size_t size) {
  void* new_ptr = realloc(ptr, size);
  if(new_ptr == nullptr) throw binom::SException(binom::ErrCode::memory_allocation_error, "Memory reallocation error!");
  else return new_ptr;
}
