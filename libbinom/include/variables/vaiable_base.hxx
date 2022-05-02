#ifndef VAIABLE_BASE_HXX
#define VAIABLE_BASE_HXX

#include "../utils/resource_control.hxx"

namespace binom::priv {

template <typename DrivenType>
class VariableBase {
protected:

  Link resource_link;

public:

  OptionalSharedRecursiveLock getLock(MtxLockType lock_type) const noexcept {
    return resource_link.getLock(lock_type);
  }

  VarType getType() const noexcept {
    if(auto lk = getLock(priv::MtxLockType::shared_locked); !lk)
      return resource_link.getType();
    else
      return VarType::invalid_type;
  }

  ValType getValType() const noexcept {
    if(auto lk = getLock(priv::MtxLockType::shared_locked); !lk)
      return toValueType(resource_link.getType());
    else
      return ValType::invalid_type;
  }

  VarBitWidth getBitWidth() const noexcept {
    if(auto lk = getLock(priv::MtxLockType::shared_locked); !lk)
      return toBitWidth(resource_link.getType());
    else
      return VarBitWidth::invalid_type;
  }

  VarNumberType getNumberType() const noexcept {
    if(auto lk = getLock(priv::MtxLockType::shared_locked); !lk)
      return toNumberType(resource_link.getType());
    else
      return VarNumberType::invalid_type;
  }

  ui8 getMemebrSize() const noexcept;

};


}

#endif // VAIABLE_BASE_HXX
