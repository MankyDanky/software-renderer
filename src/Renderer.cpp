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

float EdgeFunction(Vector3S a, Vector3S b, Vector3S p) {
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

VSOutput Renderer::VertexShader(const Vertex& vertex, const Matrix4x4&mvp, const Matrix4x4& worldMat) {
    VSOutput out;
    out.position = MultiplyVectorMatrix4(vertex.position, mvp);

    out.worldPos = MultiplyVectorMatrix(vertex.position, worldMat);
    out.normal = Vector3Normalize(MultiplyVectorDirection(vertex.normal, worldMat));

    return out;
}

ScreenVertex Renderer::PerspectiveDivide(VSOutput& in) {
    ScreenVertex out;
    out.invW = 1.0f/in.position.w;
    out.position.x = (in.position.x * out.invW + 1.0f) * 0.5f * width;
    out.position.y = (in.position.y * out.invW  + 1.0f) * 0.5f * height;
    out.position.z = in.position.z * out.invW;

    out.worldPos = Vector3Scale(in.worldPos, out.invW );
    out.normal = Vector3Scale(in.normal, out.invW );
    return out;
};

Color Renderer::FragmentShader(const ScreenVertex& in) {
    Vector3S lightDir = {0.5, 0.4, 1.0f};
    lightDir = Vector3Normalize(lightDir);
    Color objectColor = WHITE;

    float ambient = 0.1f;

    float diff = std::max(0.0f, Vector3Dot(in.normal, Vector3Scale(lightDir, -1.0f)));

    float intensity = ambient + diff*0.5;
    if (intensity > 1.0f) intensity = 1.0f;

    return {
        (unsigned char)(objectColor.r * intensity),
        (unsigned char)(objectColor.g * intensity),
        (unsigned char)(objectColor.b * intensity),
        255
    };
}

void Renderer::RasterizeTriangle(const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2) {
    int minX = std::max(0, (int)std::min({v0.position.x, v1.position.x, v2.position.x}));
    int minY = std::max(0, (int)std::min({v0.position.y, v1.position.y, v2.position.y}));
    int maxX = std::min(width - 1, (int)std::max({v0.position.x, v1.position.x, v2.position.x}));
    int maxY = std::min(height - 1, (int)std::max({v0.position.y, v1.position.y, v2.position.y}));

    
    float area = EdgeFunction(v0.position, v1.position, v2.position);
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
                    Color finalColor = FragmentShader(pixelIn);
                    PutPixel(x, y, finalColor);
                }
            }
        }
    }
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
    std::vector<ScreenVertex> processedVertices;
    Vector3S forward = {cam.rotationMatrix.m[2][0], cam.rotationMatrix.m[2][1], cam.rotationMatrix.m[2][2]};
    for (const auto& v : obj.mesh.vertices) {
        VSOutput vso = VertexShader(v, matMVP, matWorld);
        ScreenVertex clip = PerspectiveDivide(vso);
        processedVertices.push_back(clip);
    }

    for (int i = 0; i < obj.mesh.indices.size(); i += 3) {
        ScreenVertex& v0 = processedVertices[obj.mesh.indices[i]];
        ScreenVertex& v1 = processedVertices[obj.mesh.indices[i+1]];
        ScreenVertex& v2 = processedVertices[obj.mesh.indices[i+2]];

        if (Vector3Dot(v0.normal, forward) >= 0) continue;

        RasterizeTriangle(v0, v1, v2);
    }
}