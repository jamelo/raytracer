#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <cassert>

#ifdef NDEBUG
#   define Assert(x) static_cast<void>(x)
#else
#   define Assert(x) assert(x)
#endif

#endif
