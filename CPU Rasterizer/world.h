#pragma once
#include "My_Math.h"
struct PrimitiveInfo {
    uint32_t x;
    uint32_t y;
    Vec3 size;
};

struct Vertex
{
    Vec4 coordinates;
    Vec3 color;
    Vec4 normal;
};
struct Rgb
{
    unsigned char r, g, b;
};


class Triangle
{
public:
    Vertex vertices[3];
    void transform(const Vec3& position, const Vec3& rotation, const Vec3& size) {
        Mat4 translationMatrix = makeTranslationMatrix(position);
        Mat4 rotationMatrix = makeRotationMatrix(rotation);
        Mat4 scalingMatrix = makeScalingMatrix(size);
        Mat4 transformMatrix = translationMatrix * rotationMatrix * scalingMatrix;

        for (int i = 0; i < 3; i++) {
            vertices[i].coordinates = transformMatrix * vertices[i].coordinates;
            vertices[i].normal = Vec4(normalize(transformMatrix * vertices[i].normal), 0.0f);
        }
    }
    void operator*=(const Mat4& mvp) {
        for (int i = 0; i < 3; i++) {
            vertices[i].coordinates = mvp * vertices[i].coordinates;
            vertices[i].normal = Vec4(normalize(mvp * vertices[i].normal), 0.0f);
        }
    }
};

struct Polygon
{
    Triangle triangles[2];
};

class Mesh {
public:
    std::vector<uint32_t> rawIndices;
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    Mesh() {}
};

// Goals:
// Building a simple way to place objects in the world from first principle
// Design it completely myself.
//
// The problem is every time i try to create an object i have two problems
// 1. Currently tranforms are coupled with the scene not the object, I think
// each object should have its own transform (at least that what makes sense
// to me)
// 2. I think I should interface with transforms. Specifying transforms like
// translate to x, y, z doesn't sound right
// instead to should be this object is a sphere and it has the position
// x, y, z and something under the hood should transform it
// I probably should have the same for rotation and size
//
//
//
class Object {
private:
    Vec3 position;
    Vec3 rotation;
    Vec3 size;
    Vec3 albedo;
public:
    Mesh mesh;
    Object(const Vec3& pos, const Vec3& rot, const Vec3& sz, const Mesh& m, const Vec3& albedo = Vec3(0.18f, 0.18f, 0.18f))
        : position(pos), rotation(rot), size(sz), mesh(m), albedo(albedo) {
    }

    void transform() {
        for (Triangle& tri : mesh.triangles) {
            tri.transform(position, rotation, size);
        }
    }
};

// Class Helper Functions

void buildTriangles(Mesh& mesh) {
    mesh.triangles.clear();
    for (uint32_t i = 0; i < mesh.rawIndices.size(); i += 3) {
        Triangle tri;
        tri.vertices[0] = mesh.vertices[mesh.rawIndices[i]];
        tri.vertices[1] = mesh.vertices[mesh.rawIndices[i + 1]];
        tri.vertices[2] = mesh.vertices[mesh.rawIndices[i + 2]];
        mesh.triangles.push_back(tri);
    }

}

Mesh createSphere(const PrimitiveInfo& info) {
    Mesh mesh;

    const uint32_t thetaCount = std::max(info.y, (uint32_t)2);
    const uint32_t phiCount = std::max(info.x, (uint32_t)3);
    const float thetaStep = PI / thetaCount;
    const float phiStep = 2.f * PI / phiCount;
    const uint32_t numIndices = 2 * 3 * phiCount + 2 * 3 * phiCount * (thetaCount - 2);
    const uint32_t numVertices = 2 + phiCount * (thetaCount - 1);

    mesh.vertices.resize(numVertices);
    // Top vertex
    uint32_t c = 0;

    Vec3 thirdVertex = normalize(Vec3(0.f, info.size.y, 0.0));
    mesh.vertices[c++] = {
        {0.f, info.size.y, 0.0f, 1.0f},
        {1.f , 1.f, 1.f},
        Vec4(thirdVertex, 0.0f)
    };

    for (uint32_t j = 1; j < thetaCount; j++) {
        const float theta = j * thetaStep;
        for (uint32_t i = 0; i < phiCount; i++) {
            const float phi = i * phiStep;
            Vec3 p = {
                info.size.x * std::sin(theta) * std::cos(phi),
                info.size.y * std::cos(theta),
                info.size.z * std::sin(theta) * std::sin(phi)
            };

            Vec3 n = normalize(p);
            Vec3 color = Vec3(1.f, 1.f, 1.f);
            mesh.vertices[c++] = { Vec4(p, 1.0f), color, Vec4(n, 0.0f)};

        }
    }
    // Bottom vertex
    mesh.vertices[c++] = {
        {0.f, -info.size.y, 0.0f, 1.0f},
        {1.f, 1.f, 1.f},
        Vec4(normalize({0.f, -info.size.y, 0.f}), 0.0f)
    };

    // Triangles construction
    mesh.rawIndices.resize(numIndices);
    c = 0;

    for (uint32_t i = 0; i < phiCount - 1; i++) {
        mesh.rawIndices[c++] = 0;
        mesh.rawIndices[c++] = i + 1;
        mesh.rawIndices[c++] = i + 2;
    }


    mesh.rawIndices[c++] = 0;
    mesh.rawIndices[c++] = phiCount;
    mesh.rawIndices[c++] = 1;

    for (uint32_t j = 0; j < thetaCount - 2; j++) {

        for (uint32_t i = 0; i < phiCount - 1; i++) {
            const uint32_t indices[4] = {
                    1 + i + j * phiCount,
                    1 + i + (j + 1) * phiCount,
                    1 + (i + 1) + (j + 1) * phiCount,
                    1 + (i + 1) + (j)*phiCount
            };

            mesh.rawIndices[c++] = indices[0];
            mesh.rawIndices[c++] = indices[1];
            mesh.rawIndices[c++] = indices[2];

            mesh.rawIndices[c++] = indices[0];
            mesh.rawIndices[c++] = indices[2];
            mesh.rawIndices[c++] = indices[3];
        }

        const uint32_t indices[4] = {
            (phiCount)+j * phiCount,
            (phiCount)+(j + 1) * phiCount,
            1 + (j + 1) * phiCount,
            1 + j * phiCount
        };

        mesh.rawIndices[c++] = indices[0];
        mesh.rawIndices[c++] = indices[1];
        mesh.rawIndices[c++] = indices[2];

        mesh.rawIndices[c++] = indices[0];
        mesh.rawIndices[c++] = indices[2];
        mesh.rawIndices[c++] = indices[3];
    }

    const uint32_t southPoleIndex = (uint32_t)mesh.vertices.size() - 1;
    for (uint32_t i = 0; i < phiCount - 1; i++) {
        mesh.rawIndices[c++] = southPoleIndex;
        mesh.rawIndices[c++] = southPoleIndex - phiCount + i + 1;
        mesh.rawIndices[c++] = southPoleIndex - phiCount + i;
    }

    mesh.rawIndices[c++] = southPoleIndex;
    mesh.rawIndices[c++] = southPoleIndex - phiCount;
    mesh.rawIndices[c++] = southPoleIndex - 1;

    buildTriangles(mesh);

    return mesh;
}
