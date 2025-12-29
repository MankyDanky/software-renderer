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
    cubeMesh.edges = {
        {0,1}, {1,2}, {2,3}, {3,0}, // Front Face
        {4,5}, {5,6}, {6,7}, {7,4}, // Back Face
        {0,4}, {1,5}, {2,6}, {3,7}  // Connecting Lines
    };
    GameObject cube(cubeMesh);
    
    while (!WindowShouldClose()) {
        cube.transform.rotation.x += 0.01f;
        cube.transform.rotation.y += 0.02f;
        cube.transform.rotation.z += 0.02f;
        renderer.Clear(BLACK);

        renderer.DrawWireframe(cube, camera);

        renderer.Render();
    }

    CloseWindow();
    return 0;
}