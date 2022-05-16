#ifndef EXTENDED_CXX_HXX
#define EXTENDED_CXX_HXX

// A little bit of what I'm used to

#define self (*this)        // T* this -> T& this
#define elif else if        // if(condition) {...} elif(condition) {...} else {...} -> if(condition) {...} else if(condition) {...} else {...}
#define forever while(1)    // forever {... if(condition) break; ...} -> while(1) {... if(condition) break; ...}
#define ever (;;)           // for ever {... if(condition) break; ...} -> for (;;) {... if(condition) break; ...}
#define UNUSED(V) (void)(V) // Remove warning

#endif // EXTENDED_CXX_HXX
