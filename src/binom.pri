HEADERS += \
    $$PWD/binom/includes/lexer.h \
    binom/includes/file_storage/file_memory_manager.h \
    binom/includes/file_storage/file_node_visitor.h \
    binom/includes/file_storage/file_storage.h \
    binom/includes/file_storage/fmm_containers.h \
    binom/includes/utils/file.h \
    binom/includes/utils/node_visitor_base.h \
    binom/includes/utils/var_mem_mngr.h \
    binom/includes/binom.h \
    binom/includes/variables/variable.h \
    binom/includes/variables/primitive.h \
    binom/includes/variables/buffer_array.h \
    binom/includes/variables/array.h \
    binom/includes/variables/object.h \
    binom/includes/variables/value.h \
    binom/includes/utils/node_visitor.h \
    binom/includes/utils/ctypes.h \
    binom/includes/utils/types.h \
    binom/includes/utils/mem.h \
    binom/includes/utils/exception.h \
    binom/includes/utils/rwguard.h \
    binom/includes/utils/path.h \
    binom/includes/utils/query.h \
    binom/includes/utils/byte_array.h \
    binom/includes/file_storage/file_structs.h \

SOURCES += \
    $$PWD/binom/sources/file_storage/file_node_iterator.cpp \
    $$PWD/binom/sources/lexer.cpp \
    $$PWD/binom/sources/utils/node_visitor_base.cpp \
    binom/sources/utils/TestExpressionFrame.cpp \
    binom/sources/file_storage/file_node_visitor.cpp \
    binom/sources/binom.cpp \
    binom/sources/variables/variable.cpp \
    binom/sources/variables/primitive.cpp \
    binom/sources/variables/buffer_array.cpp \
    binom/sources/variables/array.cpp \
    binom/sources/variables/object.cpp \
    binom/sources/variables/value.cpp \
    binom/sources/utils/node_visitor.cpp\
    binom/sources/utils/byte_array.cpp \
    binom/sources/utils/mem.cpp \
    binom/sources/utils/path.cpp \
    binom/sources/utils/query.cpp \
    binom/sources/file_storage/file_memory_manager.cpp

#include("tcp.pri")
