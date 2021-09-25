CONFIG -= qt
TARGET = binomtk

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
    toolkit.cpp \
    toolkit/cli.cpp \
    toolkit/commands.cpp \
    toolkit/editor.cpp \
    toolkit/utils.cpp

HEADERS += \
    toolkit/cli.h \
    toolkit/utils.h

SUBDIRS += \
    BinOM_tools.pro
