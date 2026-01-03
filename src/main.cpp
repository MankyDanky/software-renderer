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
    std::vector<GameObject*> objects;
    float timer = 0.0f;
    float speed = 5.0f;
    float rotSpeed = 3.0f;
};

GameState* gState = nullptr;

void UpdateFrame() {
    float dt = GetFrameTime();
    
    if (IsKeyPressed(KEY_ONE)) gState->renderer->SetShadingMode(ShadingMode::Phong);
    if (IsKeyPressed(KEY_TWO)) gState->renderer->SetShadingMode(ShadingMode::Gouraud);
    if (IsKeyPressed(KEY_THREE)) gState->renderer->SetShadingMode(ShadingMode::Flat);
    if (IsKeyPressed(KEY_FOUR)) gState->renderer->SetShadingMode(ShadingMode::Cel);
    if (IsKeyPressed(KEY_FIVE)) gState->renderer->SetShadingMode(ShadingMode::Unlit);
    
    Vector3S right = {gState->camera.rotationMatrix.m[0][0], gState->camera.rotationMatrix.m[0][1], gState->camera.rotationMatrix.m[0][2]};
    Vector3S up = {gState->camera.rotationMatrix.m[1][0], gState->camera.rotationMatrix.m[1][1], gState->camera.rotationMatrix.m[1][2]};
    Vector3S forward = {gState->camera.rotationMatrix.m[2][0], gState->camera.rotationMatrix.m[2][1], gState->camera.rotationMatrix.m[2][2]};

    if (IsKeyDown(KEY_W)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(forward, gState->speed * dt));
    if (IsKeyDown(KEY_S)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(forward, -gState->speed * dt));
    if (IsKeyDown(KEY_D)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(right, gState->speed * dt));
    if (IsKeyDown(KEY_A)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(right, -gState->speed * dt));
    if (IsKeyDown(KEY_SPACE)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(up, -gState->speed * dt));
    if (IsKeyDown(KEY_LEFT_SHIFT)) gState->camera.position = Vector3Add(gState->camera.position, Vector3Scale(up, gState->speed * dt));

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouseDelta = GetMouseDelta();
        float sensitivity = 0.003f;
        
        gState->camera.yaw += mouseDelta.x * sensitivity;
        gState->camera.pitch += mouseDelta.y * sensitivity;
        
        const float maxPitch = 1.5f;
        if (gState->camera.pitch > maxPitch) gState->camera.pitch = maxPitch;
        if (gState->camera.pitch < -maxPitch) gState->camera.pitch = -maxPitch;
        
        Matrix4x4 yawMat = MatrixMakeRotationY(gState->camera.yaw);
        Matrix4x4 pitchMat = MatrixMakeRotationX(gState->camera.pitch);
        gState->camera.rotationMatrix = MultiplyMatrix(pitchMat, yawMat);
    }

    float t = gState->timer;
    
    gState->objects[0]->transform.scale = {1.0f + 0.2f * sinf(2.0f * t), 1.0f + 0.2f * sinf(2.0f * t), 1.0f + 0.2f * sinf(2.0f * t)};
    
    gState->objects[1]->transform.rotation.y = t * 2.0f;
    
    gState->objects[2]->transform.position.y = 0.5f * sinf(3.0f * t);
    
    gState->objects[3]->transform.rotation.x = t * 1.5f;
    gState->objects[3]->transform.rotation.z = t * 0.7f;
    
    float squash = 1.0f + 0.4f * sinf(4.0f * t);
    gState->objects[4]->transform.scale = {1.0f / sqrtf(squash), squash, 1.0f / sqrtf(squash)};
    
    gState->timer = fmod(gState->timer + dt, 1000000.0f);
    
    gState->renderer->Clear(BLACK);
    for (auto* obj : gState->objects) {
        gState->renderer->DrawMesh(*obj, gState->camera);
    }
    gState->renderer->Render();
}

int main() {
    const int width = 800;
    const int height = 450;

    InitWindow(width, height, "C++ Software Renderer");
    SetTargetFPS(0);

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

    Vector3S positions[] = {
        {0.0f, 0.0f, 0.0f},
        {-6.0f, 0.0f, 0.0f}, 
        {6.0f, 0.0f, 0.0f},  
        {-4.0f, 0.0f, 6.0f}, 
        {4.0f, 0.0f, 6.0f}  
    };
    
    for (int i = 0; i < 5; i++) {
        GameObject* obj = new GameObject(loadedMesh);
        obj->transform.position = positions[i];
        if (hasLoaded) {
            obj->texture = &monkeyTexture;
        }
        gState->objects.push_back(obj);
    }

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(UpdateFrame, 0, 1);
#else
    while (!WindowShouldClose()) {
        UpdateFrame();
    }
#endif

    for (auto* obj : gState->objects) {
        delete obj;
    }
    delete gState->renderer;
    delete gState;
    CloseWindow();
    return 0;
}