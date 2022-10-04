#ifndef LIST_IMPL_HXX
#define LIST_IMPL_HXX

#include "../../variables/variable.hxx"
#include <list>

namespace binom::priv {

class ListImplementation {
public:

  typedef std::list<Variable> List;
  typedef List::iterator Iterator;
  typedef List::const_iterator ConstIterator;
  typedef List::reverse_iterator ReverseIterator;
  typedef List::const_reverse_iterator ConstReverseIterator;
  typedef List::reference Reference;
  typedef List::const_reference ConstReference;

private:

  List variable_list;

public:
  ListImplementation() = default;
  ListImplementation(const literals::list& value_list);
  ListImplementation(const ListImplementation& other);
  ~ListImplementation();

  bool isEmpty() const noexcept;
  size_t getElementCount() const noexcept;

  void clear();

  Variable pushBack(Variable var);
  Iterator pushBack(const literals::list& value_list);

  Variable pushFront(Variable var);
  Iterator pushFront(const literals::list& value_list);

  Iterator insert(Iterator it, Variable var);
//  Iterator insert(Iterator it, literals::dllist& value_list) {

//  }

  void popBack();
  void popFront();
  void remove(Iterator it);

  Iterator begin();
  Iterator end();

  ConstIterator begin() const;
  ConstIterator end() const;

  ConstIterator cbegin() const;
  ConstIterator cend() const;

  ReverseIterator rbegin();
  ReverseIterator rend();

  ConstReverseIterator rbegin() const;
  ConstReverseIterator rend() const;

  ConstReverseIterator crbegin() const;
  ConstReverseIterator crend() const;
};

}

#endif // LIST_IMPL_HXX
