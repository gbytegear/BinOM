#include "binom/includes/structure/variables/variable.h"
#include "binom/includes/structure/variables/node_visitor.h"
#include <sstream>


using namespace binom;

OutputManip binom::output_manip;

void* Variable::clone() const {
  switch (toTypeClass(*data.type)) {
    case VarTypeClass::primitive: return toPrimitive().clone();
    case VarTypeClass::buffer_array: return toBufferArray().clone();
    case VarTypeClass::array: return toArray().clone();
    case VarTypeClass::object: return toObject().clone();
    default:
    case VarTypeClass::invalid_type: throw Exception(ErrCode::binom_invalid_type);
  }
}

void Variable::destroy() {
  if(isNull()) return;
  switch (toTypeClass(*data.type)) {
    case VarTypeClass::primitive: return toPrimitive().destroy();
    case VarTypeClass::buffer_array: return toBufferArray().destroy();
    case VarTypeClass::array: return toArray().destroy();
    case VarTypeClass::object: return toObject().destroy();
    default: break;
  }
}

Variable::Variable(VarType type) : data(nullptr)  {
  switch (toTypeClass(type)) {
    case binom::VarTypeClass::primitive: new(this) Primitive(type); break;
    case binom::VarTypeClass::buffer_array: new(this) BufferArray(type); break;
    case binom::VarTypeClass::array: new(this) Array(); break;
    case binom::VarTypeClass::object: new(this) Object(); break;
    case binom::VarTypeClass::invalid_type: break;
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

Variable::Variable(vobj object) : data(tryMalloc(9 + object.size()*sizeof(NamedVariable))) {
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
        elif(it[middle].name == value.name) throw Exception(ErrCode::binom_object_key_error, "");
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

Variable::Variable(Primitive primitive) : data(primitive.data.ptr) {primitive.data.ptr = nullptr;}
Variable::Variable(BufferArray buffer_array) : data(buffer_array.data.ptr) {buffer_array.data.ptr = nullptr;}
Variable::Variable(Array array) : data(array.data.ptr) {array.data.ptr = nullptr;}
Variable::Variable(Object object) : data(object.data.ptr) {object.data.ptr = nullptr;}

Variable::Variable(ValueRef ref) : Variable(Primitive(ref)) {}

Variable::Variable(Variable&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}
Variable::Variable(const Variable& other) : data(other.clone()) {}

Variable Variable::create(VarType type) {
  switch (type) {
    case binom::VarType::byte:
    case binom::VarType::word:
    case binom::VarType::dword:
    case binom::VarType::qword: return Primitive(type);
    case binom::VarType::byte_array:
    case binom::VarType::word_array:
    case binom::VarType::dword_array:
    case binom::VarType::qword_array: return BufferArray(type);
    case binom::VarType::array: return Array();
    case binom::VarType::object: return Object();
    case binom::VarType::end:
    case binom::VarType::invalid_type:
    default: return nullptr;
  }
}

ByteArray Variable::serialize() const {
  switch (typeClass()) {
    case VarTypeClass::primitive: return toPrimitive().serialize();
    case VarTypeClass::buffer_array: return toBufferArray().serialize();
    case VarTypeClass::array: return toArray().serialize();
    case VarTypeClass::object: return toObject().serialize();
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

Variable Variable::deserialize(ByteArray::iterator& it) {
  switch (toTypeClass (VarType(*it))) {
    case VarTypeClass::primitive: return std::move(Primitive::deserialize(it).asVar());
    case VarTypeClass::buffer_array: return std::move(BufferArray::deserialize(it).asVar());
    case VarTypeClass::array: return std::move(Array::deserialize(it).asVar());
    case VarTypeClass::object: return std::move(Object::deserialize(it).asVar());
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

Variable& Variable::operator=(Variable other) {
  destroy();
  data.ptr = other.data.ptr;
  other.data.ptr = nullptr;
  return *this;
}

bool Variable::operator==(Variable other) const {
  if(type() != other.type()) return false;
  switch (typeClass()) {
    case binom::VarTypeClass::primitive: return toPrimitive() == other.toPrimitive();
    case binom::VarTypeClass::buffer_array: return toBufferArray() == other.toBufferArray();
    case binom::VarTypeClass::array: return toArray() == other.toArray();
    case binom::VarTypeClass::object: return toObject() == other.toObject();
    default: return true;
  }
}

bool Variable::operator!=(Variable other) const {
  if(type() != other.type()) return true;
  switch (typeClass()) {
    case binom::VarTypeClass::primitive: return toPrimitive() != other.toPrimitive();
    case binom::VarTypeClass::buffer_array: return toBufferArray() != other.toBufferArray();
    case binom::VarTypeClass::array: return toArray() != other.toArray();
    case binom::VarTypeClass::object: return toObject() != other.toObject();
    default: return false;
  }
}

bool Variable::operator<(Variable other) const {
  if(type() < other.type()) return true;
  elif(type() > other.type()) return false;
  switch (typeClass()) {
    case binom::VarTypeClass::primitive: return toPrimitive() < other.toPrimitive();
    case binom::VarTypeClass::buffer_array: return toBufferArray() < other.toBufferArray();
    case binom::VarTypeClass::array: return toArray() < other.toArray();
    case binom::VarTypeClass::object: return toObject() < other.toObject();
    default: return false;
  }
}

bool Variable::operator<=(Variable other) const {
  if(type() < other.type()) return true;
  elif(type() > other.type()) return false;
  switch (typeClass()) {
    case binom::VarTypeClass::primitive: return toPrimitive() <= other.toPrimitive();
    case binom::VarTypeClass::buffer_array: return toBufferArray() <= other.toBufferArray();
    case binom::VarTypeClass::array: return toArray() <= other.toArray();
    case binom::VarTypeClass::object: return toObject() <= other.toObject();
    default: return false;
  }
}

bool Variable::operator>(Variable other) const {
  if(type() > other.type()) return true;
  elif(type() < other.type()) return false;
  switch (typeClass()) {
    case binom::VarTypeClass::primitive: return toPrimitive() > other.toPrimitive();
    case binom::VarTypeClass::buffer_array: return toBufferArray() > other.toBufferArray();
    case binom::VarTypeClass::array: return toArray() > other.toArray();
    case binom::VarTypeClass::object: return toObject() > other.toObject();
    default: return false;
  }
}

bool Variable::operator>=(Variable other) const {
  if(type() > other.type()) return true;
  elif(type() < other.type()) return false;
  switch (typeClass()) {
    case binom::VarTypeClass::primitive: return toPrimitive() >= other.toPrimitive();
    case binom::VarTypeClass::buffer_array: return toBufferArray() >= other.toBufferArray();
    case binom::VarTypeClass::array: return toArray() >= other.toArray();
    case binom::VarTypeClass::object: return toObject() >= other.toObject();
    default: return false;
  }
}

i8 Variable::getCompare(Variable other) const {
  if(type() > other.type()) return 1;
  elif(type() < other.type()) return -1;
  switch (typeClass()) {
    case binom::VarTypeClass::primitive: return toPrimitive().getCompare(std::move(other.toPrimitive()));
    case binom::VarTypeClass::buffer_array: return toBufferArray().getCompare(std::move(other.toBufferArray()));
    case binom::VarTypeClass::array: return toArray().getCompare(std::move(other.toArray()));
    case binom::VarTypeClass::object: return toObject().getCompare(std::move(other.toObject()));
    default: return false;
  }
}

NodeVisitor Variable::getNode() {return NodeVisitor(this);}

void printIndent(std::ostream& os, ui64 ind, std::string msg = "") {
  if(!ind)return;
  for(ui64 i = 0; i < ind; ++i)
    os << '|';
  os << ' ' << msg << ' ';
}


std::ostream& printWithIndent(std::ostream&, ui64, std::string, const binom::Primitive&);
std::ostream& printWithIndent(std::ostream&, ui64, std::string, const binom::BufferArray& );
std::ostream& printWithIndent(std::ostream&, ui64, std::string, const binom::Array&);
std::ostream& printWithIndent(std::ostream&, ui64, std::string, const binom::Object&);


std::ostream& printWithIndent(std::ostream& os, ui64 ind, std::string msg, const binom::Variable& var) {
  switch (var.typeClass()) {
  case VarTypeClass::primitive: return printWithIndent(os, ind, std::move(msg), var.toPrimitive());
    case VarTypeClass::buffer_array: return printWithIndent(os, ind, std::move(msg), var.toBufferArray());
    case VarTypeClass::array: return printWithIndent(os, ind, std::move(msg), var.toArray());
    case VarTypeClass::object: return printWithIndent(os, ind, std::move(msg), var.toObject());
    default: throw Exception(ErrCode::binom_invalid_type);
  }
}

std::ostream& printWithIndent(std::ostream& os, ui64 ind, std::string msg, const binom::Primitive& primitive) {
  printIndent(os, ind, msg);
  return os << *primitive;
}

std::ostream& printWithIndent(std::ostream& os, ui64 ind, std::string msg, const binom::BufferArray& buffer) {
  printIndent(os, ind, msg);
  if(buffer.getType() == VarType::byte_array && output_manip.buffer_array == OutputManip::BufferArray::STRING)
    return os << buffer.toString();
  for(const binom::ValueRef &val : buffer)
    os << val << ' ';
  return os;
}

std::ostream& printWithIndent(std::ostream& os, ui64 ind, std::string msg, const binom::Array& array) {
  printIndent(os, ind, msg);
  os << "Array(" << array.getMemberCount() << ") [\n";
  ui64 i = 0;
  for(Variable& var : array) {
    printWithIndent(os, ind + 1, std::to_string(i)+':', var) << '\n';
    ++i;
  }
  printIndent(os, ind, "");
  return os << "]";
}

std::string nameToString(binom::BufferArray buffer) {
  std::stringstream str;
  for(const binom::ValueRef &val : buffer)
    str << val << ' ';
  return str.str();
}

std::ostream& printWithIndent(std::ostream& os, ui64 ind, std::string msg, const binom::Object& object) {
  printIndent(os, ind, msg);
  os << "Object(" << object.getMemberCount() << ") {\n";
  for(NamedVariable& nvar : object) {
    if(nvar.name.getType() == VarType::byte_array)
      printWithIndent(os, ind + 1, nvar.name.toString() + ':', nvar.variable) << '\n';
    else
      printWithIndent(os, ind + 1, '[' + nameToString(nvar.name) + "]:", nvar.variable) << '\n';
  }
  printIndent(os, ind, "");
  return os << "}";
}



std::ostream& operator<<(std::ostream& os, const binom::Variable& var) {return printWithIndent(os, 0, "", var);}

std::ostream& operator<<(std::ostream& os, const binom::ValueRef val) {

  ui8 sym_count = (val.getType() == ValType::byte) ? 2
                 :(val.getType() == ValType::word) ? 4
                 :(val.getType() == ValType::dword) ? 8
                 : 16;

  switch (output_manip.primitive) {
    case OutputManip::HEX:
    return os << std::right << std::setw(sym_count) << std::setfill('0') << std::hex << std::uppercase << val.asUnsigned()
              << std::resetiosflags(std::ios_base::basefield);
    case OutputManip::SIGNED:
      switch (val.getType()) {
        default: return os << "Invalid type";
        case binom::ValType::byte: return os << static_cast<i16>(val.asI8());
        case binom::ValType::word: return os << val.asI16();
        case binom::ValType::dword: return os << val.asI32();
        case binom::ValType::qword: return os << val.asI64();
      }
    case OutputManip::UNSIGNED:
      switch (val.getType()) {
        default: return os << "Invalid type";
        case binom::ValType::byte: return os << static_cast<ui16>(val.asUi8());
        case binom::ValType::word: return os << val.asUi16();
        case binom::ValType::dword: return os << val.asUi32();
        case binom::ValType::qword: return os << val.asUi64();
      }
    break;

  }
}

std::ostream& operator<<(std::ostream& os, binom::Primitive& primitive) {return printWithIndent(os, 0, "", primitive);}
std::ostream& operator<<(std::ostream& os, const binom::BufferArray& buffer) {return printWithIndent(os, 0, "", buffer);}
std::ostream& operator<<(std::ostream& os, const binom::Array& array) {return printWithIndent(os, 0, "", array);}
std::ostream& operator<<(std::ostream& os, const binom::Object& object) {return printWithIndent(os, 0, "", object);}
