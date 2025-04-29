#pragma once

class DisjointSet;

class GraphPartitioner {
public:
    struct GraphData {
        int32_t offset;
        int32_t num;

        std::vector<idx_t> adjacency;
        std::vector<idx_t> adjacency_cost;
        std::vector<idx_t> adjacency_offset;
    };

    struct Range {
        uint32_t begin;
        uint32_t end;

        bool operator<(const Range& other) const { return begin < other.begin; }
    };

    std::vector<Range>    ranges;
    std::vector<uint32_t> indexes;
    std::vector<uint32_t> sorted_to;

public:
    GraphPartitioner(uint32_t num_elements, int32_t min_partition_size, int32_t max_partition_size);
    GraphData* NewGraph(uint32_t num_adjacency) const;

    void AddAdjaceny(GraphData* graph, uint32_t adj_index, idx_t cost);
    void AddLocalityLinks(GraphData* graph, uint32_t index, idx_t cost);

    template <typename FuncType>
    void BuildLocalityLinks(DisjointSet& disjoint_set, const Bounds3f& bounds, std::vector<int32_t>& group_indexes, FuncType& GetCenter);

    void Partition(GraphData* graph);

    void ParititionStrict(GraphData* graph, bool enable_threaded);

    void BisectGraph(GraphData* graph, GraphData* child_graphs[2]);
    void RecursiveBisectGraph(GraphData* graph);


    uint32_t m_num_elements;
    int32_t m_min_partition_size;
    int32_t  m_max_partition_size;

    std::atomic<uint32_t> m_num_parition;

    std::vector<idx_t> partition_ids;
    std::vector<int32_t> swapped_with;

    std::multimap<int32_t, uint32_t> m_locality_links;
};

template<typename FuncType>
inline void GraphPartitioner::BuildLocalityLinks(DisjointSet& disjoint_set, const Bounds3f& bounds, std::vector<int32_t>& group_indexes, FuncType& GetCenter) {
}
