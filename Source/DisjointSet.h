#pragma once

#include <vector>

#include "Common.h"

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

FORCEINLINE DisjointSet::DisjointSet(uint32_t size) { Init(size); }

FORCEINLINE void DisjointSet::Init(uint32_t size)
{
    m_Parents.reserve(size);
    for (uint32_t i = 0; i < size; i++)
    {
        m_Parents[i] = i;
    }
}

FORCEINLINE void DisjointSet::Reset() { m_Parents.clear(); }

FORCEINLINE void DisjointSet::AddDefaulted(uint32_t num)
{
    uint32_t start = m_Parents.size();
    m_Parents.reserve(start + num);
    for (uint32_t i = start; i < start + num; i++)
    {
        m_Parents[i] = i;
    }
}

FORCEINLINE void DisjointSet::Union(uint32_t x, uint32_t y)
{
    uint32_t px = m_Parents[x];
    uint32_t py = m_Parents[y];

    while (px != py)
    {
        if (px < py)
        {
            m_Parents[x] = py;
            if (x == px)
            {
                return;
            }
            x = px;
            px = m_Parents[x];
        }
        else {
            m_Parents[y] = px;
            if (y == py)
            {
                return;
            }
            y = py;
            py = m_Parents[y];
        }
    }

}

FORCEINLINE void DisjointSet::UnionSequential(uint32_t x, uint32_t y) {}

FORCEINLINE uint32_t DisjointSet::Find(uint32_t i) { return 0; }
