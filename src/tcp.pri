SOURCES += \
  TcpServer/tcp/src/TcpServer.cpp \
  TcpServer/tcp/src/TcpClient.cpp

HEADERS += \
  TcpServer/tcp/hdr/TcpServer.h \
  TcpServer/tcp/hdr/TcpClient.h

win32 {
LIBS += C:\Qt\Tools\mingw730_64\x86_64-w64-mingw32\lib\libws2_32.a
}
