#include "binom/includes/structure/vtemplate.h"

using namespace binom;


VTemplate::VTemplate(VTemplate::LiteralList literal_list) {
  for(const Literal& literal : literal_list) {
    if(literal.path.isEmpty() || literal.query.isEmpty()) continue;
    ByteArray path = literal.path.toByteArray(),
        query = literal.path.toByteArray();
    data.pushBack<bool>(literal.is_all);
    data.pushBack<ui64>(path.length());
    data.pushBack<ui64>(query.length());
    data.pushBack(path);
    data.pushBack(query);
  }
}

VTemplate::iterator VTemplate::begin() const {return data.begin();}
VTemplate::iterator VTemplate::end() const {return data.end();}

VTemplate::Literal::Literal(Path path, Query query, bool is_all)
  : path(std::move(path)), query(std::move(query)), is_all(is_all) {}

VTemplate::iterator::iterator(ByteArray::iterator it) : it(it) {}
VTemplate::iterator::iterator(const binom::VTemplate::iterator& other) : it(other.it) {}
VTemplate::iterator::iterator(binom::VTemplate::iterator&& other) : it(other.it) {}

VTemplate::VTemplateNode& VTemplate::iterator::operator*() const {return *reinterpret_cast<VTemplate::VTemplateNode*>(const_cast<iterator*>(this));}
VTemplate::VTemplateNode* VTemplate::iterator::operator->() const {return reinterpret_cast<VTemplate::VTemplateNode*>(const_cast<iterator*>(this));}

binom::VTemplate::iterator& VTemplate::iterator::operator++() {
  (++it) += // Skip `is_all`
            *reinterpret_cast<ui64*>(it) + // Skip `Path`
            *reinterpret_cast<ui64*>(it + sizeof (ui64)) + // Skip `Query`
            2*sizeof (ui64);  // Skip sizes
  return *this;
}

binom::VTemplate::iterator VTemplate::iterator::operator++(int) {iterator last = *this; ++(*this); return last;}

bool VTemplate::iterator::operator==(binom::VTemplate::iterator other) const {return it == other.it;}
bool VTemplate::iterator::operator!=(binom::VTemplate::iterator other) const {return it != other.it;}
bool VTemplate::iterator::operator>(binom::VTemplate::iterator other) const {return it > other.it;}
bool VTemplate::iterator::operator>=(binom::VTemplate::iterator other) const {return it >= other.it;}
bool VTemplate::iterator::operator<(binom::VTemplate::iterator other) const {return it < other.it;}
bool VTemplate::iterator::operator<=(binom::VTemplate::iterator other) const {return it <= other.it;}

bool VTemplate::VTemplateNode::isAll() const {return *reinterpret_cast<bool*>(it);}

Path VTemplate::VTemplateNode::getPath() const {
  ByteArray data(it + 1 + sizeof(ui64)*2, *reinterpret_cast<ui64*>(it+1));
  return Path::fromByteArray(data);
}

Query VTemplate::VTemplateNode::getQuery() const {
  ByteArray data(it + 1 + sizeof(ui64)*2 + *reinterpret_cast<ui64*>(it+1), *reinterpret_cast<ui64*>(it+1+sizeof(ui64)));
  return Query::fromByteArray(data);
}
