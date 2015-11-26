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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <glm\glm.hpp>
#include <glm\gtx\constants.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>

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

namespace Break
{

	typedef unsigned char byte;

	typedef signed char s8;
	typedef signed short s16;
	typedef signed int s32;
	typedef int64_t s64;

	typedef  unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;
	typedef uint64_t u64;

	//typedef int8_t s8;
	//typedef int16_t s16;
	//typedef int32_t s32;
	//typedef int64_t s64;

	//typedef uint8_t u8;
	//typedef uint16_t u16;
	//typedef uint32_t u32;
	//typedef uint64_t u64;

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


	//physics Globals

#define NOT_USED(x) ((void)(x))

	// Collision

	/// The maximum number of contact points between two convex shapes. Do
	/// not change this value.
#define maxManifoldPoints	2

	/// The maximum number of vertices on a convex polygon. You cannot increase
	/// this too much because b2BlockAllocator has a maximum object size.
#define maxPolygonVertices	8

	/// This is used to fatten AABBs in the dynamic tree. This allows proxies
	/// to move by a small amount without triggering a tree adjustment.
	/// This is in meters.
#define aabbExtension		0.1f

	/// This is used to fatten AABBs in the dynamic tree. This is used to predict
	/// the future position based on the current displacement.
	/// This is a dimensionless multiplier.
#define aabbMultiplier		2.0f

	/// A small length used as a collision and constraint tolerance. Usually it is
	/// chosen to be numerically significant, but visually insignificant.
#define linearSlop	0.005f

	/// A small angle used as a collision and constraint tolerance. Usually it is
	/// chosen to be numerically significant, but visually insignificant.
#define angularSlop			(2.0f / 180.0f * glm::pi<double>())

	/// The radius of the polygon/edge shape skin. This should not be modified. Making
	/// this smaller means polygons will have an insufficient buffer for continuous collision.
	/// Making it larger may create artifacts for vertex collision.
#define polygonRadius		(2.0f * linearSlop)

	/// Maximum number of sub-steps per contact in continuous physics simulation.
#define maxSubSteps			8


	// Dynamics

	/// Maximum number of contacts to be handled to solve a TOI impact.
#define maxTOIContacts		32

	/// A velocity threshold for elastic collisions. Any collision with a relative linear
	/// velocity below this threshold will be treated as inelastic.
#define velocityThreshold		1.0f

	/// The maximum linear position correction used when solving constraints. This helps to
	/// prevent overshoot.
#define maxLinearCorrection		0.2f

	/// The maximum angular position correction used when solving constraints. This helps to
	/// prevent overshoot.
#define maxAngularCorrection		(8.0f / 180.0f * glm::pi<double>())

	/// The maximum linear velocity of a body. This limit is very large and is used
	/// to prevent numerical problems. You shouldn't need to adjust this.
#define maxTranslation			2.0f
#define maxTranslationSquared	(maxTranslation * maxTranslation)

	/// The maximum angular velocity of a body. This limit is very large and is used
	/// to prevent numerical problems. You shouldn't need to adjust this.
#define maxRotation				(0.5f * glm::pi<double>())
#define maxRotationSquared		(maxRotation * maxRotation)

	/// This scale factor controls how fast overlap is resolved. Ideally this would be 1 so
	/// that overlap is removed in one time step. However using values close to 1 often lead
	/// to overshoot.
#define baumgarte				0.2f
#define toiBaugarte				0.75f


	// Sleep

	/// The time that a body must be still before it will go to sleep.
#define timeToSleep		0.5f

	/// A body cannot sleep if its linear velocity is above this tolerance.
#define linearSleepTolerance	0.01f

	/// A body cannot sleep if its angular velocity is above this tolerance.
#define angularSleepTolerance	(2.0f / 180.0f * glm::pi<double>())

}

#endif //BREAK_0_1_GLOBALS_HPP
