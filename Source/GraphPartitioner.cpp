#include "pch.h"
#include "GraphPartitioner.h"

FORCEINLINE GraphPartitioner::GraphPartitioner(uint32 num_elements, int32 min_partition_size, int32 max_partition_size) {
}

FORCEINLINE GraphPartitioner::GraphData* GraphPartitioner::NewGraph(uint32 num_adjacency) const {
    return nullptr;
}

FORCEINLINE void GraphPartitioner::AddAdjaceny(GraphData* graph, uint32 adj_index, idx_t cost) {
}

FORCEINLINE void GraphPartitioner::AddLocalityLinks(GraphData* graph, uint32 index, idx_t cost) {
}

FORCEINLINE void GraphPartitioner::Partition(GraphData* graph) {
}

FORCEINLINE void GraphPartitioner::ParititionStrict(GraphData* graph, bool enable_threaded) {
}

FORCEINLINE void GraphPartitioner::BisectGraph(GraphData* graph, GraphData* child_graphs[2]) {
}

FORCEINLINE void GraphPartitioner::RecursiveBisectGraph(GraphData* graph) {
}
