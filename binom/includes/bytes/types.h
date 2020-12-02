#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

typedef uint8_t ui8;
typedef int8_t i8;
typedef uint16_t ui16;
typedef int16_t i16;
typedef uint32_t ui32;
typedef int32_t i32;
typedef uint64_t ui64;
typedef int64_t i64;

typedef float f4;
typedef double f8;
typedef long double llf_t;

typedef int i_t;
typedef unsigned int ui_t;
typedef short s_t;
typedef unsigned short us_t;
typedef long l_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;

constexpr ui8 operator "" _ui8 (ull_t val) {return val;}
constexpr i8 operator "" _i8 (ull_t val) {return val;}
constexpr ui16 operator "" _ui16 (ull_t val) {return val;}
constexpr i16 operator "" _i16 (ull_t val) {return val;}
constexpr ui32 operator "" _ui32 (ull_t val) {return val;}
constexpr i32 operator "" _i32 (ull_t val) {return val;}
constexpr ui64 operator "" _ui64 (ull_t val) {return val;}
constexpr i64 operator "" _i64 (ull_t val) {return val;}

constexpr f4 operator "" _f4 (llf_t val) {return val;}
constexpr f8 operator "" _f8 (llf_t val) {return val;}
constexpr llf_t operator ""_llf (llf_t val) {return val;}

constexpr i_t operator "" _i (ull_t val) {return val;}
constexpr ui_t operator "" _ui (ull_t val) {return val;}
constexpr s_t operator "" _s (ull_t val) {return val;}
constexpr us_t operator "" _us (ull_t val) {return val;}
constexpr l_t operator "" _l (ull_t val) {return val;}
constexpr ul_t operator "" _ul (ull_t val) {return val;}
constexpr ll_t operator "" _ll (ull_t val) {return val;}
constexpr ull_t operator "" _ull (ull_t val) {return val;}

#endif // TYPES_H
