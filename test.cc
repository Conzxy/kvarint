// SPDX-LICENSE-IDENTIFIER: MIT
#include "kvarint.h"

#include <gtest/gtest.h>
#include <random>
#include <cstdio>

#include "endian_api.h"

extern void               kvarint_encode64be(uint64_t num, kvarint_buf64_t *buf);
extern kvarint_errcode_en kvarint_decode64be(
    void const *buf,
    size_t      len,
    size_t     *out_len,
    uint64_t   *out
);

void print_bits(uint8_t n)
{
  printf("WARN: low -> high");
  for (size_t i = 0; i < 8; ++i) {
    printf("%d ", ((n & (1 << (sizeof(n) * 8 - 1 - i))) > 0 ? 1 : 0));
  }
  printf("\n");
}

void print_buf(void const *raw_buf, size_t n)
{
  char const *buf = (char const *)raw_buf;
  printf("0x");
  for (size_t i = 0; i < n; ++i) {
    printf("%x ", (int)buf[i]);
  }
  printf("\n");
}

#define DEF_TEST_FUNC1(bytes_)                                                                     \
  kvarint_buf64_t buf64;                                                                           \
  uint64_t        in = 1;                                                                          \
                                                                                                   \
  in <<= (7 * ((bytes_)-1));                                                                       \
  in += 123;                                                                                       \
  printf("val = %llu\n", (unsigned long long)in);                                                  \
  printf("Raw layout: ");                                                                          \
  print_buf(&in, sizeof(in));                                                                      \
  kvarint_encode64(in, &buf64);                                                                    \
  ASSERT_EQ(buf64.len, (bytes_)) << "Failed to encode";                                            \
  printf("Encode layout: ");                                                                       \
  print_buf(buf64.buf, buf64.len);                                                                 \
  uint64_t out64;                                                                                  \
  size_t   len = 0;                                                                                \
  ASSERT_EQ(KVARINT_OK, kvarint_decode64(buf64.buf, buf64.len, &len, &out64))                      \
      << "Failed to decode";                                                                       \
  ASSERT_EQ(in, out64);                                                                            \
  fflush(stdout);

TEST(kvarint, encode_and_decode1)
{
  size_t          len = 0;
  kvarint_buf64_t buf32;
  kvarint_encode64(123, &buf32);
  ASSERT_EQ(buf32.len, 1);
  ASSERT_EQ(buf32.buf[0], 123);
  uint64_t out32;
  kvarint_decode64(buf32.buf, buf32.len, &len, &out32);

  ASSERT_EQ(out32, 123);
}

TEST(kvarint, encode_and_decode2) { DEF_TEST_FUNC1(2); }
TEST(kvarint, encode_and_decode3) { DEF_TEST_FUNC1(3); }
TEST(kvarint, encode_and_decode4) { DEF_TEST_FUNC1(4); }
TEST(kvarint, encode_and_decode5) { DEF_TEST_FUNC1(5); }
TEST(kvarint, encode_and_decode6) { DEF_TEST_FUNC1(6); }
TEST(kvarint, encode_and_decode7) { DEF_TEST_FUNC1(7); }
TEST(kvarint, encode_and_decode8) { DEF_TEST_FUNC1(8); }
TEST(kvarint, encode_and_decode9) { DEF_TEST_FUNC1(9); }
TEST(kvarint, encode_and_decode10) { DEF_TEST_FUNC1(10); }

TEST(kvarint, encode_and_decode_large_data)
{
  kvarint_buf64_t                         buf;
  uint64_t                                out;
  uint64_t                                N = 1000000;
  std::uniform_int_distribution<uint64_t> uid(0, (uint64_t)-1);
  std::default_random_engine              dre(time(NULL));
  size_t                                  len = 0;
  for (uint64_t i = 0; i < N; ++i) {
    auto r = uid(dre);
    kvarint_encode64(r, &buf);
    kvarint_decode64(buf.buf, buf.len, &len, &out);
    ASSERT_EQ(r, out);
  }
}

TEST(kvarint, encode_and_decode_large_data_sign)
{
  kvarint_buf64_t                        buf;
  int64_t                                out;
  int64_t                                N = 1000000;
  std::uniform_int_distribution<int64_t> uid(-INT64_MAX, INT64_MAX);
  std::default_random_engine             dre(time(NULL));
  size_t                                 len;
  for (int64_t i = 0; i < N; ++i) {
    auto r = uid(dre);
    kvarint_encode64s(r, &buf);
    kvarint_decode64s(buf.buf, buf.len, &len, &out);
    ASSERT_EQ(r, out) << "i = " << i;
  }
}

TEST(kvarint, signed_example)
{
  kvarint_buf64_t buf;
  int64_t         out;
  size_t          len;
  kvarint_encode64s(1, &buf);
  kvarint_decode64s(buf.buf, buf.len, &len, &out);

  ASSERT_EQ(1, out) << out;
}

void kvarint_encode64be(uint64_t num, kvarint_buf64_t *buf)
{
  // int idx = sizeof(buf->buf) - 1;
  int idx = 0;
  printf("Encode num = %lu\n", num);
  printf("Encode num(le) = %lu\n", kvarint_ToHostByteOrder64(num));

  while (kvarint_ToHostByteOrder64(num) > 127) {
    buf->buf[idx++] = uint8_t(0x80) | ((uint8_t)(num >> 56) & uint8_t(0x7f));
    printf("Encode buf[%d] = 0x%x\n", idx - 1, (uint8_t)buf->buf[idx - 1]);
    printf(
        "Correct Encode = 0x%x\n",
        uint8_t(0x80) | ((uint8_t)(kvarint_ToHostByteOrder64(num)) & uint8_t(0x7f))
    );
    num = kvarint_ToNetworkByteOrder64(kvarint_ToHostByteOrder64(num) >> 7);
    printf("num = %lu\n", num);
    printf("num(le) = %lu\n", kvarint_ToHostByteOrder64(num));
  }
  buf->buf[idx] = uint8_t(num >> 56);
  printf("last encode = 0x%x\n", buf->buf[idx]);
  printf("Correct last encode(le) = %lu\n", kvarint_ToHostByteOrder64(num));
  /* buf->buf[sizeof(buf->buf) - 1] &= uint8_t(0x7f); */
  /* memmove(buf->buf, &buf->buf[idx], buf->len); */
  /* buf->len                       = sizeof(buf->buf) - idx; */
  buf->len = idx + 1;
}

kvarint_errcode_en kvarint_decode64be(void const *buf, size_t len, size_t *out_len, uint64_t *out)
{
  assert(out);
  assert(out_len);

  int            idx   = 0;
  uint8_t const *buf8  = (uint8_t const *)buf;
  int            shift = 0;
  *out                 = 0;
  while (idx < len) {
    *out  |= ((uint64_t)(buf8[idx] & uint8_t(0x7f))) << shift;
    shift += 7;
    if (!(buf8[idx] & uint8_t(0x80))) {
      *out_len = idx + 1;
      *out     = kvarint_ToNetworkByteOrder64(*out);
      return KVARINT_OK;
    }
    if (shift > 63) return KVARINT_DECODE_BUF_INVALID;
    ++idx;
  }
  return KVARINT_DECODE_BUF_SHORT;
}