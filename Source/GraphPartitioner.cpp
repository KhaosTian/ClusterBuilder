#include "pch.h"
#include "GraphPartitioner.h"

GraphPartitioner::GraphPartitioner(uint32_t num_elements, int32_t min_partition_size, int32_t max_partition_size) {
}

GraphPartitioner::GraphData* GraphPartitioner::NewGraph(uint32_t num_adjacency) const {
    return nullptr;
}

void GraphPartitioner::AddAdjaceny(GraphData* graph, uint32_t adj_index, idx_t cost) {
}

void GraphPartitioner::AddLocalityLinks(GraphData* graph, uint32_t index, idx_t cost) {
}

void GraphPartitioner::Partition(GraphData* graph) {
}

void GraphPartitioner::ParititionStrict(GraphData* graph, bool enable_threaded) {
}

void GraphPartitioner::BisectGraph(GraphData* graph, GraphData* child_graphs[2]) {
}

void GraphPartitioner::RecursiveBisectGraph(GraphData* graph) {
}
