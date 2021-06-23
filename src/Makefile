# Compile objects for libraries
objects:
	g++ -c -I. binom/sources/*.cpp binom/sources/structure/*.cpp binom/sources/structure/variables/*.cpp binom/sources/structure/file_storage/*.cpp -std=c++17 -O3 -fPIC -w

# Compile objects for Tcp
objects_tcp:
	g++ -c -I./TcpServer ./TcpServer/tcp/src/*.cpp -std=c++17 -O3 -fPIC -w

# Compile objects for test (with debug symbols)
objects_dbg:
	g++ -c -I. binom/sources/*.cpp binom/sources/structure/*.cpp binom/sources/structure/variables/*.cpp binom/sources/structure/file_storage/*.cpp -lstdc++fs -std=c++17 -O3 -g

# Compile objects for Tcp (with debug symbols)
objects_tcp_dbg:
	g++ -c -I./TcpServer ./TcpServer/tcp/src/*.cpp -std=c++17 -O3 -g -fPIC

# Compile objects for ToolKit
objects_tk: objects
	g++ -c -I. toolkit/*cpp -std=c++17 -O3 -fPIC -w

# Compile objects for ToolKit (with debug symbols)
objects_tk_dbg: objects_dbg
	g++ -c -I. toolkit/*cpp -std=c++17 -O3 -fPIC -g

# Remove all objects
clean_o:
	rm -rf *.o

# Compile static library
libbinom.a: objects
	ar cr libbinom.a *.o

# Compile shared object
libbinom.so: objects
	g++ -fPIC -shared -o libbinom.so *.o

# Compile & run test programm
test: objects_dbg objects_tcp_dbg
	g++ test.cpp *.o -lstdc++fs -o test -std=c++17 -O3 -g
	./test

# Compile libraries and move to folder "lbinom" with C++ headers
libs: libbinom.a libbinom.so
	mkdir lbinom -p
	mv libbinom.a lbinom
	mv libbinom.so lbinom
	cp binom/includes lbinom -r
	make clean_o

# BinOM Command Line ToolKit (with debug symbols)
toolkit_dbg: objects_tk_dbg
	g++ toolkit.cpp *.o -lstdc++fs -o binomtk -std=c++17 -O3 -w
	make clean_o

# BinOM Command Line ToolKit
toolkit: objects_tk
	g++ toolkit.cpp *.o -lstdc++fs -o binomtk -std=c++17 -O3 -w
	make clean_o

all: libs toolkit

clean:
	rm -rf ./*.o ./*.a test ./*.binom ./*.binomdb ./lbinom/ *.pro.user
