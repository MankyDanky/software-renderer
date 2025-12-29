#pragma once
#include <cmath>

struct Vector3S {
    float x, y, z;
};

struct Vector2S {
    float x, y;
};

struct Vector4S {
    float x, y, z, w;
};


inline float Vector3Dot(Vector3S a, Vector3S b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3S Vector3Scale(Vector3S v, float s) {
    return {v.x * s, v.y * s, v.z * s};
}

inline Vector3S Vector3Add(Vector3S a, Vector3S b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vector3S Vector3Sub(Vector3S a, Vector3S b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline float Vector3Length(const Vector3S& v) {
    return sqrtf(v.x*v.x + v.y * v.y + v.z * v.z);
}

inline Vector3S Vector3Normalize(const Vector3S& v) {
    float l = Vector3Length(v);
    if (l == 0) return {0,0,0};
    return {v.x / l, v.y / l, v.z / l};
}

struct Matrix4x4 {
    float m[4][4];

    static Matrix4x4 Identity() {
        Matrix4x4 mat = {0};
        mat.m[0][0] = 1;
        mat.m[1][1] = 1;
        mat.m[2][2] = 1;
        mat.m[3][3] = 1;
        return mat;
    }
};

inline Vector4S MultiplyVectorMatrix4(const Vector3S& i, const Matrix4x4& m) {
    Vector4S v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z*m.m[2][0] + m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z*m.m[2][1] + m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z*m.m[2][2] + m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z*m.m[2][3] + m.m[3][3];
    return v;
}

inline Vector3S MultiplyVectorMatrix(const Vector3S& i, const Matrix4x4& m) {
    Vector3S v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z*m.m[2][0] + m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z*m.m[2][1] + m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z*m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z*m.m[2][3] + m.m[3][3];

    if (w != 0.0f) {
        v.x /= w; v.y /= w; v.z /= w;
    }
    return v;
}

inline Vector3S MultiplyVectorDirection(const Vector3S& i, const Matrix4x4& m) {
    Vector3S v;
        v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z*m.m[2][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z*m.m[2][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z*m.m[2][2];
    return v;
}

inline Matrix4x4 MultiplyMatrix(Matrix4x4& m1, Matrix4x4& m2) {
    Matrix4x4 out = {0};
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            for (int j = 0; j < 4; j++)
                out.m[r][c] += m1.m[r][j] * m2.m[j][c];
    return out;
}

inline Matrix4x4 MatrixMakeRotationX(float angle) {
    Matrix4x4 mat = Matrix4x4::Identity();
    mat.m[1][1] = cosf(angle); mat.m[1][2] = sinf(angle);
    mat.m[2][1] = -sinf(angle); mat.m[2][2] = cosf(angle);
    return mat;
}

inline Matrix4x4 MatrixMakeRotationY(float angle) {
    Matrix4x4 mat = Matrix4x4::Identity();
    mat.m[0][0] = cosf(angle); mat.m[0][2] = sinf(angle);
    mat.m[2][0] = -sinf(angle); mat.m[2][2] = cosf(angle);
    return mat;
}

inline Matrix4x4 MatrixMakeRotationZ(float angle) {
    Matrix4x4 mat = Matrix4x4::Identity();
    mat.m[0][0] = cosf(angle); mat.m[0][1] = sinf(angle);
    mat.m[1][0] = -sinf(angle); mat.m[1][1] = cosf(angle);
    return mat;
}

inline Matrix4x4 MatrixMakeTranslation(float x, float y, float z) {
    Matrix4x4 mat = Matrix4x4::Identity();
    mat.m[3][0] = x; mat.m[3][1] = y; mat.m[3][2] = z;
    return mat;
}

inline Matrix4x4 MatrixMakeProjection(float fovDeg, float aspectRatio, float near, float far) {
    float fovRad = 1.0 / tanf(fovDeg * 0.5f / 180.0f * 3.14159f);
    Matrix4x4 mat = Matrix4x4::Identity();
    mat.m[0][0] = aspectRatio * fovRad;
    mat.m[1][1] = fovRad;
    mat.m[2][2] = far / (far - near);
    mat.m[3][2] = (-far * near) / (far - near);
    mat.m[2][3] = 1.0f;
    mat.m[3][3] = 0.0f;
    return mat;
}