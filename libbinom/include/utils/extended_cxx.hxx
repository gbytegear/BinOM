#ifndef EXTENDED_CXX_HXX
#define EXTENDED_CXX_HXX

#include <type_traits>

template<typename T>
inline std::remove_const_t<T> unconst(T arg) { return const_cast<std::remove_const_t<T>>(arg); }

template<typename T>
inline const T addconst(T arg) { return const_cast<const T>(arg); }

// A little bit of what I'm used to

#define self (*this)        // T* this -> T& this
#define elif else if        // if(condition) {...} elif(condition) {...} else {...} -> if(condition) {...} else if(condition) {...} else {...}
#define forever while(1)    // forever {... if(condition) break; ...} -> while(1) {... if(condition) break; ...}
#define ever (;;)           // for ever {... if(condition) break; ...} -> for (;;) {... if(condition) break; ...}
#define UNUSED(V) (void)(V) // Remove warning

#endif // EXTENDED_CXX_HXX
