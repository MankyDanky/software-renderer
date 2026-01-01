#include "Renderer.h"
#include <cmath>

Renderer::Renderer(int w, int h) : width(w), height(h), tileSize(64) {
    pixelBuffer = new Color[width * height];
    depthBuffer = new float[width * height];
    Image img = GenImageColor(width, height, BLACK);
    screenTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;
    threadPool = std::make_unique<ThreadPool>(numThreads);

    InitTiles();
}

Renderer::~Renderer() {
    UnloadTexture(screenTexture);
    delete[] pixelBuffer;
    delete[] depthBuffer;
}

void Renderer::InitTiles() {
    tilesX = (width + tileSize - 1) / tileSize;
    tilesY = (height + tileSize - 1) / tileSize;
    tiles.resize(tilesX * tilesY);

    for (int ty = 0; ty < tilesY; ty++) {
        for (int tx = 0; tx < tilesX; tx++) {
            Tile& tile = tiles[ty * tilesX + tx];
            tile.startX = tx * tileSize;
            tile.startY = ty * tileSize;
            tile.endX = std::min(tile.startX + tileSize, width);
            tile.endY = std::min(tile.startY + tileSize, height);
            tile.triangleIndices.reserve(644);
        }
    }
}

void Renderer::SetTileSize(int size) {
    tileSize = size;
    InitTiles();
}

int Renderer::GetThreadCount() const {
    return numThreads;
}

void Renderer::ClearTiles() {
    for (auto& tile : tiles) {
        tile.triangleIndices.clear();
    }
    triangleBuffer.clear();
}

void Renderer::BinTriangleToTiles(int triangleIndex) {
    const TriangleData& tri = triangleBuffer[triangleIndex];

    int startTileX = std::max(0, tri.minX / tileSize);
    int startTileY = std::max(0, tri.minY / tileSize);
    int endTileX = std::min(tilesX - 1, tri.maxX / tileSize);
    int endTileY = std::min(tilesY - 1, tri.maxY / tileSize);

    for (int ty = startTileY; ty <= endTileY; ty++) {
        for (int tx = startTileX; tx <= endTileX; tx++) {
            tiles[ty * tilesX + tx].triangleIndices.push_back(triangleIndex);
        }
    }
}

void Renderer::Clear(Color color) {
    int pixelsPerThread = (width * height + numThreads - 1) / numThreads;
    for (unsigned int t = 0; t < numThreads; t++) {
        int start = t * pixelsPerThread;
        int end = std::min(start + pixelsPerThread, width * height);
        threadPool->Enqueue([this, start, end, color]() {
            for (int i = start; i < end; i++) {
                pixelBuffer[i] = color;
                depthBuffer[i] = std::numeric_limits<float>::max();
            }
        });
    }
    threadPool->WaitAll();
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

float EdgeFunction(Vector3S a, Vector3S b, Vector3S p) {
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

void Renderer::RasterizeTriangleInTile(const TriangleData& tri, const Tile& tile, const CameraS& cam) {
    int minX = std::max(tri.minX, tile.startX);
    int minY = std::max(tri.minY, tile.startY);
    int maxX = std::min(tri.maxX, tile.endX-1);
    int maxY = std::min(tri.maxY, tile.endY - 1);

    const ScreenVertex& v0 = tri.v0;
    const ScreenVertex& v1 = tri.v1;
    const ScreenVertex& v2 = tri.v2;
    
    float area = tri.area;
    if (area == 0) return;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Vector3S p = {(float)x, (float)y, 0};

            
            float w0 = EdgeFunction(v1.position, v2.position, p);
            float w1 = EdgeFunction(v2.position, v0.position, p);
            float w2 = EdgeFunction(v0.position, v1.position, p);

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                float lambda0 = w0 / area;
                float lambda1 = w1 / area;
                float lambda2 = w2 / area;

                float z = lambda0 * v0.position.z + lambda1 * v1.position.z + lambda2 * v2.position.z;

                int index = y * width + x;
                if (z < depthBuffer[index]) {
                    depthBuffer[index] = z;
                    
                    float pixelInvW = lambda0 * v0.invW + lambda1 * v1.invW + lambda2 * v2.invW;
                    float pixelW = 1.0f / pixelInvW;
                    ScreenVertex pixelIn;
                    pixelIn.position = p;
                    pixelIn.normal.x = lambda0 * v0.normal.x + lambda1 * v1.normal.x + lambda2 * v2.normal.x;
                    pixelIn.normal.y = lambda0 * v0.normal.y + lambda1 * v1.normal.y + lambda2 * v2.normal.y;
                    pixelIn.normal.z = lambda0 * v0.normal.z + lambda1 * v1.normal.z + lambda2 * v2.normal.z;
                    pixelIn.normal = Vector3Scale(pixelIn.normal, pixelW);
    
                    pixelIn.normal = Vector3Normalize(pixelIn.normal);

                    pixelIn.worldPos.x = lambda0 * v0.worldPos.x + lambda1 * v1.worldPos.x + lambda2 * v2.worldPos.x;
                    pixelIn.worldPos.y = lambda0 * v0.worldPos.y + lambda1 * v1.worldPos.y + lambda2 * v2.worldPos.y;
                    pixelIn.worldPos.z = lambda0 * v0.worldPos.z + lambda1 * v1.worldPos.z + lambda2 * v2.worldPos.z;
                    pixelIn.worldPos = Vector3Scale(pixelIn.worldPos, pixelW);
                    Color finalColor = FragmentShader(pixelIn, cam);
                    PutPixel(x, y, finalColor);
                }
            }
        }
    }
}

