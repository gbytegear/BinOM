#ifndef NODE_VISITOR_H
#define NODE_VISITOR_H

#include "variable.h"
#include "../path.h"
#include "../query.h"

#include <vector>

namespace binom {

class NodeVisitor;

typedef std::vector<NodeVisitor> NodeVector;

class NodeVisitor {
  enum class RefType : ui8 {
    variable,
    named_variable,
    value
  };

  union Ref {
    void* ptr;
    Variable* variable;
    NamedVariable* named_variable;
    ValueRef value;

    Ref() : ptr(nullptr) {}
    Ref(Variable* var) : variable(var) {}
    Ref(NamedVariable* named_var) : named_variable(named_var) {}
    Ref(ValueRef val) : value(val) {}
    Ref(const Ref& ref) {memcpy(this, &ref, sizeof (Ref));}
    Ref(const Ref&& ref) {memcpy(this, &ref, sizeof (Ref));}

    Ref& operator=(Ref ref) {return *new(this) Ref(ref);}

  };

  RefType ref_type;
  Ref ref;

  bool test(Query query, ui64 index);

public:

  class NodeIterator;

  NodeVisitor(Variable* var);
  NodeVisitor(NamedVariable* named_var);
  NodeVisitor(ValueRef val);
  NodeVisitor(const NodeVisitor& other);

  NodeVisitor& operator=(Variable* var);
  NodeVisitor& operator=(NamedVariable* named_var);
  NodeVisitor& operator=(ValueRef val);
  NodeVisitor& operator=(const NodeVisitor& other);

  VarType getType() const;
  VarTypeClass getTypeClass() const {return toTypeClass(getType());}

  bool isPrimitive() const {return getTypeClass() == VarTypeClass::primitive;}
  bool isBufferArray() const {return getTypeClass() == VarTypeClass::buffer_array;}
  bool isArray() const {return getTypeClass() == VarTypeClass::array;}
  bool isObject() const {return getTypeClass() == VarTypeClass::object;}

  ui64 getElementCount() const;

  NodeVisitor& stepInside(ui64 index);
  NodeVisitor& stepInside(BufferArray name);
  NodeVisitor& stepInside(PathNode path);

  BufferArray& rename(BufferArray old_name, BufferArray new_name);

  ValueRef getValue() const;
  ValueRef getValue(ui64 index) const;
  Variable& getVariable() const;
  Variable& getVariable(ui64 index) const;
  Variable& getVariable(BufferArray name) const;
  Variable& getVariable(PathNode path) const;
  BufferArray getName() const;

  NodeVisitor getChild(ui64 index) {return NodeVisitor(*this).stepInside(index);}
  NodeVisitor getChild(BufferArray name) {return NodeVisitor(*this).stepInside(std::move(name));}
  NodeVisitor getChild(PathNode path) {return NodeVisitor(*this).stepInside(std::move(path));}

  NodeVisitor operator[](ui64 index) const {return NodeVisitor(*this).stepInside(index);}
  NodeVisitor operator[](BufferArray name) const {return NodeVisitor(*this).stepInside(std::move(name));}
  NodeVisitor& operator[](PathNode path) const {return NodeVisitor(*this).stepInside(std::move(path));}

  NodeVisitor& operator()(ui64 index) {return stepInside(index);}
  NodeVisitor& operator()(BufferArray name) {return stepInside(std::move(name));}
  NodeVisitor& operator()(PathNode path) {return stepInside(std::move(path));}

  NodeVector find(Query query);

  NodeIterator begin();
  NodeIterator end();

};



class NodeVisitor::NodeIterator {
  Variable* parent = nullptr;

  union Ptr {
    Variable* variable;
    NamedVariable* named_variable;
    ValueIterator value_it;

