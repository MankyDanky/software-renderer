#pragma once
#include "raylib.h"
#include "MathS.h"
#include "GameObject.h"
#include "CameraS.h"

struct VSOutput {
    Vector3S position;
    Vector3S worldPos;
    Vector3S normal;
};

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void Clear(Color color);
    void Render();

    void DrawMesh(const GameObject& obj, const CameraS& cam);

private:
    int width, height;
    Color* pixelBuffer;
    float* depthBuffer;
    Texture2D screenTexture;

    VSOutput VertexShader(const Vector3S& vertex, const Matrix4x4& mvp, const Matrix4x4& worldMat);
    Color FragmentShader(const VSOutput& interpolated);
    void RasterizeTriangle(const VSOutput& v0, const VSOutput& v1, const VSOutput& v2);

    void PutPixel(int x, int y, Color color);
    void DrawLine(int x0, int y0, int x1, int y1, Color color);
};