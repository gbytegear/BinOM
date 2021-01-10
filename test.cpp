#include "binom/includes/structure/variables/variable.h"
#include <cassert>

using namespace binom;

inline void testPrimitiver() {
  std::clog << "Primitive test:\n";
  Variable byte = 8_ui8;
  std::clog << "|Byte:         " << byte.toPrimitive() << std::endl;
  Variable word = 16_ui16;
  std::clog << "|Word:         " << word.toPrimitive() << std::endl;
  Variable dword = 32_ui32;
  std::clog << "|Dword:        " << dword.toPrimitive() << std::endl;
  Variable qword = 64_ui64;
  std::clog << "|Qword:        " << qword.toPrimitive() << std::endl;


  byte.toPrimitive() = 8_ui64;
  std::clog << "|Byte:         " << byte.toPrimitive() << std::endl;
  word.toPrimitive() = 4_ui64;
  std::clog << "|Word:         " << word.toPrimitive() << std::endl;
  dword.toPrimitive() = 2_ui64;
  std::clog << "|Dword:        " << dword.toPrimitive() << std::endl;
  qword.toPrimitive() = 1_ui64;
  std::clog << "|Qword:        " << qword.toPrimitive() << std::endl;

}

inline void testBufferArray() {
  Variable byte_array = "Hello world";
  Variable word_array = ui16arr{1, 2, 3};
  Variable dword_array = ui32arr{1, 2, 3};
  Variable qword_array = ui64arr{1, 2, 3};

  std::clog << "Buffer array test:\n";
  std::clog << "|Byte array:   " << byte_array.toBufferArray() << std::endl;
  std::clog << "|String:       " << byte_array.toBufferArray().toString() << std::endl;
  std::clog << "|Word array:   " << word_array.toBufferArray() << std::endl;
  std::clog << "|Dword array:  " << dword_array.toBufferArray() << std::endl;
  std::clog << "|Qword array:  " << qword_array.toBufferArray() << std::endl;

  byte_array.toBufferArray() += byte_array.toBufferArray();
  std::clog << "|Byte array:   " << byte_array.toBufferArray() << std::endl;
  std::clog << "|String:       " << byte_array.toBufferArray().toString() << std::endl;

  word_array.toBufferArray() += word_array.toBufferArray();
  std::clog << "|Word array:   " << word_array.toBufferArray() << std::endl;

  dword_array.toBufferArray() += dword_array.toBufferArray();
  std::clog << "|Dword array:  " << dword_array.toBufferArray() << std::endl;

  qword_array.toBufferArray() += qword_array.toBufferArray();
  std::clog << "|Qword array:  " << qword_array.toBufferArray() << std::endl;

  std::clog << "Buffer compare test:\n"
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

  std::clog << "Add/remove operations:\n"
               "|Start string: " << byte_array.toBufferArray().toString() << "\n"
               "|Subarr: " << byte_array.toBufferArray().subarr(12, 13).toString() << '\n';


  std::clog << "|Operations:\n"
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

  std::clog << "|End string: " << byte_array.toBufferArray().toString() << "\n";
}


inline void testArray() {

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
  std::clog << arr << '\n';

  arr.toArray().popBack(2);
  arr.toArray().popFront(2);

  std::clog << arr << '\n';

  arr.toArray().clear();

  std::clog << arr << '\n';

  arr.toArray().pushBack(ui8arr{1,2,3});
  arr.toArray().pushBack(ui16arr{1,2,3});
  arr.toArray().pushBack(ui32arr{1,2,3});
  arr.toArray().pushBack(ui64arr{1,2,3});
  arr.toArray().pushFront(64_ui64);
  arr.toArray().pushFront(32_ui32);
  arr.toArray().pushFront(16_ui16);
  arr.toArray().pushFront(8_ui8);
  arr.toArray().pushFront(varr {
                            8_ui8,
                            16_ui16,
                            32_ui32,
                            64_ui64,
                            ui8arr{1,2,3},
                            ui16arr{1,2,3},
                            ui32arr{1,2,3},
                            ui64arr{1,2,3}
                         });
  arr.toArray().pushFront(obj{
                            {"4",64_ui64},
                            {"9",ui16arr{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}},
                            {"3",32_ui32},
                            {"00",ui16arr{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}},
                            {"5",64_ui64},
                            {"2",16_ui16},
                            {"1",8_ui8},
                            {"6",ui16arr{1,2,3,4,5,6,7,8,9,10}},
                            {"0","Hello world"}
                          });

  std::clog << arr << '\n';
}

inline void testObject() {
  Variable object = obj{
    {"4",64_ui64},
    {"9",ui16arr{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}},
    {"3",32_ui32},
    {"00",ui16arr{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}},
    {"5",64_ui64},
    {"2",16_ui16},
    {"1",8_ui8},
    {"6",ui16arr{1,2,3,4,5,6,7,8,9,10}},
    {"0","Hello world"}
  };

  std::clog << object << '\n';

  object.toObject().insert("01",8_ui8);
  object.toObject().insert("10",8_ui8);
  object.toObject().insert("8",8_ui8);
  object.toObject().insert("7",8_ui8);
  object.toObject().insert("Hello world","Hello world");
  object.toObject().remove("4");

  object.toObject() += {"Hello", "World"};

  std::clog << object << '\n';

  std::clog << object.toObject().getVariable("Hello world").toBufferArray().toString() << '\n';
}


int main() {
  try {
    std::clog << "===================================================================\n";
    testPrimitiver();
    std::clog << "===================================================================\n";
    testBufferArray();
    std::clog << "===================================================================\n";
    testArray();
    std::clog << "===================================================================\n";
    testObject();



  } catch(binom::SException except) {
    std::cerr << binom::SException::ectos(except.code()) << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
