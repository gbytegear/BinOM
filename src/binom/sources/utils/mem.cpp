#include "binom/includes/utils/mem.h"

void binom::tryFree(void* ptr) {
  if(ptr) free(ptr);
}

void* binom::tryMalloc(size_t size) {
  if(!size) return nullptr;
  if(void* ptr = malloc(size); ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error);
  else return ptr;
}

void* binom::tryRealloc(void* ptr, size_t size) {
  if(!size) {
    tryFree(ptr);
    return nullptr;
  }
  if(void* new_ptr = realloc(ptr, size); new_ptr == nullptr) throw binom::Exception(binom::ErrCode::memory_allocation_error);
  else return new_ptr;
}
