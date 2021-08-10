#ifndef NODE_VISITOR_BASE_H
#define NODE_VISITOR_BASE_H

#include "../variables/variable.h"
#include "../utils/path.h"
#include "../utils/query.h"
#include <memory>

namespace binom {

enum class VisitorType : ui8 {
  ram_storage_visitor = 0,
  file_storage_visitor = 1
};

class NodeVisitorBase {
protected:
  virtual ~NodeVisitorBase() = default;
public:

  virtual VarType getType() const = 0;
  virtual VisitorType getVisitorType() const = 0;
  VarTypeClass getTypeClass() const {return toTypeClass(getType());}
  virtual bool isNull() const = 0;
  bool isInvalid() const {return getTypeClass() == VarTypeClass::invalid_type;}
  bool isPrimitive() const {return getTypeClass() == VarTypeClass::primitive;}
  bool isBufferArray() const {return getTypeClass() == VarTypeClass::buffer_array;}
  bool isArray() const {return getTypeClass() == VarTypeClass::array;}
  bool isObject() const {return getTypeClass() == VarTypeClass::object;}
  bool isIterable() const  {return !isPrimitive();}
  virtual bool isValueRef() const = 0;
  virtual ui64 getElementCount() const = 0;

  virtual NodeVisitorBase& stepInside(ui64 index) = 0;
  virtual NodeVisitorBase& stepInside(BufferArray name) = 0;
  virtual NodeVisitorBase& stepInside(Path path) = 0;

  virtual void setVariable(Variable var) = 0;
  virtual void pushBack(Variable var) = 0;
  virtual void pushFront(Variable var) = 0;
  virtual void insert(ui64 index, Variable var) = 0;
  virtual void insert(BufferArray name, Variable var) = 0;
  virtual void remove(ui64 index, ui64 count = 1) = 0;
  virtual void remove(BufferArray name) = 0;
  virtual void remove(Path path) = 0;

  virtual NodeVisitorBase& operator()(ui64 index) = 0;
  virtual NodeVisitorBase& operator()(BufferArray name) = 0;
  virtual NodeVisitorBase& operator()(Path path) = 0;

  NodeVisitor& toRAMVisitor() {
    if(getVisitorType() != VisitorType::ram_storage_visitor)
      throw Exception(ErrCode::binom_invalid_visitor);
    return *reinterpret_cast<NodeVisitor*>(this);
  }

  FileNodeVisitor& toFileVisitor() {
    if(getVisitorType() != VisitorType::file_storage_visitor)
      throw Exception(ErrCode::binom_invalid_visitor);
    return *reinterpret_cast<FileNodeVisitor*>(this);
  }
};

}

#endif // NODE_VISITOR_BASE_H
