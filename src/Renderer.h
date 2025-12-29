#pragma once
#include "raylib.h"
#include "Math.h"
#include "GameObject.h"
#include "Camera.h"

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void Clear(Color color);
    void Render();

    void DrawWireframe(const GameObject& obj, const Camera& cam);

private:
    int width, height;
    Color* pixelBuffer;
    Texture2D screenTexture;

    void PutPixel(int x, int y, Color color);
    void DrawLine(int x0, int y0, int x1, int y1, Color color);
};