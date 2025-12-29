#pragma once
#include "Components.h"

class GameObject {
public: 
    Mesh mesh;
    Transform transform;

    GameObject(Mesh m) : mesh(m) {}
};