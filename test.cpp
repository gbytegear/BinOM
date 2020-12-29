#include "binom/includes/structure/variables/variable.h"
#include <cassert>

using namespace binom;

void testPrimitiver() {
  std::cout << "Primitive test:\n";
  Variable byte = 8_ui8;
  std::cout << "|Byte:         " << byte.toPrimitive() << std::endl;
  Variable word = 16_ui16;
  std::cout << "|Word:         " << word.toPrimitive() << std::endl;
  Variable dword = 32_ui32;
  std::cout << "|Dword:        " << dword.toPrimitive() << std::endl;
  Variable qword = 64_ui64;
  std::cout << "|Qword:        " << qword.toPrimitive() << std::endl;


  byte.toPrimitive() = 8_ui64;
  std::cout << "|Byte:         " << byte.toPrimitive() << std::endl;
  word.toPrimitive() = 4_ui64;
  std::cout << "|Word:         " << word.toPrimitive() << std::endl;
  dword.toPrimitive() = 2_ui64;
  std::cout << "|Dword:        " << dword.toPrimitive() << std::endl;
  qword.toPrimitive() = 1_ui64;
  std::cout << "|Qword:        " << qword.toPrimitive() << std::endl;

}

void testBufferArray() {
  Variable byte_array = "Hello world";
  Variable word_array = ui16arr{1, 2, 3};
  Variable dword_array = ui32arr{1, 2, 3};
  Variable qword_array = ui64arr{1, 2, 3};

  std::cout << "Buffer array test:\n";
  std::cout << "|Byte array:   " << byte_array.toBufferArray() << std::endl;
  std::cout << "|String:       " << byte_array.toBufferArray().toString() << std::endl;
  std::cout << "|Word array:   " << word_array.toBufferArray() << std::endl;
  std::cout << "|Dword array:  " << dword_array.toBufferArray() << std::endl;
  std::cout << "|Qword array:  " << qword_array.toBufferArray() << std::endl;

  byte_array.toBufferArray() += byte_array.toBufferArray();
  std::cout << "|Byte array:   " << byte_array.toBufferArray() << std::endl;
  std::cout << "|String:       " << byte_array.toBufferArray().toString() << std::endl;

  word_array.toBufferArray() += word_array.toBufferArray();
  std::cout << "|Word array:   " << word_array.toBufferArray() << std::endl;

  dword_array.toBufferArray() += dword_array.toBufferArray();
  std::cout << "|Dword array:  " << dword_array.toBufferArray() << std::endl;

  qword_array.toBufferArray() += qword_array.toBufferArray();
  std::cout << "|Qword array:  " << qword_array.toBufferArray() << std::endl;

  std::cout << "Buffer compare test:\n"
               "|test == test: " << ("test"_buffer == "test"_buffer) << "\n"
               "|test == _test: " << ("test"_buffer == "_test"_buffer) << "\n"
               "|test != test: " << ("test"_buffer != "test"_buffer) << "\n"
               "|test != _test: " << ("test"_buffer != "_test"_buffer) << "\n"
               "|1 > 0: " << ("1"_buffer > "0"_buffer) << "\n"
               "|0 > 0: " << ("0"_buffer > "0"_buffer) << "\n"
               "|0 > 1: " << ("0"_buffer > "1"_buffer) << "\n"
               "|1 < 0: " << ("1"_buffer < "0"_buffer) << "\n"
               "|0 < 0: " << ("0"_buffer < "0"_buffer) << "\n"
               "|0 < 1: " << ("0"_buffer < "1"_buffer) << "\n"
               "|1 >= 0: " << ("1"_buffer >= "0"_buffer) << "\n"
               "|0 >= 0: " << ("0"_buffer >= "0"_buffer) << "\n"
               "|0 >= 1: " << ("0"_buffer >= "1"_buffer) << "\n"
               "|1 <= 0: " << ("1"_buffer <= "0"_buffer) << "\n"
               "|0 <= 0: " << ("0"_buffer <= "0"_buffer) << "\n"
               "|0 <= 1: " << ("0"_buffer <= "1"_buffer) << "\n";

  byte_array.toBufferArray() = "Hello World!HELLO!!!WORLD!Hello World";

  std::cout << "Add/remove operations:\n"
               "|Start string: " << byte_array.toBufferArray().toString() << "\n"
               "|Subarr: " << byte_array.toBufferArray().subarr(12, 13).toString() << '\n';


  std::cout << "|Operations:\n"
               "||popBack(11)\n"
               "||popFront(11)\n"
               "||popBack()\n"
               "||popFront()\n"
               "||remove(5,2)\n"
               "||remove(5)\n"
               "||insert(5, ui64(' '))\n";

  byte_array.toBufferArray().popBack(11);
  byte_array.toBufferArray().popFront(11);
  byte_array.toBufferArray().popBack();
  byte_array.toBufferArray().popFront();
  byte_array.toBufferArray().remove(5,2);
  byte_array.toBufferArray().remove(5);
  byte_array.toBufferArray().insert(5, ui64(' '));

  std::cout << "|End string: " << byte_array.toBufferArray().toString() << "\n";
}


void testArray() {

  Variable arr = varr {
                 8_ui8,
                 16_ui16,
                 32_ui32,
                 64_ui64,
                 ui8arr{1,2,3},
                 ui16arr{1,2,3},
                 ui32arr{1,2,3},
                 ui64arr{1,2,3}
              };
  std::cout << arr;

  arr.toArray().popBack(2);
  arr.toArray().popFront(2);

  std::cout << arr;

  arr.toArray().clear();

  std::cout << arr;

  arr.toArray().pushBack(ui8arr{1,2,3});
  arr.toArray().pushBack(ui16arr{1,2,3});
  arr.toArray().pushBack(ui32arr{1,2,3});
  arr.toArray().pushBack(ui64arr{1,2,3});
  arr.toArray().pushFront(64_ui64);
  arr.toArray().pushFront(32_ui32);
  arr.toArray().pushFront(16_ui16);
  arr.toArray().pushFront(8_ui8);

  std::cout << arr;
}


int main() {
  try {
    std::cout << "===================================================================\n";
    testPrimitiver();
    std::cout << "===================================================================\n";
    testBufferArray();
    std::cout << "===================================================================\n";
    testArray();

  } catch(binom::SException except) {
    std::cout << binom::SException::ectos(except.code()) << except.what() << std::endl;
  } catch(...) {
    std::cout << "Unknown exception!" << std::endl;
  }

  return 0;
}
