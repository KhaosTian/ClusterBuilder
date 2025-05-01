#pragma once

#include "VectorMath.h"

class BoundingBox {
public:
    Vector3f GetMin() const {}
    Vector3f GetMax() const {}
};

class BoundingSphere {
public:
    
};

using Bounds3f = BoundingBox;
using Sphere3f = BoundingSphere;
