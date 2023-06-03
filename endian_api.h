#ifndef KVARINT_ENDIAN_API_H__
#define KVARINT_ENDIAN_API_H__

#include "platform_macro.h"

#ifdef KVARINT_ON_WIN
#  include "endian_api_win.h"
#elif defined(KVARINT_ON_UNIX)
#  include "endian_api_linux.h"
#endif

#endif
