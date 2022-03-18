TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
TARGET = binom_v2_test

SOURCES += test/test.cxx \
    libbinom/source/variables/generic_value.cxx

HEADERS += \
    libbinom/include/utils/bit_map64.hxx \
    libbinom/include/utils/err.hxx \
    libbinom/include/utils/memctrl.hxx \
    libbinom/include/utils/resource_control.hxx \
    libbinom/include/utils/tree.hxx \
    libbinom/include/utils/type_aliases.hxx \
    libbinom/include/utils/types.hxx \
    libbinom/include/variables/generic_value.hxx \
    test/all_test.hxx \
    test/generic_value_test.hxx \
    test/types_test.hxx \
    test/tester.hxx
