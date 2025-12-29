#pragma once
#include <vector>
#include "MathS.h"

struct TransformS {
    Vector3S position = {0, 0, 0};
    Vector3S rotation = {0, 0, 0};
    Vector3S scale = {1, 1, 1};
};

struct MeshS {
    std::vector<Vector3S> vertices;
    std::vector<std::pair<int,int>> edges;
};