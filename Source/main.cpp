#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "Cluster.h"
#include "StridedView.h"
#include "VectorMath.h"
#include "TriangleUtil.h"

struct Vert
{
    std::vector<Vector3f> Positions;
};

static void ClusterTriangles(const std::vector<Vert>& verts, const std::vector<uint32_t>& indexes, std::vector<Cluster>& clusters,
                             const Bounds3f& meshBounds)
{
    uint32_t numTriangles = indexes.size() / 3;

    Adjacency adjacency{indexes.size()};

}

int main() 
{
    int i = FloorLog2(560);

    std::cout << i << "\n";

    return 0; 

}
