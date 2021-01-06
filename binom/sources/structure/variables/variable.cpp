#include "binom/includes/structure/variables/variable.h"


using namespace binom;
// Constructor

void* Variable::clone() const {
  switch (toTypeClass(*data.type)) {
    case VarTypeClass::primitive: return toPrimitive().clone();
    case VarTypeClass::buffer_array: return toBufferArray().clone();
    case VarTypeClass::array: return toArray().clone();
    case VarTypeClass::object: return toObject().clone();
    case VarTypeClass::matrix: return toMatrix().clone();
    case VarTypeClass::table: return toTable().clone();
    case VarTypeClass::invalid_type: throw SException(ErrCode::binom_invalid_type, "destroy(): Invalid type!");
  }
}

void Variable::destroy() {
  if(isNull()) return;
  switch (toTypeClass(*data.type)) {
    case VarTypeClass::primitive: return toPrimitive().destroy();
    case VarTypeClass::buffer_array: return toBufferArray().destroy();
    case VarTypeClass::array: return toArray().destroy();
    case VarTypeClass::object: return toObject().destroy();
    case VarTypeClass::matrix: return toMatrix().destroy();
    case VarTypeClass::table: return toTable().destroy();
    case VarTypeClass::invalid_type: throw SException(ErrCode::binom_invalid_type, "destroy(): Invalid type!");
  }
}

Variable::Variable(bool value) : data(tryMalloc(2)) {
  data.type[0] = VarType::byte;
  reinterpret_cast<bool*>(data.bytes)[1] = value;
}

Variable::Variable(ui8 value) : data(tryMalloc(2)) {
  data.type[0] = VarType::byte;
  data.bytes[1] = value;
}

Variable::Variable(ui16 value) : data(tryMalloc(3)) {
  data.type[0] = VarType::word;
  *reinterpret_cast<ui16*>(data.bytes + 1) = value;
}

Variable::Variable(ui32 value) : data(tryMalloc(5)) {
  data.type[0] = VarType::dword;
  *reinterpret_cast<ui32*>(data.bytes + 1) = value;
}

Variable::Variable(ui64 value) : data(tryMalloc(9)) {
  data.type[0] = VarType::qword;
  *reinterpret_cast<ui64*>(data.bytes + 1) = value;
}




Variable::Variable(i8 value) : data(tryMalloc(2)) {
  data.type[0] = VarType::byte;
  *reinterpret_cast<i8*>(data.bytes + 1) = value;
}

Variable::Variable(i16 value) : data(tryMalloc(3)) {
  data.type[0] = VarType::word;
  *reinterpret_cast<i16*>(data.bytes + 1) = value;
}

Variable::Variable(i32 value) : data(tryMalloc(5)) {
  data.type[0] = VarType::dword;
  *reinterpret_cast<i32*>(data.bytes + 1) = value;
}

Variable::Variable(i64 value) : data(tryMalloc(9)) {
  data.type[0] = VarType::qword;
  *reinterpret_cast<i64*>(data.bytes + 1) = value;
}

Variable::Variable(f32 value) : data(tryMalloc(5)) {
  data.type[0] = VarType::dword;
  *reinterpret_cast<f32*>(data.bytes + 1) = value;
}

Variable::Variable(f64 value) : data(tryMalloc(9)) {
  data.type[0] = VarType::qword;
  *reinterpret_cast<f64*>(data.bytes + 1) = value;
}




Variable::Variable(const char* str) : data(tryMalloc(9 + strlen(str))) {
  data.type[0] = VarType::byte_array;
  ui64 size = strlen(str);
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memcpy(reinterpret_cast<char*>(data.bytes + 9), str, size);
}

Variable::Variable(const std::string str) : data(tryMalloc(9 + str.length())) {
  data.type[0] = VarType::byte_array;
  ui64 size = str.length();
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memcpy(reinterpret_cast<char*>(data.bytes + 9), str.c_str(), size);
}




Variable::Variable(size_t size, ui8 value) : data(tryMalloc(9 + size)) {
  data.type[0] = VarType::byte_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memset(data.bytes + 9, value, size);
}

Variable::Variable(size_t size, ui16 value) : data(tryMalloc(9 + size*2)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  size_t i = 0;
  for(ui16* it = reinterpret_cast<ui16*>(data.bytes + 9); i < size; (++it, ++i) )
    *it = value;
}

Variable::Variable(size_t size, ui32 value) : data(tryMalloc(9 + size*4)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  size_t i = 0;
  for(ui32* it = reinterpret_cast<ui32*>(data.bytes + 9); i < size; (++it, ++i) )
    *it = value;
}

Variable::Variable(size_t size, ui64 value) : data(tryMalloc(9 + size*8)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  size_t i = 0;
  for(ui64* it = reinterpret_cast<ui64*>(data.bytes + 9); i < size; (++it, ++i) )
    *it = value;

}




