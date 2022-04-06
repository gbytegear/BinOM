# <a href="https://gbytegear.github.io/BinOM/"><img src="https://gbytegear.github.io/BinOM/src/img/BinOM.ico" height="40">BinOM</a>

**BinOM**(*Binary Object Model*) - library for working with a hierarchical data format for general purposes.

## Basic goals:
* Development of a generic data format for building structures of any complexity
* Ensuring the most optimal read and data processing speed
* Development of tools for the most convenient work with data

## BinOM Types info
### Number value widths:
* byte - 8 bit width;
* word - 16 bit width;
* dword - 32 bit width;
* qword - 64 bit width.

### Number value types:
* ui - unsigned integer;
* si - signed integer;
* f - float.

## Types table:
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
    <th>boolean</th>
    <td>Boolean value</td>
    <td>true/false</td>
  </tr>
  <tr>
    <th>0x03</th>
    <th>ui8</th>
    <td>8 bit unsigned integer</td>
    <td>0..255</td>
  </tr>
  <tr>
    <th>0x04</th>
    <th>si8</th>
    <td>8 bit signed integer</td>
    <td>-128..127</td>
  </tr>
  <tr>
    <th>0x05</th>
    <th>ui16</th>
    <td>16 bit unsigned integer</td>
    <td>0..65535</td>
  </tr>
  <tr>
    <th>0x06</th>
    <th>si16</th>
    <td>16 bit signed integer</td>
    <td>-32768..32767</td>
  </tr>
  <tr>
    <th>0x07</th>
    <th>ui32</th>
    <td>32 bit unsigned integer</td>
    <td>0..4294967295</td>
  </tr>
  <tr>
    <th>0x08</th>
    <th>si32</th>
    <td>32 bit signed integer</td>
    <td>-2147483648..2147483647</td>
  </tr>
  <tr>
    <th>0x09</th>
    <th>f32</th>
    <td>32 bit floating point number</td>
    <td>+/-3.4E-38..3.4E+38</td>
  </tr>
  <tr>
    <th>0x0A</th>
    <th>ui64</th>
    <td>64 bit unsigned integer</td>
    <td>0..18446744073709551615</td>
  </tr>
  <tr>
    <th>0x0B</th>
    <th>si64</th>
    <td>64 bit signed integer</td>
    <td>-9223372036854775808..9223372036854775807</td>
  </tr>
  <tr>
    <th>0x0C</th>
    <th>f64</th>
    <td>64 bit floating point number</td>
    <td>+/-1.7E-308..1.7E+308</td>
  </tr>
  <tr>
    <th>0x0D</th>
    <th>ui8 array</th>
    <td>Array of 8 bit unsigned integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x0E</th>
    <th>si8 array</th>
    <td>Array of 8 bit signed integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x0F</th>
    <th>ui16</th>
    <td>Array of 16 bit unsigned integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x10</th>
    <th>si16 array</th>
    <td>Array of 16 bit signed integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x11</th>
    <th>ui32 array</th>
    <td>Array of 32 bit unsigned integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x12</th>
    <th>si32 array</th>
    <td>Array of 32 bit signed integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x13</th>
    <th>f32 array</th>
    <td>Array of 32 bit floating point number</td>
    <td></td>
  </tr>
  <tr>
    <th>0x14</th>
    <th>ui64 array</th>
    <td>Array of 64 bit unsigned integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x15</th>
    <th>si64 array</th>
    <td>Array of 64 bit signed integer</td>
    <td></td>
  </tr>
  <tr>
    <th>0x16</th>
    <th>f64 array</th>
    <td>Array of 64 bit floating point number</td>
    <td></td>
  </tr>
  <tr>
    <th>0x17</th>
    <th>array</th>
    <td>Heterogeneous array</td>
    <td></td>
  </tr>
  <tr>
    <th>0x18</th>
    <th>less map</th>
    <td>Associative heterogeneous container with key-sorted by descending</td>
    <td></td>
  </tr>
  <tr>
    <th>0x19</th>
    <th>greater map</th>
    <td>Associative heterogeneous container with key-sorted by ascending</td>
    <td></td>
  </tr>
</table>

### C++ Container types:
* `binom::Number` - container for numeric data types;
* `binom::BufferArray` - сontainer for the same type of numeric values;
* `binom::Array` - heterogeneous array;
* `binom::Map` - sorted associative container that contains key-value pairs with unique keys.
