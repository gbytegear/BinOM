SOURCES += \
  $$PWD/binom/sources/structure/union_node_visitor.cpp \
  binom/sources/structure/rwguard.cpp \
  binom/sources/binom.cpp \
  binom/sources/byte_array.cpp \
  binom/sources/structure/file_storage/data_base_node_visitor.cpp \
  binom/sources/structure/file_storage/file_io.cpp \
  binom/sources/structure/file_storage/file_virtual_memory_controller.cpp \
  binom/sources/structure/path.cpp \
  binom/sources/structure/query.cpp \
  binom/sources/structure/mem.cpp \
  binom/sources/structure/variables/value.cpp \
  binom/sources/structure/variables/variable.cpp \
  binom/sources/structure/variables/primitive.cpp \
  binom/sources/structure/variables/buffer_array.cpp \
  binom/sources/structure/variables/array.cpp \
  binom/sources/structure/variables/object.cpp \
  binom/sources/structure/variables/node_visitor.cpp
# Included to source:
#  binom/sources/structure/file_storage/data_base_node_visitor_query.h
#  binom/sources/structure/variables/node_visitor_query.h

HEADERS += \
  $$PWD/binom/includes/structure/file_storage/fvmc_utils.h \
  $$PWD/binom/includes/structure/union_node_visitor.h \
  binom/includes/structure/file_storage/data_base.h \
  binom/includes/structure/file_storage/data_base_node_visitor.h \
  binom/includes/structure/file_storage/file_virtual_memory_controller.h \
  binom/includes/structure/path.h \
  binom/includes/structure/query.h \
  binom/includes/structure/file_storage/file_structs.h \
  binom/includes/binom.h \
  binom/includes/byte_array.h \
  binom/includes/mem.h \
  binom/includes/structure/rwguard.h \
  binom/includes/structure/variables/value.h \
  binom/includes/ctypes.h \
  binom/includes/exception.h \
  binom/includes/structure/types.h \
  binom/includes/structure/variables/variable.h \
  binom/includes/structure/variables/primitive.h \
  binom/includes/structure/variables/buffer_array.h \
  binom/includes/structure/variables/array.h \
  binom/includes/structure/variables/object.h \
  binom/includes/structure/variables/node_visitor.h \
  binom/includes/structure/file_storage/file_io.h

include("tcp.pri")
