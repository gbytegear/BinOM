#include "binom/includes/structure/variables/variable.h"

int main() {
  using namespace binom;

  using T = VarType;

  Variable test =
  mtrx(
  {T::qword, T::byte_array, T::byte_array, T::byte, T::object},
  {
          0_ui64, "admin", "admin", 0xff_ui8, obj{},
          1_ui64, "guest", "guest", 0x00_ui8, obj{},
          2_ui64, "sh13max", "ifvfyvfrc13", 0xff_ui8, obj{}
  });

  return 0;
}
