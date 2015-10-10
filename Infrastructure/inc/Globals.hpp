//
// Created by Moustapha on 29/09/2015.
//

#ifndef BREAK_0_1_GLOBALS_HPP
#define BREAK_0_1_GLOBALS_HPP

#include <memory>
#include <stdint.h>
#include <map>
#include <vector>

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
