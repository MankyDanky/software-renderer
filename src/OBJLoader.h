#pragma once
#include "Components.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ObjLoader {
public:
    static bool LoadOBJ(const std::string& filepath, MeshS& outMesh) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr<<"Failed to open OBJ file: "<<filepath<<std::endl;
            return false;
        }

        std::vector<Vector3S> positions;
        std::vector<Vector3S> normals;

        struct FaceVertex {
            int posIndex;
            int normalIndex;
        };
        std::vector<std::vector<FaceVertex>> faces;
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss>>prefix;

            if (prefix == "v") {
                Vector3S pos;
                iss >> pos.x >> pos.y >> pos.z;
                positions.push_back(pos);
            } else if (prefix == "vn") {
                Vector3S normal;
                iss >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            } else if (prefix == "f") {
                std::vector<FaceVertex> faceVerts;
                std::string vertexData;
                while (iss >> vertexData) {
                    FaceVertex fv = {-1, -1};

                    size_t firstSlash = vertexData.find('/');
                    if (firstSlash == std::string::npos) {
                        fv.posIndex = std::stoi(vertexData) - 1;
                    } else {
                        fv.posIndex = std::stoi(vertexData.substr(0, firstSlash)) - 1;
                        size_t secondSlash = vertexData.find('/', firstSlash + 1);
                        if (secondSlash != std::string::npos) {
                            std::string normalStr = vertexData.substr(secondSlash + 1);
                            if (!normalStr.empty()) {
                                fv.normalIndex = std::stoi(normalStr) - 1;
                            }
                        }
                    }
                    faceVerts.push_back(fv);
                }

                if (faceVerts.size() >= 3) {
                    faces.push_back(faceVerts);
                }
            }
        }

        file.close();

        outMesh.vertices.clear();
        outMesh.indices.clear();

        for (const auto& face : faces) {
            int baseIndex = static_cast<int>(outMesh.vertices.size());

            for (size_t i = 0; i < face.size(); i++) {
                Vertex v;
                v.position = positions[face[i].posIndex];

                if (face[i].normalIndex >= 0 && face[i].normalIndex < static_cast<int>(normals.size())) {
                    v.normal = normals[face[i].normalIndex];
                } else {
                    Vector3S p0 = positions[face[0].posIndex];
                    Vector3S p1 = positions[face[1].posIndex];
                    Vector3S p2 = positions[face[2].posIndex];
                    Vector3S edge1 = Vector3Sub(p1, p0);
                    Vector3S edge2 = Vector3Sub(p2, p0);
                    v.normal = Vector3Normalize(Vector3Cross(edge1, edge2));
                }

                outMesh.vertices.push_back(v);
            }

            for (size_t i = 1; i < face.size() - 1; i++) {
                outMesh.indices.push_back(baseIndex);
                outMesh.indices.push_back(baseIndex + static_cast<int>(i));
                outMesh.indices.push_back(baseIndex + static_cast<int>(i + 1));
            }
        }

        std::cout << "Loaded OBJ: " << positions.size() << " positions, "
                  << normals.size() << " normals, "
                  << outMesh.vertices.size() << " vertices, "
                  << outMesh.indices.size() / 3 << " triangles"<<std::endl;
        return true;
    }
};