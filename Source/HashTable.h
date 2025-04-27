#pragma once
#include <cstdint>
#include <initializer_list>

static inline uint32_t MurmurFinalize32(uint32_t hash)
{
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return hash;
}

static inline uint32_t Murmur32(std::initializer_list<uint32_t> initList)
{
    uint32_t hash = 0;
    for (auto element : initList)
    {
        element *= 0xcc9e2d51;
        element = (element << 15) | (element >> (32 - 15));
        element *= 0x1b873593;

        hash ^= element;
        hash = (hash << 13) | (hash >> (32 - 13));
        hash = hash * 5 + 0xe6546b64;
    }

    return MurmurFinalize32(hash);
}
