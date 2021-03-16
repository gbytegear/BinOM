
objects:
	g++-10 -c -I. binom/sources/*.cpp binom/sources/structure/*.cpp binom/sources/structure/variables/*.cpp binom/sources/structure/file_storage/*.cpp -lstdc++fs -std=c++20 -O3 -fPIC

clean_o:
	rm -rf *.o

libbinom.a: objects
	ar cr libbinom.a *.o

libbinom.so: objects
	g++-10 -fPIC -shared -o libbinom.so *.o

test: libbinom.a
	g++-10 test.cpp -L/home/oldev/projects/BinOM -lbinom -lstdc++fs -o test -std=c++20 -O3

all: libbinom.so libbinom.a test clean_o

clean:
	rm -rf ./*.o ./*.a test ./*.binom ./*.binomdb
