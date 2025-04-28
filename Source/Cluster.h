#pragma once

class Cluster {
public:
    Cluster() {}

public:
    Vector3f& GetPosition(uint32_t vertIndex);
    Normal3f& GetNormal(uint32_t vertIndex);
    Vector2f& GetUVs(uint32_t vertIndex);
    Color3f&  GetColor(uint32_t vertIndex);

    const Vector3f& GetPosition(uint32_t vertIndex) const;
    const Normal3f& GetNormal(uint32_t vertIndex) const;
    const Vector2f& GetUVs(uint32_t vertIndex) const;
    const Color3f&  GetColor(uint32_t vertIndex) const;

    static const uint16_t ClusterSize = 128;

    uint32_t NumVerts = 0;
    uint32_t NumTris  = 0;

    std::vector<float>    Verts;
    std::vector<uint32_t> Indexes;
    std::vector<int32_t>  MaterialIndexes;

    Bounds3f Bounds;
    uint64_t GUID     = 0;
    int32_t  MipLevel = 0;
};
