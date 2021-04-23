#ifndef VTEMPLATE_H
#define VTEMPLATE_H

#include "variables/variable.h"
#include "query.h"

#include <tuple>

namespace binom {

class VTemplate {
  ByteArray data;
public:
  class Literal;
  typedef std::initializer_list<Literal> LiteralList;
  class iterator;
  class VTemplateNode;

  VTemplate(LiteralList literal_list);

  bool test(Variable& var);
  bool test(NodeVisitor& node);
  bool test(DBNodeVisitor& db_node);

  iterator begin() const;
  iterator end() const;

};


class VTemplate::Literal  {
  friend class VTemplate;
  Path path;
  Query query;
  bool is_all;
public:
  Literal(Path path, Query query, bool is_all = false);
  Literal(const Literal&) = delete;
  Literal(Literal&&) = delete;
};

class VTemplate::VTemplateNode {
  ByteArray::iterator it;
public:
  bool isAll() const;
  Path getPath() const;
  Query getQuery() const;
};

class VTemplate::iterator {
  ByteArray::iterator it;
public:
  iterator(ByteArray::iterator it);
  iterator(const iterator& other);
  iterator(iterator&& other);

  VTemplateNode& operator*() const;
  VTemplateNode* operator->() const;

  iterator& operator++();
  iterator operator++(int);

  bool operator==(iterator other) const;
  bool operator!=(iterator other) const;
  bool operator>(iterator other) const;
  bool operator>=(iterator other) const;
  bool operator<(iterator other) const;
  bool operator<=(iterator other) const;

};

}

#endif // VTEMPLATE_H
