// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _KVARINT_H__
#define _KVARINT_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "macro.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  KVARINT_OK = 0,
  KVARINT_DECODE_BUF_SHORT,
  KVARINT_DECODE_BUF_INVALID,
} kvarint_errcode_en;

#define KVARINT_DECL_DECODE_FUNC_(bits_)                                       \
  kvarint_errcode_en kvarint_decode##bits_(void const *buf, size_t buf_size,   \
                                           uint##bits_##_t *out)

KVARINT_DECL_DECODE_FUNC_(64);
KVARINT_DECL_DECODE_FUNC_(32);
KVARINT_DECL_DECODE_FUNC_(16);
KVARINT_DECL_DECODE_FUNC_(8);

#define KVARINT_DECL_DECODE_FUNC_SIGNED_(bits_)                                \
  INLINE kvarint_errcode_en kvarint_decode##bits_##s(                          \
      void const *buf, size_t buf_size, int##bits_##_t *out)                   \
  {                                                                            \
    /* Explain this to uint_*_t * to avoid right shift be a arithemetic shift  \
     * that leave the most significant bit(MSB) no change.                     \
     *                                                                         \
     * since encode use left shift, it is a logic shift that fill the MSB with \
     * 0, we must be also ensure the right shift operation be a logic shift.   \
     */                                                                        \
    auto p = (uint##bits_##_t *)out;                                           \
    const auto ret = kvarint_decode##bits_(buf, buf_size, p);                  \
                                                                               \
    /* *p = (p is odd) ? (*p+1) / 2 : p / 2; */                                \
    *p = (*p & 1) ? ~((*p - 1) >> 1) : (*p >> 1);                              \
    return ret;                                                                \
  }

KVARINT_DECL_DECODE_FUNC_SIGNED_(64)
KVARINT_DECL_DECODE_FUNC_SIGNED_(32)
KVARINT_DECL_DECODE_FUNC_SIGNED_(16)
KVARINT_DECL_DECODE_FUNC_SIGNED_(8)

#define KVARINT_DEF_BUF_STRUCT_(bits_, max_bytes_)                             \
  typedef struct {                                                             \
    char buf[max_bytes_];                                                      \
    size_t len;                                                                \
  } kvarint_buf##bits_##_t

KVARINT_DEF_BUF_STRUCT_(64, 10);
KVARINT_DEF_BUF_STRUCT_(32, 5);
KVARINT_DEF_BUF_STRUCT_(16, 3);
KVARINT_DEF_BUF_STRUCT_(8, 2);

#define KVARINT_DECL_ENCODE_FUNC_(bits_)                                       \
  void kvarint_encode##bits_(uint##bits_##_t num, kvarint_buf##bits_##_t *buf)

KVARINT_DECL_ENCODE_FUNC_(64);
KVARINT_DECL_ENCODE_FUNC_(32);
KVARINT_DECL_ENCODE_FUNC_(16);
KVARINT_DECL_ENCODE_FUNC_(8);

#define KVARINT_DECL_ENCODE_FUNC_SIGNED_(bits_)                                \
  INLINE void kvarint_encode##bits_##s(int##bits_##_t num,                     \
                                       kvarint_buf##bits_##_t *buf)            \
  {                                                                            \
    /* Since -1 is (~0), ie. the maximum number of uint_*_t,                   \
     * it will be encoded to 10 bytes of uint64_t.                             \
     *                                                                         \
     * However, user want the -1 also be encoded to 1 bytes like 1.            \
     * For other negative whose absolute value is small, also be encoded to    \
     * same bytes with their absolute.                                         \
     *                                                                         \
     * Algorithm:                                                              \
     *  Distribute the negative and positive to the odd and even number        \
     *  in the uint_*_t range.                                                 \
     *                                                                         \
     *  In this way, -1 can be encoded to 1 bytes since it is mapped to 1.     \
     *                                                                         \
     * \see https://github.com/dominictarr/signed-varint                       \
     */                                                                        \
                                                                               \
    /* num = (num < 0) ? (num * -2 - 1) : (num * 2) */                         \
    num = (num & ((uint##bits_##_t)(1) << ((bits_)-1))) ? (((~num) << 1) + 1)  \
                                                        : (num << 1);          \
    kvarint_encode##bits_((uint##bits_##_t)num, buf);                          \
  }

KVARINT_DECL_ENCODE_FUNC_SIGNED_(64)
KVARINT_DECL_ENCODE_FUNC_SIGNED_(32)
KVARINT_DECL_ENCODE_FUNC_SIGNED_(16)
KVARINT_DECL_ENCODE_FUNC_SIGNED_(8)

#ifdef __cplusplus
} //! extern "C"
#endif

#endif // !_KVARINT_H__
