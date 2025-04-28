#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "Cluster.h"
#include "Parallel.h"
#include "StridedView.h"
#include "TriangleUtil.h"
#include "VectorMath.h"

struct Vert
{
    std::vector<Vector3f> Positions;
};

static void ClusterTriangles(Vert& verts, const std::vector<uint32_t>& indexes, std::vector<Cluster>& clusters,
                             const Bounds3f& meshBounds)
{
    uint32_t numTriangles = indexes.size() / 3;

    Adjacency adjacency{indexes.size()};
    EdgeHash  edgeHash{indexes.size()};

    auto GetPosition = [&verts, &indexes](uint32_t edgeIndex) { return verts.Positions[indexes[edgeIndex]]; };

    // 将每个索引视作一条边，构建边的哈希表
    ParallelFor("ClusterTriangles.ParalleFor", indexes.size(), 4096,
                [&](int edgeIndex) { edgeHash.AddConcurrent(edgeIndex, GetPosition); });

    // 将每个索引视作一条边，确定边的邻接关系
    ParallelFor("ClusterTriangles.ParalleFor", indexes.size(), 1024,
                [&](int edgeIndex)
                {
                    int32_t adjIndex = -1;// -1表示没有邻接边
                    int32_t adjCount = 0;

                    // 遍历边的邻接边
                    edgeHash.ForAllMatching(edgeIndex, false, GetPosition,
                                            [&](int32_t edgeIndex, int32_t otherEdgeIndex)
                                            {
                                                adjIndex = otherEdgeIndex; // 记录邻接边的索引
                                                adjCount++;
                                            });

                    // 通常共边三角形的那条共边是一对方向相反的边互相邻接
                    if (adjCount > 1)
                        adjIndex = -2;// 如果超过了1条邻接边，说明是个复杂连接

                    adjacency.Direct[edgeIndex] = adjIndex; // 记录直接邻边
                });
}

int main() { return 0; }
