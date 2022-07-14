//#define FULL_TEST
#define TEST_FULL_INFO
#include "test/all_test.hxx"
#include "libbinom/include/binom_impl/print_variable.hxx"

#include "libbinom/include/binom_impl/avl_tree.hxx"

#include <mutex>

int main() {
  testTypesConversions();
  testGenericValue();
  testNumber();
  testBits();
  testBufferArray();

#ifdef FULL_TEST // Questionable or incompletely implemented tests
  testRecursiveSharedMutex();
  testVariable(); // Not ended!
#endif

  testAllBugs();

  AVLTree avl_tree;
  avl_tree.insert(new AVLTree::Node(KeyValue(5)));
  avl_tree.insert(new AVLTree::Node(KeyValue(.125)));
  avl_tree.insert(new AVLTree::Node(KeyValue(20)));
  avl_tree.insert(new AVLTree::Node(KeyValue(1)));
  avl_tree.insert(new AVLTree::Node(KeyValue(.25)));
  avl_tree.insert(new AVLTree::Node(KeyValue(literals::ui16arr{1,2,3,4,5,6,7,8,9,10})));
  avl_tree.insert(new AVLTree::Node(KeyValue(256)));
  avl_tree.insert(new AVLTree::Node(KeyValue(256_ui64)));
  avl_tree.insert(new AVLTree::Node(KeyValue(74)));
  avl_tree.insert(new AVLTree::Node(KeyValue(-.0)));
  avl_tree.insert(new AVLTree::Node(KeyValue(.125_f32)));
  avl_tree.insert(new AVLTree::Node(KeyValue(83)));
  avl_tree.insert(new AVLTree::Node(KeyValue(.0_f32)));
  avl_tree.insert(new AVLTree::Node(KeyValue(20_ui16)));
  avl_tree.insert(new AVLTree::Node(KeyValue(.5)));
  avl_tree.insert(new AVLTree::Node(KeyValue(literals::ui8arr{1,2,3,4,5,6,7,8,9,10})));

  for(auto& element : avl_tree) {
    binom::utils::printVariable(element.getKey());
  }

  delete avl_tree.extract(KeyValue(5));
  delete avl_tree.extract(KeyValue(.125)); // BUG
  delete avl_tree.extract(KeyValue(20));
  delete avl_tree.extract(KeyValue(1));
  delete avl_tree.extract(KeyValue(.25));
  delete avl_tree.extract(KeyValue(literals::ui16arr{1,2,3,4,5,6,7,8,9,10}));
  delete avl_tree.extract(KeyValue(256));

  std::clog << "==================\n";

  for(auto& element : avl_tree) {
    binom::utils::printVariable(element.getKey());
  }

  delete avl_tree.extract(KeyValue(256_ui64));
  delete avl_tree.extract(KeyValue(74));
  delete avl_tree.extract(KeyValue(-.0));
  delete avl_tree.extract(KeyValue(.125_f32));
  delete avl_tree.extract(KeyValue(83));
  delete avl_tree.extract(KeyValue(.0_f32));
  delete avl_tree.extract(KeyValue(20_ui16));
  delete avl_tree.extract(KeyValue(.5));
  delete avl_tree.extract(KeyValue(literals::ui8arr{1,2,3,4,5,6,7,8,9,10}));

  std::clog << "==================\n";

  for(auto& element : avl_tree) {
    binom::utils::printVariable(element.getKey());
  }


}
