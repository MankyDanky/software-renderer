#pragma once
#include <vector>
#include "MathS.h"

struct Vertex {
    Vector3S position;
    Vector3S normal;
};

struct TransformS {
    Vector3S position = {0, 0, 0};
    Vector3S rotation = {0, 0, 0};
    Vector3S scale = {1, 1, 1};
};

struct MeshS {
    std::vector<Vertex> vertices;
    std::vector<int> indices;
};