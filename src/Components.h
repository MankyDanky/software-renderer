#pragma once
#include <vector>
#include "Math.h"

struct Transform {
    Vector3 position = {0, 0, 0};
    Vector3 rotation = {0, 0, 0};
    Vector3 scale = {1, 1, 1};
};

struct Mesh {
    std::vector<Vector3> vertices;
    std::vector<std::pair<int,int>> edges;
};