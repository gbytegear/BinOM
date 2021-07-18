#include "binom/includes/file_storage/fmm_containers.h"

using namespace binom;


HeapMap heap_map;
thread_local std::vector<VMemoryBlock> thr_memory_blocks;

void thr() {
    for(int i = 0, end = rand()%50 + 50; i < end; ++i)
        switch (rand()%2) {
            case 0:{ // Allocate
                VMemoryBlock block;
                block_size size = rand()%(4096*5) + 1;
                do {
                    block = heap_map.allocBlock(size);
                    if(block.isEmpty()) {
                        heap_map.expandMemory(heap_page_data_size);
                    }
                } while (false);
                thr_memory_blocks.push_back(block);
            }continue;
            case 1: // Free
                if(thr_memory_blocks.empty()) continue;
                heap_map.freeBlock(thr_memory_blocks[rand()%thr_memory_blocks.size()].v_index);
            continue;
        }
}


#include <thread>

void test_heap_blocks() {
    srand(time(nullptr));
    std::thread thr1(thr);
    std::thread thr2(thr);
    std::thread thr3(thr);
    std::thread thr4(thr);
    thr1.join();
    thr2.join();
    thr3.join();
    thr4.join();
    heap_map.check();
}

