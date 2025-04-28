#pragma once

#include <vector>

class DisjointSet
{
public:
    DisjointSet() {}
    DisjointSet(uint32_t size);

    void Init(uint32_t size);
    void Reset();
    void AddDefaulted(uint32_t num = 1);

    void     Union(uint32_t x, uint32_t y);
    void     UnionSequential(uint32_t x, uint32_t y);
    uint32_t Find(uint32_t i);

    uint32_t operator[](uint32_t i) const { return m_Parents[i]; }

private:
    std::vector<uint32_t> m_Parents;
};
