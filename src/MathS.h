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

inline Vector3S Vector3Cross(Vector3S a, Vector3S b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
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

inline Matrix4x4 MultiplyMatrix(const Matrix4x4& m1, const Matrix4x4& m2) {
    Matrix4x4 out = {0};
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            for (int j = 0; j < 4; j++)
                out.m[r][c] += m1.m[r][j] * m2.m[j][c];
    return out;
}

inline Matrix4x4 MatrixMakeScale(float sx, float sy, float sz) {
    Matrix4x4 mat = Matrix4x4::Identity();
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

inline Matrix4x4 MatrixInverseTranspose3x3(const Matrix4x4& m) {
    float a = m.m[0][0], b = m.m[0][1], c = m.m[0][2];
    float d = m.m[1][0], e = m.m[1][1], f = m.m[1][2];
    float g = m.m[2][0], h = m.m[2][1], i = m.m[2][2];
    
    float det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
    
    if (fabsf(det) < 1e-8f) {
        return Matrix4x4::Identity();
    }
    
    float invDet = 1.0f / det;
    
    Matrix4x4 result = Matrix4x4::Identity();
    
    float c00 = (e * i - f * h);
    float c01 = -(d * i - f * g);
    float c02 = (d * h - e * g);
    float c10 = -(b * i - c * h);
    float c11 = (a * i - c * g);
    float c12 = -(a * h - b * g);
    float c20 = (b * f - c * e);
    float c21 = -(a * f - c * d);
    float c22 = (a * e - b * d);
    
    result.m[0][0] = c00 * invDet;
    result.m[0][1] = c01 * invDet;
    result.m[0][2] = c02 * invDet;
    result.m[1][0] = c10 * invDet;
    result.m[1][1] = c11 * invDet;
    result.m[1][2] = c12 * invDet;
    result.m[2][0] = c20 * invDet;
    result.m[2][1] = c21 * invDet;
    result.m[2][2] = c22 * invDet;
    
    return result;
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

inline Matrix4x4 MatrixTranspose(const Matrix4x4& m) {
    Matrix4x4 out = {0};
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            out.m[r][c] = m.m[c][r];
        }
    }
    return out;
}