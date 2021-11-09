#include "libbinom/include/variables/variable.h"

using namespace binom;

void* Array::cloneImpl() const {
  ui64 size = msize();
  void* ptr = tryMalloc(size);
  memcpy(ptr, data.ptr, 9);

  void** it = reinterpret_cast<void**>(reinterpret_cast<ui8*>(ptr) + 9);
  for(Variable& var : *this) {
    *it = var.clone();
    ++it;
  }
  return ptr;
}

void Array::destroyImpl() {
  if(!data.ptr) return;
  for(Variable& var : *this) var.destroy();
  free(data.ptr);
  data.ptr = nullptr;
}

Array::Array() : data(tryMalloc(9)) {
  *data.type = VarType::array;
  length() = 0;
}

Array::Array(varr array) : data(tryMalloc(9 + array.size()*sizeof(Variable))) {
  data.type[0] = VarType::array;
  *reinterpret_cast<ui64*>(data.bytes + 1) = array.size();
  void** it = reinterpret_cast<void**>(data.bytes + 9);
  for(const Variable& value : array) {
    *it = value.getDataPointer();
    const_cast<Variable&>(value).data.ptr = nullptr;
    ++it;
  }
}

Array::Array(Array&& other) : data(other.data.ptr) {other.data.ptr = nullptr;}
Array::Array(const binom::Array& other) : data(other.clone()) {}

ByteArray Array::serialize() const {
  ByteArray serialized;
  serialized += byte(VarType::array);
  for(Variable& var : *this)
    serialized += var.serialize ();
  serialized += byte(VarType::end);
  return serialized;
}

binom::Array Array::deserialize(ByteArray::iterator& it) {
  VarType type = VarType(*it);
  if(type != VarType::array) throw Exception(ErrCode::binom_invalid_type);
  ++it;
  Array arr;
  while (VarType(*it) != VarType::end) {
    arr.pushBack(Variable::deserialize(it));
  }
  ++it;
  return arr;
}

