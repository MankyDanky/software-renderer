#pragma once
#include "raylib.h"
#include "MathS.h"
#include <string>
#include <cmath>
#include <algorithm>

struct TextureS {
    Color* pixels = nullptr;
    int width = 0;
    int height = 0;

    bool Load(const std::string& filepath) {
        Image img = LoadImage(filepath.c_str());
        if (img.data == nullptr) {
            return false;
        }

        ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

        width = img.width;
        height = img.height;

        int pixelCount = width * height;
        pixels = new Color[pixelCount];
        memcpy(pixels, img.data, pixelCount * sizeof(Color));

        UnloadImage(img);
        return true;
    }

    void Unload() {
        if (pixels) {
            delete[] pixels;
            pixels = nullptr;
        }
    }

    Color Sample(float u, float v) const {
        if (!pixels) return WHITE;

        u = u - floorf(u);
        v = v - floorf(v);
        v = 1.0f - v;

        int x = (int)(u * (width - 1));
        int y = (int)(v * (height - 1));

        x = std::max(0, std::min(width - 1, x));
        y = std::max(0, std::min(height - 1, y));

        return pixels[y * width + x];
    }

    Color SampleBilinear(float u, float v) const {
        if (!pixels) return WHITE;

        u = u - floorf(u);
        v = 1.0f - (v - floorf(v));

        float fx = u * (width - 1);
        float fy = v * (height - 1);
        
        int x0 = (int)fx;
        int y0 = (int)fy;
        int x1 = std::min(x0 + 1, width - 1);
        int y1 = std::min(y0 + 1, height - 1);

        float tx = fx - x0;
        float ty = fy - y0;

        Color c00 = pixels[y0 * width + x0];
        Color c10 = pixels[y0 * width + x1];
        Color c01 = pixels[y1 * width + x0];
        Color c11 = pixels[y1 * width + x1];

        return {
            (unsigned char)((c00.r * (1-tx) + c10.r * tx) * (1-ty) + (c01.r * (1-tx) + c11.r * tx) * ty),
            (unsigned char)((c00.g * (1-tx) + c10.g * tx) * (1-ty) + (c01.g * (1-tx) + c11.g * tx) * ty),
            (unsigned char)((c00.b * (1-tx) + c10.b * tx) * (1-ty) + (c01.b * (1-tx) + c11.b * tx) * ty),
            255
        };
    }
};