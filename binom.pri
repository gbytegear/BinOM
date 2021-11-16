HEADERS += \
    $$PWD/libbinom/include/file_storage/file_node_iterator.h \
    libbinom/include/file_storage/file_object_element_finder.h \
    libbinom/include/lexer.h \
    libbinom/include/file_storage/file_memory_manager.h \
    libbinom/include/file_storage/file_node_visitor.h \
    libbinom/include/file_storage/file_storage.h \
    libbinom/include/file_storage/fmm_containers.h \
    libbinom/include/utils/file.h \
    libbinom/include/utils/node_visitor_base.h \
    libbinom/include/utils/var_mem_mngr.h \
    libbinom/include/binom.h \
    libbinom/include/variables/variable.h \
    libbinom/include/variables/primitive.h \
    libbinom/include/variables/buffer_array.h \
    libbinom/include/variables/array.h \
    libbinom/include/variables/object.h \
    libbinom/include/variables/value.h \
    libbinom/include/utils/node_visitor.h \
    libbinom/include/utils/ctypes.h \
    libbinom/include/utils/types.h \
    libbinom/include/utils/mem.h \
    libbinom/include/utils/exception.h \
    libbinom/include/utils/rwguard.h \
    libbinom/include/utils/path.h \
    libbinom/include/utils/query.h \
    libbinom/include/utils/byte_array.h \
    libbinom/include/file_storage/file_structs.h \

SOURCES += \
    libbinom/source/file_storage/file_node_iterator.cpp \
    libbinom/source/file_storage/file_object_element_finder.cpp \
    libbinom/source/lexer.cpp \
    libbinom/source/utils/node_visitor_base.cpp \
    libbinom/source/utils/TestExpressionFrame.cpp \
    libbinom/source/file_storage/file_node_visitor.cpp \
    libbinom/source/binom.cpp \
    libbinom/source/variables/variable.cpp \
    libbinom/source/variables/primitive.cpp \
    libbinom/source/variables/buffer_array.cpp \
    libbinom/source/variables/array.cpp \
    libbinom/source/variables/object.cpp \
    libbinom/source/variables/value.cpp \
    libbinom/source/utils/node_visitor.cpp\
    libbinom/source/utils/byte_array.cpp \
    libbinom/source/utils/mem.cpp \
    libbinom/source/utils/path.cpp \
    libbinom/source/utils/query.cpp \
    libbinom/source/file_storage/file_memory_manager.cpp

#include("tcp.pri")
