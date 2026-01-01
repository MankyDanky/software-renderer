#include "raylib.h"
#include "Renderer.h"
#include "GameObject.h"
#include "CameraS.h"
#include "OBJLoader.h"
#include "Texture.h"
#include <vector>

int main() {
    const int width = 800;
    const int height = 450;

    const float speed = 5.0f;
    const float rotSpeed = 3.0f;


    InitWindow(width, height, "C++ Software Renderer");

    Renderer renderer(width, height);
    CameraS camera;
    camera.position = {0, 0, -5.0f};
    
    MeshS loadedMesh;
    if (!ObjLoader::LoadOBJ("models/suzanne.obj", loadedMesh)) {
        TraceLog(LOG_ERROR, "Failed to load OBJ file: models/suzanne.obj");
        CloseWindow();
        return -1;
    }
    
    TextureS monkeyTexture;
    bool hasLoaded = monkeyTexture.Load("models/suzanneTexture.png");
    
    GameObject cube(loadedMesh);
    if (hasLoaded) {
        cube.texture = &monkeyTexture;
    }
    
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

        cube.transform.rotation.x += dt;
        cube.transform.rotation.y += 2 * dt;
        cube.transform.rotation.z += 3 * dt;
        renderer.Clear(BLACK);

        renderer.DrawMesh(cube, camera);

        renderer.Render();
    }

    CloseWindow();
    return 0;
}