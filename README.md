# <img src="https://gbytegear.github.io/BinOM/src/img/BinOM.ico" height="40">BinOM

* [Definitions](#definitions)
* [Build library](#build-library)

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


