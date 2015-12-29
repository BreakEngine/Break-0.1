//
// Created by Moustapha on 29/09/2015.
//

#ifndef BREAK_0_1_GLOBALS_HPP
#define BREAK_0_1_GLOBALS_HPP

#include <memory>
#include <stdint.h>
#include <map>
#include <vector>
#include <algorithm>

#if defined(WIN32)||defined(_WIN32)||defined(__WIN32__)||defined(_WIN64)||defined(WIN64)||defined(__MINGW32__)||defined(__MINGW64__)
    #define OS_WINDOWS
#elif defined(__linux__) || defined(__unix__)
    #define OS_LINUX
#endif

#if defined(_MSC_VER)
    #define MSVC
#endif

#ifdef OS_WINDOWS
#ifdef COMPILE_DLL
    #define BREAK_API __declspec(dllexport)
#else
	#define BREAK_API __declspec(dllimport)
#endif
#endif

#ifdef OS_LINUX
    #define BREAK_API
#endif

#define __bswap_constant_16(x) \
     ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))

#if defined __GNUC__ && __GNUC__ >= 2
# define __bswap_16(x) \
     (__extension__							      \
      ({ register unsigned short int __v, __x = (x);			      \
	 if (__builtin_constant_p (__x))				      \
	   __v = __bswap_constant_16 (__x);				      \
	 else								      \
	   __asm__ ("rorw $8, %w0"					      \
		    : "=r" (__v)					      \
		    : "0" (__x)						      \
		    : "cc");						      \
	 __v; }))
#else
/* This is better than nothing.  */
# define __bswap_16(x) \
     (__extension__							      \
      ({ register unsigned short int __x = (x); __bswap_constant_16 (__x); }))
#endif

//ByteSwap.h
/* Swap bytes in 32 bit value.  */
#define __bswap_constant_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) |		      \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#if defined __GNUC__ && __GNUC__ >= 2
# if __WORDSIZE == 64 || (defined __i486__ || defined __pentium__	      \
			  || defined __pentiumpro__ || defined __pentium4__   \
			  || defined __k8__ || defined __athlon__	      \
			  || defined __k6__)
/* To swap the bytes in a word the i486 processors and up provide the
   `bswap' opcode.  On i386 we have to use three instructions.  */
#  define __bswap_32(x) \
     (__extension__							      \
      ({ register unsigned int __v, __x = (x);				      \
	 if (__builtin_constant_p (__x))				      \
	   __v = __bswap_constant_32 (__x);				      \
	 else								      \
	   __asm__ ("bswap %0" : "=r" (__v) : "0" (__x));		      \
	 __v; }))
# else
#  define __bswap_32(x)							      \
     (__extension__							      \
      ({ register unsigned int __v, __x = (x);				      \
	 if (__builtin_constant_p (__x))				      \
	   __v = __bswap_constant_32 (__x);				      \
	 else								      \
	   __asm__ ("rorw $8, %w0;"					      \
		    "rorl $16, %0;"					      \
		    "rorw $8, %w0"					      \
		    : "=r" (__v)					      \
		    : "0" (__x)						      \
		    : "cc");						      \
	 __v; }))
# endif
#else
# define __bswap_32(x) \
     (__extension__							      \
      ({ register unsigned int __x = (x); __bswap_constant_32 (__x); }))
#endif

#define PI 3.141592741f

namespace Break{

    typedef unsigned char byte;

    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float real32;
    typedef double real64;

    typedef u32 type_id;
    typedef u64 hash_id;


    #define GENERATE_TYPE_ID 0xFFFFFFFF - __COUNTER__

    class Object;

    #define RTTI(CLASS) static const type_id Type = GENERATE_TYPE_ID

    #define KILOBYTE(VAL) VAL*1024LL
    #define MEGABYTE(VAL) KILOBYTE(VAL)*1024LL
    #define GIGABYTE(VAL) MEGABYTE(VAL)*1024LL

    #define STATIC_OPTIMAL_SIZE MEGABYTE(8)
    #define DYNAMIC_OPTIMAL_SIZE MEGABYTE(40)


}

#endif //BREAK_0_1_GLOBALS_HPP
