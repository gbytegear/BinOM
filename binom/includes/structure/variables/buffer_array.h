#ifndef BUFFER_ARRAY_H
#define BUFFER_ARRAY_H

#include "../types.h"
#include "value.h"

namespace binom {
class BufferArray {
  union types {
      void* ptr;
      VarType* type;
      byte* bytes;
      types(void* ptr) : ptr(ptr) {}
  } data;

  inline ui64& length() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}

  ui64 msize() const;
  void mch(size_t new_size);
  void* madd(size_t add_size);
  void msub(size_t sub_size);
  void* maddto(void* to, size_t size);
  void msubfrom(void* from, size_t size);

  void destroy();
  void* clone() const;

  friend class Variable;
  friend class Object;
  friend struct NamedVariable;

  struct _value_7_1 {ui8 val:7;bool sign:1;};
  static ByteArray toChainNumber(ui64 number);
  static ui64 fromChainNumber(ByteArray::iterator& it);
    
  BufferArray(void* buffer) : data(buffer) {}

public:

  typedef ValueIterator iterator;
  typedef const ValueIterator const_iterator;

  BufferArray(VarType type);

  BufferArray(const char* str);
  BufferArray(const std::string str);

  BufferArray(const wchar_t* wstr);
  BufferArray(const std::wstring wstr);

  BufferArray(size_t size, ui8 value);
  BufferArray(size_t size, ui16 value);
  BufferArray(size_t size, ui32 value);
  BufferArray(size_t size, ui64 value);
  BufferArray(size_t size, i8 value);
  BufferArray(size_t size, i16 value);
  BufferArray(size_t size, i32 value);
  BufferArray(size_t size, i64 value);

  BufferArray(ui8* values, size_t size);
  BufferArray(ui16* values, size_t size);
  BufferArray(ui32* values, size_t size);
  BufferArray(ui64* values, size_t size);
  BufferArray(i8* values, size_t size);
  BufferArray(i16* values, size_t size);
  BufferArray(i32* values, size_t size);
  BufferArray(i64* values, size_t size);

  BufferArray(ui8arr array);
  BufferArray(ui16arr array);
  BufferArray(ui32arr array);
  BufferArray(ui64arr array);

  BufferArray(i8arr array);
  BufferArray(i16arr array);
  BufferArray(i32arr array);
  BufferArray(i64arr array);

  BufferArray(ByteArray arr);

  BufferArray(const BufferArray& other);
  BufferArray(BufferArray&& other);
  BufferArray(Primitive primitive);

  ~BufferArray() {destroy();}

  VarType getType() {return *data.type;}

  ByteArray serialize() const;
  static BufferArray deserialize(ByteArray::iterator& it);

  inline bool isEmpty() const {return !length();}
  inline ui64 getMemberCount() const {return *reinterpret_cast<ui64*>(data.bytes + 1);}
  inline ui8 getMemberSize() const {
    switch (*data.type) {
      case VarType::byte_array: return 1;
      case VarType::word_array: return 2;
      case VarType::dword_array: return 4;
      case VarType::qword_array: return 8;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
  }

  static inline ui8 getMemberSize(VarType type) {
    switch (type) {
      case VarType::byte_array: return 1;
      case VarType::word_array: return 2;
      case VarType::dword_array: return 4;
      case VarType::qword_array: return 8;
      default: throw Exception(ErrCode::binom_invalid_type);
    }
  }


  inline ValueRef getValue(ui64 index) const {
    if(index >= getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of buffer array range!");
    return begin()[index];
  }

  void* getDataPointer() {return data.bytes + 9;}
  ui64 getDataSize() {return getMemberCount() * getMemberSize();}
  VarType getType() const {return *data.type;}

  ValueRef pushBack(ui64 value);
  ValueRef pushBack(i64 value);
  ValueRef pushBack(f64 value);
  iterator pushBack(const BufferArray& other);
  inline ValueRef pushBack(const ValueRef value) {return pushBack(value.asUnsigned());}

  ValueRef pushFront(ui64 value);
  ValueRef pushFront(i64 value);
  ValueRef pushFront(f64 value);
  iterator pushFront(const BufferArray& other);
  inline ValueRef pushFront(const ValueRef value) {return pushFront(value.asUnsigned());}

  ValueRef insert(const ui64 index, const ui64 value);
  ValueRef insert(const ui64 index, const i64 value);
  ValueRef insert(const ui64 index, const f64 value);
  iterator insert(const ui64 index, const BufferArray& other);
  inline ValueRef insert(ui64 index, const ValueRef value) {return insert(index, value.asUnsigned());}

  void popBack(const ui64 n = 1);
  void popFront(const ui64 n = 1);
  void remove(const ui64 index, const ui64 n = 1);
  BufferArray subarr(const ui64 index, const ui64 n);

  void clear();

  inline ValueRef operator[](ui64 index) const {return getValue(index);}

  inline BufferArray& operator+=(const ui64 value) {pushBack(value); return *this;}
  inline BufferArray& operator+=(const i64 value) {pushBack(value); return *this;}
  inline BufferArray& operator+=(const f64 value) {pushBack(value); return *this;}
  inline BufferArray& operator+=(const BufferArray& other) {pushBack(other); return *this;}
  inline BufferArray& operator+=(const ValueRef value) {pushBack(value); return *this;}

  BufferArray& operator=(BufferArray other);

  bool operator==(const BufferArray& other) const;
  inline bool operator!=(const BufferArray& other) const {return !(*this == other);}
  bool operator>(const BufferArray& other) const;
  bool operator<(const BufferArray& other) const;
  bool operator>=(const BufferArray& other) const;
  bool operator<=(const BufferArray& other) const;

  iterator begin() const;
  iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  std::string toString() const;
  std::wstring toWString() const;
  ByteArray toByteArray() const;

  Variable& asVar() {return *reinterpret_cast<Variable*>(this);}
};



}

const binom::BufferArray operator "" _vbfr(const char* c_str, std::size_t);

#endif
