
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/libbinom/include/file_storage/file_node_iterator.h \
    $$PWD/libbinom/include/file_storage/file_object_element_finder.h \
    $$PWD/libbinom/include/lexer.h \
    $$PWD/libbinom/include/file_storage/file_memory_manager.h \
    $$PWD/libbinom/include/file_storage/file_node_visitor.h \
    $$PWD/libbinom/include/file_storage/file_storage.h \
    $$PWD/libbinom/include/file_storage/fmm_containers.h \
    $$PWD/libbinom/include/utils/file.h \
    $$PWD/libbinom/include/utils/node_visitor_base.h \
    $$PWD/libbinom/include/utils/var_mem_mngr.h \
    $$PWD/libbinom/include/binom.h \
    $$PWD/libbinom/include/variables/variable.h \
    $$PWD/libbinom/include/variables/primitive.h \
    $$PWD/libbinom/include/variables/buffer_array.h \
    $$PWD/libbinom/include/variables/array.h \
    $$PWD/libbinom/include/variables/object.h \
    $$PWD/libbinom/include/variables/value.h \
    $$PWD/libbinom/include/utils/node_visitor.h \
    $$PWD/libbinom/include/utils/ctypes.h \
    $$PWD/libbinom/include/utils/types.h \
    $$PWD/libbinom/include/utils/mem.h \
    $$PWD/libbinom/include/utils/exception.h \
    $$PWD/libbinom/include/utils/rwguard.h \
    $$PWD/libbinom/include/utils/path.h \
    $$PWD/libbinom/include/utils/query.h \
    $$PWD/libbinom/include/utils/byte_array.h \
    $$PWD/libbinom/include/file_storage/file_structs.h \

SOURCES += \
    $$PWD/libbinom/source/file_storage/file_node_iterator.cpp \
    $$PWD/libbinom/source/file_storage/file_object_element_finder.cpp \
    $$PWD/libbinom/source/lexer.cpp \
    $$PWD/libbinom/source/utils/node_visitor_base.cpp \
    $$PWD/libbinom/source/utils/TestExpressionFrame.cpp \
    $$PWD/libbinom/source/file_storage/file_node_visitor.cpp \
    $$PWD/libbinom/source/binom.cpp \
    $$PWD/libbinom/source/variables/variable.cpp \
    $$PWD/libbinom/source/variables/primitive.cpp \
    $$PWD/libbinom/source/variables/buffer_array.cpp \
    $$PWD/libbinom/source/variables/array.cpp \
    $$PWD/libbinom/source/variables/object.cpp \
    $$PWD/libbinom/source/variables/value.cpp \
    $$PWD/libbinom/source/utils/node_visitor.cpp\
    $$PWD/libbinom/source/utils/byte_array.cpp \
    $$PWD/libbinom/source/utils/mem.cpp \
    $$PWD/libbinom/source/utils/path.cpp \
    $$PWD/libbinom/source/utils/query.cpp \
    $$PWD/libbinom/source/file_storage/file_memory_manager.cpp
