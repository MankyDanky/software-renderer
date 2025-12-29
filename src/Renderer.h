#pragma once
#include "raylib.h"
#include "MathS.h"
#include "GameObject.h"
#include "CameraS.h"

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void Clear(Color color);
    void Render();

    void DrawWireframe(const GameObject& obj, const CameraS& cam);

    void DrawMeshFilled(const GameObject& obj, const CameraS& cam);
    void DrawTriangleFilled(Vector3S v0, Vector3S v1, Vector3S v2, Color color);

private:
    int width, height;
    Color* pixelBuffer;
    float* depthBuffer;
    Texture2D screenTexture;

    void PutPixel(int x, int y, Color color);
    void DrawLine(int x0, int y0, int x1, int y1, Color color);
};