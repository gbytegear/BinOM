#include "binom/includes/structure/variables/variable.h"

using namespace binom;

ui64 Object::msize() const {return 9 + length() * sizeof (NamedVariable);}

void Object::mch(size_t new_size) {
  data.ptr = tryRealloc(data.ptr, new_size);
}

void* Object::madd(size_t add_size) {
  size_t shift = msize();
  mch(shift + add_size);
  return data.bytes + shift;
}

void Object::msub(size_t sub_size) {
  mch(msize() - sub_size);
}

void* Object::maddto(void* to, size_t size) {
  size_t pos = reinterpret_cast<byte*>(to) - data.bytes;
  size_t old_size = msize();
  madd(size);
  memmove(data.bytes + pos + size, data.bytes + pos, old_size - pos);
  return data.bytes + pos;
}

void Object::msubfrom(void* from, size_t size) {
  if(from < data.ptr) throw Exception(ErrCode::binom_out_of_range);
  size_t old_size = msize();
  memmove(from, reinterpret_cast<byte*>(from) + size,
          old_size - (reinterpret_cast<byte*>(from) - data.bytes) - size);
  mch(old_size - size);
}

void* Object::clone() const {
  ui64 size = msize();
  byte* ptr = tryMalloc<byte>(size);
  memcpy(ptr, data.ptr, 9);

  ObjectIterator it = reinterpret_cast<ObjectIterator>(ptr + 9);
  for(NamedVariable& this_it : *this) {
    *it = this_it;
    ++it;
  }
  return ptr;
}

void Object::destroy() {
  if(!data.ptr) return;
  for(NamedVariable& nvar : *this) {
    nvar.name.destroy();
    nvar.variable.destroy();
  }
  free(data.ptr);
  data.ptr = nullptr;
}

Object::Object() : data(tryMalloc(9)) {
  *data.type = VarType::object;
  length() = 0;
}

