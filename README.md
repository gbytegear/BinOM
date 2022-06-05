# <a href="https://gbytegear.github.io/BinOM/"><img src="https://gbytegear.github.io/BinOM/src/img/BinOM.ico" height="40">BinOM</a>

**BinOM**(*Binary Object Model*) - library for working with a hierarchical data format for general purposes.

## Basic goals:
* Development of a generic data format for building structures of any complexity
* Ensuring the most optimal read and data processing speed
* Development of tools for the most convenient work with data

## RoadMap
* [ ] Implement container classes
  * [x] `binom::Number`
  * [x] `binom::BitArray`
  * [x] `binom::BufferArray`
  * [x] `binom::Array`
  * [x] `binom::SinglyLinkedList`
  * [x] `binom::DoublyLinkedList`
  * [ ] `binom::Map`
* [ ] Implement serialization of BinOM containers
* [ ] Implement file storage
  * [ ] File memory manager
  * [ ] File node accessor

## Build & Run

### Library
For build shared and static library - run in project directory:
```bash
make lib -j
```
Library files will be placed in build directiry:
```
<BinOM Project directory>/build/libbinom.a
<BinOM Project directory>/build/libbinom.so
```
Link library with your project:
```bash
g++ -I<Path to BinOM project directory>/libbinom/include -L<Path to BinOM project directory>/build -lbinom -lpthread <your sources>
```

### Automatic test
For build and run automatic test - run in project directory:
```bash
make test -j
```

## BinOM Types info
### Types:
* null - NULL
* boolean - Boolean value
* ui8 - Unsigned 8-bit integer number
* si8 - Signed 8-bit integer number
* ui16 - Unsigned 16-bit integer number
* si16 - Signed 16-bit integer number
* ui32 - Unsigned 32-bit integer number
* si32 - Signed 32-bit integer number
* f32 - 32-bit number with floating point
* ui64 - Unsigned 64-bit integer number
* si64 - Signed 64-bit integer number
* f64 - 64-bit number with floating point
* bit_array - Array of boolean values
* ui8_array - Array of unsigned 8-bit integer numbers
* si8_array - Array of signed 8-bit integer numbers
* ui16_array - Array of unsigned 16-bit integer numbers
* si16_array - Array of signed 16-bit integer numbers
* ui32_array - Array of unsigned 32-bit integer numbers
* si32_array - Array of signed 32-bit integer numbers
* f32_array - Array of 32-bit numbers with floating point
* ui64_array - Array of unsigned 64-bit integer numbers
* si64_array - Array of signed 64-bit integer numbers
* f64_array - Array of 64-bit numbers with floating point
* array - Heterogeneous array
* singly_linked_list - Heterogeneous singly linked list
* doubly_linked_list - Heterogeneous doubly linked list
* less_map - Associative heterogeneous container with key-sorted by descending
* greater_map - Associative heterogeneous container with key-sorted by ascending

### Type properties:
#### Type class enum / C++ Class:
* null - `/*Class not provided*/` - null
* number - `binom::Number` - container for numeric data types;
* bit_array - `binom::BitArray` - сontainer for boolean values;
* buffer_array - `binom::BufferArray` - сontainer for the same type of numeric values;
* array - `binom::Array` - heterogeneous array;
* singly_linked_list - `binom::SinglyLinkedList` - heterogeneous singly linked list;
* doubly_linked_list - `binom::DoublyLinkedList` - heterogeneous doubly linked list;
* map - `binom::Map` - sorted associative container that contains key-value pairs with unique keys.

#### Number value widths:
* byte - 8 bit width;
* word - 16 bit width;
* dword - 32 bit width;
* qword - 64 bit width.

#### Number value types:
* unsigned_integer;
* signed_integer;
* float_point.

#### Container Sort Type:
* unsorted;
* less;
* greater;

#### Value type:
* boolean - Boolean value
* ui8 - Unsigned 8-bit integer number
* si8 - Signed 8-bit integer number
* ui16 - Unsigned 16-bit integer number
* si16 - Signed 16-bit integer number
* ui32 - Unsigned 32-bit integer number
* si32 - Signed 32-bit integer number
* f32 - 32-bit number with floating point
* ui64 - Unsigned 64-bit integer number
* si64 - Signed 64-bit integer number
