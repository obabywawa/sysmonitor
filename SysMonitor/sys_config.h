#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H

#include "json.h"
using namespace Json;
#include <string>
using std::string;

#include <map>
#include <vector>
#include <list>
using namespace std;

//#include "func.h"

#if !defined(HAS_PLATFORM)
# define HAS_PLATFORM
# if defined(_MSC_VER)
#   define PLATFORM_API_WINDOWS         1
#   define PLATFORM_API_POSIX           0
# else  // _MSC_VER
#   error "PLATFORM is unknown"
# endif // !_MSC_VER
#endif // HAS_PLATFORM

// 
#if defined(_MSC_VER)
#define COMPILER_MSVC
#endif // MSC_VER

#ifdef COMPILER_MSVC
#define GG_LONGLONG(x) x##I64
#define GG_ULONGLONG(x) x##UI64
#else
#define GG_LONGLONG(x) x##LL
#define GG_ULONGLONG(x) x##ULL
#endif


typedef char                            int8_t;
typedef short                           int16_t;
typedef int                             int32_t;

typedef unsigned char                   uint8_t;
typedef unsigned short                  uint16_t;
typedef unsigned int                    uint32_t;

#if defined(COMPILER_MSVC)
typedef __int64                         int64_t;
typedef unsigned __int64                uint64_t;
#else
typedef long long                       int64_t;
typedef unsigned long long              uint64_t;
#endif // COMPILER_MSVC

typedef float                           float_t;
typedef double                          double_t;

typedef uint32_t                        bool_t;


const uint8_t  kuint8max = ((uint8_t)0xFF);
const uint16_t kuint16max = ((uint16_t)0xFFFF);
const uint32_t kuint32max = ((uint32_t)0xFFFFFFFF);
const uint64_t kuint64max = ((uint64_t)GG_LONGLONG(0xFFFFFFFFFFFFFFFF));
const  int8_t  kint8min = ((int8_t)(-128)/*0x80*/);
const  int8_t  kint8max = ((int8_t)0x7F);
const  int16_t kint16min = ((int16_t)(-32768)/*0x8000*/);
const  int16_t kint16max = ((int16_t)0x7FFF);
const  int32_t kint32min = ((int32_t)0x80000000);
const  int32_t kint32max = ((int32_t)0x7FFFFFFF);
const  int64_t kint64min = ((int64_t)GG_LONGLONG(0x8000000000000000));
const  int64_t kint64max = ((int64_t)GG_LONGLONG(0x7FFFFFFFFFFFFFFF));


//////////////////////////////////////////////////////////////////////////
//
#ifndef NULL
# ifdef __cplusplus
#   define NULL                         0
# else
#   define NULL                         ((void *)0)
# endif
#endif

#ifndef FALSE
# define FALSE                          0
#endif

#ifndef TRUE
# define TRUE                           1
#endif

#ifndef IN
# define IN
#endif

#ifndef OUT
# define OUT
#endif


//#ifndef __cplusplus
#ifndef max
# define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif // max

#ifndef min
# define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif // min
// #endif

//////////////////////////////////////////////////////////////////////////
// unused
#if !defined(UNUSED_PARAM)
#define UNUSED_PARAM(p)	                ((void)(p))	/* to avoid warnings */
#endif

#if !defined(UNUSED_LOCAL_VARIABLE)
#define UNUSED_LOCAL_VARIABLE(lv)	      UNUSED_PARAM(lv)
#endif

#ifndef TDEL
#define TDEL(a)				if(a!=NULL) { delete a; a=NULL; }
#endif

#ifndef TDELARRAY
#define TDELARRAY(a)		if(a!=NULL) { delete[] a; a=NULL; }
#endif

#ifndef CLOSEHANDLE
#define CLOSEHANDLE(a)		if(a!=NULL) { CloseHandle(a); a=NULL; }
#endif

#endif // SYS_CONFIG_H