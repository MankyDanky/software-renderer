#include "raylib.h"
#include <cmath>

const int screenWidth = 800;
const int screenHeight = 450;

Color* pixelBuffer = nullptr;
Texture2D screenTexture;

void InitRenderer() {
    pixelBuffer = new Color[screenWidth * screenHeight];

    Image screenImage = GenImageColor(screenWidth, screenHeight, BLACK);
    screenTexture = LoadTextureFromImage(screenImage);
    UnloadImage(screenImage);
}

void CleanupRenderer() {
    UnloadTexture(screenTexture);
    delete[] pixelBuffer;
}

void PutPixel(int x, int y, Color color) {
    if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
        pixelBuffer[y * screenWidth + x] = color;
    }
}

// Bresenham Line Algorithm
void DrawLines(int x0, int y0, int x1, int y1, Color color) {
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

void DrawTriangleWireframe(int x0, int y0, int x1, int y1, int x2, int y2, Color color) {
    DrawLines(x0, y0, x1, y1, color);
    DrawLines(x1, y1, x2, y2, color);
    DrawLines(x2, y2, x0, y0, color);
}

void ClearScreen(Color color) {
    for (int i = 0; i < screenWidth*screenHeight; i++) {
        pixelBuffer[i] = color;
    }
}

int main() {
    
    InitWindow(screenWidth, screenHeight, "C++ Software Renderer");

    SetTargetFPS(60);

    InitRenderer();
    
    while (!WindowShouldClose()) {
        ClearScreen(BLACK);
        PutPixel(screenWidth/2, screenHeight/2, RED);
        DrawLines(0, 0, screenWidth-1, screenHeight-1, YELLOW);
        UpdateTexture(screenTexture, pixelBuffer);
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(screenTexture, 0, 0, WHITE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CleanupRenderer();
    CloseWindow();
    return 0;
}