void Renderer::RasterizeTile(int tileIndex, const CameraS& cam) {
    const Tile& tile = tiles[tileIndex];

    for (int triIdx : tile.triangleIndices) {
        RasterizeTriangleInTile(triangleBuffer[triIdx], tile, cam);
    }
}

VSOutput Renderer::VertexShader(const Vertex& vertex, const Matrix4x4&mvp, const Matrix4x4& worldMat) {
    VSOutput out;
    out.position = MultiplyVectorMatrix4(vertex.position, mvp);

    out.worldPos = MultiplyVectorMatrix(vertex.position, worldMat);
    out.normal = Vector3Normalize(MultiplyVectorDirection(vertex.normal, worldMat));

    return out;
}

ScreenVertex Renderer::PerspectiveDivide(const VSOutput& in) {
    ScreenVertex out;
    out.invW = 1.0f/in.position.w;
    out.position.x = (in.position.x * out.invW + 1.0f) * 0.5f * width;
    out.position.y = (in.position.y * out.invW  + 1.0f) * 0.5f * height;
    out.position.z = in.position.z * out.invW;

    out.worldPos = Vector3Scale(in.worldPos, out.invW );
    out.normal = Vector3Scale(in.normal, out.invW );
    return out;
};

Color Renderer::FragmentShader(const ScreenVertex& in, const CameraS& cam) {
    Vector3S lightDir = {0.5, 0.4, 1.0f};
    Vector3S viewDir = Vector3Normalize(Vector3Sub(cam.position, in.worldPos));
    lightDir = Vector3Normalize(lightDir);
    Color objectColor = WHITE;

    float ambient = 0.1f;
    Vector3S refl = Vector3Sub(lightDir, Vector3Scale(Vector3Scale(in.normal, Vector3Dot(in.normal, lightDir)), 2));
    float specularity = powf(std::max(0.0f, Vector3Dot(viewDir, refl)), 16);

    float diff = std::max(0.0f, Vector3Dot(in.normal, Vector3Scale(lightDir, -1.0f)));

    float intensity = std::min(1.0f, ambient + diff*0.5f + specularity * 0.5f);
    if (intensity > 1.0f) intensity = 1.0f;

    return {
        (unsigned char)(objectColor.r * intensity),
        (unsigned char)(objectColor.g * intensity),
        (unsigned char)(objectColor.b * intensity),
        255
    };
}

float Renderer::GetPlaneDistance(const Vector4S& v, int planeIndex) {
    switch (planeIndex) {
        case 0: return v.x + v.w;
        case 1: return v.w - v.x;
        case 2: return v.y + v.w;
        case 3: return v.w - v.y;
        case 4: return v.z;
        case 5: return v.w - v.z;
        default: return 0.0f;
    }
}

VSOutput Renderer::LerpVSOutput(const VSOutput& a, const VSOutput& b, float t) {
    VSOutput out;
    out.position.x = a.position.x + t * (b.position.x - a.position.x);
    out.position.y = a.position.y + t * (b.position.y - a.position.y);
    out.position.z = a.position.z + t * (b.position.z - a.position.z);
    out.position.w = a.position.w + t * (b.position.w - a.position.w);

    out.worldPos.x = a.worldPos.x + t * (b.worldPos.x - a.worldPos.x);
    out.worldPos.y = a.worldPos.y + t * (b.worldPos.y - a.worldPos.y);
    out.worldPos.z = a.worldPos.z + t * (b.worldPos.z - a.worldPos.z);

    out.normal.x = a.normal.x + t * (b.normal.x - a.normal.x);
    out.normal.y = a.normal.y + t * (b.normal.y - a.normal.y);
    out.normal.z = a.normal.z + t * (b.normal.z - a.normal.z);
    return out;
}

