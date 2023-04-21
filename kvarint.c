// SPDX-LICENSE-IDENTIFIER: MIT
#include "kvarint.h"

#include <assert.h>
#include <stdio.h>

#define DEF_ENCODE_FUNC_(bits_, num_t_, buf_t_)                                \
  void kvarint_encode##bits_(num_t_ num, buf_t_ *buf)                          \
  {                                                                            \
    int idx = 0;                                                               \
    while (num > 127) {                                                        \
      buf->buf[idx++] = 0x80 | ((uint8_t)num & 0x7f);                          \
      num >>= 7;                                                               \
    }                                                                          \
    buf->buf[idx] = num;                                                       \
    buf->len = idx + 1;                                                        \
  }

#define DEF_ENCODE_FUNC2_(bits_)                                               \
  DEF_ENCODE_FUNC_(bits_, uint##bits_##_t, kvarint_buf##bits_##_t)

DEF_ENCODE_FUNC2_(8)
DEF_ENCODE_FUNC2_(16)
DEF_ENCODE_FUNC2_(32)
DEF_ENCODE_FUNC2_(64)

#define DEF_DECODE_FUNC_(bits_)                                                \
  kvarint_errcode_en kvarint_decode##bits_(void const *buf, size_t len,        \
                                           uint##bits_##_t *out)               \
  {                                                                            \
    assert(out);                                                               \
                                                                               \
    int idx = 0;                                                               \
    uint8_t const *buf8 = (uint8_t const *)buf;                                \
    int shift = 0;                                                             \
    *out = 0;                                                                  \
    while (idx < len) {                                                        \
      *out |= ((uint64_t)(buf8[idx] & 0x7f)) << shift;                         \
      shift += 7;                                                              \
      if (!(buf8[idx] & 0x80)) return KVARINT_OK;                              \
      ++idx;                                                                   \
      if (shift > 63) return KVARINT_DECODE_BUF_INVALID;                       \
    }                                                                          \
                                                                               \
    return KVARINT_DECODE_BUF_SHORT;                                           \
  }

DEF_DECODE_FUNC_(64)
DEF_DECODE_FUNC_(32)
DEF_DECODE_FUNC_(16)
DEF_DECODE_FUNC_(8)
