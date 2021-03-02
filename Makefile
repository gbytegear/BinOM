libbinom.a:
	g++ -c -I. binom/sources/*.cpp binom/sources/structure/*.cpp binom/sources/structure/variables/*.cpp binom/sources/structure/file_storage/*.cpp -lstdc++fs -std=gnu++2a
	ar cr libbinom.a *.o
	rm -rf ./*.o

test: libbinom.a
	g++ test.cpp -L/home/oldev/projects/BinOM -lbinom -lstdc++fs -o test

clean:
	rm -rf ./*.o ./*.a test ./*.binom ./*.binomdb