std::vector<VSOutput> Renderer::ClipPolygonAgainstPlane(const std::vector<VSOutput>& polygon, int planeIndex) {
    std::vector<VSOutput> output;

    for (size_t i = 0; i < polygon.size(); i++) {
        const VSOutput& current = polygon[i];
        const VSOutput& next = polygon[(i+1) % polygon.size()];

        float currentDist = GetPlaneDistance(current.position, planeIndex);
        float nextDist = GetPlaneDistance(next.position, planeIndex);

        bool currentInside = currentDist >= 0;
        bool nextInside = nextDist >= 0;

        if (currentInside) {
            output.push_back(current);

            if (!nextInside) {
                float t = currentDist / (currentDist - nextDist);
                output.push_back(LerpVSOutput(current, next, t));
            }
        } else if (nextInside) {
            float t = currentDist / (currentDist - nextDist);
            output.push_back(LerpVSOutput(current, next, t));
        }
    }

    return output;
}

std::vector<VSOutput> Renderer::ClipTriangleAgainstFrustum(const VSOutput& v0, const VSOutput& v1, const VSOutput& v2) {
    std::vector<VSOutput> polygon = {v0, v1, v2};
    for (int plane = 0; plane < 6; plane++) {
        polygon = ClipPolygonAgainstPlane(polygon, plane);
        if (polygon.empty()) break;
    }
    return polygon;
}

void Renderer::DrawMesh(const GameObject& obj, const CameraS& cam) {
    Matrix4x4 matRotZ = MatrixMakeRotationZ(obj.transform.rotation.z);
    Matrix4x4 matRotY = MatrixMakeRotationY(obj.transform.rotation.y);
    Matrix4x4 matRotX = MatrixMakeRotationX(obj.transform.rotation.x);
    Matrix4x4 matTrans = MatrixMakeTranslation(obj.transform.position.x, obj.transform.position.y, obj.transform.position.z);

    Matrix4x4 matWorld = Matrix4x4::Identity();
    matWorld = MultiplyMatrix(matRotZ, matRotX);
    matWorld = MultiplyMatrix(matWorld, matRotY);
    matWorld = MultiplyMatrix(matWorld, matTrans);

    Matrix4x4 matView = MatrixMakeTranslation(-cam.position.x, -cam.position.y, -cam.position.z);
    matView = MultiplyMatrix(matView, MatrixTranspose(cam.rotationMatrix));
    Matrix4x4 matProj = MatrixMakeProjection(cam.fov, (float)height / (float)width, 0.1f, 1000.0f);

    Matrix4x4 matMVP = Matrix4x4::Identity();
    matMVP = MultiplyMatrix(matWorld, matView);
    matMVP = MultiplyMatrix(matMVP, matProj);

    ClearTiles();

    std::vector<VSOutput> processedVertices;
    for (const auto& v : obj.mesh.vertices) {
        processedVertices.push_back(VertexShader(v, matMVP, matWorld));
    }

    for (int i = 0; i < obj.mesh.indices.size(); i += 3) {
        const VSOutput& vs0 = processedVertices[obj.mesh.indices[i]];
        const VSOutput& vs1 = processedVertices[obj.mesh.indices[i+1]];
        const VSOutput& vs2 = processedVertices[obj.mesh.indices[i+2]];
        Vector3S toCamera = Vector3Sub(cam.position, vs0.worldPos);
        if (Vector3Dot(vs0.normal, toCamera) <= 0) continue;

        std::vector<VSOutput> clippedPolygon = ClipTriangleAgainstFrustum(vs0, vs1, vs2);

        if (clippedPolygon.size() >= 3) {
            ScreenVertex sv0 = PerspectiveDivide(clippedPolygon[0]);
            for (size_t j = 1; j < clippedPolygon.size() - 1; j++) {
                ScreenVertex sv1 = PerspectiveDivide(clippedPolygon[j]);
                ScreenVertex sv2 = PerspectiveDivide(clippedPolygon[j + 1]);

                TriangleData tri;
                tri.v0 = sv0;
                tri.v1 = sv1;
                tri.v2 = sv2;
                tri.area = EdgeFunction(sv0.position, sv1.position, sv2.position);

                if (std::abs(tri.area) < 0.001f) continue;

                tri.minX = std::max(0, (int)std::floor(std::min({sv0.position.x, sv1.position.x, sv2.position.x})));
                tri.minY = std::max(0, (int)std::floor(std::min({sv0.position.y, sv1.position.y, sv2.position.y})));
                tri.maxX = std::min(width - 1, (int)std::ceil(std::max({sv0.position.x, sv1.position.x, sv2.position.x})));
                tri.maxY = std::min(height - 1, (int)std::ceil(std::max({sv0.position.y, sv1.position.y, sv2.position.y})));

                if (tri.minX > tri.maxX || tri.minY > tri.maxY) continue;

                int triIndex = static_cast<int>(triangleBuffer.size());
                triangleBuffer.push_back(tri);
                BinTriangleToTiles(triIndex);
            }
        }
    }

    int totalTiles = tilesX * tilesY;
    for (int i = 0; i < totalTiles; i++) {
        if (!tiles[i].triangleIndices.empty()) {
            threadPool->Enqueue([this, i, cam]() {
                RasterizeTile(i, cam);
            });
        }
    }
    threadPool->WaitAll();
}