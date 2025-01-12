#ifndef WINDOWS_COMPAT_H
#define WINDOWS_COMPAT_H

#include <cstdlib>  // for std::abort
#include <ctime>    // for std::time, std::localtime

// Define WORD if not already defined
#ifndef WORD
typedef unsigned short WORD;
#endif

// Mimic the Windows SYSTEMTIME struct layout
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

// Drop-in replacement for the Windows DebugBreak()
// Here we either use a compiler intrinsic if available, or fallback to abort().
inline void DebugBreak()
{
#if defined(_MSC_VER)
    __debugbreak();      // MSVC
#elif defined(__GNUC__) || defined(__clang__)
    __builtin_trap();    // GCC/Clang
#else
    std::abort();        // Fallback
#endif
}

// Drop-in replacement for the Windows GetLocalTime()
// We fill out an emulated SYSTEMTIME using std::localtime()
inline void GetLocalTime(SYSTEMTIME* lpSystemTime)
{
    if (!lpSystemTime) return;

    std::time_t rawTime;
    std::time(&rawTime);
    std::tm* localTm = std::localtime(&rawTime);

    if (!localTm) {
        // If for some reason localtime failed, zero everything out
        lpSystemTime->wYear         = 0;
        lpSystemTime->wMonth        = 0;
        lpSystemTime->wDayOfWeek    = 0;
        lpSystemTime->wDay          = 0;
        lpSystemTime->wHour         = 0;
        lpSystemTime->wMinute       = 0;
        lpSystemTime->wSecond       = 0;
        lpSystemTime->wMilliseconds = 0;
        return;
    }

    // Windows SYSTEMTIME defines Sunday=0, Monday=1, etc. 
    // This matches tm_wday in <ctime> which is days since Sunday [0..6].
    lpSystemTime->wYear         = static_cast<WORD>(localTm->tm_year + 1900);
    lpSystemTime->wMonth        = static_cast<WORD>(localTm->tm_mon + 1);
    lpSystemTime->wDay          = static_cast<WORD>(localTm->tm_mday);
    lpSystemTime->wDayOfWeek    = static_cast<WORD>(localTm->tm_wday);
    lpSystemTime->wHour         = static_cast<WORD>(localTm->tm_hour);
    lpSystemTime->wMinute       = static_cast<WORD>(localTm->tm_min);
    lpSystemTime->wSecond       = static_cast<WORD>(localTm->tm_sec);
    lpSystemTime->wMilliseconds = 0; // Not provided by standard <ctime>
}

#endif // WINDOWS_COMPAT_H
