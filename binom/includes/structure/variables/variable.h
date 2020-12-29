#ifndef VARIABLE_H
#define VARIABLE_H

#include "../types.h"

#include "primitive.h"
#include "buffer_array.h"
#include "array.h"
#include "object.h"
#include "matrix.h"
#include "table.h"

namespace binom {

class Variable {

  union types {
    void* ptr;
    VarType* type;
    byte* bytes;

    Primitive primitive;
    BufferArray buffer_array;
    Array array;
    Object object;
    Matrix matrix;
    Table table;

    types(void* ptr) : ptr(ptr) {}
    ~types() {}
  } data;

  void* clone() const;
  void destroy();

  friend class Array;
  friend class Object;
  friend class Matrix;
  friend class Table;

public:

  Variable() : data(nullptr) {}

  // Bool init
  Variable(bool value);

  // Primitive init

  Variable(ui8 value);
  Variable(ui16 value);
  Variable(ui32 value);
  Variable(ui64 value);

  Variable(i8 value);
  Variable(i16 value);
  Variable(i32 value);
  Variable(i64 value);
  Variable(f32 value);
  Variable(f64 value);

  // Buffer_array init
  Variable(const char* str);
  Variable(const std::string str);

  Variable(size_t size, ui8 value);
  Variable(size_t size, ui16 value);
  Variable(size_t size, ui32 value);
  Variable(size_t size, ui64 value);
  Variable(size_t size, i8 value);
  Variable(size_t size, i16 value);
  Variable(size_t size, i32 value);
  Variable(size_t size, i64 value);

  Variable(ui8* values, size_t size);
  Variable(ui16* values, size_t size);
  Variable(ui32* values, size_t size);
  Variable(ui64* values, size_t size);

  Variable(i8* values, size_t size);
  Variable(i16* values, size_t size);
  Variable(i32* values, size_t size);
  Variable(i64* values, size_t size);

  Variable(ui8arr array);
  Variable(ui16arr array);
  Variable(ui32arr array);
  Variable(ui64arr array);

  Variable(i8arr array);
  Variable(i16arr array);
  Variable(i32arr array);
  Variable(i64arr array);

  // Array
  Variable(varr array);

  // Object
  Variable(obj object);

  // Matrix
  Variable(mtrx matrix);

  // Table
  Variable(tbl table);

  Variable(Variable&& other);
  Variable(Variable& other);

  ~Variable() {destroy();}

  inline void* getDataPointer() const {return data.ptr;}

  inline VarType type() const noexcept             {return (data.type == nullptr)? VarType::invlid_type :*data.type;}
  inline VarTypeClass typeClass() const noexcept   {return (data.type == nullptr)? VarTypeClass::invalid_type :toTypeClass(*data.type);}
  inline bool isNull() const noexcept              {return data.type == nullptr;}

  inline bool isPrimitive() const noexcept         {return typeClass() == VarTypeClass::primitive;}
  inline bool isBufferArray() const noexcept       {return typeClass() == VarTypeClass::buffer_array;}
  inline bool isArray() const noexcept             {return typeClass() == VarTypeClass::array;}
  inline bool isObject() const noexcept            {return typeClass() == VarTypeClass::object;}
  inline bool isMatrix() const noexcept            {return typeClass() == VarTypeClass::matrix;}
  inline bool isTable() const noexcept             {return typeClass() == VarTypeClass::table;}

  inline Primitive& toPrimitive() const noexcept         {return const_cast<Primitive&>(data.primitive);}
  inline BufferArray& toBufferArray() const noexcept     {return const_cast<BufferArray&>(data.buffer_array);}
  inline Array& toArray() const noexcept                 {return const_cast<Array&>(data.array);}
  inline Object& toObject() const noexcept               {return const_cast<Object&>(data.object);}
  inline Matrix& toMatrix() const noexcept               {return const_cast<Matrix&>(data.matrix);}
  inline Table& toTable() const noexcept                 {return const_cast<Table&>(data.table);}

  inline Variable& operator=(const Variable& other) {
    if(!isNull())destroy();
    data.ptr = other.clone();
    return *this;
  }

  inline ui64 length() {
    return
          (isNull())
        ? 0
        : (isBufferArray() || isArray() || isObject() || isMatrix() || isTable())
        ? *reinterpret_cast<ui64*>(data.bytes + 1)
        : 1;
  }


};








// Initers or other

struct NamedVariable {
  BufferArray name;
  Variable variable;
};

struct mtrx {
  std::initializer_list<VarType> type_list;
  std::initializer_list<Variable> var_list;

  mtrx(std::initializer_list<VarType> types, std::initializer_list<Variable> vars)
    : type_list(types), var_list(vars)
  {if(!isValid())throw SException(ErrCode::binom_invalid_initer, "Invalid matrix initer");}

  ui64 getNeededMemory() {return type_list.size() + var_list.size() * PTR_SZ;}
  ui64 getColumnCount() {return type_list.size();}
  ui64 getRowCount() {return var_list.size() / type_list.size();}

  private:
  bool isValid() {
    std::initializer_list<VarType>::iterator type(type_list.begin());
    std::initializer_list<Variable>::iterator var(var_list.begin());
    for(;var != var_list.end();(++type, ++var)) {
      if(type == type_list.end()) type = type_list.begin();
      if(var->type() != *type) return false;
    }
    return type == type_list.end();
  }
};

struct tbl {
  struct ColumnInfo {
    VarType type;
    BufferArray name;
  };
  std::initializer_list<ColumnInfo> column_list;
  std::initializer_list<Variable> var_list;

  tbl(std::initializer_list<ColumnInfo> columns, std::initializer_list<Variable> vars)
    : column_list(columns), var_list(vars)
  {if(!isValid())throw SException(ErrCode::binom_invalid_initer, "Invalid table initer");}

  ui64 getNeededMemory() {return column_list.size() * sizeof (ColumnInfo) + var_list.size() + PTR_SZ;}
  ui64 getColumnCount() {return column_list.size();}
  ui64 getRowCount() {return var_list.size() / column_list.size();}

private:
  bool isValid() {
    std::initializer_list<ColumnInfo>::iterator column(column_list.begin());
    std::initializer_list<Variable>::iterator var(var_list.begin());
    for(;var != var_list.end();(++column, ++var)) {
      if(column == column_list.end()) column = column_list.begin();
      if(var->type() != column->type) return false;
    }
    return column == column_list.end();
  }
};

}


std::ostream& operator<<(std::ostream& os, const binom::Variable& var);

#endif
