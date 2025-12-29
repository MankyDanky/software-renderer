#include "Renderer.h"
#include <cmath>

Renderer::Renderer(int w, int h) : width(w), height(h) {
    pixelBuffer = new Color[width * height];
    depthBuffer = new float[width * height];
    Image img = GenImageColor(width, height, BLACK);
    screenTexture = LoadTextureFromImage(img);
    UnloadImage(img);
}

Renderer::~Renderer() {
    UnloadTexture(screenTexture);
    delete[] pixelBuffer;
    delete[] depthBuffer;
}

void Renderer::Clear(Color color) {
    for (int i = 0; i < width * height; i++) {
        pixelBuffer[i] = color;
        depthBuffer[i] = std::numeric_limits<float>::max();
    }
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

    for (int i = 0; i < obj.mesh.indices.size(); i+=3) {
        int i0 = obj.mesh.indices[i];
        int i1 = obj.mesh.indices[i+1];
        int i2 = obj.mesh.indices[i+2];

        Vector3S p0 = projectedPoints[i0];
        Vector3S p1 = projectedPoints[i1];
        Vector3S p2 = projectedPoints[i2];
        DrawLine(p0.x, p0.y, p1.x, p1.y, GREEN);
        DrawLine(p1.x, p1.y, p2.x, p2.y, GREEN);
        DrawLine(p2.x, p2.y, p0.x, p0.y, GREEN);
    }
}

float EdgeFunction(Vector3S a, Vector3S b, Vector3S p) {
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

void Renderer::DrawTriangleFilled(Vector3S v0, Vector3S v1, Vector3S v2, Color color) {
    int minX = std::min({v0.x, v1.x, v2.x});
    int minY = std::min({v0.y, v1.y, v2.y});
    int maxX = std::max({v0.x, v1.x, v2.x});
    int maxY = std::max({v0.y, v1.y, v2.y});

    minX = std::max(0, minX);
    minY = std::max(0, minY);
    maxX = std::min(width - 1, maxX);
    maxY = std::min(height - 1, maxY);

    float area = EdgeFunction(v0, v1, v2);

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Vector3S p = {(float) x, (float) y, 0};

            float w0 = EdgeFunction(v1, v2, p);
            float w1 = EdgeFunction(v2, v0, p);
            float w2 = EdgeFunction(v0, v1, p);

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                float lambda0 = w0/area;
                float lambda1 = w1/area;
                float lambda2 = w2/area;

                float z = lambda0 * v0.z + lambda1 * v1.z + lambda2 * v2.z;

                int index = y * width + x;
                if (z < depthBuffer[index]) {
                    depthBuffer[index] = z;
                    PutPixel(x, y, color);
                }   
            }
        }
    }
}

void Renderer::DrawMeshFilled(const GameObject& obj, const CameraS& cam) {
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

    for (int i = 0; i < obj.mesh.indices.size(); i+=3) {
        int i0 = obj.mesh.indices[i];
        int i1 = obj.mesh.indices[i+1];
        int i2 = obj.mesh.indices[i+2];

        Vector3S p0 = projectedPoints[i0];
        Vector3S p1 = projectedPoints[i1];
        Vector3S p2 = projectedPoints[i2];

        Color faceColor = (i % 2 == 0) ? RED : MAROON;
        if (i > 12) faceColor = (i % 2 == 0) ? BLUE : DARKBLUE;
        if (i > 24) faceColor = (i % 2 == 0) ? GREEN : DARKGREEN;

        DrawTriangleFilled(p0, p1, p2, faceColor);
    }
}