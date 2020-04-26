#pragma once
#include "fea_utils/bitmask.hpp"

namespace fea {
#if defined(NDEBUG)
#define FEA_RELEASE_BUILD 1
inline constexpr bool debug_build = false;
#else
#define FEA_DEBUG_BUILD 1
inline constexpr bool debug_build = true;
#endif

enum class platform_t : unsigned {
	aix,
	bsd,
	hpux,
	ios,
	linuxx,
	macos,
	solaris,
	windows,
	count,
};

enum class platform_group_t : unsigned {
	posix = 0b0001,
	unixx = 0b0010,
	count = 0b0000,
};
FEA_ENABLE_BITMASK_OPERATORS(platform_group_t);

//#define FEA_AIX 0
//#define FEA_BSD 0
//#define FEA_HPUX 0
//#define FEA_LINUX 0
//#define FEA_IOS 0
//#define FEA_MACOS 0
//#define FEA_SOLARIS 0
//#define FEA_WINDOWS 0
//
//#define FEA_POSIX 0
//#define FEA_UNIX 0

#if defined(_AIX)
#undef FEA_AIX
#define FEA_AIX 1
inline constexpr platform_t platform = platform_t::aix;

#elif defined(__unix__)
}
#include <sys/param.h>
namespace fea {
#if defined(BSD)
#undef FEA_BSD
#define FEA_BSD 1
inline constexpr platform_t platform = platform_t::bsd;
#endif

#elif defined(__hpux)
#undef FEA_HPUX
#define FEA_HPUX 1
inline constexpr platform_t platform = platform_t::hpux;

#elif defined(__linux__)
#undef FEA_LINUX
#define FEA_LINUX 1
inline constexpr platform_t platform = platform_t::linuxx;

#elif defined(__APPLE__) && defined(__MACH__)
}
#include <TargetConditionals.h>
namespace fea {
#if TARGET_OS_IPHONE == 1
#undef FEA_IOS
#define FEA_IOS 1
inline constexpr platform_t platform = platform_t::ios;
#elif TARGET_OS_MAC == 1
#undef FEA_MACOS
#define FEA_MACOS 1
inline constexpr platform_t platform = platform_t::macos;
#endif

#elif defined(__sun) && defined(__SVR4)
#undef FEA_SOLARIS
#define FEA_SOLARIS 1
inline constexpr platform_t platform = platform_t::solaris;

#elif defined(_WIN32)
#undef FEA_WINDOWS
#define FEA_WINDOWS 1
inline constexpr platform_t platform = platform_t::windows;

#else
inline constexpr platform_t platform = platform_t::count;
#endif

#if !defined(_WIN32) \
		&& (defined(__unix__) || defined(__unix) \
				|| (defined(__APPLE__) && defined(__MACH__)) \
				|| defined(__CYGWIN__))
}
#include <unistd.h>

namespace fea {

#if defined(_POSIX_VERSION)
#undef FEA_POSIX
#define FEA_POSIX 1
#undef FEA_UNIX
#define FEA_UNIX 1
inline constexpr platform_group_t platform_group
		= platform_group_t::posix | platform_group_t::unixx;
#else
#undef FEA_UNIX
#define FEA_UNIX 1
inline constexpr platform_group_t platform_group = platform_group_t::unixx;
#endif

#else
inline constexpr platform_group_t platform_group = platform_group_t::count;
#endif

} // namespace fea
