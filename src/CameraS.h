#pragma once
#include "MathS.h"

struct CameraS {
    Vector3S position = {0, 0, -5.0f};
    Matrix4x4 rotationMatrix = Matrix4x4::Identity();
    float fov = 90.0f;
    float yaw = 0.0f;    // Horizontal rotation (radians)
    float pitch = 0.0f;  // Vertical rotation (radians)
};