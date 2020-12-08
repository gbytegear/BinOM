#include "binom/includes/structure/variables/variable.h"

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
  std::cout << "Buffer array test:\n";
  Variable byte_array = "Hello world";
  std::cout << "|Byte array:   " << byte_array.toBufferArray() << std::endl;
  std::cout << "|String:       " << byte_array.toBufferArray().toString() << std::endl;
  Variable word_array = ui16arr{1, 2, 3};
  std::cout << "|Word array:   " << word_array.toBufferArray() << std::endl;
  Variable dword_array = ui32arr{1, 2, 3};
  std::cout << "|Dword array:  " << dword_array.toBufferArray() << std::endl;
  Variable qword_array = ui64arr{1, 2, 3};
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
}


int main() {
  try {
    testPrimitiver();
    testBufferArray();
  } catch(binom::SException except) {
    std::cout << binom::SException::ectos(except.code()) << except.what() << std::endl;
  } catch(...) {
    std::cout << "Unknown exception!" << std::endl;
  }

  return 0;
}
