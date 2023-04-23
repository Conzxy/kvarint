#ifndef _KVARINT_MACRO_H__
#define _KVARINT_MACRO_H__

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE __inline__ __attribute__((always_inline))
#else
#define INLINE __inline__
#endif

#elif (defined(_MSC_VER) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif //! INLINE

#endif //! _KVARINT_MACRO_H__
