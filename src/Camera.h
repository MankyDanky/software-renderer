#pragma once
#include "Math.h"

struct Camera {
    Vector3 position = {0, 0, -5.0f};
    Vector3 rotation = {0, 0, 0};
    float fov = 600.0f;
};