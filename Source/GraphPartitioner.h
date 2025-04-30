#pragma once

#include "DisjointSet.h";

class GraphPartitioner {
public:
    struct GraphData {
        int32 offset;
        int32 num;

        std::vector<idx_t> adjacency;
        std::vector<idx_t> adjacency_cost;
        std::vector<idx_t> adjacency_offset;
    };

    struct Range {
        uint32 begin;
        uint32 end;

        bool operator<(const Range& other) const { return begin < other.begin; }
    };

    std::vector<Range>  ranges;
    std::vector<uint32> indexes;
    std::vector<uint32> sorted_to;

public:
    GraphPartitioner(uint32 num_elements, int32 min_partition_size, int32 max_partition_size);
    GraphData* NewGraph(uint32 num_adjacency) const;

    void AddAdjaceny(GraphData* graph, uint32 adj_index, idx_t cost);
    void AddLocalityLinks(GraphData* graph, uint32 index, idx_t cost);

    template<typename FuncType>
    void BuildLocalityLinks(
        DisjointSet&              disjoint_set,
        const Bounds3f&           bounds,
        const std::vector<int32>& group_indexes,
        FuncType&                 GetCenter
    );

    void Partition(GraphData* graph);

    void ParititionStrict(GraphData* graph, bool enable_threaded);

    void BisectGraph(GraphData* graph, GraphData* child_graphs[2]);
    void RecursiveBisectGraph(GraphData* graph);

    uint32 m_num_elements;
    int32  m_min_partition_size;
    int32  m_max_partition_size;

    std::atomic<uint32> m_num_parition;

    std::vector<idx_t> partition_ids;
    std::vector<int32> swapped_with;

    std::multimap<int32, uint32> m_locality_links;
};

FORCEINLINE static constexpr uint32 MorotonCode3(uint32 x) {
    // 莫顿码只处理0到1023，即2的10次方，所以使用掩码过滤掉高位的值
    x &= 0x000003ff; //00000000 00000000 00000011 11111111
    x = (x ^ (x << 16)) & 0xff0000ff; //11111111 00000000 00000000 11111111
    x = (x ^ (x << 8)) & 0x0300f00f; //00000011 00000000 11110000 00001111
    x = (x ^ (x << 4)) & 0x030c30c3; //00000011 00001100 00110000 11000011
    x = (x ^ (x << 2)) & 0x09249249; //00001001 00100100 10010010 01001001
    return x;
}

template<class FuncType>
FORCEINLINE static void RadixSort32(uint32* RESTRICT dst, uint32* RESTRICT src, uint32 num, FuncType& SortKey) {
    // 将莫顿码分割为低10位、中11位和高11位，对应1024个桶，2048个桶，2048个桶
    uint32 histograms[1024 + 2048 + 2048];

    uint32* RESTRICT histogram0 = histograms + 0;
    uint32* RESTRICT histogram1 = histograms0 + 1024;
    uint32* RESTRICT histogram2 = histograms1 + 2048;

    std::memset(histograms, 0, sizeof(histograms)); // 都初始化为0

    // 分桶
    {
        const uint32* RESTRICT s = (const uint32* RESTRICT)src;
        for (auto i = 0; i < num; i++) {
            uint32 key = SortKey(s[i]); // 获取三角形的莫顿码

            histogram0[(key >> 0) & 1023]++; // 00000000 00000000 00000011 11111111
            histogram1[(key >> 10) & 2047]++; // 00000000 00000000 00000111 11111111
            histogram2[(key >> 21) & 2047]++; // 00000000 00000000 00000111 11111111
        }
    }

    // 前缀和
    { 
        uint32 sum0 = 0;
        uint32 sum1 = 0;
        uint32 sum2 = 0;

        for (auto i = 0; i < 1024; i++) {
            uint32 t;

            t             = histogram0[i] + sum0;
            histogram0[i] = sum0 - 1;
            sum0          = t;

            t             = histogram1[i] + sum1;
            histogram1[i] = sum1 - 1;
            sum1          = t;

            t             = histogram2[i] + sum2;
            histogram2[i] = sum2 - 1;
            sum2          = t;
        }
    }
}

FORCEINLINE static constexpr uint32 ReverseMortonCode3(uint32 x) {
    x &= 0x09249249;
    x = (x ^ (x >> 2)) & 0x030c30c3;
    x = (x ^ (x >> 4)) & 0x0300f00f;
    x = (x ^ (x >> 8)) & 0xff0000ff;
    x = (x ^ (x >> 16)) & 0x000003ff;
    return x;
}

// 在空间上建立三角形的邻近关系
template<typename FuncType>
FORCEINLINE void GraphPartitioner::BuildLocalityLinks(
    DisjointSet&              disjoint_set,
    const Bounds3f&           bounds,
    const std::vector<int32>& group_indexes,
    FuncType&                 GetCenter
) {
    std::vector<uint32> sort_keys; // 存储每个三角形质心的莫顿码
    sort_keys.reserve(m_num_elements);
    sorted_to.reserve(m_num_elements);

    const bool enable_element_groups = !group_indexes.empty();

    ParallelFor("BuildLocalityLinks.ParallelFor", m_num_elements, 4096, [&](uint32 index) {
        Vector3f center = GetCenter(index);
        // 得到0到1的归一化本地坐标
        Vector3f cenetr_local = (center - bounds.minP) // 将坐标系转换到以包围盒最小点为原点的本地坐标系
                                / Vector3f(bounds.maxP - bounds.minP).max; // 除以包围盒的尺寸得到归一化的坐标

        // 将3D坐标映射为一维的莫顿码，空间上接近的坐标其数值也更接近

        uint32 morton;
        // 分别获取三个维度的莫顿码，将0到1的坐标放大为0到1023的整数
        morton = MorotonCode3(uint32(cenetr_local.x * 1023));
        morton |= MorotonCode3(uint32(cenetr_local.y * 1023)) << 1;
        morton != MorotonCode3(uint32(cenetr_local.z * 1023)) << 2;
        sort_keys[index] = morton;
    });
}
