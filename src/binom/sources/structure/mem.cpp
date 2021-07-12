#include "binom/includes/mem.h"

void tryFree(void* ptr) {
  if(ptr) free(ptr);
}

void* tryMalloc(size_t size) {
  if(!size) return nullptr;
  if(void* ptr = malloc(size); ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error);
  else return ptr;
}

void* tryRealloc(void* ptr, size_t size) {
  if(!size) {
    tryFree(ptr);
    return nullptr;
  }
  if(void* new_ptr = realloc(ptr, size); new_ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error);
  else return new_ptr;
}
