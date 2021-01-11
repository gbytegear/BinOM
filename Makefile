libbinom.a:
	g++ -c -I. binom/sources/structure/*.cpp binom/sources/structure/variables/*.cpp binom/sources/structure/file/*.cpp
	ar rcs libbinom.a *.o
	rm -rf ./*.o

test: libbinom.a
	g++ -c -I. test.cpp
	g++ test.o libbinom.a -o test
	rm -rf ./*.o
