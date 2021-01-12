//#include "binom/includes/structure/variables/variable.h"
#include "binom/includes/binom.h"
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

inline void testNodeVisitor() {
  Variable arr = varr {
    8_ui8,
    16_ui16,
    32_ui32,
    64_ui64,
    ui8arr{1,2,3},
    ui16arr{1,2,3},
    ui32arr{1,2,3},
    ui64arr{1,2,3},
    varr {
      8_ui8,
      16_ui16,
      32_ui32,
      64_ui64,
    },
    obj {
      {"byte", 8_ui8},
      {"word", 16_ui16},
      {"dword", 32_ui32},
      {"qword", 64_ui64}
    }
  };

  NodeVisitor node(&arr);

  std::clog << "Root: " << node.getVariable() << '\n';

  std::clog << "8th element: "<< node.stepInside(8).getVariable() << '\n';

  node = &arr;

  std::clog << "9th/\"word\" element: " << node.stepInside(9).stepInside("word").getVariable() << '\n';

  std::clog << "Foreach test object:\n";

  (node = &arr).stepInside (9);

  for(NodeVisitor child : node) {
    std::clog << child.getNamedVariable ().name.toString () << ':' << child.getVariable () << '\n';
  }

  std::clog << "Foreach test array:\n";

  (node = &arr).stepInside (8);

  for(NodeVisitor child : node) {
    std::clog << child.getVariable() << '\n';
  }

  std::clog << "Foreach test buffer array:\n";

  (node = &arr).stepInside (4);

  for(NodeVisitor child : node) {
    std::clog << child.getValue() << '\n';
  }

}

#include <cstdio>

std::ostream& operator<<(std::ostream& os, const binom::ByteArray byte_array) {
  for(binom::byte b : byte_array) {
    os << std::right << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ui16(b) << ' ';
  }
  return os;
}

inline void testSerialization() {
  Variable arr = varr {
    8_ui8,
    16_ui16,
    32_ui32,
    64_ui64,
    ui8arr{1,2,3},
    ui16arr{1,2,3},
    ui32arr{1,2,3},
    ui64arr{1,2,3},
    "Hello world",
    varr {
      8_ui8,
      16_ui16,
      32_ui32,
      64_ui64,
      },
    obj {
      {"byte", 8_ui8},
      {"word", 16_ui16},
      {"dword", 32_ui32},
      {"qword", 64_ui64}
    }
  };

  ByteArray ser(arr.serialize());

  FILE* file = fopen64 ("test.binom", "w+");
  fwrite (ser.begin (), 1, ser.length (), file);
  fclose (file);

  std::clog << "Serialized:\n" << ser << "\nSize: " << std::dec << ser.length () << " bytes\n";

}

#include <cmath>


inline void testChainNumber() {

  auto toChainNumber = [](ui64 number)->ByteArray {

    auto shiftBits = [](ui8 prev, ui8 next, ui8 shift)->ui8{return (next << shift) + (prev >> (8 - shift));};
    auto get7Bit = [](ui8 val)->ui8 {return (val > 127)? val - 128 : val;};

    ui8* number_bytes = reinterpret_cast<ui8*>(&number);
    ByteArray bytes((number == 0) ? 1 : static_cast<ui64>(floor (log10 (number) / log10 (128))) + 1);
    for(ui8 i = 0; i < bytes.length(); ++i) {
      bytes[i] = get7Bit(shiftBits(number_bytes[i - 1], number_bytes[i], i));
      if(i == bytes.length() - 1)
        bytes[i] = (bytes[i] > 127) ? bytes[i] - 128 : bytes[i];
      else
        bytes[i] |= 128;
    }
    return bytes;
  };

  auto fromChainNumber = [](ByteArray::iterator it)->ui64 {
    auto get7Bit = [](ui8 val)->ui8 {return (val > 127)? val - 128 : val;};

    ui64 number = 0;
    ui8 i = 0;
    for(; it[i] > 127; ++i) {
      number += static_cast<ui64>( get7Bit(it[i]) * pow(128,i));
    }
    number += static_cast<ui64>(get7Bit(it[i]) * pow(128,i));
    return number;
  };

  for(ui64 start_number = 1; start_number < 9223372036854775808ull; start_number += start_number) {
    ByteArray bytes(toChainNumber(start_number));
    ui64 end_number = fromChainNumber (bytes.begin ());

    std::clog << std::dec << start_number << "( " << ByteArray(&start_number, 8) << ')'
              << " => " << bytes
              << " => " << std::dec << end_number << "( " << ByteArray(&end_number, 8) << ')'
              << ((start_number != end_number)? "Err!" : "") << '\n';
  }
}


inline void testDeserialize() {
  Primitive pri(65535_ui64);
  ByteArray ser = pri.serialize ();
  ByteArray::iterator it = ser.begin();
  Primitive new_pri(Primitive::deserialize(it));

  std::clog
    << "Preserialize: " << pri << '\n'
    << "Serialized: " << ser << '\n'
    << "Deserialized: " << new_pri << '\n';


  BufferArray array = "Hello world";
  ser = array.serialize();
  it = ser.begin();
  BufferArray new_arr(BufferArray::deserialize (it));

  std::clog
      << "Preserialize: " << array.toString() << '\n'
      << "Serialized: " << ser << '\n'
      << "Deserialized: " << new_arr.toString() << '\n';
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
    std::clog << "===================================================================\n";
    testNodeVisitor();
    std::clog << "===================================================================\n";
    testChainNumber ();
    std::clog << "===================================================================\n";
    testSerialization ();
    std::clog << "===================================================================\n";
    testDeserialize ();





  } catch(binom::SException except) {
    std::cerr << binom::SException::ectos(except.code()) << except.what() << std::endl;
  } catch(...) {
    std::cerr << "Unknown exception!" << std::endl;
  }

  return 0;
}
