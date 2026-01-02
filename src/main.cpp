#include "raylib.h"
#include "Renderer.h"
#include "GameObject.h"
#include "CameraS.h"
#include "OBJLoader.h"
#include "Texture.h"
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

struct GameState {
    Renderer* renderer;
    CameraS camera;
    GameObject* cube;
    float timer = 0.0f;
    float speed = 5.0f;
    float rotSpeed = 3.0f;
};

GameState* gState = nullptr;

void UpdateFrame() {
    float dt = GetFrameTime();
    
    Vector3S right = {gState->camera.rotationMatrix.m[0][0], gState->camera.rotationMatrix.m[0][1], gState->camera.rotationMatrix.m[0][2]};
    Vector3S up = {gState->camera.rotationMatrix.m[1][0], gState->camera.rotationMatrix.m[1][1], gState->camera.rotationMatrix.m[1][2]};
    Vector3S forward = {gState->camera.rotationMatrix.m[2][0], gState->camera.rotationMatrix.m[2][1], gState->camera.rotationMatrix.m[2][2]};

    if (IsKeyDown(KEY_W)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(forward, gState->speed * dt));
    if (IsKeyDown(KEY_S)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(forward, -gState->speed * dt));
    if (IsKeyDown(KEY_D)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(right, gState->speed * dt));
    if (IsKeyDown(KEY_A)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(right, -gState->speed * dt));
    if (IsKeyDown(KEY_E)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(up, gState->speed * dt));
    if (IsKeyDown(KEY_Q)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(up, -gState->speed * dt));

    if (IsKeyDown(KEY_LEFT)) {
        Matrix4x4 rot = MatrixMakeRotationY(-gState->rotSpeed * dt);
        gState->camera.rotationMatrix = MultiplyMatrix(rot, gState->camera.rotationMatrix);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        Matrix4x4 rot = MatrixMakeRotationY(gState->rotSpeed * dt);
        gState->camera.rotationMatrix = MultiplyMatrix(rot, gState->camera.rotationMatrix);
    }
    if (IsKeyDown(KEY_UP)) {
        Matrix4x4 rot = MatrixMakeRotationX(-gState->rotSpeed * dt);
        gState->camera.rotationMatrix = MultiplyMatrix(rot, gState->camera.rotationMatrix);
    }
    if (IsKeyDown(KEY_DOWN)) {
        Matrix4x4 rot = MatrixMakeRotationX(gState->rotSpeed * dt);
        gState->camera.rotationMatrix = MultiplyMatrix(rot, gState->camera.rotationMatrix);
    }

    gState->cube->transform.scale = {1.5f * fabs(sinf(3 * gState->timer)), 1.0f, 1.0f};
    gState->timer = fmod(gState->timer + dt, 1000000.0f);
    
    gState->renderer->Clear(BLACK);
    gState->renderer->DrawMesh(*gState->cube, gState->camera);
    gState->renderer->Render();
}

int main() {
    const int width = 800;
    const int height = 450;

    InitWindow(width, height, "C++ Software Renderer");

    gState = new GameState();
    gState->renderer = new Renderer(width, height);
    gState->camera.position = {0, 0, -5.0f};

    MeshS loadedMesh;
    if (!ObjLoader::LoadOBJ("models/Chicken.obj", loadedMesh)) {
        TraceLog(LOG_ERROR, "Failed to load OBJ file");
        CloseWindow();
        return -1;
    }

    static TextureS monkeyTexture;
    bool hasLoaded = monkeyTexture.Load("models/ChickenTexture.png");

    gState->cube = new GameObject(loadedMesh);
    if (hasLoaded) {
        gState->cube->texture = &monkeyTexture;
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(UpdateFrame, 0, 1);
#else
    while (!WindowShouldClose()) {
        UpdateFrame();
    }
#endif

    delete gState->cube;
    delete gState->renderer;
    delete gState;
    CloseWindow();
    return 0;
}