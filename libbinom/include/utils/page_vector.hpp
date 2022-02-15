#ifndef PAGE_VECTOR_H
#define PAGE_VECTOR_H

#include "types.hpp"
#include "bit_map64.hpp"
#include <vector>
#include <shared_mutex>
#include <mutex>

namespace binom::priv {

template<class T>
struct PageVector {

  typedef T Value;
  struct Data {
    ui64 key;
    Value& value;
  };

private:

  struct Page {
    std::mutex page_mtx;
    priv::BitMap64 bit_map;
    T data[64];
  };
  std::shared_mutex page_vector_mtx;
  std::vector<Page*> page_vector;
  void allocPage() {page_vector.emplace_back(new Page);}

  Data take() noexcept {
    page_vector_mtx.lock_shared();
    typename std::vector<Page*>::iterator pg_it = page_vector.begin();
    typename std::vector<Page*>::iterator end = page_vector.end();

    while (true) {
      for(;pg_it != end; ++pg_it) {
        Page* page = *pg_it;
        std::scoped_lock lk(page->page_mtx);
        if(page->bit_map.value64() != 0xFFFFFFFFFFFFFFFF) { // [#][#][#][#][#][#][#][#]
          if(page->bit_map.values32()[0] != 0xFFFFFFFF) {   // [#][#][#][#][ ][ ][ ][ ]
            if(page->bit_map.values16()[0] != 0xFFFF) {     // [#][#][ ][ ][ ][ ][ ][ ]
              if(page->bit_map.values8()[0] != 0xFF) {      // [#][ ][ ][ ][ ][ ][ ][ ]
                for(BitIterator it = page->bit_map.begin(),
                                end = page->bit_map.iteratorAt(8);
                    it != end; ++it) {
                  if(it) continue;
                  it.set(true);
                  page_vector_mtx.unlock_shared();
                  return Data{std::distance(page_vector.begin(), pg_it) + it.getBitIndex(), page->data[it.getBitIndex()]};
                }
                continue;
              } else {                                      // [ ][#][ ][ ][ ][ ][ ][ ]
                for(BitIterator it = page->bit_map.iteratorAt(8),
                                end = page->bit_map.iteratorAt(16);
                    it != end; ++it) {
                  if(it) continue;
                  it.set(true);
                  page_vector_mtx.unlock_shared();
                  return Data{std::distance(page_vector.begin(), pg_it) + it.getBitIndex(), page->data[it.getBitIndex()]};
                }
                continue;
              }
            } else {                                        // [ ][ ][#][#][ ][ ][ ][ ]
              if(page->bit_map.values8()[2] != 0xFF) {      // [ ][ ][#][ ][ ][ ][ ][ ]
                for(BitIterator it = page->bit_map.iteratorAt(16),
                                end = page->bit_map.iteratorAt(24);
                    it != end; ++it) {
                  if(it) continue;
                  it.set(true);
                  page_vector_mtx.unlock_shared();
                  return Data{std::distance(page_vector.begin(), pg_it) + it.getBitIndex(), page->data[it.getBitIndex()]};
                }
                continue;
              } else {                                      // [ ][ ][ ][#][ ][ ][ ][ ]
                for(BitIterator it = page->bit_map.iteratorAt(24),
                                end = page->bit_map.iteratorAt(32);
                    it != end; ++it) {
                  if(it) continue;
                  it.set(true);
                  page_vector_mtx.unlock_shared();
                  return Data{std::distance(page_vector.begin(), pg_it) + it.getBitIndex(), page->data[it.getBitIndex()]};
                }
                continue;
              }
            }
          } else {                                          // [ ][ ][ ][ ][#][#][#][#]
            if(page->bit_map.values16()[2] != 0xFFFF) {     // [ ][ ][ ][ ][#][#][ ][ ]
              if(page->bit_map.values8()[4] != 0xFF) {      // [ ][ ][ ][ ][#][ ][ ][ ]
                for(BitIterator it = page->bit_map.iteratorAt(32),
                                end = page->bit_map.iteratorAt(40);
                    it != end; ++it) {
                  if(it) continue;
                  it.set(true);
                  page_vector_mtx.unlock_shared();
                  return Data{std::distance(page_vector.begin(), pg_it) + it.getBitIndex(), page->data[it.getBitIndex()]};
                }
                continue;
              } else {                                      // [ ][ ][ ][ ][ ][#][ ][ ]
                for(BitIterator it = page->bit_map.iteratorAt(40),
                                end = page->bit_map.iteratorAt(48);
                    it != end; ++it) {
                  if(it) continue;
                  it.set(true);
                  page_vector_mtx.unlock_shared();
                  return Data{std::distance(page_vector.begin(), pg_it) + it.getBitIndex(), page->data[it.getBitIndex()]};
                }
                continue;
              }
            } else {                                        // [ ][ ][ ][ ][ ][ ][#][#]
              if(page->bit_map.values8()[6] != 0xFF) {      // [ ][ ][ ][ ][ ][ ][#][ ]
                for(BitIterator it = page->bit_map.iteratorAt(48),
                                end = page->bit_map.iteratorAt(56);
                    it != end; ++it) {
                  if(it) continue;
                  it.set(true);
                  page_vector_mtx.unlock_shared();
                  return Data{std::distance(page_vector.begin(), pg_it) + it.getBitIndex(), page->data[it.getBitIndex()]};
                }
                continue;
              } else {                                      // [ ][ ][ ][ ][ ][ ][ ][#]
                for(BitIterator it = page->bit_map.iteratorAt(56),
                                end = page->bit_map.end();
                    it != end; ++it) {
                  if(it) continue;
                  it.set(true);
                  page_vector_mtx.unlock_shared();
                  return Data{std::distance(page_vector.begin(), pg_it) + it.getBitIndex(), page->data[it.getBitIndex()]};
                }
                continue;
              }
            }
          }
        } else continue;
      }

      // Allocate new page
      ui64 page_count = page_vector.size();
      page_vector_mtx.unlock_shared();
      page_vector_mtx.lock();
      if(page_count == page_vector.size()) allocPage();
      end = page_vector.end();
      pg_it = end - 1;
      page_vector_mtx.unlock();
      page_vector_mtx.lock_shared();
    }
  }

  void reset(ui64 index) noexcept {
    std::shared_lock lk(page_vector_mtx);
    page_vector[index/64]->bit_map.iteratorAt(index%64).set(false);
  }

public:

  template<typename... Args>
  Data emplace(Args&&... args) noexcept {
    Data data = take();
    new(&data.value) Value(std::forward<Args>(args)...);
    return data;
  }

  void destroy(ui64 index) noexcept {get(index).~Value(); return reset(index);};

  Value& get(ui64 index) const noexcept {
    std::shared_lock lk(page_vector_mtx);
    return page_vector[index/64]->data[index%64];
  }

  Value& operator[](ui64 index) const noexcept {
    std::shared_lock lk(page_vector_mtx);
    return page_vector[index/64]->data[index%64];
  }
};

}

#endif // PAGE_VECTOR_H
