CONFIG -= qt

include("binom.pri")

linux-g++ | linux-g++-64 | linux-g++-32 {
CONFIG += c++17 console
LIBS += -lstdc++fs -lpthread;
}


win32 {
    CONFIG += c++17 console
    LIBS += -lstdc++fs C:\Qt\Tools\mingw730_64\x86_64-w64-mingw32\lib\libws2_32.a;
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

DISTFILES += \
    ../example/test.json
