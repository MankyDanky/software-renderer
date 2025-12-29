#include "raylib.h"
#include "Renderer.h"
#include "GameObject.h"
#include "CameraS.h"
#include <vector>

int main() {
    const int width = 800;
    const int height = 450;


    InitWindow(width, height, "C++ Software Renderer");

    SetTargetFPS(60);

    Renderer renderer(width, height);
    CameraS camera;
    camera.position = {0, 0, -5.0f};

    MeshS cubeMesh;
    cubeMesh.vertices = {
        // Front Face (Normal: 0, 0, -1)
        {{-1, -1, -1}, {0, 0, -1}}, {{1, -1, -1}, {0, 0, -1}}, {{1, 1, -1}, {0, 0, -1}}, {{-1, 1, -1}, {0, 0, -1}},
        // Back Face (Normal: 0, 0, 1)
        {{-1, -1, 1}, {0, 0, 1}}, {{1, -1, 1}, {0, 0, 1}}, {{1, 1, 1}, {0, 0, 1}}, {{-1, 1, 1}, {0, 0, 1}},
        // Left Face (Normal: -1, 0, 0)
        {{-1, -1, -1}, {-1, 0, 0}}, {{-1, 1, -1}, {-1, 0, 0}}, {{-1, 1, 1}, {-1, 0, 0}}, {{-1, -1, 1}, {-1, 0, 0}},
        // Right Face (Normal: 1, 0, 0)
        {{1, -1, -1}, {1, 0, 0}}, {{1, 1, -1}, {1, 0, 0}}, {{1, 1, 1}, {1, 0, 0}}, {{1, -1, 1}, {1, 0, 0}},
        // Top Face (Normal: 0, 1, 0)
        {{-1, 1, -1}, {0, 1, 0}}, {{1, 1, -1}, {0, 1, 0}}, {{1, 1, 1}, {0, 1, 0}}, {{-1, 1, 1}, {0, 1, 0}},
        // Bottom Face (Normal: 0, -1, 0)
        {{-1, -1, -1}, {0, -1, 0}}, {{1, -1, -1}, {0, -1, 0}}, {{1, -1, 1}, {0, -1, 0}}, {{-1, -1, 1}, {0, -1, 0}}
    };

    cubeMesh.indices = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Left
        12, 13, 14, 14, 15, 12, // Right
        16, 17, 18, 18, 19, 16, // Top
        20, 21, 22, 22, 23, 20  // Bottom
    };
    GameObject cube(cubeMesh);
    
    while (!WindowShouldClose()) {
        cube.transform.rotation.x += 0.01f;
        cube.transform.rotation.y += 0.02f;
        cube.transform.rotation.z += 0.02f;
        renderer.Clear(BLACK);

        renderer.DrawMesh(cube, camera);

        renderer.Render();
    }

    CloseWindow();
    return 0;
}