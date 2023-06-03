// SPDX-LICENSE-IDENTIFIER: MIT
#ifndef KVARINT_WIN_CORE_UTIL_ENDIAN_API_H_
#define KVARINT_WIN_CORE_UTIL_ENDIAN_API_H_

#include <winsock2.h>
#include <stdint.h>
#include "macro.h"

// network byte order conversion function

/**
 * To network byte order
 */
inline uint8_t  kvarint_ToNetworkByteOrder8(uint8_t host8) { return host8; }
inline uint16_t kvarint_ToNetworkByteOrder16(uint16_t host16) { return htons(host16); }
inline uint32_t kvarint_ToNetworkByteOrder32(uint32_t host32) { return htonl(host32); }
inline uint64_t kvarint_ToNetworkByteOrder64(uint64_t host64) { return htonll(host64); }

/**
 * To host byte order
 */
inline uint8_t  kvarint_ToHostByteOrder8(uint8_t net8) { return net8; }
inline uint16_t kvarint_ToHostByteOrder16(uint16_t net16) { return ntohs(net16); }
inline uint32_t kvarint_ToHostByteOrder32(uint32_t net32) { return ntohl(net32); }
inline uint64_t kvarint_ToHostByteOrder64(uint64_t net64) { return ntohll(net64); }

#endif
