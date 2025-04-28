#pragma once
#include <memory>
#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <string>

#if defined(_WIN32)
#include <intrin.h>
#define DEBUG_BREAK() __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK() raise(SIGTRAP)
#endif

#define FORCEINLINE __forceinline

using ErrorHandler = void (*)(const char*, const char*, int, const char*);

static void DefaultErrorHandler(const char* expr, const char* file, int line, const char* msg = nullptr)
{
    std::fprintf(stderr, "Assertion Failed:\n");
    std::fprintf(stderr, "  Expression: %s\n", expr);
    std::fprintf(stderr, "  File:       %s\n", file);
    std::fprintf(stderr, "  Line:       %d\n", line);
    if (msg)
    {
        std::fprintf(stderr, "  Message:    %s\n", msg);
    }
    std::fflush(stderr);
}

static ErrorHandler g_ErrorHandler = &DefaultErrorHandler;

#define CHECK(expr)                                                                                                                                  \
    do                                                                                                                                               \
    {                                                                                                                                                \
        if (!(expr))                                                                                                                                 \
        {                                                                                                                                            \
            if (g_ErrorHandler)                                                                                                                      \
            {                                                                                                                                        \
                g_ErrorHandler(#expr, __FILE__, __LINE__, nullptr);                                                                                  \
            }                                                                                                                                        \
            DEBUG_BREAK();                                                                                                                           \
            std::abort();                                                                                                                            \
        }                                                                                                                                            \
    }                                                                                                                                                \
    while (0)
#define CHECK_MSG(expr, msg)                                                                                                                         \
    do                                                                                                                                               \
    {                                                                                                                                                \
        if (!(expr))                                                                                                                                 \
        {                                                                                                                                            \
            if (g_ErrorHandler)                                                                                                                      \
            {                                                                                                                                        \
                g_ErrorHandler(#expr, __FILE__, __LINE__, msg);                                                                                      \
            }                                                                                                                                        \
            DEBUG_BREAK();                                                                                                                           \
            std::abort();                                                                                                                            \
        }                                                                                                                                            \
    }                                                                                                                                                \
    while (0)
