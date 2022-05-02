#pragma once

#if defined(_WIN64) || defined(_WIN32)
#define WINDOWS

#ifdef _WIN64
// define something for Windows (64-bit only)
#else
// define something for Windows (32-bit only)
#endif

#elif __APPLE__
#include "TargetConditionals.h"
#define APPLE

#if TARGET_OS_IPHONE && TARGET_OS_SIMULATOR
// define something for simulator
// (although, checking for TARGET_OS_IPHONE should not be required).
#elif TARGET_OS_IPHONE && TARGET_OS_MACCATALYST
// define something for Mac's Catalyst
#elif TARGET_OS_IPHONE
// define something for iphone
#else
#define TARGET_OS_OSX 1
// define something for OSX
#endif

#elif defined(__linux) || defined(__unix) || defined(__posix)
#define LINUX

#ifdef __linux
// linux
#elif __unix // all unices not caught above
// Unix
#elif __posix
// POSIX
#endif

#endif