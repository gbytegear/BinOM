# <a href="https://gbytegear.github.io/BinOM/"><img src="https://gbytegear.github.io/BinOM/src/img/BinOM.ico" height="40">BinOM</a>

**BinOM**(*Binary Object Model*) - library for working with a hierarchical data format for general purposes.

**WARNING:** This project is at the testing stage, you should not use it in production!

### BinOM Data Types

<table>
 <tr>
  <th>Type</th><th>Definition</th><th>Value</th>
 </tr>
 <tr>
  <th colspan="3">Primitive type class</th>
 </tr>
 <tr>
  <th>byte</th>
  <td>8 bit variable</td>
  <td>Unsigned: 0..255;<br>
   Signed: -128..127;
  </td>
 </tr>
 <tr>
  <th>word</th>
  <td>16 bit variable</td>
  <td>
   Unsigned: 0..65535;<br>
   Signed:-32768..32767;
  </td>
 </tr>
 <tr>
  <th>dword</th>
  <td>32 bit variable</td>
  <td>
   Unsigned: 0..4294967295;<br>
   Signed: -2147483648..2147483647;<br>
   Float: +/-3.4E-38..3.4E+38
  </td>
 </tr>
 <tr>
  <th>qword</th>
  <td>64 bit variable</td>
  <td>
   Unsigned: 0..18446744073709551615;<br>
   Signed: -9223372036854775808..9223372036854775807;<br>
   Float: +/-1.7E-308..1.7E+308
  </td>
 </tr>
 <tr>
  <th colspan="3">Buffer Array type class</th>
 </tr>
 <tr>
  <th>byte array</th>
  <td>Array of 8 bit values</td>
  <td>Array of bytes</td>
 </tr>
 <tr>
  <th>word array</th>
  <td>Array of 16 bit values</td>
  <td>Array of words</td>
 </tr>
 <tr>
  <th>dword array</th>
  <td>Array of 32 bit values</td>
  <td>Array of dwords</td>
 </tr>
 <tr>
  <th>qword array</th>
  <td>Array of 64 bit values</td>
  <td>Array of qwords</td>
 </tr>
 <tr>
  <th colspan="3">Array type class</th>
 </tr>
 <tr>
  <th>array</th>
  <td>Heterogeneous array</td>
  <td>Array of any BinOM types</td>
 </tr>
 <tr>
  <th colspan="3">Object type class</th>
 </tr>
 <tr>
  <th>object</th>
  <td>Heterogeneous "key-value" container</td>
  <td>Map with BuffeArray type key and any BinOM type value</td>
 </tr>
 
</table>

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
$ cd BinOM
$ make all
```
The compiled library and BinOM ToolKit will be in `BinOM/build/`

Link library with your project:
```
$ g++ -Ipath/to/lbinom/includes -Lpath/to/libbinom/ -lbinom -lstdc++fs <your sources>
```
#### Supported compliers and arhictectures:
There are plans to test this project on all the latest compiler releases, but the following are currently supported: 
* Windows MinGw (x32, x64)
* Linux GCC (x32, x64, aarch64)
* Linux Clang (aarch64)

The code of this project can be compiled on other compilers that support *C++17* and *std::filesystems*, but the project has not been tested using them.
Be careful and report bugs when you encounter problems. When reporting a build error, please add the name and version of the compiler and the output to the console with errors to the message. Please make sure your compiler supports *C++17* and *std::filesystems* when reporting a bug. 


### BinOM C++ Classes
* `binom::Variable` - container for storing and processing BinOM data in RAM
  * `binom::Primitive` - container for primitive data types
  * `binom::BufferArray` - container for buffer array data types
  * `binom::Array` - container for array data type
  * `binom::Object` - container for object data type
  * `binom::NodeVisitor` - interface for traversing the hierarchical BinOM structure
* `binom::SerializedStorage` - container for storing static BinOM data in File
* `binom::DynamicStorage` - container for storing and processing dynamic BinOM data in File
  * `binom::FileNodeVisitor` - interface for traversing the hierarchical BinOM structure (thread-safe)
* `binom::Path` - path to BinOM node
* `binom::Query` - query for conditionally searching data in a BinOM node
* `binom::lexer` - BSDL to `binom::Variable` translator interface (More details in the [BSDL section](#binom-struct-description-language-bsdl))

Documentation describing all classes and functions is expected soon.

### Library usage example

```cpp
#include "binom/includes/binom.h"
#include "binom/includes/lexer.h"
#include <fstream>


