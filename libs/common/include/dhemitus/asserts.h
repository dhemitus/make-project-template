#pragma once
#include "preference.h"

#define ASSERTIONS_ENABLED

#ifdef ASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

DHEMITUS_API void report_assertion_failure(const char *expression, const char *message, const char *file, i32 line);

#define ASSERTION(expr)                                                \
    {                                                                   \
        if (expr) {                                                     \
        } else {                                                        \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);    \
            debugBreak();                                               \
        }                                                               \
    }

#define ASSERTION_MSG(expr, message)                                        \
    {                                                                       \
        if (expr) {                                                         \
        } else {                                                            \
            report_assertion_failure(#expr, message, __FILE__, __LINE__);   \
            debugBreak();                                                   \
        }                                                                   \
    }

#ifdef _DEBUG
#define ASSERTION_DEBUG(expr)                                           \
    {                                                                   \
        if (expr) {                                                     \
        } else {                                                        \
            report_assertion_failure(#expr, "", __FILE__, __LINE__);    \
            debugBreak();                                               \
        }                                                               \
    }
#else
#define ASSERTION_DEBUG(expr)
#endif

#else
#define ASSERTION(expr)
#define ASSERTION_MSG(expr, message)
#define ASSERTION_DEBUG(expr)
#endif
