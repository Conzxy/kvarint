# kvarint

## Introduction
**varint** 是Google提出为了减少序列化数据payload的大小而创造的一个压缩算法，具体来说是用可变字节表示一个整型。  
比如序列化数据有一个size header，是32位数，但是对于该payload是123，那么可以用1个字节表示而不是4个字节。

算法原理大致来说，就是一个字节的 **最高位(Most significant bit)** 作为 **指示位(indicator bit)** ，仅 **低7位** 表示数据：
* 1：表示这个字节是数据的一部分但不是最后一个，应该继续读取以获取完整的数据
* 0：表示这个字节是数据的最后一个包含数据的字节，终止读取

该算法在Google的多个项目中均有出现，比如 `grpc`, `leveldb`, `protobuf`, etc.

> 由于只有一个源文件，很容易引入项目中，故不提供 `CMake` 等编译脚本

当然，其实对于浮点数也是可以用的，毕竟都是bits罢了。

### Byte order
该库已经自动处理好了字节序的转换，因此用户并不需要关心字节序的转化。  
如果你硬是塞了转化好字节序的内容，可能会得到不符合预期的结果，该库并不负责。

## API
均提供64/32/16/8 bits版本的API。

### Encode
```c
void kvarint_encode64(uint64_t num, kvarint_buf64_t *buf);
void kvarint_encode32(uint32_t num, kvarint_buf32_t *buf);
void kvarint_encode16(uint16_t num, kvarint_buf16_t *buf);
void kvarint_encode8(uint8_t num, kvarint_buf8_t *buf);

void kvarint_encode64s(int64_t num, kvarint_buf64_t *buf);
void kvarint_encode32s(int32_t num, kvarint_buf32_t *buf);
void kvarint_encode16s(int16_t num, kvarint_buf16_t *buf);
void kvarint_encode8s(int8_t num, kvarint_buf8_t *buf);
```
由于用户不需要知晓varint的具体编码原理，当然也不需要知晓多大的 **缓冲(buffer)** 才合适，这个有一个库定义的缓冲结构体负责：
```c
#define KVARINT_DEF_BUF_STRUCT_(bits_, max_bytes_)                             \
  typedef struct {                                                             \
    char buf[max_bytes_];                                                      \
    /* The written size */                                                     \
    size_t len;                                                                \
  } kvarint_buf##bits_##_t

// 具体类型
kvarint_buf64_t
kvarint_buf32_t
kvarint_buf16_t
kvarint_buf8_t
```
编码完了之后，访问其成员即可获得存储数据的缓冲和编码的大小。

### Decode
```c
kvarint_errcode_en kvarint_decode64(void const *buf, size_t buf_size, size_t *out_len, uint64_t *out);
kvarint_errcode_en kvarint_decode32(void const *buf, size_t buf_size, size_t *out_len, uint32_t *out);
kvarint_errcode_en kvarint_decode16(void const *buf, size_t buf_size, size_t *out_len, uint16_t *out);
kvarint_errcode_en kvarint_decode8 (void const *buf, size_t buf_size, size_t *out_len, uint8_t *out);

kvarint_errcode_en kvarint_decode64s(void const *buf, size_t buf_size, size_t *out_len, int64_t *out);
kvarint_errcode_en kvarint_decode32s(void const *buf, size_t buf_size, size_t *out_len, int32_t *out);
kvarint_errcode_en kvarint_decode16s(void const *buf, size_t buf_size, size_t *out_len, int16_t *out);
kvarint_errcode_en kvarint_decode8s (void const *buf, size_t buf_size, size_t *out_len, int8_t *out);
```
需要用户提供缓冲的大小以便检测错误。  
错误码有以下几种：
```c
typedef enum {
  KVARINT_OK = 0,
  KVARINT_DECODE_BUF_SHORT, // The buffer don't contains entire varint data, can't be parsed
  KVARINT_DECODE_BUF_INVALID, // The buffer can't be parsed to varint, over the max size
} kvarint_errcode_en;
```

## Test
### Build
由于图省事，我是用 `gtest` 写的单元测试，所以用的也是C++。
```
$ g++ -g -o test test.cc kvarint.c -lgtest -lgtest_main
```

大端序的测试可以通过 `qemu` 或 `docker`[1] 进行虚拟环境的模拟：
```shell
$ docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
$ docker run --rm -it s390x/ubuntu bash
```

## Reference
[1] https://stackoverflow.com/a/3339484
