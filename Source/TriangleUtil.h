#pragma once
#include <algorithm>
#include <bit>
#include <map>
#include <unordered_map>
#include <utility>
#include <vector>
#include "HashTable.h"
#include "VectorMath.h"

static inline uint32_t HashPosition(const Vector3f& position) {
    auto ToUint = [](float f) {
        union {
            float    f;
            uint32_t i;
        } u = { f };
        return f == 0.0 ? 0u : u.i; // 兼容-0.0，确保零值哈希一致
    };

    // 将位置的三个浮点数坐标映射到一维哈希key
    return Murmur32({ ToUint(position.x), ToUint(position.y), ToUint(position.z) });
}

static inline uint32_t Cycle3(uint32_t value) {
    uint32_t value_mod3     = value % 3;
    uint32_t next_value_mod3 = (1 << value_mod3) & 3;
    return value - value_mod3 + next_value_mod3;
}

struct EdgeHash {
    HashTable hash_table {};
    EdgeHash(uint32_t num): hash_table { 1 << std::bit_floor(num), num } {}

    template<typename FuncType>
    void AddConcurrent(int32_t edge_index, FuncType&& GetPosition) {
        // 根据边索引获取坐标和其相邻坐标
        const Vector3f position0 = GetPosition(edge_index);
        const Vector3f position1 = GetPosition(Cycle3(edge_index));

        // 将两个顶点的坐标分别映射为一维的哈希值
        uint32_t hash0 = HashPosition(position0);
        uint32_t hash1 = HashPosition(position1);

        // 继续将二者的哈希值映射为一个哈希值
        uint32_t hash = Murmur32({ hash0, hash1 });

        // 将哈希值映射到边索引
        hash_table.AddConcurrent(hash, edge_index);
    }

    template<typename FuncType1, typename FuncType2>
    void ForAllMatching(int32_t edge_index, bool need_add, FuncType1&& GetPosition, FuncType2&& Function) {
        // 根据边索引获取坐标和其相邻坐标
        const Vector3f position0 = GetPosition(edge_index);
        const Vector3f position1 = GetPosition(Cycle3(edge_index));

        // 将两个顶点的坐标分别映射为一维的哈希值
        uint32_t hash0 = HashPosition(position0);
        uint32_t hash1 = HashPosition(position1);

        // 继续将二者的哈希值映射为一个哈希值
        uint32_t hash = Murmur32({ hash0, hash1 });

        // 从头节点开始遍历该哈希桶所有边
        for (uint32_t other_edge_index = hash_table.First(hash); hash_table.IsValid(other_edge_index); other_edge_index = hash_table.Next(other_edge_index)) {
            // 匹配和当前边共享顶点但是方向相反的边，即两个三角形共享一条边
            if (position0 == GetPosition(Cycle3(other_edge_index)) && position1 == GetPosition(other_edge_index)) {
                Function(edge_index, other_edge_index);
            }
        }

        // 如果有需要就加入到哈希表中
        if (need_add) {
            hash_table.Add(Murmur32({ hash1, hash0 }), edge_index);
        }
    }
};

struct Adjacency {
    // 存储每个边的一个直接邻接边，-1表示没有直接邻接
    std::vector<int32_t> direct;

    // 存储额外的邻接关系，当一个边有多个邻接边时使用
    std::multimap<int32_t, int32_t> extended;

    Adjacency(size_t num) {
        // 初始化Direct数组，所有值设为-1表示尚未连接
        direct.resize(num, -1);
    }

    // 向Extended映射中添加键值对，确保不重复添加
    void AddUnique(int32_t key, int32_t value) {
        // 获取指定键的所有已存在值
        auto range = extended.equal_range(key);
        bool found = false;

        // 检查是否已存在相同的键值对
        for (auto& it = range.first; it != range.second; ++it) {
            if (it->second == value) {
                found = true;
                break;
            }
        }

        // 仅在不存在时添加新的键值对
        if (!found) {
            extended.insert({ key, value });
        }
    }

    // 在两个边之间建立邻接连接
    // 如果两个边都没有直接邻接边，则使用Direct数组存储它们之间的关系。
    // 否则，使用Extended多重映射存储它们之间的关系。
    void Link(int32_t edge_index0, int32_t edge_index1) {
        // 如果两个边都没有直接邻接边，使用Direct数组连接它们
        if (direct[edge_index0] < 0 && direct[edge_index1] < 0) {
            direct[edge_index0] = edge_index1;
            direct[edge_index1] = edge_index0;
        } else {
            // 否则使用Extended存储它们之间的连接关系
            AddUnique(edge_index0, edge_index1);
            AddUnique(edge_index1, edge_index0);
        }
    }

    // 遍历指定边的所有邻接边，并对每个邻接对应用给定函数
    template<typename FuncType>
    void ForAll(int32_t edge_index, FuncType&& Function) const {
        // 首先检查Direct数组中的直接邻接
        int32_t adjIndex = direct[edge_index];
        if (adjIndex >= 0) {
            // 对直接邻接应用函数
            Function(edge_index, adjIndex);
        }

        // 然后检查Extended中的所有邻接
        auto range = extended.equal_range(edge_index);
        for (auto& it = range.first; it != range.second; ++it) {
            // 对每个额外邻接应用函数
            Function(edge_index, it->second);
        }
    }
};
