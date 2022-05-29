TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
TARGET = binom_v2_test
LIBS += -lpthread
QMAKE_CXXFLAGS += -Wextra

SOURCES += test/test.cxx \
    libbinom/source/utils/resource_control.cxx \
    libbinom/source/variables/bit_array.cxx \
    libbinom/source/variables/buffer_array.cxx \
    libbinom/source/variables/number.cxx \
    libbinom/source/variables/variable.cxx

HEADERS += \
    libbinom/include/utils/avl_tree.hxx \
    libbinom/include/utils/bit_map64.hxx \
    libbinom/include/utils/bits.hxx \
    libbinom/include/utils/err.hxx \
    libbinom/include/utils/extended_cxx.hxx \
    libbinom/include/utils/extended_type_traits.hxx \
    libbinom/include/utils/generic_arithmetic.hxx \
    libbinom/include/utils/heritable_initializer_list.hxx \
    libbinom/include/utils/memctrl.hxx \
    libbinom/include/utils/print_variable.hxx \
    libbinom/include/utils/resource_control.hxx \
    libbinom/include/utils/shared_recursive_mutex_wrapper.hxx \
    libbinom/include/utils/type_aliases.hxx \
    libbinom/include/utils/types.hxx \
    libbinom/include/utils/util_functions.hxx \
    libbinom/include/variables/array.hxx \
    libbinom/include/variables/bit_array.hxx \
    libbinom/include/variables/buffer_array.hxx \
    libbinom/include/variables/doubly_linked_list.hxx \
    libbinom/include/variables/generic_value.hxx \
    libbinom/include/variables/map.hxx \
    libbinom/include/variables/number.hxx \
    libbinom/include/variables/singly_linked_list.hxx \
    libbinom/include/variables/variable.hxx \
    test/all_test.hxx \
    test/bits_test.hxx \
    test/generic_value_test.hxx \
    test/number_test.hxx \
    test/recursive_shared_mutex_test.hxx \
    test/types_test.hxx \
    test/tester.hxx \
    test/variable_test.hxx
