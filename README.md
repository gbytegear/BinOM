# <img src="https://gbytegear.github.io/BinOM/src/img/BinOM.ico" height="40">BinOM

* [Definitions](#definitions)
* [How to...](#how-to)
  * [Build library](#build-library)
  * [Link library with your programm](#link-library-with-your-programm)
<!--
  * Use library
    * Create BinOM Variable
    * BinOM Variable Data Types interfaces
    * Serialize BinOM Variable and write to file
    * NodeVisitor usage
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

For build BinOM library from source code from this repository, required GCC compile and Make utilite:
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
#include <path-to-binom-include-dir/binom.h>

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

Build & test:

```bash
$ g++ -o test test.cpp -L$PWD -lbinom
$ ./test
```
