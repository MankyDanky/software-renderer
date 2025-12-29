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
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, // Front vertices
        {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}      // Back vertices
    };
    cubeMesh.indices = {
        // Front Face
        0, 1, 2,  2, 3, 0,
        // Right Face
        1, 5, 6,  6, 2, 1,
        // Back Face
        7, 6, 5,  5, 4, 7,
        // Left Face
        4, 0, 3,  3, 7, 4,
        // Top Face
        3, 2, 6,  6, 7, 3,
        // Bottom Face
        4, 5, 1,  1, 0, 4
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