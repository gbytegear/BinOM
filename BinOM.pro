CONFIG -= qt

include("binom.pri")

linux-g++ | linux-g++-64 | linux-g++-32 {
CONFIG += c++17 console
LIBS += -lstdc++fs -lpthread;
}

win32 {
    CONFIG += c++17 console
    LIBS += -lstdc++fs -lpthread;
    QMAKE_LFLAGS = -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc
    QMAKE_LFLAGS_EXCEPTIONS_ON = -mthreads
}

SOURCES += \
    test.cpp

HEADERS += \
    tests/file_node_visitor_test.h \
    tests/heap_blocks_test.h \
    tests/memory_manager_test.h \
    tests/multithreading_test.h \
    tests/query_test.h \
    tests/var_test.h

SUBDIRS += \
    BinOM_tools.pro

