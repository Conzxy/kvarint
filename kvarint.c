// SPDX-LICENSE-IDENTIFIER: MIT
#include "kvarint.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "./endian_api.h"

inline static bool is_machine_litte_endian()
{
  static int n = 1;
  return (*(char *)&n == 1);
}

#define DEF_ENCODE_FUNC_(bits_, num_t_, buf_t_)                                                    \
  void kvarint_encode##bits_(num_t_ num, buf_t_ *buf)                                              \
  {                                                                                                \
    int idx = 0;                                                                                   \
    if (!is_machine_litte_endian()) {                                                              \
      idx = sizeof(buf->buf) - 1;                                                                  \
      while (num > 127) {                                                                          \
        buf->buf[idx--] = 0x80 | ((uint8_t)num & 0x7f);                                            \
        num             >>= 7;                                                                     \
      }                                                                                            \
      buf->buf[idx]                  = 0x80 | num;                                                 \
      buf->buf[sizeof(buf->buf) - 1] &= 0x7f;                                                      \
      buf->len                       = sizeof(buf->buf) - idx;                                     \
      memmove(buf->buf, &buf->buf[idx], buf->len);                                                 \
    } else {                                                                                       \
      while (num > 127) {                                                                          \
        buf->buf[idx++] = 0x80 | ((uint8_t)num & 0x7f);                                            \
        num             >>= 7;                                                                     \
      }                                                                                            \
      buf->buf[idx] = num;                                                                         \
      buf->len      = idx + 1;                                                                     \
    }                                                                                              \
  }

#define DEF_ENCODE_FUNC2_(bits_) DEF_ENCODE_FUNC_(bits_, uint##bits_##_t, kvarint_buf##bits_##_t)

void kvarint_encode8(uint8_t num, kvarint_buf8_t *buf)
{
  buf->buf[0] = num;
  buf->len    = 1;
}

// DEF_ENCODE_FUNC2_(8)
DEF_ENCODE_FUNC2_(16)
DEF_ENCODE_FUNC2_(32)
DEF_ENCODE_FUNC2_(64)

#define DEF_DECODE_FUNC_(bits_)                                                                    \
  kvarint_errcode_en                                                                               \
      kvarint_decode##bits_(void const *buf, size_t len, size_t *out_len, uint##bits_##_t *out)    \
  {                                                                                                \
    assert(out);                                                                                   \
    assert(out_len);                                                                               \
                                                                                                   \
    int            idx   = 0;                                                                      \
    uint8_t const *buf8  = (uint8_t const *)buf;                                                   \
    int            shift = 0;                                                                      \
    *out                 = 0;                                                                      \
    while (idx < len) {                                                                            \
      *out  |= ((uint64_t)(buf8[idx] & 0x7f)) << shift;                                            \
      shift += 7;                                                                                  \
      if (!(buf8[idx] & 0x80)) {                                                                   \
        *out_len = idx + 1;                                                                        \
        if (!is_machine_litte_endian()) {                                                          \
          *out = kvarint_ToNetworkByteOrder64(*out);                                               \
        }                                                                                          \
        return KVARINT_OK;                                                                         \
      }                                                                                            \
      if (shift > 63) return KVARINT_DECODE_BUF_INVALID;                                           \
      ++idx;                                                                                       \
    }                                                                                              \
    return KVARINT_DECODE_BUF_SHORT;                                                               \
  }

kvarint_errcode_en kvarint_decode8(void const *buf, size_t len, size_t *out_len, uint8_t *out)
{
  assert(out);
  assert(out_len);
  if (len < 1) {
    return KVARINT_DECODE_BUF_SHORT;
  }

  *out_len = 1;
  *out     = *(char const *)buf;
  return KVARINT_OK;
}

DEF_DECODE_FUNC_(64)
DEF_DECODE_FUNC_(32)
DEF_DECODE_FUNC_(16)
// DEF_DECODE_FUNC_(8)