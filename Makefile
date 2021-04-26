# Compile objects for libraries
objects:
	g++ -c -I. binom/sources/*.cpp binom/sources/structure/*.cpp binom/sources/structure/variables/*.cpp binom/sources/structure/file_storage/*.cpp -std=c++17 -O3 -fPIC

# Compile objects for test (with debug symbols)
objects_dbg:
	g++ -c -I. binom/sources/*.cpp binom/sources/structure/*.cpp binom/sources/structure/variables/*.cpp binom/sources/structure/file_storage/*.cpp -lstdc++fs -std=c++17 -O3 -g

# Remove all objects
clean_o:
	rm -rf *.o

# Compile static library
libbinom.a: objects
	ar cr libbinom.a *.o

# Compile shared object
libbinom.so: objects
	g++ -fPIC -shared -o libbinom.so *.o

# Compile test programm
test: objects_dbg
	g++ test.cpp *.o -lstdc++fs -o test -std=c++17 -O3 -g

# Compile libraries and move to folder "lbinom" with C++ headers
libs: libbinom.a libbinom.so
	mkdir lbinom -p
	mv libbinom.a lbinom
	mv libbinom.so lbinom
	cp binom/includes/* lbinom -r

all: libs

clean:
	rm -rf ./*.o ./*.a test ./*.binom ./*.binomdb ./lbinom/ *.pro.user
