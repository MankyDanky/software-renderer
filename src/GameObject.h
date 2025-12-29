#pragma once
#include "Components.h"

class GameObject {
public: 
    MeshS mesh;
    TransformS transform;

    GameObject(MeshS m) : mesh(m) {}
};