#define DEBUG
#include "libbinom/include/file_storage/fmm_containers.h"

using namespace binom;


HeapMap heap_map;
uint64_t allocated = 0;

void thr() {
  std::vector<VMemoryBlock> thr_memory_blocks;
  for(int i = 0, end = rand()%1000 + 1000; i < end; ++i)
    switch (rand()%2) {
      case 0:{ // Allocate
        VMemoryBlock block;
        block_size size = rand()%(4096*5) + 1;
        {
          do {
            block = heap_map.allocBlock(size);
            if(block.isEmpty()) {
              heap_map.expandMemory(heap_page_data_size);
              allocated += heap_page_data_size;
            }
          } while (block.isEmpty());
        }
        thr_memory_blocks.push_back(block);
      }continue;
      case 1: { // Free
        if(thr_memory_blocks.empty()) continue;
        std::vector<VMemoryBlock>::size_type index = rand()%thr_memory_blocks.size();
        heap_map.freeBlock(thr_memory_blocks[index].v_index);
        thr_memory_blocks.erase(thr_memory_blocks.begin() + index);
      }continue;
    }
}


#include <thread>

void test_heap_blocks() {

  std::clog << "\n\test_heap_blocks()\n\n\n";

  srand(time(nullptr));
  std::thread thr1(thr);
  std::thread thr2(thr);
  std::thread thr3(thr);
  std::thread thr4(thr);
  std::thread thr5(thr);
  std::thread thr6(thr);
  std::thread thr7(thr);
  std::thread thr8(thr);
  thr1.join();
  thr2.join();
  thr3.join();
  thr4.join();
  thr5.join();
  thr6.join();
  thr7.join();
  thr8.join();
  heap_map.check();
  std::clog << "Allocated (outer counted): " << allocated << '\n';
}