Variable::Variable(size_t size, i8 value) : data(tryMalloc(9 + size)) {
  data.type[0] = VarType::byte_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memset(data.bytes + 9, value, size);
}

Variable::Variable(size_t size, i16 value) : data(tryMalloc(9 + size*2)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  size_t i = 0;
  for(i16* it = reinterpret_cast<i16*>(data.bytes + 9); i < size; (++it, ++i) )
    *it = value;

}

Variable::Variable(size_t size, i32 value) : data(tryMalloc(9 + size*4)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  size_t i = 0;
  for(i32* it = reinterpret_cast<i32*>(data.bytes + 9); i < size; (++it, ++i) )
    *it = value;

}

Variable::Variable(size_t size, i64 value) : data(tryMalloc(9 + size*8)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  size_t i = 0;
  for(i64* it = reinterpret_cast<i64*>(data.bytes + 9); i < size; (++it, ++i) )
    *it = value;

}




Variable::Variable(ui8* values, size_t size) : data(tryMalloc(9 + size)) {
  data.type[0] = VarType::byte_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memcpy(data.bytes + 9, values, size);
}

Variable::Variable(ui16* values, size_t size) : data(tryMalloc(9 + size*2)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  ui16* v_it = values;
  for(ui64* it = reinterpret_cast<ui64*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
    *it = *v_it;

}

Variable::Variable(ui32* values, size_t size) : data(tryMalloc(9 + size*4)) {
  data.type[0] = VarType::dword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  ui32* v_it = values;
  for(ui32* it = reinterpret_cast<ui32*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
    *it = *v_it;

}

Variable::Variable(ui64* values, size_t size) : data(tryMalloc(9 + size*8)) {
  data.type[0] = VarType::qword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  ui64* v_it = values;
  for(ui64* it = reinterpret_cast<ui64*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
    *it = *v_it;

}




Variable::Variable(i8* values, size_t size) : data(tryMalloc(9 + size)) {
  data.type[0] = VarType::byte_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;
  memcpy(data.bytes + 9, values, size);
}

Variable::Variable(i16* values, size_t size) : data(tryMalloc(9 + size*2)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  i16* v_it = values;
  for(i64* it = reinterpret_cast<i64*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
    *it = *v_it;

}

Variable::Variable(i32* values, size_t size) : data(tryMalloc(9 + size*4)) {
  data.type[0] = VarType::dword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  i32* v_it = values;
  for(i32* it = reinterpret_cast<i32*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
    *it = *v_it;

}

Variable::Variable(i64* values, size_t size) : data(tryMalloc(9 + size*8)) {
  data.type[0] = VarType::qword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = size;

  i64* v_it = values;
  for(i64* it = reinterpret_cast<i64*>(data.bytes + 9); ui64(v_it - values) < size; (++it, ++v_it) )
    *it = *v_it;

}





Variable::Variable(ui8arr array) : data(tryMalloc(9 + array.size())) {
  data.type[0] = VarType::byte_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  ui8* it = data.bytes + 9;
  for(ui8 value : array) {
    *it = value;
    ++it;
  }
}

Variable::Variable(ui16arr array) : data(tryMalloc(9 + array.size()*2)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  ui16* it = reinterpret_cast<ui16*>(data.bytes + 9);
  for(ui16 value : array) {
    *it = value;
    ++it;
  }
}

Variable::Variable(ui32arr array) : data(tryMalloc(9 + array.size()*4)) {
  data.type[0] = VarType::dword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  ui32* it = reinterpret_cast<ui32*>(data.bytes + 9);
  for(ui32 value : array) {
    *it = value;
    ++it;
  }
}

Variable::Variable(ui64arr array) : data(tryMalloc(9 + array.size()*8)) {
  data.type[0] = VarType::qword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  ui64* it = reinterpret_cast<ui64*>(data.bytes + 9);
  for(ui64 value : array) {
    *it = value;
    ++it;
  }
}





Variable::Variable(i8arr array) : data(tryMalloc(9 + array.size())) {
  data.type[0] = VarType::byte_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  i16* it = reinterpret_cast<i16*>(data.bytes + 9);
  for(i8 value : array) {
    *it = value;
    ++it;
  }
}

Variable::Variable(i16arr array) : data(tryMalloc(9 + array.size()*2)) {
  data.type[0] = VarType::word_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  i16* it = reinterpret_cast<i16*>(data.bytes + 9);
  for(i16 value : array) {
    *it = value;
    ++it;
  }
}

Variable::Variable(i32arr array) : data(tryMalloc(9 + array.size()*4)) {
  data.type[0] = VarType::dword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  i32* it = reinterpret_cast<i32*>(data.bytes + 9);
  for(i32 value : array) {
    *it = value;
    ++it;
  }
}

Variable::Variable(i64arr array) : data(tryMalloc(9 + array.size()*8)) {
  data.type[0] = VarType::qword_array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  i64* it = reinterpret_cast<i64*>(data.bytes + 9);
  for(i64 value : array) {
    *it = value;
    ++it;
  }
}




Variable::Variable(varr array) : data(tryMalloc(9 + array.size()*sizeof(Variable))) {
  data.type[0] = VarType::array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  void** it = reinterpret_cast<void**>(data.bytes + 9);
  for(const Variable& value : array) {
    *it = value.getDataPointer();
    const_cast<Variable&>(value).data.ptr = nullptr;
    ++it;
  }
}

Variable::Variable(obj object) : data(tryMalloc(9 + object.size()*sizeof(NamedVariable))) {
  data.type[0] = VarType::object;
  *reinterpret_cast<ui64*>(data.bytes + 1) = object.size();
  i64 in_count = 0;


  for(const NamedVariable& value : object) {

    NamedVariable* it = reinterpret_cast<NamedVariable*>(data.bytes + 9);

    if(!in_count) {
      it->name.data.ptr = value.name.data.ptr;
      it->variable.data.ptr = value.variable.data.ptr;
      const_cast<NamedVariable&>(value).name.data.ptr = nullptr;
      const_cast<NamedVariable&>(value).variable.data.ptr = nullptr;
    } else {

      i64 left = 0;
      i64 right = in_count;
      i64 middle = 0;

      while (left <= right) {
        middle = (left + right) / 2;
        if(middle >= in_count) break;

        if(it[middle].name > value.name) right = middle - 1;
        elif(it[middle].name < value.name) left = middle + 1;
        elif(it[middle].name == value.name) throw SException(ErrCode::binom_object_key_error, "");
      }

      for(; (middle < in_count)? it[middle].name < value.name : false ;++middle);

      if(middle < in_count) {
        memmove(data.bytes + 9 + (middle+1)*sizeof (NamedVariable),
                data.bytes + 9 + middle*sizeof (NamedVariable),
                (in_count - middle)*sizeof (NamedVariable));
        it = it + middle;
        it->name.data.ptr = value.name.data.ptr;
        it->variable.data.ptr = value.variable.data.ptr;
        const_cast<NamedVariable&>(value).name.data.ptr = nullptr;
        const_cast<NamedVariable&>(value).variable.data.ptr = nullptr;
      } else {
        it = it + middle;
        it->name.data.ptr = value.name.data.ptr;
        it->variable.data.ptr = value.variable.data.ptr;
        const_cast<NamedVariable&>(value).name.data.ptr = nullptr;
        const_cast<NamedVariable&>(value).variable.data.ptr = nullptr;
      }
    }
    ++in_count;
  }
}

Variable::Variable(mtrx matrix) : data(tryMalloc(17 + matrix.getNeededMemory())) {
  data.type[0] = VarType::matrix;
  *reinterpret_cast<ui64*>(data.bytes + 1) = matrix.getRowCount();
  *reinterpret_cast<ui64*>(data.bytes + 9) = matrix.getColumnCount();

  { // Init matrix type list
    VarType* it = data.type + 17;
    for(VarType type : matrix.type_list) {
      *it = type;
      ++it;
    }
  }

  { // Init variable values
    void** it = reinterpret_cast<void**>(data.bytes + 17 + matrix.type_list.size());
    for(const Variable& var : matrix.var_list) {
      *it = var.getDataPointer();
      const_cast<Variable&>(var).data.ptr = nullptr;
      ++it;
    }
  }

}

Variable::Variable(tbl table) : data(tryMalloc(17 + table.getNeededMemory())) {
  data.type[0] = VarType::table;
  *reinterpret_cast<ui64*>(data.bytes + 1) = table.getRowCount();
  *reinterpret_cast<ui64*>(data.bytes + 9) = table.getColumnCount();

  { // Init table columns
    tbl::ColumnInfo* it = reinterpret_cast<tbl::ColumnInfo*>(data.bytes + 17);
    for(const tbl::ColumnInfo& column : table.column_list) {
      it->type = column.type;
      it->name.data.ptr = column.name.data.ptr;
      const_cast<tbl::ColumnInfo&>(column).name.data.ptr = nullptr;
      ++it;
    }
  }

  {
    void** it = reinterpret_cast<void**>(data.bytes + 17 + table.column_list.size() * sizeof (tbl::ColumnInfo));
    for(const Variable& var : table.var_list) {
      *it = var.getDataPointer();
      const_cast<Variable&>(var).data.ptr = nullptr;
      ++it;
    }
  }

}

Variable::Variable(Variable&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}
Variable::Variable(Variable& other) : data(other.clone()) {}



std::ostream& operator<<(std::ostream& os, const binom::Variable& var) {
  switch (var.typeClass()) {
    case VarTypeClass::primitive: return os << var.toPrimitive();
    case VarTypeClass::buffer_array: return os << var.toBufferArray();
    case VarTypeClass::array: return os << var.toArray();
    case VarTypeClass::object: return os << var.toObject();

    default: throw SException(ErrCode::binom_invalid_type, "Not implemented!");
  }
}

