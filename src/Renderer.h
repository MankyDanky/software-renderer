#pragma once
#include "raylib.h"
#include "MathS.h"
#include "GameObject.h"
#include "ThreadPool.h"
#include "CameraS.h"
#include "Texture.h"

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
};

struct TriangleData {
    ScreenVertex v0, v1, v2;
    float area;
    int minX, minY, maxX, maxY;
    const TextureS* texture;
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

private:
    int width, height;
    Color* pixelBuffer;
    float* depthBuffer;
    Texture2D screenTexture;

    std::unique_ptr<ThreadPool> threadPool;
    unsigned int numThreads;

    int tileSize;
    int tilesX, tilesY;
    std::vector<Tile> tiles;
    std::vector<TriangleData> triangleBuffer;

    void InitTiles();
    void ClearTiles();
    void BinTriangleToTiles(int triangleIndex);
    void RasterizeTile(int tileIndex, const CameraS& cam);
    void RasterizeTriangleInTile(const TriangleData& tri, const Tile& tile, const CameraS& cam);

    VSOutput VertexShader(const Vertex& vertex, const Matrix4x4& mvp, const Matrix4x4& worldMat);
    Color FragmentShader(const ScreenVertex& interpolated, const CameraS& cam, const TextureS* texture);
    ScreenVertex PerspectiveDivide(const VSOutput& in);

    std::vector<VSOutput> ClipTriangleAgainstFrustum(const VSOutput& v0, const VSOutput& v1, const VSOutput& v2);
    std::vector<VSOutput> ClipPolygonAgainstPlane(const std::vector<VSOutput>& polygon, int planeIndex);
    float GetPlaneDistance(const Vector4S& v0, int planeIndex);
    VSOutput LerpVSOutput(const VSOutput& a, const VSOutput& b, float t);

    void PutPixel(int x, int y, Color color);
    void DrawLine(int x0, int y0, int x1, int y1, Color color);
};