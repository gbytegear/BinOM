#include "binom/includes/binom.h"
#include "binom/includes/lexer.h"
#include <fstream>


int main() {

  // BinOM project namespace (all magic here)
  using namespace binom;

  // Create BinOM variable
  Variable var = varr{ // Array
    vobj{ // Object
      {"prop_1", 42_ui8}, // Byte
      {"prop_2", ui32arr{1,2,3,4,5}}, // Word array
      {"prop_3", "Hello, world!"} // Byte array from c-string
    }
  };

  // Create BinOM serialized storage file
  SerializedStorage s_storage("data.binom");
  s_storage = var; // Save Variable to BinOM file
  Variable loaded = s_storage; // Load Variable from BinOM file
  std::cout << loaded << '\n'; // Print Variable

  // Create BinOM dynamic storage file (with struct from var as default)
  DynamicStorage d_storage("data.binomdb", var);
  FileNodeVisitor root = d_storage;
  // Change "prop_3" variable in data.binomdb file
  root[{0, "prop_3"}].setVariable("Goodbie, world!");
  // Get "prop_2" value from data.binomdb file in Variable
  Variable from_storgae = root[0].getVariable("prop_2");

  // Convert structure description to Variable (More details in the BSDL section)
  std::ifstream t("struct.bsdl");
  std::stringstream str_stream;
  str_stream << t.rdbuf();
  Variable data = lexer << str_stream.str();
  std::cout << "From BSDL: " << data << '\n';

  // Serialize Variable
  ByteArray buffer = var.serialize();
  // ... And send it over network ...
}
