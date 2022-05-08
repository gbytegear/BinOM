#include "libbinom/include/variables/number.hxx"

using namespace binom;
using namespace binom::priv;

OptionalSharedRecursiveLock GenericValueRef::getLock(binom::priv::MtxLockType lock_type) const noexcept {
  return owner.getLock(lock_type);
}

Number Variable::createRefVariable(const Number& other) noexcept {return priv::Link(other.resource_link);}

bool GenericValueRef::checkLock(const OptionalSharedRecursiveLock& lock) noexcept {return lock.has_value();}