    Ptr(VarTypeClass type, Variable& parent, bool is_end = false) {
      switch (type) {
        case VarTypeClass::buffer_array: value_it = is_end? parent.toBufferArray().end() : parent.toBufferArray().begin(); return;
        case VarTypeClass::array:        variable = is_end? parent.toArray().end() : parent.toArray().begin(); return;
        case VarTypeClass::object:       named_variable = is_end? parent.toObject().end() : parent.toObject().begin(); return;
        default: throw SException(ErrCode::binom_invalid_type);
      }
    }

    Ptr(VarTypeClass type, Ptr& other) {
      switch (type) {
        case VarTypeClass::buffer_array: value_it = other.value_it; return;
        case VarTypeClass::array:        variable = other.variable; return;
        case VarTypeClass::object:       named_variable = other.named_variable; return;
        default: throw SException(ErrCode::binom_invalid_type);
      }
    }

  };

  VarTypeClass type;
  Ptr ptr;

public:
  NodeIterator(NodeVisitor& node, bool is_end = false)
    : parent(&node.getVariable()),
      type(toTypeClass(node.getType())),
      ptr(type, *parent, is_end) {}

  NodeIterator(NodeIterator& other)
    : parent(other.parent),
      type(other.type),
      ptr(type, other.ptr) {}

  NodeIterator(NodeIterator&& other)
    : parent(other.parent),
    type(other.type),
    ptr(type, other.ptr) {}

  NodeIterator& operator++() {
    switch (type) {
      case VarTypeClass::buffer_array: ++ptr.value_it; break;
      case VarTypeClass::array:        ++ptr.variable; break;
      case VarTypeClass::object:       ++ptr.named_variable; break;
      default: throw SException(ErrCode::binom_invalid_type);
    }
    return *this;
  }

  NodeIterator operator++(int) {
    NodeIterator tmp(*this);
    switch (type) {
      case VarTypeClass::buffer_array: ++ptr.value_it; break;
      case VarTypeClass::array:        ++ptr.variable; break;
      case VarTypeClass::object:       ++ptr.named_variable; break;
      default: throw SException(ErrCode::binom_invalid_type);
    }
    return tmp;
  }

  NodeIterator& operator--(){
    switch (type) {
      case VarTypeClass::buffer_array: --ptr.value_it; break;
      case VarTypeClass::array:        --ptr.variable; break;
      case VarTypeClass::object:       --ptr.named_variable; break;
      default: throw SException(ErrCode::binom_invalid_type);
    }
    return *this;
  }

  NodeIterator operator--(int) {
    NodeIterator tmp(*this);
    switch (type) {
      case VarTypeClass::buffer_array: --ptr.value_it; break;
      case VarTypeClass::array:        --ptr.variable; break;
      case VarTypeClass::object:       --ptr.named_variable; break;
      default: throw SException(ErrCode::binom_invalid_type);
    }
    return tmp;
  }

  bool operator==(NodeIterator& other) {
    if(type != other.type) return false;
    switch (type) {
      case VarTypeClass::buffer_array: return ptr.value_it == other.ptr.value_it;
      case VarTypeClass::array:        return ptr.variable == other.ptr.variable;
      case VarTypeClass::object:       return ptr.named_variable == other.ptr.named_variable;
      default: throw SException(ErrCode::binom_invalid_type);
    }
  }

  bool operator!=(NodeIterator& other) {
     if(type != other.type) return false;
     switch (type) {
       case VarTypeClass::buffer_array: return ptr.value_it != other.ptr.value_it;
       case VarTypeClass::array:        return ptr.variable != other.ptr.variable;
       case VarTypeClass::object:       return ptr.named_variable != other.ptr.named_variable;
       default: throw SException(ErrCode::binom_invalid_type);
     }
  }

  NodeVisitor operator*() {
    switch (type) {
      case VarTypeClass::buffer_array: return *ptr.value_it;
      case VarTypeClass::array:        return ptr.variable;
      case VarTypeClass::object:       return ptr.named_variable;
      default: throw SException(ErrCode::binom_invalid_type);
    }
  }


};

}

#endif
