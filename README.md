# <a href="https://gbytegear.github.io/BinOM/"><img src="https://gbytegear.github.io/BinOM/src/img/BinOM.ico" height="40">Strict-typed binary object model (STBinOM)</a>

STBinOM - more strict-typed version of binary object model.

## Basic goals:
* Development of a generic data format for building structures of any complexity
* Ensuring the most optimal read and data processing speed
* Development of tools for the most convenient work with data

## STBinOM types
All STBinOM types are based on types from C++ and STL, due to which they inherit from these tools a commonality that allows them to build structures of any complexity. The numeric type class "Primitive" has been renamed to "Number" which, unlike the previous one, separates numeric types by the presence of a sign or a floating point. The "object" type is divided into sorted containers, such as a map and a set, which can also differ in sorting type.

### Container types:
* Number - container for numeric data types;
* BufferArray - сontainer for the same type of numeric values;
* Array - array of similar numeric values;
* Set - container of sorted values;
* Map - container of sorted keys associated with values.

### Number value widths:
* byte - 8 bit width;
* word - 16 bit width;
* dword - 32 bit width;
* qword - 64 bit width.

### Number value types:
* ui - unsigned integer;
* i - signed integer;
* f - float.

### Type hierarchy:
* null
* Number
  * byte width
    * ui8
    * i8
  * word width
    * ui16
    * i16
  * dword width
    * ui32
    * i32
    * f32
  * qword width
    * ui64
    * i64
    * f64
* BufferArray
  * byte width
    * ui8 array
    * i8 array
  * word width
    * ui16 array
    * i16 array
  * dword width
    * ui32 array
    * i32 array
    * f32 array
  * qword width
    * ui64 array
    * i64 array
    * f64 array
* Array
* SortedContainers
  * Sets
    * Set
      * less set
      * greater set
    * Multiset
      * less multiset
      * greater multiset
  * Maps
    * Map
      * less map
      * greater map
    * Multimap
      * less back multimap
      * greater back multimap
      * less front multimap
      * greater front multimap

### Types list:
<table>
  <tr>
    <th>Code</th><th>Type</th><th>Definition</th><th>Value</th>
  </tr>
  <tr>
    <th>0x01</th>
    <th>null</th>
    <td>Null type</td>
    <td>null</td>
  </tr>
  <tr>
    <th>0x02</th>
    <th>ui8</th>
    <td>8 bit unsigned integer</td>
    <td>0..255</td>
  </tr>
  <tr>
    <th>0x03</th>
    <th>i8</th>
    <td>8 bit signed integer</td>
    <td>-128..127</td>
  </tr>
  <tr>
    <th>0x04</th>
    <th>ui16</th>
    <td>16 bit unsigned integer</td>
    <td>0..65535</td>
  </tr>
  <tr>
    <th>0x05</th>
    <th>i16</th>
    <td>16 bit signed integer</td>
    <td>-32768..32767</td>
  </tr>
  <tr>
    <th>0x06</th>
    <th>ui32</th>
    <td>32 bit unsigned integer</td>
    <td>0..4294967295</td>
  </tr>
  <tr>
    <th>0x07</th>
    <th>i32</th>
    <td>32 bit signed integer</td>
    <td>-2147483648..2147483647</td>
  </tr>
  <tr>
    <th>0x08</th>
    <th>f32</th>
    <td>32 bit floating point number</td>
    <td>+/-3.4E-38..3.4E+38</td>
  </tr>
  <tr>
    <th>0x09</th>
    <th>ui64</th>
    <td>64 bit unsigned integer</td>
    <td>0..18446744073709551615</td>
  </tr>
  <tr>
    <th>0x0A</th>
    <th>i64</th>
    <td>64 bit signed integer</td>
    <td>-9223372036854775808..9223372036854775807</td>
  </tr>
  <tr>
    <th>0x0B</th>
    <th>i64</th>
    <td>64 bit floating point number</td>
    <td>+/-1.7E-308..1.7E+308</td>
  </tr>
  <tr>
    <th>0x0C</th>
    <th>ui8 array</th>
    <td>Array of 8 bit unsigned integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x0D</th>
    <th>i8 array</th>
    <td>Array of 8 bit signed integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x0E</th>
    <th>ui16</th>
    <td>Array of 16 bit unsigned integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x0F</th>
    <th>i16 array</th>
    <td>Array of 16 bit signed integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x10</th>
    <th>ui32 array</th>
    <td>Array of 32 bit unsigned integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x11</th>
    <th>i32 array</th>
    <td>Array of 32 bit signed integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x12</th>
    <th>f32 array</th>
    <td>Array of 32 bit floating point number</td>
    <td></td>
  </tr>
  <tr>
    <th>0x13</th>
    <th>ui64 array</th>
    <td>Array of 64 bit unsigned integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x14</th>
    <th>i64 array</th>
    <td>Array of 64 bit signed integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x15</th>
    <th>i64 array</th>
    <td>Array of 64 bit floating point number</td>
    <td></td>
  </tr>
  <tr>
    <th>0x15</th>
    <th>i64 array</th>
    <td>Array of 64 bit floating point number</td>
    <td></td>
  </tr>
  <tr>
    <th>0x16</th>
    <th>array</th>
    <td>Heterogeneous array</td>
    <td></td>
  </tr>
  <tr>
    <th>0x17</th>
    <th>less set</th>
    <td>Heterogeneous set of descending sorted unique values</td>
    <td></td>
  </tr>
  <tr>
    <th>0x18</th>
    <th>greater set</th>
    <td>Heterogeneous set of ascending sorted unique values</td>
    <td></td>
  </tr>
  <tr>
    <th>0x19</th>
    <th>less multiset</th>
    <td>Heterogeneous set of descending sorted values</td>
    <td></td>
  </tr>
  <tr>
    <th>0x1A</th>
    <th>greater multiset</th>
    <td>Heterogeneous set of ascending sorted values</td>
    <td></td>
  </tr>
  <tr>
    <th>0x1B</th>
    <th>less map</th>
    <td>Associative heterogeneous container with key-sorted by descending</td>
    <td></td>
  </tr>
  <tr>
    <th>0x1C</th>
    <th>greater map</th>
    <td>Associative heterogeneous container with key-sorted by ascending</td>
    <td></td>
  </tr>
  <tr>
    <th>0x1D</th>
    <th>less back multimap</th>
    <td>Associative heterogeneous container with key-sorted by descending, with the addition of matching by key records at the back.</td>
    <td></td>
  </tr>
  <tr>
    <th>0x1E</th>
    <th>greater back multimap</th>
    <td>Associative heterogeneous container with key-sorted by ascending, with the addition of matching by key records at the back.</td>
    <td></td>
  </tr>
  <tr>
    <th>0x1F</th>
    <th>less front multimap</th>
    <td>Associative heterogeneous container with key-sorted by descending, with the addition of matching by key records at the front.</td>
    <td></td>
  </tr>
  <tr>
    <th>0x20</th>
    <th>greater front multimap</th>
    <td>Associative heterogeneous container with key-sorted by ascending, with the addition of matching by key records at the front.</td>
    <td></td>
  </tr>