int main() {

  using namespace binom;

  // Create BinOM variable
  Variable var = varr{ // Array
    vobj{ // Object
      {"prop_1", 42_ui8}, // Byte
      {"prop_2", ui32arr{1,2,3,4,5}}, // Word array
      {"prop_3", "Hello, world!"} // Byte array from c-string
    }
  };
  /* Description:
    In BinOM it is possible to initialize a structure using initialization lists.
    This method is most convenient for initializing BinOM structures of any complexity.
    The following initialization lists are available for specifying structures:
    * varr - array,
    * vobj - object,
    * ui8arr - byte array,
    * ui16arr - word array,
    * ui32arr - dword array,
    * ui64ahr - qword array,
    * i8arr - byte array,
    * i16arr - word array,
    * i32arr - dword array,
    * i64arr - qword array.
    To indicate the size of numbers, it is better to use the declared
    literals of numbers:
    * 1_ui8 - byte,
    * 2_ui16 - word,
    * 3_ui32 - dword,
    * 4_ui64 - qword,
    * 5_i8 - byte,
    * 6_i16 - word,
    * 7_i32 - dword,
    * 8_i64 - qword,
    * 9.5_f32 - dword,
    * 10.5_f64 - qword.
    You can also use strings of different encodings to initialize
    buffer arrays:
    * "..." and u8"..." - byte array
    * L"..." - implementation defined
    * u16"..." - word array
    * u32"..." - dword array
    
    All initialization lists declared in `libbinom/include/utils/types.h`
    All number literals declared in `libbinom/include/utils/ctypes.h`
  */

  // Create BinOM serialized storage file
  SerializedStorage s_storage("data.binom");
  s_storage = var; // Save Variable to BinOM file
  Variable loaded = s_storage; // Load Variable from BinOM file
  std::cout << loaded << '\n'; // Print Variable

  // Create BinOM dynamic storage file (with struct from var as default)
  DynamicStorage d_storage("data.binomdb", var);
  FileNodeVisitor root = d_storage; // Atomaticly call DynamicStorage::getRoot()
  // Change "prop_3" variable in data.binomdb file
  root[{0, "prop_3"}].setVariable("Goodbie, world!"); // or root[0].setVariable("prop_3", "Goodbie, world!");
  // Get "prop_2" value from data.binomdb file in Variable
  Variable from_storgae = root[0]["prop_2"]; // or auto from_storgae = root[0].getVariable("prop_2");

  // Convert structure description to Variable (More details in the BSDL section)
  std::ifstream t("struct.bsdl");
  std::stringstream str_stream;
  str_stream << t.rdbuf();
  Variable data = lexer << str_stream.str();
  std::cout << "From BSDL: " << data << '\n';

  // Serialize Variable
  ByteArray buffer = var.serialize();
  // ... And send it over network ...
}
```

### BinOM Struct Description Language (BSDL)
JSON-like structure description language (JSON-compatible)

#### BSDL Literals:
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

Where `f` - float, `i` - integer, `si` - signed integer, `ui` - unsigned integer

All types of brackets (`{}, [], <> or ()`) are equivalent to each other when specifying the data type.

There can be two kinds of separator operators for name and value: `: or =`

Value separators: `, ; | \ white-space-symbols`

Comments: `//...\n or /*...*/`

Example:
```
{
  usr: [
    { login: "admin" password: "admin" access_lvl: 0xFF_ui8 attributes: [] data: {} }
    { login: "guest" password: "" access_lvl: 0x00_ui8 attributes: [] data: {} }
    { login: "user" password: "any_user_password" access_lvl: 0x7F_ui8 attributes: [] data: {} }
  ]
  
  grp: [
    { name: "SYSTEM" users: [ "admin" "guest" ] usr_attributes: [ "unremovable" "const_access_lvl" ] change_access_lvl: 0xFF_ui8 }
  ]
  
  msg: {
    admin_guest: [
      { from: "admin" to: "guest" content: "Hello, Guest. Please, login with your account." }
    ]
  }
}
```

### BinOM Toolkit (binomtk)

BinOM Toolkit - a set of tools for working with BinOM data.
#### Build:
In directory with source code of this project execute command:
```bash
$ make tk
```

#### Commands:
* **cat** - print BinOM-files content
```
$ binomtk cat <file_#1> <file_#2> ... <file_#N>
```
* **pnode** - print nodes from BinOM-file
```
# path example: name[0]name_1.name_2
$ binomtk pnode <file> <path_to_node_#1> <path_to_node_#2> ... <path_to_node_#N>
```
* **mk** - create BinOM-file
```
$ binomtk mk <file>
```
* **edit** - edit BinOM-file
```
$ binomtk edit <file>
```
* **build** - build BinOM-file from BinOM source
```
$ binomtk build <source-file> <output-file>
```
* **convert** - automatically converts BinOM files to serialized or dynamic types
```
$ binomtk convert <old_file> <new_file>
```

#### Optional flags:
* **help** - print this manual
```
$ binomtk -help [or:-h, --help]
```
* **license** - print license
```
$ binomtk -license [or:-l, --license]
```
* **outflag** - change BinOM-tree output mode
```
# primitive: 's' - signed int, 'u' - unsigned int, 'h' - hex
# buffer array: 'p' - primitive array, 't' - string (if printable)
$ binomtk -outflag hp
# primitive - hex, buffer array - primitive array
```
