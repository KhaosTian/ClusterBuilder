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

template<typename FuncType>
FORCEINLINE void GraphPartitioner::BuildLocalityLinks(
    DisjointSet&              disjoint_set,
    const Bounds3f&           bounds,
    const std::vector<int32>& group_indexes,
    FuncType&                 GetCenter
) {
    std::vector<uint32> sort_keys;
    sort_keys.reserve(m_num_elements);
    sorted_to.reserve(m_num_elements);

    const bool enable_element_groups = !group_indexes.empty();

    ParallelFor("BuildLocalityLinks.ParallelFor", m_num_elements, 4096, [&](uint32 index) {
        Vector3f center       = GetCenter(index);
        Vector3f cenetr_local = (center - bounds.minP) / Vector3f(bounds.maxP - bounds.minP).max;

        uint32 morton;
    });
}
