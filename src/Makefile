CXX_CMP = g++
CXX_FLAGS = -std=c++17 -O3 -fPIC -w
CXX_FLAGS_DBG = -std=c++17 -Wall -g
CXX_SRC = binom/sources/*.cpp binom/sources/variables/*.cpp binom/sources/file_storage/*.cpp binom/sources/utils/*.cpp
LD_SHARED_FLAGS = -fPIC -shared
LD_FLAGS =

ifeq ($(CXX_CMP), g++)
LIBS = -lpthread -lstdc++fs
endif
ifeq ($(CXX_CMP), clang++)
LIBS = -lpthread -lc++fs
endif

# Compile objects for libraries
objects:
	$(CXX_CMP) -c -I. $(CXX_SRC) $(CXX_FLAGS)

# Compile objects for Tcp
objects_tcp:
	$(CXX_CMP) -c -I./TcpServer ./TcpServer/tcp/src/*.cpp $(CXX_FLAGS)

# Compile objects for test (with debug symbols)
objects_dbg:
	$(CXX_CMP) -c -I. $(CXX_SRC) $(CXX_FLAGS_DBG)

# Compile objects for Tcp (with debug symbols)
objects_tcp_dbg:
	$(CXX_CMP) -c -I./TcpServer ./TcpServer/tcp/src/*.cpp $(CXX_FLAGS_DBG)

# Compile static library
libbinom.a: objects
	ar cr libbinom.a *.o

# Compile shared object
libbinom.so: objects
	$(CXX_CMP) $(LD_SHARED_FLAGS) $(LIBS) -o libbinom.so *.o

# Compile test programm with debug symbols
test_dbg: objects_dbg
	$(CXX_CMP) -I. test.cpp *.o -o test $(LIBS) $(CXX_FLAGS_DBG)

# Compile test programm
test: objects
	$(CXX_CMP) -I. test.cpp *.o -o test $(LIBS) $(CXX_FLAGS)

# Compile libraries and move to folder "lbinom" with C++ headers
libs: libbinom.a libbinom.so

# Toolkit
tk: libbinom.a
	make clean_o
	$(CXX_CMP) -I. ./toolkit/*.cpp ./toolkit.cpp -o binomtk $(LIBS) ./libbinom.a
	rm -rf ./*.o

all: libs tk
	mkdir ../build -p
	mv libbinom.a ../build
	mv libbinom.so ../build
	mv binomtk ../build
	cp binom/includes ../build -r
	make clean

clean:
	rm -rf ./*.o ./*.a test ./*.binom ./*.binomdb ./lbinom/ *.pro.user

# Remove all object files
clean_o:
	rm -rf *.o
