#ifndef CRTP_HXX
#define CRTP_HXX

template <typename Driven>
class CRTP {
protected:
  Driven& downcast() noexcept {return *reinterpret_cast<Driven*>(this);}
  const Driven& downcast() const noexcept {return *reinterpret_cast<const Driven*>(this);}
};

#endif // CRTP_HXX