Variable& Array::getVariable(ui64 index) const {
  if(index >= getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of array range");
  return begin()[index];
}

Variable& Array::insert(ui64 index, Variable var) {
  if(index > getMemberCount()) throw Exception(ErrCode::binom_out_of_range, "Out of array range");
  Variable* new_var = reinterpret_cast<Variable*>(maddto(data.bytes + msize(), sizeof (Variable)));
  ++length();
  new_var->data.ptr = var.data.ptr;
  var.data.ptr = nullptr;
  return *new_var;
}

Variable& Array::pushBack(Variable var) {
  Variable* new_var = reinterpret_cast<Variable*>(madd(sizeof (Variable)));
  ++length();
  new_var->data.ptr = var.data.ptr;
  var.data.ptr = nullptr;
  return *new_var;
}

Variable& Array::pushFront(Variable var) {
  Variable* new_var = reinterpret_cast<Variable*>(maddto(data.bytes + 9, sizeof (Variable)));
  ++length();
  new_var->data.ptr = var.data.ptr;
  var.data.ptr = nullptr;
  return *new_var;
}

Array Array::subarr(ui64 index, ui64 n) {
  if(index + n > getMemberCount())
    n = getMemberCount() - index;
  if(!n) return Array();
  ByteArray result(9 + n * sizeof (Variable));
  result.set<VarType>(0,0,VarType::array);
  result.set<ui64>(0,1,n);
  for(Variable* start = reinterpret_cast<Variable*>(data.bytes + 9 + index*sizeof(Variable)),
      *it = start,
      *res_it = result.begin<Variable>(9); ui64(it - start) < n; (++it, ++res_it))
    *res_it = *it;
  void* _result = result.unfree();
  return *reinterpret_cast<Array*>(&_result);
}

void Array::remove(ui64 index, ui64 n) {
  if(index + n > getMemberCount())
    n = getMemberCount() - index;
  if(!n) return;
  Variable* start = reinterpret_cast<Variable*>(data.bytes + 9 + index*sizeof(Variable));
  for(Variable* it = start;ui64(it - start) < n;++it)
    it->destroy();
  msubfrom(start, n*sizeof(Variable));
  length() -= n;
}

void Array::popBack(ui64 n) {
  if(n > getMemberCount()) throw Exception(ErrCode::binom_out_of_range);
  Variable* start = reinterpret_cast<Variable*>(data.bytes + msize() - n*sizeof(Variable));
  {
    Variable* it = start;
    for(ui64 i = 0;i < n;(++i,++it))
      it->destroy();
  }
  msubfrom(start, n*sizeof(Variable));
  length() -= n;
}

void Array::popFront(ui64 n) {
  if(n > getMemberCount()) throw Exception(ErrCode::binom_out_of_range);
  Variable* start = reinterpret_cast<Variable*>(data.bytes + 9);
  {
    Variable* it = start;
    for(ui64 i = 0;i < n;(++i,++it))
      it->destroy();
  }
  msubfrom(start, n*sizeof(Variable));
  length() -= n;
}

void Array::clear() {
  for(Variable& var : *this)
    var.destroy();
  mch(9);
  length() = 0;
}

Array& Array::operator+=(Variable var) {
  Variable* new_var = reinterpret_cast<Variable*>(madd(sizeof (Variable)));
  ++length();
  new_var->data.ptr = var.data.ptr;
  var.data.ptr = nullptr;
  return *this;
}

Array& Array::operator=(Array other) {
  destroy();
  data.ptr = other.data.ptr;
  other.data.ptr = nullptr;
  return *this;
}

bool Array::operator==(Array other) const {
  if(length() != other.length()) return false;
  ArrayIterator it = other.begin();
  for(Variable& var : *this) {
    if(var != *it) return false;
    ++it;
  }
  return true;
}

bool Array::operator!=(Array other) const {
  if(length() != other.length()) return true;
  ArrayIterator it = other.begin();
  for(Variable& var : *this) {
    if(var != *it) return true;
    ++it;
  }
  return false;
}

bool Array::operator<(Array other) const {
  if(length() < other.length()) return true;
  elif(length() > other.length()) return false;
  ArrayIterator it = other.begin();
  for(Variable& var : *this) {
    i8 cmp = var.getCompare(*it);
    if(cmp < 0) return true;
    elif(cmp > 0) return false;
    ++it;
  }
  return false;
}

bool Array::operator<=(Array other) const {
  if(length() < other.length()) return true;
  elif(length() > other.length()) return false;
  ArrayIterator it = other.begin();
  for(Variable& var : *this) {
    i8 cmp = var.getCompare(*it);
    if(cmp < 0) return true;
    elif(cmp > 0) return false;
    ++it;
  }
  return true;
}

bool Array::operator>(Array other) const {
  if(length() > other.length()) return true;
  elif(length() < other.length()) return false;
  ArrayIterator it = other.begin();
  for(Variable& var : *this) {
    i8 cmp = var.getCompare(*it);
    if(cmp < 0) return false;
    elif(cmp > 0) return true;
    ++it;
  }
  return false;
}

bool Array::operator>=(Array other) const {
  if(length() > other.length()) return true;
  elif(length() < other.length()) return false;
  ArrayIterator it = other.begin();
  for(Variable& var : *this) {
    i8 cmp = var.getCompare(*it);
    if(cmp < 0) return false;
    elif(cmp > 0) return true;
    ++it;
  }
  return true;
}

ui8 Array::getCompare(Array other) const {
  if(length() > other.length()) return 1;
  elif(length() < other.length()) return -1;
  ArrayIterator it = other.begin();
  for(Variable& var : *this) {
    i8 cmp = var.getCompare(*it);
    if(cmp) return cmp;
    ++it;
  }
  return 0;
}
