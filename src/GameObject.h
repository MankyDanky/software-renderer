#pragma once
#include "Components.h"
#include "Texture.h"

class GameObject {
public: 
    MeshS mesh;
    TransformS transform;
    TextureS* texture = nullptr;

    GameObject(MeshS m) : mesh(m) {}
};