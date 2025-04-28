#include "pch.h"
#include "Cluster.h"
#include "Parallel.h"
#include "StridedView.h"
#include "EdgeHash.h"
#include "VectorMath.h"
#include "Adjacency.h"
#include "DisjointSet.h"

struct Vert {
    std::vector<Vector3f> Positions;
};

static void ClusterTriangles(Vert& verts, const std::vector<uint32_t>& indexes, std::vector<Cluster>& clusters, const Bounds3f& mesh_bounds) {
    uint32_t num_triangles = static_cast<uint32_t>(indexes.size() / 3);

    Adjacency adjacency { indexes.size() };
    EdgeHash  edge_hash { indexes.size() };

    auto GetPosition = [&verts, &indexes](uint32_t edge_index) { return verts.Positions[indexes[edge_index]]; };

    // 将每个索引视作一条边，构建边的哈希表
    ParallelFor("ClusterTriangles.ParalleFor", indexes.size(), 4096, [&](int edge_index) { edge_hash.AddConcurrent(edge_index, GetPosition); });

    // 将每个索引视作一条边，确定边的邻接关系
    ParallelFor("ClusterTriangles.ParalleFor", indexes.size(), 1024, [&](int edge_index) {
        int32_t adj_index = -1; // -1表示没有邻接边
        int32_t adj_count = 0;

        // 遍历边的邻接边
        edge_hash.ForAllMatching(edge_index, false, GetPosition, [&](int32_t edge_ndex, int32_t other_edge_index) {
            adj_index = other_edge_index; // 记录邻接边的索引
            adj_count++;
        });

        // 通常共边三角形的那条共边是一对方向相反的边互相邻接
        if (adj_count > 1) adj_index = -2; // 如果超过了1条邻接边，说明是个复杂连接

        adjacency.direct[edge_index] = adj_index; // 记录直接邻边
    });

    DisjointSet disjoint_set(num_triangles);

    // 遍历所有边
    for (uint32_t edge_index = 0, num = indexes.size(); edge_index < num; edge_index++) {
        // 处理复杂边
        if (adjacency.direct[edge_index] == -2) {
            std::vector<std::pair<int32_t, int32_t>> edges;
            // 收集所有匹配当前边的边
            edge_hash.ForAllMatching(edge_index, false, GetPosition, [&](int32_t edge_index0, int32_t edge_index1) { edges.emplace_back(edge_index0, edge_index1); });

            // 标准库排序保证确定性
            std::sort(edges.begin(), edges.end());

            // 建立邻接关系
            for (const auto& edge: edges) {
                adjacency.Link(edge.first, edge.second);
            }
        }

        // 遍历当前边的邻接边
        adjacency.ForAll(edge_index, [&](int32_t edge_index0, int32_t edge_index1) {
            // 合并邻边三角形
            if (edge_index0 > edge_index1) {
                disjoint_set.UnionSequential(edge_index0 / 3, edge_index1 / 3);
            }
        });
    }
}

int main() {
    return 0;
}
