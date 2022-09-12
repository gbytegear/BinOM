#ifndef AVL_TREE_TEST_HXX
#define AVL_TREE_TEST_HXX

#include "tester.hxx"
#include "libbinom/include/binom_impl/avl_tree.hxx"
#include "libbinom/include/variables/key_value.hxx"
#include "libbinom/include/utils/reverse_iterator.hxx"
#include "print_variable.hxx"

void printAVLTree(binom::priv::AVLTree& avl_tree) {
  TEST_ANNOUNCE(AVL Tree print:)
  GRP_PUSH
  for(auto& element : reverse_iterator::ReverseRange(avl_tree)) {
    binom::utils::printVariable(element.getKey());
  }
  GRP_POP
}

void testAVLTree() {
  using namespace binom;
  using namespace binom::priv;
  using AVLNode = AVLTree::AVLNode;

  RAIIPerfomanceTest test_perf("AVLTree test: ");
  SEPARATOR
  TEST_ANNOUNCE(AVLTree test)
  GRP_PUSH
  TEST_ANNOUNCE(AVLTree nodes inserting...)
  GRP_PUSH

    PRINT_RUN(AVLTree avl_tree);
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(5))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(.125))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(20))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(1))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(.25))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(literals::ui16arr{1,2,3,4,5,6,7,8,9,10}))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(256))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(256_ui64))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(74))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(-.0))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(.125_f32))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(83))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(.0_f32))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(20_ui16))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(.5))));
    PRINT_RUN(avl_tree.insert(new AVLNode(KeyValue(literals::ui8arr{1,2,3,4,5,6,7,8,9,10}))));

    printAVLTree(avl_tree);

  GRP_POP
  TEST_ANNOUNCE(AVLTree nodes removing...)
  GRP_PUSH

    PRINT_RUN(delete avl_tree.extract(KeyValue(5)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(.125)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(20)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(1)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(.25)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(literals::ui16arr{1,2,3,4,5,6,7,8,9,10})));
    PRINT_RUN(delete avl_tree.extract(KeyValue(256)));

    printAVLTree(avl_tree);

    PRINT_RUN(delete avl_tree.extract(KeyValue(256_ui64)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(74)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(-.0)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(.125_f32)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(83)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(.0_f32)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(20_ui16)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(.5)));
    PRINT_RUN(delete avl_tree.extract(KeyValue(literals::ui8arr{1,2,3,4,5,6,7,8,9,10})));

    printAVLTree(avl_tree);

  GRP_POP
  GRP_POP
}

#endif // AVL_TREE_TEST_HXX
