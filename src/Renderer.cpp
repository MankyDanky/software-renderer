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
