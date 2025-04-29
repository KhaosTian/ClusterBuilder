#pragma once

struct Adjacency {
    // 存储每个边的一个直接邻接边，-1表示没有直接邻接
    std::vector<int32> direct;

    // 存储额外的邻接关系，当一个边有多个邻接边时使用
    std::multimap<int32, int32> extended;

    Adjacency(size_t num);
    void AddUnique(int32 key, int32 value);
    void Link(int32 edge_index0, int32 edge_index1);

    template<typename FuncType>
    void ForAll(int32 edge_index, FuncType&& Function) const;
};

// 遍历指定边的所有邻接边，并对每个邻接对应用给定函数
template<typename FuncType>
FORCEINLINE void Adjacency::ForAll(int32 edge_index, FuncType&& Function) const {
    // 首先检查Direct数组中的直接邻接
    int32 adj_index = direct[edge_index];
    if (adj_index >= 0) {
        // 对直接邻接应用函数
        Function(edge_index, adj_index);
    }

    // 然后检查Extended中的所有邻接
    auto [begin, end] = extended.equal_range(edge_index);
    for (auto& it = begin; it != end; ++it) {
        // 对每个额外邻接应用函数
        Function(edge_index, it->second);
    }
}