// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef _KVARINT_H__
#define _KVARINT_H__

#include <stdint.h>
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
    return kvarint_decode##bits_(buf, buf_size, (uint##bits_##_t *)out);       \
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