</table>

### Data types in binary deserialized format:
* `[0x00]` - unused (serialized data separator)
* `[0x01]` - null
* `[0x02](value:[...])` - ui8
* `[0x03](value:[...])` - i8
* `[0x04](value:[...][...])` - ui16
* `[0x05](value:[...][...])` - i16
* `[0x06](value:[...][...][...][...])` - ui32
* `[0x07](value:[...][...][...][...])` - i32
* `[0x08](value:[...][...][...][...])` - f32
* `[0x09](value:[...][...][...][...][...][...][...][...])` - ui64
* `[0x0A](value:[...][...][...][...][...][...][...][...])` - i64
* `[0x0B](value:[...][...][...][...][...][...][...][...])` - f64
* `[0x0C](length:[...][...][...][...][...][...][...][...])(value:[...])...` - ui8 array
* `[0x0D](length:[...][...][...][...][...][...][...][...])(value:[...])...` - i8 array
* `[0x0E](length:[...][...][...][...][...][...][...][...])(value:[...][...])...` - ui16 array
* `[0x0F](length:[...][...][...][...][...][...][...][...])(value:[...][...])...` - i16 array
* `[0x10](length:[...][...][...][...][...][...][...][...])(value:[...][...][...][...])...` - ui32 array
* `[0x11](length:[...][...][...][...][...][...][...][...])(value:[...][...][...][...])...` - i32 array
* `[0x12](length:[...][...][...][...][...][...][...][...])(value:[...][...][...][...])...` - f32 array
* `[0x13](length:[...][...][...][...][...][...][...][...])(value:[...][...][...][...][...][...][...][...])...` - ui64 array
* `[0x14](length:[...][...][...][...][...][...][...][...])(value:[...][...][...][...][...][...][...][...])...` - i64 array
* `[0x15](length:[...][...][...][...][...][...][...][...])(value:[...][...][...][...][...][...][...][...])...` - f64 array
* `[0x16](length:[...][...][...][...][...][...][...][...])(variable)...` - array
* `[0x17](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(variable))` - less set (AVL/RB-tree)
* `[0x18](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(variable))` - greater set (AVL/RB-tree)
* `[0x19](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(array))` - less multiset (AVL/RB-tree)
* `[0x1A](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(array))` - greater multiset (AVL/RB-tree)
* `[0x1B](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(key variable)(value variable))` - less map (AVL/RB-tree)
* `[0x1C](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(key variable)(value variable))` - greater map (AVL/RB-tree)
* `[0x1D](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(key variable)(value array))` - less back multimap (AVL/RB-tree)
* `[0x1E](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(key variable)(value array))` - greater back multimap (AVL/RB-tree)
* `[0x1F](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(key variable)(value array))` - less front multimap (AVL/RB-tree)
* `[0x20](root node:(left pointer:[...][...][...][...][...][...][...][...])(right pointer:[...][...][...][...][...][...][...][...])(key variable)(value array))` - greater front multimap (AVL/RB-tree)

