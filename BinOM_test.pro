TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
TARGET = binom_v2_test
LIBS += -lpthread
QMAKE_CXXFLAGS += -Wextra

SOURCES += test/test.cxx

HEADERS += \
    libbinom/include/utils/avl_tree.hxx \
    libbinom/include/utils/bit_map64.hxx \
    libbinom/include/utils/crtp.hxx \
    libbinom/include/utils/err.hxx \
    libbinom/include/utils/generic_arithmetic.hxx \
    libbinom/include/utils/memctrl.hxx \
    libbinom/include/utils/resource_control.hxx \
    libbinom/include/utils/shared_recursive_mutex_wrapper.hxx \
    libbinom/include/utils/type_aliases.hxx \
    libbinom/include/utils/types.hxx \
    libbinom/include/variables/generic_value.hxx \
    libbinom/include/variables/number.hxx \
    libbinom/include/variables/vaiable_base.hxx \
    test/all_test.hxx \
    test/generic_value_test.hxx \
    test/recursive_shared_mutex_test.hxx \
    test/types_test.hxx \
    test/tester.hxx
