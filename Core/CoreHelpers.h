#ifndef _CORE_HELPERS_H_
#define _CORE_HELPERS_H_

// Remove #include <Windows.h>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cassert>

// If you still need Windows COM stuff, you might include <unknwn.h> for IUnknown,
// but if not needed, you can just rely on your own interfaces.

#define ASSERT_ALWAYS(expression) ALWAYS_ASSERT_RAW(expression,__FILE__,__LINE__,__FUNCTION__,#expression)

void ALWAYS_ASSERT_RAW(bool cond, const char fileName[], const int lineNum, const char funcName[], const char expression[]);

// Safe release macro for COM objects or any object with a Release() method
#define MY_SAFE_RELEASE(p) { if (p!=NULL) { p->Release(); p = NULL; } }

// Restrict keywords below might be for C++ AMP or some specific compiler extension;
// keep them if you need them, or remove them otherwise.
inline float SafeInv(float x)
{
    return x == 0 ? 0.0f : 1.0f/x;
}

inline float FastSqr(float x)
{
    return x*x;
}

inline float MaxFloat(float x, float y)
{
    return x > y ? x : y;
}

inline float MinFloat(float x, float y)
{
    return x < y ? x : y;
}

inline int MaxInt(int x, int y)
{
    return x > y ? x : y;
}

inline int MinInt(int x, int y)
{
    return x < y ? x : y;
}

inline float RandNorm()
{
    return static_cast<float>(rand() % 10001) / 10000.0f;
}

inline float Abs(float f)
{
    return f >= 0.0f ? f : -f;
}

inline float Saturate(float x)
{
    return MaxFloat(0.0f, MinFloat(1.0f, x));
}

inline float Saturate255(float x)
{
    return MaxFloat(0.0f, MinFloat(255.0f, x));
}

inline unsigned char ColorToChar(float x)
{
    return static_cast<unsigned char>(Saturate255(x * 256.0f));
}

template <class A>
inline void Swap(A & lhs, A & rhs)
{
    A temp = lhs;
    lhs = rhs;
    rhs = temp;
}

inline std::string FourDigitString(int i)
{
    char camName[2048];
    std::sprintf(camName, "%04d", i);
    return std::string(camName);
}

// Replaced QueryPerformance* calls with <chrono>
inline unsigned long long GetQualityTimeMicroSec()
{
    using namespace std::chrono;
    // Return the current time since epoch in microseconds:
    auto now = high_resolution_clock::now();
    return duration_cast<microseconds>(now.time_since_epoch()).count();
}

template <class A, class B>
inline A AlignSize(A x, B size)
{
    return (((x + size) - 1) / size) * size;
}

inline unsigned long long AlignSize64(unsigned long long x, unsigned long long size)
{
    return (((x + size) - 1) / size) * size;
}

inline unsigned int AlignSize32(unsigned int x, unsigned int size)
{
    return (((x + size) - 1) / size) * size;
}

// These two functions must be defined somewhere else or in this header:
std::string GetNextLineFromFile(FILE * fin, bool & isEof);
std::string LocalTimeAsString();

#endif // _CORE_HELPERS_H_
