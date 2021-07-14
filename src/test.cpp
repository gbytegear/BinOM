#include "binom/includes/binom.h"

using namespace binom;

int main() {
  try {

    output_manip.buffer_array = OutputManip::PRIMITIVE;
    output_manip.primitive = OutputManip::UNSIGNED;

    Variable var = vobj{
        {"a", 1_ui8},
        {"b", 2_ui16},
        {"c", 4_ui32},
        {"d", 8_ui64},
        {"e", ui8arr{1,2,3}},
        {"f", ui16arr{4,5,6}},
        {"g", ui32arr{7,8,9}},
        {"h", ui64arr{10,11,12}},
        {"j", varr{1,2,3}},
        {"k", vobj{
           {"a", 1_ui8},
           {"b", 2_ui16},
           {"c", 4_ui32},
           {"d", 8_ui64},
           {"e", ui8arr{1,2,3}},
           {"f", ui16arr{4,5,6}},
           {"g", ui32arr{7,8,9}},
           {"h", ui64arr{10,11,12}},
           {"j", varr{1,2,3}}
         }}
      };

    ByteArray ser = var.serialize();

    Variable var_deser = Variable::deserialize(ser);


    std::clog << "Constructed:\n" << var << "\n\n"
                 "Serialized: " << BufferArray(ser) << "\n\n"
                 "Deserialized:\n" << var_deser << "\n\n";


    std::clog << "=========================================================================\n"
                 "|                    Test completed successfully                        |\n"
                 "=========================================================================\n";

  } catch(Exception& except) {
    std::cerr << except.full() << std::endl;
  } catch(std::exception& except) {
    std::cerr << except.what() << std::endl;
  }

  return 0;
}
