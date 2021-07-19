#include "binom/includes/file_storage/file_memory_manager.h"

using namespace binom;

FMemoryManager::FMemoryManager(std::string_view file_path) : file(file_path) {
  init();
}

void FMemoryManager::init() {
  std::scoped_lock slk(mtx);
  if(file.isEmpty()) {
    file.resize(sizeof(db_header));
    file.write(0, db_header);
    return;
  }

  file.read(db_header, 0);

  switch (db_header.checkFileVersion()) {
    case binom::DBHeader::VersionDifference::identical: break;
    case binom::DBHeader::VersionDifference::file_type:
      throw Exception(ErrCode::binomdb_invalid_file, "Invalid file type");
    case binom::DBHeader::VersionDifference::major:
      std::cerr << "Warning: major version difference!\n";
    break;
    case binom::DBHeader::VersionDifference::minor:
      std::cerr << "Warning: minor version difference!\n";
    break;
  }

  if(db_header.first_node_page_index) {
    real_index next_page_index = db_header.first_node_page_index;
    NodePageInfo page_info;
    while (next_page_index) {
      page_info.page_position = next_page_index;
      file.read(page_info.descriptor, next_page_index);
      node_page_vector.push_back(page_info);
    }
  }

  if(db_header.first_heap_page_index) {
    real_index next_page_index = db_header.first_heap_page_index;
    HeapPageInfo page_info;
    while (next_page_index) {
      page_info.page_position = next_page_index;
      file.read(page_info.descriptor, next_page_index);
      heap_page_vector.push_back(page_info);
      heap_map.expandMemory(heap_page_data_size);
    }
  }

  switch (toTypeClass(db_header.root_node.type)) {
    case VarTypeClass::buffer_array:
    case VarTypeClass::array:
    case VarTypeClass::object:
      heap_map.occupyBlock(db_header.root_node.index, db_header.root_node.size);
    default: break;
  }

  for(auto node_page_info : node_page_vector) {
    for(auto bit : node_page_info.descriptor.node_map)
      if(bit.get()) {
        NodeDescriptor descriptor;
        file.read(descriptor,
                  node_page_info.page_position +
                  sizeof (NodePageDescriptor) +
                  bit.getBitIndex()*sizeof (NodeDescriptor));
        switch (toTypeClass(descriptor.type)) {
          case VarTypeClass::buffer_array:
          case VarTypeClass::array:
          case VarTypeClass::object:
            if(descriptor.size == 0) continue; // For empty containers
            heap_map.occupyBlock(descriptor.index, descriptor.size);
          continue;
          default:continue;
        }
      }
  }

}
