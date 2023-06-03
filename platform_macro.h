// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef KVARINT_UTIL_PLATFORM_MACRO_H__
#define KVARINT_UTIL_PLATFORM_MACRO_H__

#ifdef __linux__
#  define KVARINT_ON_LINUX 1
#endif

#if defined(_WIN32)
#  define KVARINT_ON_WIN 1
#endif

#if (defined(__unix__) || defined(__linux__)) && !defined(KVARINT_TEST_THREAD)
#  define KVARINT_ON_UNIX 1
#endif

#define _XKEYCHECK_H
#ifndef _WINSOCKAPI_
// #define _WINSOCKAPI_
#endif

#endif
