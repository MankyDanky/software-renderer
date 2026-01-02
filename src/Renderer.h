#pragma once
#include "raylib.h"
#include "MathS.h"
#include "GameObject.h"
#include "CameraS.h"
#include "Texture.h"

#ifndef __EMSCRIPTEN__
#include "ThreadPool.h"
#endif

enum class ShadingMode {
    Phong,      // Per-pixel lighting with specular
    Gouraud,    // Per-vertex lighting, interpolated
    Flat,       // Per-triangle lighting using face normal
    Cel,        // Toon/cel shading with quantized bands
    Unlit       // No lighting, just texture/color
};

struct VSOutput {
    Vector4S position;
    
    Vector3S worldPos;
    Vector3S normal;
    Vector2S uv;
};

struct ScreenVertex {
    Vector3S position;
    float invW;

    Vector3S worldPos;
    Vector3S normal;
    Vector2S uv;
    float lightIntensity;  // For Gouraud shading (pre-computed per vertex)
};

struct TriangleData {
    ScreenVertex v0, v1, v2;
    float area;
    int minX, minY, maxX, maxY;
    const TextureS* texture;
    Vector3S faceNormal;     // For flat shading
    float flatIntensity;     // Pre-computed intensity for flat shading
};

struct Tile {
    int startX, startY;
    int endX, endY;
    std::vector<int> triangleIndices;
};

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void Clear(Color color);
    void Render();

    void DrawMesh(const GameObject& obj, const CameraS& cam);

    void SetTileSize(int size);
    int GetThreadCount() const;

    void SetShadingMode(ShadingMode mode) { currentShadingMode = mode; }
    ShadingMode GetShadingMode() const { return currentShadingMode; }
    const char* GetShadingModeName() const;

private:
    int width, height;
    Color* pixelBuffer;
    float* depthBuffer;
    Texture2D screenTexture;

#ifndef __EMSCRIPTEN__
    std::unique_ptr<ThreadPool> threadPool;
    unsigned int numThreads;
#endif

    int tileSize;
    int tilesX, tilesY;
    std::vector<Tile> tiles;
    std::vector<TriangleData> triangleBuffer;
    ShadingMode currentShadingMode = ShadingMode::Phong;

    void InitTiles();
    void ClearTiles();
    void BinTriangleToTiles(int triangleIndex);
    void RasterizeTile(int tileIndex, const CameraS& cam);
    void RasterizeTriangleInTile(const TriangleData& tri, const Tile& tile, const CameraS& cam);

    VSOutput VertexShader(const Vertex& vertex, const Matrix4x4& mvp, const Matrix4x4& worldMat, const Matrix4x4& normalMat, const CameraS& cam);
    Color FragmentShader(const ScreenVertex& interpolated, const CameraS& cam, const TextureS* texture, const TriangleData& tri);
    ScreenVertex PerspectiveDivide(const VSOutput& in);
    float ComputeLightIntensity(const Vector3S& normal, const Vector3S& worldPos, const CameraS& cam);

    std::vector<VSOutput> ClipTriangleAgainstFrustum(const VSOutput& v0, const VSOutput& v1, const VSOutput& v2);
    std::vector<VSOutput> ClipPolygonAgainstPlane(const std::vector<VSOutput>& polygon, int planeIndex);
    float GetPlaneDistance(const Vector4S& v0, int planeIndex);
    VSOutput LerpVSOutput(const VSOutput& a, const VSOutput& b, float t);

    void PutPixel(int x, int y, Color color);
    void DrawLine(int x0, int y0, int x1, int y1, Color color);
};