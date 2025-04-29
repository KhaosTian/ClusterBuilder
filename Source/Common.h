#pragma once


#include <intrin.h>
#define DEBUG_BREAK() __debugbreak()

#define FORCEINLINE __forceinline

#define RESTRICT __restrict

using ErrorHandler = void (*)(const char*, const char*, int, const char*);

FORCEINLINE static void DefaultErrorHandler(const char* expr, const char* file, int line, const char* msg = nullptr) {
    std::fprintf(stderr, "Assertion Failed:\n");
    std::fprintf(stderr, "  Expression: %s\n", expr);
    std::fprintf(stderr, "  File:       %s\n", file);
    std::fprintf(stderr, "  Line:       %d\n", line);
    if (msg) {
        std::fprintf(stderr, "  Message:    %s\n", msg);
    }
    std::fflush(stderr);
}

static ErrorHandler g_ErrorHandler = &DefaultErrorHandler;

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            if (g_ErrorHandler) { \
                g_ErrorHandler(#expr, __FILE__, __LINE__, nullptr); \
            } \
            DEBUG_BREAK(); \
            std::abort(); \
        } \
    } while (0)


FORCEINLINE static uint32_t MurmurFinalize32(uint32_t hash) {
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}

FORCEINLINE static uint32_t Murmur32(std::initializer_list<uint32_t> init_list) {
    uint32_t hash = 0;
    for (auto element: init_list) {
        element *= 0xcc9e2d51;
        element = (element << 15) | (element >> (32 - 15));
        element *= 0x1b873593;

        hash ^= element;
        hash = (hash << 13) | (hash >> (32 - 13));
        hash = hash * 5 + 0xe6546b64;
    }

    return MurmurFinalize32(hash);
}