Object::Object(vobj object) : data(tryMalloc(9 + object.size()*sizeof(NamedVariable))) {
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

ByteArray Object::serialize() const {
  ByteArray serialised;
  serialised += byte(VarType::object);
  for(NamedVariable& n_var : *this) {
    serialised += n_var.name.toString().c_str();
    serialised += n_var.variable.serialize();
  }
  serialised += byte(VarType::end);
  return serialised;
}

Object Object::deserialize(ByteArray::iterator& it) {
  VarType type = VarType(*it);
  if(type != VarType::object) throw Exception(ErrCode::binom_invalid_type);
  ++it;
  Object obj;
  while (VarType(*it) != VarType::end) {
    char* name = reinterpret_cast<char*>(it);
    it += strlen(name) + 1;
    obj.insert(name, Variable::deserialize(it));
  }
  ++it;
  return obj;
}

bool Object::contains(BufferArray name) const {
  i64 left = 0;
  i64 right = length();
  i64 middle = 0;

  NamedVariable* it = reinterpret_cast<NamedVariable*>(data.bytes + 9);

  while (left <= right) {
    middle = (left + right) / 2;
    if(middle >= length()) break;

    if(it[middle].name > name) right = middle - 1;
    elif(it[middle].name < name) left = middle + 1;
    elif(it[middle].name == name) return true;
  }
  return false;
}

Variable& Object::insert(BufferArray name, Variable var) {
  if(!length()) {
    NamedVariable& member = *reinterpret_cast<NamedVariable*>(madd(sizeof (NamedVariable)));
    member.name.data.ptr = name.data.ptr;
    member.variable.data.ptr = var.data.ptr;
    name.data.ptr = nullptr;
    var.data.ptr = nullptr;
    ++length();

    return member.variable;
  } else {
    i64 left = 0;
    i64 right = length();
    i64 middle = 0;

    NamedVariable* it = reinterpret_cast<NamedVariable*>(data.bytes + 9);

    while (left <= right) {
      middle = (left + right) / 2;
      if(middle >= length()) break;

      if(it[middle].name > name) right = middle - 1;
      elif(it[middle].name < name) left = middle + 1;
      elif(it[middle].name == name) {
        it[middle].variable = var;
        return it[middle].variable;
      }
    }

    for(; (middle < length())? it[middle].name < name : false ;++middle);

    NamedVariable& member = *reinterpret_cast<NamedVariable*>(maddto(it + middle, sizeof (NamedVariable)));
    member.name.data.ptr = name.data.ptr;
    member.variable.data.ptr = var.data.ptr;
    name.data.ptr = nullptr;
    var.data.ptr = nullptr;
    ++length();

    return member.variable;
  }
}

void Object::remove(BufferArray name) {
  i64 left = 0;
  i64 right = length();
  i64 middle = 0;

  NamedVariable* it = reinterpret_cast<NamedVariable*>(data.bytes + 9);

  while (left <= right) {
    middle = (left + right) / 2;
    if(middle >= length()) break;

    if(it[middle].name > name) right = middle - 1;
    elif(it[middle].name < name) left = middle + 1;
    elif(it[middle].name == name) {
      msubfrom(it + middle, sizeof (NamedVariable));
      --length();
      return;
    }
  }

  throw Exception(ErrCode::binom_out_of_range, "");
}

BufferArray& Object::rename(BufferArray old_name, BufferArray new_name) {
  NamedVariable& old_named_variable = getNamedVariable(old_name);

  i64 left = 0;
  i64 right = length();
  i64 middle = 0;

  NamedVariable* it = reinterpret_cast<NamedVariable*>(data.bytes + 9);

  while (left <= right) {
    middle = (left + right) / 2;
    if(middle >= length()) break;

    if(it[middle].name > new_name) right = middle - 1;
    elif(it[middle].name < new_name) left = middle + 1;
    elif(it[middle].name == new_name) throw Exception(ErrCode::binom_object_key_error, "");
  }

  for(; (middle < length())? it[middle].name < new_name : false ;++middle);
  NamedVariable new_named_variable{std::move(new_name), std::move(old_named_variable.variable)};
  old_named_variable.name.destroy();

  if(&old_named_variable < it + middle)
    memmove(&old_named_variable, &old_named_variable + 1, ((it + --middle) - &old_named_variable)*sizeof(NamedVariable));
  else
    memmove (it + middle + 1, it + middle, (&old_named_variable - (it + middle))*sizeof(NamedVariable));
  new(it + middle) NamedVariable{std::move(new_named_variable.name), std::move(new_named_variable.variable)};

  return it[middle].name;
}

Object& Object::operator=(Object other) {
  destroy();
  data.ptr = other.data.ptr;
  other.data.ptr = nullptr;
  return *this;
}

Object& Object::operator +=(NamedVariable named_variable) {
  insert(named_variable.name, named_variable.variable);
  return *this;
}

bool Object::operator==(Object other) const {
  if(length() != other.length()) return false;
  ObjectIterator it = other.begin();
  for(NamedVariable& nvar : *this) {
    if(nvar.name != it->name ||
       nvar.variable != it->variable) return false;
    ++it;
  }
  return true;
}

bool Object::operator!=(Object other) const {
  if(length() != other.length()) return true;
  ObjectIterator it = other.begin();
  for(NamedVariable& nvar : *this) {
    if(nvar.name != it->name ||
       nvar.variable != it->variable) return true;
    ++it;
  }
  return false;
}

bool Object::operator<(Object other) const {
  if(length() < other.length()) return true;
  elif(length() > other.length()) return false;
  ObjectIterator it = other.begin();
  for(NamedVariable& nvar : *this) {
    i8 cmp = nvar.name.getCompare(it->name);
    if(cmp > 0) return false;
    elif(cmp < 0) return true;
    cmp = nvar.variable.getCompare(it->variable);
    if(cmp > 0) return false;
    elif(cmp < 0) return true;
    ++it;
  }
  return false;
}

bool Object::operator<=(Object other) const {
  if(length() < other.length()) return true;
  elif(length() > other.length()) return false;
  ObjectIterator it = other.begin();
  for(NamedVariable& nvar : *this) {
    i8 cmp = nvar.name.getCompare(it->name);
    if(cmp > 0) return false;
    elif(cmp < 0) return true;
    cmp = nvar.variable.getCompare(it->variable);
    if(cmp > 0) return false;
    elif(cmp < 0) return true;
    ++it;
  }
  return true;
}

bool Object::operator>(Object other) const {
  if(length() < other.length()) return false;
  elif(length() > other.length()) return true;
  ObjectIterator it = other.begin();
  for(NamedVariable& nvar : *this) {
    i8 cmp = nvar.name.getCompare(it->name);
    if(cmp > 0) return true;
    elif(cmp < 0) return false;
    cmp = nvar.variable.getCompare(it->variable);
    if(cmp > 0) return true;
    elif(cmp < 0) return false;
    ++it;
  }
  return false;
}

bool Object::operator>=(Object other) const {
  if(length() < other.length()) return false;
  elif(length() > other.length()) return true;
  ObjectIterator it = other.begin();
  for(NamedVariable& nvar : *this) {
    i8 cmp = nvar.name.getCompare(it->name);
    if(cmp > 0) return true;
    elif(cmp < 0) return false;
    cmp = nvar.variable.getCompare(it->variable);
    if(cmp > 0) return true;
    elif(cmp < 0) return false;
    ++it;
  }
  return true;
}

i8 Object::getCompare(Object other) const {
  if(length() > other.length()) return 1;
  elif(length() < other.length()) return -1;
  ObjectIterator it = other.begin();
  for(NamedVariable& nvar : *this) {
    i8 ncmp = nvar.name.getCompare(it->name);
    if(ncmp) return ncmp;
    i8 cmp = nvar.variable.getCompare(it->variable);
    if(cmp) return cmp;
    ++it;
  }
  return 0;
}

Object& Object::operator+=(Object other) {
  for(NamedVariable& named_variable : other)
    insert(named_variable.name, named_variable.variable);
  return *this;
}

NamedVariable& Object::getNamedVariable(BufferArray name) const {
  i64 left = 0;
  i64 right = length();
  i64 middle = 0;

  NamedVariable* it = reinterpret_cast<NamedVariable*>(data.bytes + 9);

  while (left <= right) {
    middle = (left + right) / 2;
    if(middle >= length()) break;

    if(it[middle].name > name) right = middle - 1;
    elif(it[middle].name < name) left = middle + 1;
    elif(it[middle].name == name) return it[middle];
  }

  throw Exception(ErrCode::binom_out_of_range, "");
}

Variable& Object::getVariable(BufferArray name) const {
  i64 left = 0;
  i64 right = length();
  i64 middle = 0;

  NamedVariable* it = reinterpret_cast<NamedVariable*>(data.bytes + 9);

  while (left <= right) {
    middle = (left + right) / 2;
    if(middle >= length()) break;

    if(it[middle].name > name) right = middle - 1;
    elif(it[middle].name < name) left = middle + 1;
    elif(it[middle].name == name) return it[middle].variable;
  }

  throw Exception(ErrCode::binom_out_of_range, "");
}

Array Object::getNameArray() {
  ByteArray buffer(1 + 8 + getMemberCount()*PTR_SZ);
  buffer[0] = byte(VarType::array);
  buffer.set<ui64>(0,1,getMemberCount());
  BufferArray* it = buffer.begin<BufferArray>(9);
  for(NamedVariable& named : *this) {*it = named.name;++it;}
  void* ptr = buffer.unfree();
  return *reinterpret_cast<Array*>(&ptr);
}

Array Object::getMemberArray() {
  ByteArray buffer(1 + 8 + getMemberCount()*PTR_SZ);
  buffer[0] = byte(VarType::array);
  buffer.set<ui64>(0,1,getMemberCount());
  Variable* it = buffer.begin<Variable>(9);
  for(NamedVariable& named : *this) {*it = named.variable;++it;}
  void* ptr = buffer.unfree();
  return *reinterpret_cast<Array*>(&ptr);
}
