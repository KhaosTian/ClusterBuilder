#pragma once

class Cluster {
public:
    Cluster() {}

public:
    Vector3f& GetPosition(uint32 vertIndex);
    Normal3f& GetNormal(uint32 vertIndex);
    Vector2f& GetUVs(uint32 vertIndex);
    Color3f&  GetColor(uint32 vertIndex);

    const Vector3f& GetPosition(uint32 vertIndex) const;
    const Normal3f& GetNormal(uint32 vertIndex) const;
    const Vector2f& GetUVs(uint32 vertIndex) const;
    const Color3f&  GetColor(uint32 vertIndex) const;

    static const uint16_t ClusterSize = 128;

    uint32 NumVerts = 0;
    uint32 NumTris  = 0;

    std::vector<float>    Verts;
    std::vector<uint32> Indexes;
    std::vector<int32>  MaterialIndexes;

    Bounds3f Bounds;
    uint64_t GUID     = 0;
    int32  MipLevel = 0;
};
