#include "Renderer.h"
#include <cmath>

Renderer::Renderer(int w, int h) : width(w), height(h) {
    pixelBuffer = new Color[width * height];
    Image img = GenImageColor(width, height, BLACK);
    screenTexture = LoadTextureFromImage(img);
    UnloadImage(img);
}

Renderer::~Renderer() {
    UnloadTexture(screenTexture);
    delete[] pixelBuffer;
}

void Renderer::Clear(Color color) {
    for (int i = 0; i < width * height; i++) 
        pixelBuffer[i] = color;
}

void Renderer::Render() {
    UpdateTexture(screenTexture, pixelBuffer);
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(screenTexture, 0, 0, WHITE);
    DrawFPS(10, 10);
    EndDrawing();
}

void Renderer::PutPixel(int x, int y, Color color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixelBuffer[y * width + x] = color;
    }
}

// Bresenham Line Algorithm
void Renderer::DrawLine(int x0, int y0, int x1, int y1, Color color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        PutPixel(x0, y0, color);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2*err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Renderer::DrawWireframe(const GameObject& obj, const CameraS& cam) {
    Matrix4x4 matRotZ = MatrixMakeRotationZ(obj.transform.rotation.z);
    Matrix4x4 matRotY = MatrixMakeRotationY(obj.transform.rotation.y);
    Matrix4x4 matRotX = MatrixMakeRotationX(obj.transform.rotation.x);
    Matrix4x4 matTrans = MatrixMakeTranslation(obj.transform.position.x, obj.transform.position.y, obj.transform.position.z);

    Matrix4x4 matWorld = Matrix4x4::Identity();
    matWorld = MultiplyMatrix(matRotZ, matRotX);
    matWorld = MultiplyMatrix(matWorld, matRotY);
    matWorld = MultiplyMatrix(matWorld, matTrans);

    Vector3S target = {0, 0, 1};
    Matrix4x4 matCameraRot = MatrixMakeRotationY(cam.rotation.y);
    Matrix4x4 matView = MatrixMakeTranslation(-cam.position.x, -cam.position.y, -cam.position.z);

    Matrix4x4 matProj = MatrixMakeProjection(cam.fov, (float)height / (float)width, 0.1f, 1000.0f);

    Matrix4x4 matMVP = Matrix4x4::Identity();
    matMVP = MultiplyMatrix(matWorld, matView);
    matMVP = MultiplyMatrix(matMVP, matProj);

    std::vector<Vector3S> projectedPoints;
    for (Vector3S v : obj.mesh.vertices) {
        Vector3S transformed = MultiplyVectorMatrix(v, matMVP);

        transformed.x += 1.0f; transformed.y += 1.0f;
        transformed.x *= 0.5f * (float)width;
        transformed.y *= 0.5f * (float)height;;

        projectedPoints.push_back(transformed);
    }

    for (const auto& edge : obj.mesh.edges) {
        Vector3S p1 = projectedPoints[edge.first];
        Vector3S p2 = projectedPoints[edge.second];
        DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
    }
}