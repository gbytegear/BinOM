# <img src="https://gbytegear.github.io/BinOM/src/img/BinOM.ico" height="40">BinOM

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
Then you need to go to the directory with source files `./src/` and compile library:
```bash
$ cd BinOM/src
$ make all
```
The compiled library and BinOM ToolKit will be in `BinOM/build/`

Link library with your project:
```
$ g++ -Ipath/to/lbinom/includes -Lpath/to/lbinom/ -lbinom -lstdc++fs <your sources>
```

### Library usage example

```cpp
#include "binom/includes/binom.h"
#include "binom/includes/lexer.h"
#include <fstream>


int main() {

  // BinOM project namespace (all magic here)
  using namespace binom;

  // Create BinOM variable
  Variable var = varr{ // Array
    vobj{ // Object
      {"prop_1", 42_ui8}, // Byte
      {"prop_2", ui32arr{1,2,3,4,5}}, // Word array
      {"prop_3", "Hello, world!"} // Byte array from c-string
    }
  };

  // Create BinOM serialized storage file
  SerializedStorage s_storage("data.binom");
  s_storage = var; // Save Variable to BinOM file
  Variable loaded = s_storage; // Load Variable from BinOM file
  std::cout << loaded << '\n'; // Print Variable

  // Create BinOM dynamic storage file (with struct from var as default)
  DynamicStorage d_storage("data.binomdb", var);
  FileNodeVisitor root = d_storage;
  // Change "prop_3" variable in data.binomdb file
  root["prop_3"].setVariable("Goodbie, world!");
  // Get "prop_2" value from data.binomdb file in Variable
  Variable from_storgae = root.getVariable("prop_2");

  // Convert structure description to Variable (More details in the BSDL section)
  std::ifstream t("struct.bsdl");
  std::stringstream buffer;
  buffer << t.rdbuf();
  Variable data = lexer << buffer.str();

  // Serialize Variable
  ByteArray buffer = var.serialize();
  // ... And send it over network ...
}
```

### BinOM Struct Description Language (BSDL)
JSON-like structure description language (JSON-compatible)

#### BSDL Types:
* **byte** - `5_si8, 5_ui8, 5_i8, 0x05_ui8, 05_i8`
* **word** - `5_si16, 5_ui16, 5_i16, 0x05_i16, 05_i16`
* **dword** - `5, 0.5, 0.5_f32, 5_si32, 5_ui32, 5_i32, 0x05_i32`
* **qword** - `0.5_f64, 5_si64, 5_ui64, 5_i64, 0x05_i64`
* **byte array** - `i8[...], ui8[...], si8[...], "any string"`
* **word array** - `i16[...], ui16[...], si16[...]`
* **dword array** - `i32[...], ui32[...], si32[...]`
* **qword array** - `i64[...], ui64[...], si64[...]`
* **array** - `[...], arr[...]`
* **object** - `{...}, obj{...}`

All types of brackets (`{}, [], <> or ()`) are equivalent to each other when specifying the data type.

There can be two kinds of separator operators for name and value: `: or =`

Value separators: `, ; | \ white-space-symbols`

Comments: `//...\n or /*...*/`

Example:
```json
{
  usr: [
    { login: "admin", password: "admin", access_lvl: 0xFF_ui8, attributes: [], data: {} }
    { login: "guest", password: "", access_lvl: 0x00_ui8, attributes: [], data: {} }
    { login: "user", password: "any_user_password", access_lvl: 0x7F_ui8, attributes: [], data: {} }
  ],
  grp: [
    { name: "SYSTEM", users: [ "admin", "guest" ], usr_attributes: [ "unremovable", "const_access_lvl" ], change_access_lvl: 0xFF_ui8 }
  ],
  msg: {
    admin_guest: [
      { from: "admin", to: "guest", content: "Hello, Guest. Please, login with your account." }
    ]
  }
}
```

