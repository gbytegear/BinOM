#ifndef LIST_H
#define LIST_H

#include <cassert>

template<typename T>
class List {
  class Node {
    Node* prev = nullptr;
    Node* next = nullptr;
    T data;
  public:
    Node(T data) : data(data) {}

    Node* addNext(T data) {
      assert(next == nullptr);
      return next = new Node(data);
    }

    Node* addPrev(T data) {
      assert(prev == nullptr);
      return prev = new Node(data);
    }
  };
};

#endif // LIST_H
