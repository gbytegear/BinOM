#ifndef TYPE_ALIASES_HXX
#define TYPE_ALIASES_HXX

#include <cstdint>
#include <limits>
#include "extended_cxx.hxx"

//! Shorter and clearer data type aliases
namespace type_alias {

// i - int;
// f - float;
// u - unsigned;
// n - negative;
// s - short;
// l - long;
// 8/16/32/64 - data type bit depth


// Arithmetic types with explicit defined bit width and sign

typedef uint8_t   ui8;
typedef int8_t    i8;
typedef uint16_t  ui16;
typedef int16_t   i16;
typedef uint32_t  ui32;
typedef int32_t   i32;
typedef uint64_t  ui64;
typedef int64_t   i64;

typedef float     f32;
typedef double    f64;


// Abbreviated definition of C++ arithmetic data types

typedef float               f_t;
typedef double              lf_t;
typedef long double         llf_t;

typedef int                 i_t;
typedef unsigned int        ui_t;
typedef short               s_t;
typedef unsigned short      us_t;
typedef long                l_t;
typedef unsigned long       ul_t;
typedef long long           ll_t;
typedef unsigned long long  ull_t;


//

typedef ui8  byte;
typedef ui16 word;
typedef ui32 dword;
typedef ui64 qword;

constexpr ui8 operator "" _ui8 (ull_t val) {return val;}
constexpr i8 operator "" _i8 (ull_t val) {return static_cast<ll_t>(val);}
constexpr i8 operator "" _ni8 (ull_t val) {return -static_cast<ll_t>(val);}
constexpr ui16 operator "" _ui16 (ull_t val) {return val;}
constexpr i16 operator "" _i16 (ull_t val) {return static_cast<ll_t>(val);}
constexpr i16 operator "" _ni16 (ull_t val) {return -static_cast<ll_t>(val);}
constexpr ui32 operator "" _ui32 (ull_t val) {return val;}
constexpr i32 operator "" _i32 (ull_t val) {return static_cast<ll_t>(val);}
constexpr i32 operator "" _ni32 (ull_t val) {return -static_cast<ll_t>(val);}
constexpr ui64 operator "" _ui64 (ull_t val) {return val;}
constexpr i64 operator "" _i64 (ull_t val) {return static_cast<ll_t>(val);}
constexpr i64 operator "" _ni64 (ull_t val) {return -static_cast<ll_t>(val);}

constexpr f32 operator "" _f32 (llf_t val) {return val;}
constexpr f32 operator "" _nf32 (llf_t val) {return -val;}
constexpr f64 operator "" _f64 (llf_t val) {return val;}
constexpr f64 operator "" _nf64 (llf_t val) {return -val;}
constexpr llf_t operator ""_llf (llf_t val) {return val;}

constexpr i_t operator "" _i (ull_t val) {return static_cast<ll_t>(val);}
constexpr i_t operator "" _ni (ull_t val) {return -static_cast<ll_t>(val);}
constexpr ui_t operator "" _ui (ull_t val) {return val;}
constexpr s_t operator "" _s (ull_t val) {return static_cast<ll_t>(val);}
constexpr s_t operator "" _ns (ull_t val) {return -static_cast<ll_t>(val);}
constexpr us_t operator "" _us (ull_t val) {return val;}
constexpr l_t operator "" _l (ull_t val) {return static_cast<ll_t>(val);}
constexpr l_t operator "" _nl (ull_t val) {return -static_cast<ll_t>(val);}
constexpr ul_t operator "" _ul (ull_t val) {return val;}
constexpr ll_t operator "" _ll (ull_t val) {return static_cast<ll_t>(val);}
constexpr ll_t operator "" _nll (ull_t val) {return -static_cast<ll_t>(val);}
constexpr ull_t operator "" _ull (ull_t val) {return val;}

constexpr ui64 operator""_kb(ull_t val) {return val*1024;}        //!< convert to kilobytes
constexpr ui64 operator""_mb(ull_t val) {return val*1048576;}     //!< convert to megabytes
constexpr ui64 operator""_gb(ull_t val) {return val*1073741824;}  //!< convert to gigabytes

constexpr f_t FNaN = std::numeric_limits<f_t>::quiet_NaN();       //!< float not a number value
constexpr lf_t LFNaN = std::numeric_limits<lf_t>::quiet_NaN();    //!< long float not a number value
constexpr llf_t LLFNaN = std::numeric_limits<llf_t>::quiet_NaN(); //!< long long float not a number value
constexpr f32 F32NaN = std::numeric_limits<f32>::quiet_NaN();     //!< float 32 not a number value
constexpr f64 F64NaN = std::numeric_limits<f32>::quiet_NaN();     //!< float 64 not a number value

}

#endif // TYPE_ALIASES_HXX
