#pragma once
#include <algorithm>
#include <bit>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>
#include "HashTable.h"



inline uint32_t HashPosition(const Vector3f& position)
{
    union
    {
        float  f;
        uint32_t i;
    } x;
    union
    {
        float  f;
        uint32_t i;
    } y;
    union
    {
        float  f;
        uint32_t i;
    } z;

    x.f = position.x;
    y.f = position.y;
    z.f = position.z;

    return Murmur32({position.x == 0.0f ? 0u : x.i, position.y == 0.0f ? 0u : y.i, position.z == 0.0f ? 0u : z.i});
}

inline uint32_t FloorLog2(uint32_t value)
{
    unsigned long bitIndex;
    return _BitScanReverse(&bitIndex, value) ? bitIndex : 0;
}

inline uint32_t Cycle3(uint32_t value)
{
    uint32_t valueMod3 = value % 3;
    uint32_t nextValueMod3 = (1 << valueMod3) & 3;
    return value - valueMod3 + nextValueMod3;
}

struct EdgeHash
{
    std::unordered_map<int32_t, int32_t> HashTable{};
    EdgeHash(int32_t num) { HashTable.emplace(1 << FloorLog2(num), num); }

    template <typename FuncType>
    void AddConcurrent(int32_t edgeIndex, FuncType&& GetPosition)
    {
        const Vector3f position0 = GetPosition(edgeIndex);
        const Vector3f position1 = GetPosition(Cycle3(edgeIndex));

        uint32_t hash0 = HashPosition(position0);
        uint32_t hash1 = HashPosition(position1);
        uint32_t hash0 = HashPosition(position0);
    }
};

struct Adjacency
{
    // 存储每个边的一个直接邻接边，-1表示没有直接邻接
    std::vector<int32_t> Direct;

    // 存储额外的邻接关系，当一个边有多个邻接边时使用
    std::multimap<int32_t, int32_t> Extended;

    Adjacency(size_t num)
    {
        // 初始化Direct数组，所有值设为-1表示尚未连接
        Direct.resize(num, -1);
    }

    // 向Extended映射中添加键值对，确保不重复添加
    void AddUnique(int32_t key, int32_t value)
    {
        // 获取指定键的所有已存在值
        auto range = Extended.equal_range(key);
        bool found = false;

        // 检查是否已存在相同的键值对
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second == value)
            {
                found = true;
                break;
            }
        }

        // 仅在不存在时添加新的键值对
        if (!found)
        {
            Extended.insert({key, value});
        }
    }

    // 在两个边之间建立邻接连接
    // 如果两个边都没有直接邻接边，则使用Direct数组存储它们之间的关系。
    // 否则，使用Extended多重映射存储它们之间的关系。
    void Link(int32_t edgeIndex0, int32_t edgeIndex1)
    {
        // 如果两个边都没有直接邻接边，使用Direct数组连接它们
        if (Direct[edgeIndex0] < 0 && Direct[edgeIndex1] < 0)
        {
            Direct[edgeIndex0] = edgeIndex1;
            Direct[edgeIndex1] = edgeIndex0;
        }
        else
        {
            // 否则使用Extended存储它们之间的连接关系
            AddUnique(edgeIndex0, edgeIndex1);
            AddUnique(edgeIndex1, edgeIndex0);
        }
    }

    // 遍历指定边的所有邻接边，并对每个邻接对应用给定函数
    template <typename FuncType>
    void ForAll(int32_t EdgeIndex, FuncType&& Function) const
    {
        // 首先检查Direct数组中的直接邻接
        int32_t AdjIndex = Direct[EdgeIndex];
        if (AdjIndex >= 0)
        {
            // 对直接邻接应用函数
            Function(EdgeIndex, AdjIndex);
        }

        // 然后检查Extended中的所有邻接
        auto range = Extended.equal_range(EdgeIndex);
        for (auto& it = range.first; it != range.second; ++it)
        {
            // 对每个额外邻接应用函数
            Function(EdgeIndex, it->second);
        }
    }
};
