#include "raylib.h"
#include "Renderer.h"
#include "GameObject.h"
#include "CameraS.h"
#include <vector>

int main() {
    const int width = 800;
    const int height = 450;

    const float speed = 5.0f;
    const float rotSpeed = 3.0f;


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
        float dt = GetFrameTime();

        Vector3S right = {camera.rotationMatrix.m[0][0], camera.rotationMatrix.m[0][1], camera.rotationMatrix.m[0][2]};
        Vector3S up = {camera.rotationMatrix.m[1][0], camera.rotationMatrix.m[1][1], camera.rotationMatrix.m[1][2]};
        Vector3S forward = {camera.rotationMatrix.m[2][0], camera.rotationMatrix.m[2][1], camera.rotationMatrix.m[2][2]};

        if (IsKeyDown(KEY_W)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(forward, speed * dt));
        }
        if (IsKeyDown(KEY_S)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(forward, -speed * dt));
        }
        if (IsKeyDown(KEY_D)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(right, speed * dt));
        }
        if (IsKeyDown(KEY_A)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(right, -speed * dt));
        }
        if (IsKeyDown(KEY_E)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(up, speed * dt));
        }
        if (IsKeyDown(KEY_Q)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(up, -speed * dt));
        }

        if (IsKeyDown(KEY_LEFT)) {
            Matrix4x4 rot = MatrixMakeRotationY(-rotSpeed * dt);
            camera.rotationMatrix = MultiplyMatrix(rot, camera.rotationMatrix);
        }
        if (IsKeyDown(KEY_RIGHT)) {
            Matrix4x4 rot = MatrixMakeRotationY(rotSpeed * dt);
            camera.rotationMatrix = MultiplyMatrix(rot, camera.rotationMatrix);
        }
        if (IsKeyDown(KEY_UP)) {
            Matrix4x4 rot = MatrixMakeRotationX(-rotSpeed * dt);
            camera.rotationMatrix = MultiplyMatrix(rot, camera.rotationMatrix);
        }
        if (IsKeyDown(KEY_DOWN)) {
            Matrix4x4 rot = MatrixMakeRotationX(rotSpeed * dt);
            camera.rotationMatrix = MultiplyMatrix(rot, camera.rotationMatrix);
        }

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