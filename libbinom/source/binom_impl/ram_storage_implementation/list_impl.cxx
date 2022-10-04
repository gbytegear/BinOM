#include "libbinom/include/binom_impl/ram_storage_implementation/list_impl.hxx"
#include "libbinom/include/utils/util_functions.hxx"
#include "libbinom/include/variables/list.hxx"

using namespace binom;
using namespace binom::priv;
using namespace binom::literals;

ListImplementation::ListImplementation(const literals::list& value_list) {for(auto& element : value_list) variable_list.emplace_back(element.move());}

ListImplementation::ListImplementation(const ListImplementation& other) {for(auto& element : other) variable_list.emplace_back(element);}

ListImplementation::~ListImplementation() {}

bool ListImplementation::isEmpty() const noexcept {return variable_list.empty();}

size_t ListImplementation::getElementCount() const noexcept {return variable_list.size();}

void ListImplementation::clear() {return variable_list.clear();}

Variable ListImplementation::pushBack(Variable var) {return variable_list.emplace_back(std::move(var));}

ListImplementation::Iterator ListImplementation::pushBack(const literals::list& value_list) {
  Iterator it = variable_list.rend().base();
  for(auto& element : value_list) variable_list.emplace_back(std::move(element));
  return ++it;
}

Variable ListImplementation::pushFront(Variable var) {return variable_list.emplace_front(std::move(var));}

ListImplementation::Iterator ListImplementation::pushFront(const literals::list& value_list) {
  for(auto it = value_list.end() - 1, end = value_list.begin() - 1;
      it != end; ++it) variable_list.emplace_front(std::move(*it));
  return variable_list.begin();
}

ListImplementation::Iterator ListImplementation::insert(Iterator it, Variable var) { return variable_list.emplace(it, std::move(var)); }

void ListImplementation::popBack() {return variable_list.pop_back();}

void ListImplementation::popFront() {return variable_list.pop_front();}

void ListImplementation::remove(Iterator it) {variable_list.erase(it);}

ListImplementation::Iterator ListImplementation::begin() {return variable_list.begin();}

ListImplementation::Iterator ListImplementation::end() {return variable_list.end();}

ListImplementation::ConstIterator ListImplementation::begin() const {return variable_list.begin();}

ListImplementation::ConstIterator ListImplementation::end() const {return variable_list.end();}

ListImplementation::ConstIterator ListImplementation::cbegin() const {return variable_list.cbegin();}

ListImplementation::ConstIterator ListImplementation::cend() const {return variable_list.cend();}

ListImplementation::ReverseIterator ListImplementation::rbegin() {return variable_list.rbegin();}

ListImplementation::ReverseIterator ListImplementation::rend() {return variable_list.rend();}

ListImplementation::ConstReverseIterator ListImplementation::rbegin() const {return variable_list.rbegin();}

ListImplementation::ConstReverseIterator ListImplementation::rend() const {return variable_list.rend();}

ListImplementation::ConstReverseIterator ListImplementation::crbegin() const {return variable_list.crbegin();}

ListImplementation::ConstReverseIterator ListImplementation::crend() const {return variable_list.crend();}
