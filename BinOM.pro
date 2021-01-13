CONFIG -= qt

include("binom.pri")

linux-g++ | linux-g++-64 | linux-g++-32 {
CONFIG += c++2a
}


win32 {
    CONFIG += 17
    LIBS += -lstdc++fs;
    QMAKE_LFLAGS = -Wl,-enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc
    QMAKE_LFLAGS_EXCEPTIONS_ON = -mthreads
}

SOURCES += \
    test.cpp
