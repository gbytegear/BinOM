#include "binom/includes/mem.h"

void tryFree(void* ptr) {
  if(ptr == nullptr) return;
  try {free(ptr);}
  catch (...) {throw binom::Exception(binom::ErrCode::memory_free_error);}
}

void* tryMalloc(size_t size) {
  if(void* ptr = malloc(size); ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error);
  else return ptr;
}

void* tryRealloc(void* ptr, size_t size) {
  if(void* new_ptr = realloc(ptr, size); new_ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error);
  else return new_ptr;
}
