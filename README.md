# <img src="https://gbytegear.github.io/BinOM/src/img/BinOM.ico" height="40">BinOM

* [Definitions](#definitions)
* [How to...](#how-to)
  * [Build library](#build-library)
  * [Link library with your programm](#link-library-with-your-programm)
  * [Usage BinOM library](#usage-binom-library)
    * [BinOM Variable](#binom-variable)
    * [NodeVisitor usage](#nodevisitor-usage)
 <!--
    * BinOM Variable Data Types interfaces
    * Serialize BinOM Variable and write to file
    * Create DataBase container
    * DBNodeVisitor Usage
-->

## Definitions

**BinOM**(*Binary Object Model*) - serializtion / deserializtion data format implemnted as C++ library

### BinOM Data Types
* Primitive type class:
  * **byte** - 8 bit variable
  * **word** - 16 bit variable
  * **dword** - 32 bit variable
  * **qword** - 64 bit variable
* Buffer Array type class:
  * **byte array** - array of 8 bit values (*Can be interpreted as UTF-8 string*)
  * **word array** - array of 16 bit values (*Can be interpreted as UTF-16 string*)
  * **dword array** - array of 32 bit values
  * **qword array** - array of 64 bit values
* Array type class:
  * **array** - heterogeneous array
* Object type class:
  * **object** - heterogeneous "key-value" container

## How to...
### Build library

For build BinOM library from source code from this repository, required GCC compile and Make utilite, or MinGw for windows:
```bash
# On Debian/Ubuntu and other GNU/Linux distributions with APT package manager
$ sudo apt install gcc
$ sudo apt install make
# On CentOS/RadHat and other GNU/Linux distributions with RPM package manager
$ sudo yum install gcc
$ sudo yum install make
```
You can download source files from browser in .zip archive, or use git client:
```bash
$ git clone https://github.com/gbytegear/BinOM.git
```
Then you need to go to the directory with source files and compile library:
```bash
$ cd BinOM
# Compile as static library
$ make libbinom.a
# Compile as shared library
$ make libbinom.so
```
The compiled library will be in the current directory

### Link library with your programm
For link with your programm of both of types library you need C++ header files. You can find header files in `binom/includes/` directory:

Use this programm for test library linking:

```cpp
// test.cpp
#include <binom/includes/binom.h>

using namespace binom;

int main() {
  Variable test_var = obj{
  {"usr", varr{
    obj{
      {"id", 0_ui64},
      {"login", "admin"},
      {"password", "admin"},
      {"access_lvl", 0xff_ui8},
      {"props", obj{
        {"db_access", 0xff_ui8},
        {"server_cli_access", 0xff_ui8}
      }}
    },
    obj{
      {"id", 0_ui64},
      {"login", "guest"},
      {"password", "guest"},
      {"access_lvl", 0x00_ui8},
      {"props", obj{}}
    },
  }},
  {"grp", varr{
    obj{
      {"id", 0_ui64},
      {"name", "system"},
      {"owner", 0_ui64},
      {"members", ui64arr{0_ui64}},
      {"access_lvl", 0xff_ui8},
      {"props", obj{
        {"db_access", 0xff_ui8},
        {"server_cli_access", 0xff_ui8}
      }}
    },
    obj{
      {"id", 0_ui64},
      {"name", "unlogined"},
      {"owner", 0_ui64},
      {"members", ui64arr{1_ui64}},
      {"access_lvl", 0x00_ui8 },
      {"props", obj{}}
    }
  }}
  };

  BinOM::saveToFile("test.binom", test_var);

  return 0;
}

```

Build & run:

```bash
# In directory with source files of your project
$ mkdir -p binom/includes
$ cp <path to dir with binom source files>/binom/includes/* ./binom/includes
$ g++ -o test test.cpp -I. -L$PWD -lbinom
$ ./test
```

## Usage BinOM library
### BinOM Variable

```cpp
using namespace binom;

//...

// Create BinOM Variable:

Variable var = varr{
  1_ui8,                   // byte
  -2_i16,                  // word
  3_ui32,                  // dword
  -4_i64,                  // qword
  ui8arr{1,2,3},           // byte_array
  i16arr{-4,-5,-6},        // word_array
  ui32arr{7,8,9},          // dword_array
  i64arr{-10,-11,-12},     // qword_array
  "Hello world",           // byte_array from UTF-8 string(unix)/ASCII string(windows)
  varr{                    // array
    1_ui8,
    -2_i16,
    3_ui32,
    -4_i64,
  },
  obj{                     // object
    {"prop_1", 1_ui8},
    {"other_prop_2", -2_i16},
    {"prop with spaces in name", 3_ui32},
    {"property", -4_i64}
  }
};

//...

// Handly getting data from Variable


/* toArray() - interpret Variable as Array
 * getVariable(0) - get variable with index 0 from array container
 * toPrimitive() -  interpret Variable as Primitive
 * getValue() - get value reference(ValueRef) from Primitive
 * asUnsigned() - cast value reference(ValueRef) to unsigned 64-bit int
*/
ui64 number = var
                 .toArray()
                 .getVariable(0)
                 .toPrimitive()
                 .getValue()
                 .asUnsigned();

/* toArray() - interpret Variable as Array
 * getVariable(8) - get variable with index 0 from array container
 * toBufferArray() -  interpret Variable as BufferArray
 * toString() - cast BufferArray to std::string
*/
std::string str = var
                     .toArray()
                     .getVariable(8)
                     .toBufferArray()
                     .toString();
/* toArray() - interpret Variable as Array
 * getVariable(10) - get variable with index 10 from array container
 * toObject() -  interpret Variable as Object
 * getVariable("property") - get variable with name property from object container
 * toPrimitive() -  interpret Variable as Primitive
 * getValue() - get value reference(ValueRef) from Primitive
 * asSigned() - cast value reference(ValueRef) to signed 64-bit int
*/
i64 snumber = var
                 .toArray()
                 .getVariable(10)
                 .toObject()
                 .getVariable("property")
                 .toPrimitive()
                 .getValue()
                 .asSigned();

```

### NodeVisitor usage

NodeVisitor - interface for traversing the nodes of the BinOM structure

```cpp

/* var Variable strucuture
 * Array(11) [
 * | 0: 01
 * | 1: FFFE
 * | 2: 00000003
 * | 3: FFFFFFFFFFFFFFFC
 * | 4: 01 02 03
 * | 5: FFFC FFFB FFFA
 * | 6: 00000007 00000008 00000009
 * | 7: FFFFFFFFFFFFFFF6 FFFFFFFFFFFFFFF5 FFFFFFFFFFFFFFF4
 * | 8: 48 65 6C 6C 6F 20 77 6F 72 6C 64
 * | 9: Array(4) [
 * || 0: 01
 * || 1: FFFE
 * || 2: 00000003
 * || 3: FFFFFFFFFFFFFFFC
 * |]
 * | 10: Object(4) {
 * || prop_1: 01
 * || other_prop_2: FFFE
 * || prop with spaces in name: 00000003
 * || property: FFFFFFFFFFFFFFFC
 * |}
 * ]
*/


/*
 * NodeVisitor constructs as pointer on root element of structure
 * Array(11) [ <--- NodeVisitor node_visitor
 * | 0: 01
 * | 1: FFFE
 * ...
*/
NodeVisitor node_visitor(&var);

/* getChild(9) - create new NodeVisitor pointed on child node with index 9
 * Array(11) [ <--- NodeVisitor node_visitor
 * | 0: 01                    |
 * | 1: FFFE                  | create new
 * ...                        ᐯ
 * | 9: Array(4) [ <--- new NodeVisitor
 * || 0: 01
 * || 1: FFFE
 * ...
 * 
 * stepInside(2) - move NodeVisitor to child node with index 2
 * | 9: Array(4) [ <--------- *
 * || 0: 01                   | move
 * || 1: FFFE                 ᐯ
 * || 2: 00000003 <--- NodeVisitor
 * || 3: FFFFFFFFFFFFFFFC
 * |]
 * 
 * getValue() - get value reference(ValueRef) from NodeVisitor
 * asUnsigned() - cast value reference(ValueRef) to unsigned 64-bit int
*/
ui64 number_1 = node_visitor.getChild(9).stepInside(2).getValue().asUnsigned();
// Analogues: 
/* ui64 number_1 = node_visitor[9](2).asUnsigned();
 * ui64 number_1 = node_visitor[{9,2}].asUnsigned();
*/


```
