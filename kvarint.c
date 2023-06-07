// SPDX-LICENSE-IDENTIFIER: MIT
#include "kvarint.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "./endian_api.h"

/*
 * You should know before read following:
 * BE = Big endian
 * LE = little endian
 *
 * To handle the encode and decode in BE environment, follow a common byte order
 * in buffer. To most machine is LE, I determine the common byte order be LE. So
 * encode in BE env must convert it to LE, and decode in BE env must convert it
 * to BE.
 *
 * Solution:
 * To former, put the highest byte(ie. lowest byte in LE) to buffer in order.
 * To latter, convert the buffer to a integer then convert it to BE.
 */
inline static bool is_machine_litte_endian()
{
  static int n = 1;
  return (*(char *)&n == 1);
}

#define DEF_ENCODE_FUNC_(index_, bits_, num_t_, buf_t_)                        \
  void kvarint_encode##bits_(num_t_ num, buf_t_ *buf)                          \
  {                                                                            \
    int idx = 0;                                                               \
    if (is_machine_litte_endian()) {                                           \
      while (num > 127) {                                                      \
        buf->buf[idx++] = 0x80 | ((uint8_t)num & 0x7f);                        \
        num >>= 7;                                                             \
      }                                                                        \
    } else {                                                                   \
      while (num > 127) {                                                      \
        /* Read the highest byte in BE env is similar erad the lowest byte in  \
         * LE env.                                                             \
         *                                                                     \
         * The shift operation in BE env is similar LE env, also multiple 2 or \
         * divide by 2. You may think the shift operation in BE is not         \
         * intuitive, but you can think it compute then put the value to       \
         * memory in BE layout(CPU compute put the register and memory in      \
         * fact). \ref https://stackoverflow.com/a/7184905 */                  \
        buf->buf[idx++] = 0x80 | (((uint8_t *)&num)[index_] & 0x7f);           \
        num >>= 7;                                                             \
      }                                                                        \
    }                                                                          \
    buf->buf[idx] = num;                                                       \
    buf->len = idx + 1;                                                        \
  }

#define DEF_ENCODE_FUNC2_(index_, bits_)                                       \
  DEF_ENCODE_FUNC_(index_, bits_, uint##bits_##_t, kvarint_buf##bits_##_t)

void kvarint_encode8(uint8_t num, kvarint_buf8_t *buf)
{
  buf->buf[0] = num;
  buf->len = 1;
}

// DEF_ENCODE_FUNC2_(8)
DEF_ENCODE_FUNC2_(1, 16)
DEF_ENCODE_FUNC2_(3, 32)
DEF_ENCODE_FUNC2_(7, 64)

#define DEF_DECODE_FUNC_(bits_)                                                \
  kvarint_errcode_en kvarint_decode##bits_(                                    \
      void const *buf, size_t len, size_t *out_len, uint##bits_##_t *out)      \
  {                                                                            \
    assert(out);                                                               \
    assert(out_len);                                                           \
                                                                               \
    int idx = 0;                                                               \
    uint8_t const *buf8 = (uint8_t const *)buf;                                \
    int shift = 0;                                                             \
    *out = 0;                                                                  \
    while (idx < len) {                                                        \
      *out |= ((uint64_t)(buf8[idx] & 0x7f)) << shift;                         \
      shift += 7;                                                              \
      if (!(buf8[idx] & 0x80)) {                                               \
        *out_len = idx + 1;                                                    \
        if (!is_machine_litte_endian()) {                                      \
          *out = kvarint_ToNetworkByteOrder64(*out);                           \
        }                                                                      \
        return KVARINT_OK;                                                     \
      }                                                                        \
      if (shift > 63) return KVARINT_DECODE_BUF_INVALID;                       \
      ++idx;                                                                   \
    }                                                                          \
    return KVARINT_DECODE_BUF_SHORT;                                           \
  }

kvarint_errcode_en kvarint_decode8(void const *buf, size_t len, size_t *out_len,
                                   uint8_t *out)
{
  assert(out);
  assert(out_len);
  if (len < 1) {
    return KVARINT_DECODE_BUF_SHORT;
  }

  *out_len = 1;
  *out = *(char const *)buf;
  return KVARINT_OK;
}

DEF_DECODE_FUNC_(64)
DEF_DECODE_FUNC_(32)
DEF_DECODE_FUNC_(16)
// DEF_DECODE_FUNC_(8)
