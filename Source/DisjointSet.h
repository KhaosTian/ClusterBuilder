#pragma once

class DisjointSet {
public:
    DisjointSet() {}
    DisjointSet(uint32 size);

    void Init(uint32 size);
    void Reset();
    void AddDefaulted(uint32 num = 1);

    void     Union(uint32 x, uint32 y);
    void     UnionSequential(uint32 x, uint32 y);
    uint32 Find(uint32 i);

    uint32 operator[](uint32 i) const { return m_parents[i]; }

private:
    std::vector<uint32> m_parents; // 数组存储的是每个节点的父节点索引
};