#ifndef NODE_VISITOR_H
#define NODE_VISITOR_H

#include "variable.h"

namespace binom {

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

  VarType getType();

  NodeVisitor& stepInside(ui64 index);
  NodeVisitor& stepInside(BufferArray name);

  BufferArray& rename(BufferArray name);

  ValueRef getValue() const;
  ValueRef getValue(ui64 index) const;
  Variable& getVariable() const;
  Variable& getVariable(ui64 index) const;
  Variable& getVariable(BufferArray name) const;
  NamedVariable& getNamedVariable() const;
  NamedVariable& getNamedVariable(BufferArray name) const;

};

}

#endif
